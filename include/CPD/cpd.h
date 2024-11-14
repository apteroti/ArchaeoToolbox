/*

Coherent Point Drift Algorithm!
Author: Nicholas McDonald

Utilizes nanoflann for kdtree lookup

*/

#include <Eigen/Dense>
#include <Eigen/SVD>

#include <functional>
#include <iostream>

#include "nanoflann.hpp"

#define PI 3.14159265f

namespace cpd {

using namespace Eigen;


//For convenience:
//  Rowmajor makes sense because we want points to be ordered consecutively in an NxD Matrix!
typedef Eigen::Matrix<double,-1,-1,RowMajor> RowMatrix;

//All the properties needed for the algorithm below!

float D = 0.0f;   //Dimensionality of Pointsets
float N = 0.0f;   //Size of Sampled Pointset
float M = 0.0f;   //Size of Centroid Pointset

float s;          //Scale
Eigen::Matrix3d R;       //Rotation
Eigen::Vector3d t;       //Translation
float var;        //Variance

const float w = 0.0f;   //Noise Estimate (Free Parameter)
RowMatrix P;      //Posterior Probability Matrix (Assignments)
Eigen::VectorXd PX;      //Sum over X
Eigen::VectorXd PY;      //Sum over Y

//Options
const bool enforcescale = true; //Force Scale to be s = 1

//Parameters Relevant for Acceleration!

//KD-Tree
const bool usetree = true;  //Whether to use the tree acceleration
const bool usedual = true;

nanoflann::KDTreeEigenMatrixAdaptor<RowMatrix>* kdtree = NULL;
nanoflann::KDTreeEigenMatrixAdaptor<RowMatrix>* ytree = NULL;
const int nleaf = 25;       //Number of Points in Leaf-Node of Kd-Tree

/*
================================================================================
              Initialization, Estimation and Maximization Steps
================================================================================
*/

Eigen::JacobiSVD<RowMatrix> svd;

void initialize(RowMatrix& X, RowMatrix& Y){

  s = 1.0f;                     //Initialize Scale to 1
  R = Eigen::Matrix3d::Identity();     //Initialize Rotation Matrix to Identity
  t = Eigen::Vector3d::Zero();         //Initialize Translation Vector to Zero

  N = X.rows();                 //Get the Number of Points in "Sampled Set"
  M = Y.rows();                 //Get the Number of Points in "Centroid Set"

  var = 0.0f;                   //Compute the Worst-Case Variance
  for(size_t n = 0; n < N; n++)
  for(size_t m = 0; m < M; m++){
    Eigen::Vector3d d = (X.block<1,3>(n,0)-Y.block<1,3>(m,0)); //Distance Between Two Points x_n, y_m
    var += d.dot(d)/(float)(D*N*M);                           //Normalize and Add
  }

  P = RowMatrix::Zero(M,N);      //Allocate Memory for Probability Matrix
  //Sort the static pointset (i.e. samples) into a kd-tree!
  if(usetree){
    if(kdtree != NULL) delete kdtree; //handle "reinitialization"
    kdtree = new nanoflann::KDTreeEigenMatrixAdaptor<RowMatrix>(D, std::cref(X), nleaf);
    kdtree->index->buildIndex();

    if(usedual){
      if(ytree != NULL) delete ytree;
      ytree = new nanoflann::KDTreeEigenMatrixAdaptor<RowMatrix>(D, std::cref(Y), nleaf);
      ytree->index->buildIndex();
    }

  }

}

/*
================================================================================
            Gauss Transform Methods / Probability Computation
================================================================================
*/

//Individual Assignment Probability for x_n, y_m!
float Pmn(Vector3d x, Vector3d& y){
  return exp(-0.5f/var*(x-y).dot(x-y)); //Gaussian Probability
};

//Individual Assignment Probability for x_n, y_m!
float Pmn(double& L2S){
  return exp(-0.5f/var*L2S); //Gaussian Probability
};

float bias = 0.0f;

void direct(RowMatrix& X, RowMatrix& Y){

  for(size_t m = 0; m < M; m++){

    float Z = bias;

    Vector3d YV = Y.block<1,3>(m,0);
    YV = s*R*YV+t;  //Rigid Transform Here!

    for(size_t n = 0; n < N; n++){
      P(m,n) = Pmn(X.block<1,3>(n,0), YV); //Compute Probability
      Z += P(m,n);                                        //Accumulate Partition Function
    }

    for(size_t n = 0; n < N; n++){
      P(m,n) /= Z;                                        //Normalize Density
      PX(n) += P(m,n);                                    //Accumulate Probability along N
      PY(m) += P(m,n);                                    //Accumulate Probability along M
    }

  }

}

void singletree(RowMatrix& X, RowMatrix& Y){

  std::vector<std::pair<long int,double> > matches;
  nanoflann::SearchParams params;

  for(size_t m = 0; m < M; m++){

    float Z = bias;

    Vector3d YV = Y.block<1,3>(m,0);
    YV = s*R*YV+t;  //Rigid Transform Here!

    const size_t nmatches = kdtree->index->radiusSearch(&YV(0), 9.0f*var, matches, params);

    for(auto& match: matches){
      P(m,match.first) = Pmn(match.second); //Distances Squared Returned by Nanoflann
      Z += P(m,match.first);
    }

    for(auto& match: matches){
      P(m,match.first) /= Z;
      PX(match.first) += P(m,match.first);          //Accumulate Probability along M
      PY(m) += P(m,match.first);                    //Accumulate Probability along N
    }

  }

}

void dualtree(RowMatrix& X, RowMatrix& Y){

  std::vector<std::pair<long int,double> > xmatches;
  std::vector<std::pair<long int,double> > ymatches;
  nanoflann::SearchParams params;

  std::vector<bool> openset(M, true);

  const double yrad = 15.0f;

  for(size_t i = 0; i < openset.size(); i++){
    if(!openset[i]) continue; //Skip Processed Y-Elements
    openset[i] = false;

    Vector3d YC = Y.block<1,3>(i,0);        //Search around this Y-Guy

    //Search without Rigid Transform
    const size_t mmatches = ytree->index->radiusSearch(&YC(0), yrad, ymatches, params);

    //Search with Rigid Transform
    YC = s*R*YC+t;  //Rigid Transform Here!
    const size_t nmatches = kdtree->index->radiusSearch(&YC(0), 9.0f*var+yrad, xmatches, params);

    //Iterate over all nearby points to this guy
    for(auto& ymatch: ymatches){
      if(!openset[ymatch.first]) continue;  //Already processed
      openset[ymatch.first] = false;

      float Z = bias;

      Vector3d YV = Y.block<1,3>(ymatch.first,0);
      YV = s*R*YV+t;  //Rigid Transform Here!

      for(auto& xmatch: xmatches){
        P(ymatch.first,xmatch.first) = Pmn(X.block<1,3>(xmatch.first,0), YV); //Distances Squared Returned by Nanoflann
        Z += P(ymatch.first,xmatch.first);
      }

      for(auto& xmatch: xmatches){
        P(ymatch.first,xmatch.first) /= Z;
        PX(xmatch.first) += P(ymatch.first,xmatch.first);          //Accumulate Probability along M
        PY(ymatch.first) += P(ymatch.first,xmatch.first);                    //Accumulate Probability along N
      }

    }

  }

}

/*
================================================================================
                  Expectation Maximization Algorithm
================================================================================
*/

void estimate(RowMatrix& X, RowMatrix& Y){

  //Zero-Out Probabilities
  P = RowMatrix::Zero(M,N);
  PX = VectorXd::Zero(N);
  PY = VectorXd::Zero(M);

  //Compute Bias
  bias = pow(2.0f*PI*var, D/2.0f)*w/(1.0f-w)*M/N;

  //Compute P, PX, PY (~ Gauss Transform)
  if(usetree){
    if(usedual) dualtree(X, Y);
    else singletree(X, Y);
  }
  else direct(X, Y);

}

void maximize(RowMatrix& X, RowMatrix& Y){

  float Np = 1.0f/P.sum();                                //Normalization Constant
  Vector3d uX = X.transpose()*PX*Np;                      //Average Position, X-Set
  Vector3d uY = Y.transpose()*PY*Np;                      //Average Position, Y-Set

  RowMatrix XC = X.rowwise() - uX.transpose();            //Centered X-Set
  RowMatrix YC = Y.rowwise() - uY.transpose();            //Centered Y-Set

  RowMatrix A = XC.transpose()*P.transpose()*YC;          //Singular Value Decomp. Matrix

  svd.compute(A, ComputeFullU|ComputeFullV);              //Compute the SVD of A
  RowMatrix U = svd.matrixU();                            //Get the SVD Matrix U
  RowMatrix V = svd.matrixV();                            //Get the SVD Matrix V

  RowMatrix C = RowMatrix::Identity(D, D);                //Construct the SVD-Derived Matrix C
  C(D-1, D-1) = (U*V.transpose()).determinant();

  //Compute the Rigid Transformation Parameters
  R = U*C*V.transpose();
  s = (A.transpose()*R).trace()/(YC.transpose()*PY.asDiagonal()*YC).trace();

  //Recompute Standard Deviation
  var = Np/D*((XC.transpose()*PX.asDiagonal()*XC).trace() - s*(A.transpose()*R).trace());

  if(enforcescale) s = 1.0f;  //Enforce Constant Scale
  t = uX - s*R*uY;

}

/*
================================================================================
                          Iteration and Utilization
================================================================================
*/

void output(){
  std::cout<<"Rigid Transform (Var = "<<var<<"): "<<std::endl;
  std::cout<<"R "<<R<<std::endl;
  std::cout<<"t "<<t<<std::endl;
  std::cout<<"s "<<s<<std::endl;
}


//Single Iteration Step
float oldvar = 0.0f;
bool itersolve(RowMatrix& X, RowMatrix& Y, int& N, const float tol = 0.01f){

  if(N-- <= 0 || abs(oldvar - var) <= tol) return false;

  oldvar = var;
//  cout<<"Iteration: "<<N<<endl;
//  cout<<"Estimating..."<<endl;
//timer::benchmark<std::chrono::milliseconds>([&](){

    cpd::estimate(X, Y);

// });

//  cout<<"Maximizing..."<<endl;

//  timer::benchmark<std::chrono::milliseconds>([&](){

  cpd::maximize(X, Y);

//  });

//  cpd::output();
  return true;

}

void solve(RowMatrix& X, RowMatrix& Y, int& maxN, const float tol = 0.01f){
  while(itersolve(X, Y, maxN, tol));
}


}

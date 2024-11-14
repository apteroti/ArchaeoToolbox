/***********************************************************************************************
                                                                    
************************************************************************************************                                                                                       
* ArchaeoToolbox                                                                               *
* Geometric Morphometrics Software                                                             *
*                                                                                              *
* Copyright(C) 2023                                                                            *
* Kaveh Yousef Pouran                                                                          *
* Laboratori d’Arqueozoologia, Universitat Autònoma de Barcelona                               *
*                                                                                              *
* All rights reserved.                                                                         *
*                                                                                              *
* This program is free software; you can redistribute it and/or modify                         *   
* it under the terms of the GNU General Public License as published by                         *
* the Free Software Foundation; either version 2 of the License, or                            *
* (at your option) any later version.                                                          *
*                                                                                              *
* This program is distributed in the hope that it will be useful,                              *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                               *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                                *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)                             *
* for more details.                                                                            *
*                                                                                              *

 ***********************************************************************************************                                                                                                                                                               
                                                                               .          
                                                  .                            =:         
                                                  #                            +*         
                                                 ##                            %@.        
                                                =@@                            #@%        
                                               .@@*                            @@@:       
                                               %@@*                           #@@@=       
                                               =@@@#-                     .:+#@@@#        
                                                *@@@@@*=::.:=-=+*%%%+-=*%@@@@@@@=         
                                                 -%@@@@@@@@@@@@@@@@@@@@@@@@%#+-           
                                                   .-=+*#@@@@@@@@@@@@@@@@+.               
                                                       =@@@@@@@@@@@@@@@@@@@@*.            
                                                    .=%@@@@@@@@@@@@@@@@@@@@@@*            
                                              -****%@@@@@@@@@@@@@@@@@@@@@%@@@=            
                                             .@@@@@@@@@@@@@@@@@@@@@@@@@@%  -.             
                                              -@@@@@@@@@@@@@@@@@@@@@@@@@*.                
                                              %@@@@@@@@@@@@@@@@@@@@@@@@@@+                
                                            :%@@@@@@@@@@@@@@@@@@@@@@@@@@%                 
                                    .:=*#%%%@@@@@@@@@@@@@@@@@@@@@@@@@%-=.                 
                                -+%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*                     
                           .-+#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@-                     
                     .--=*%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@-                     
                  :*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@=                     
                .#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@+                     
               :@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@+                     
               %@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@.                     
               +@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@-                      
                #@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@+.                       
           .-+*@@@+:@@@@@@@@@@@@@@@%**+==-------===+@@@@@@@@@@@@@-                        
         :#@@@%%%+ .@@@@@@@@@@@*-:                 +@@@@@@@@@@@@@@%+:                     
       +%@@*.      -@@@@@@@@@=                    =@@@@**#*=--*%@@@@@@*                   
       -*=.       :@@@@@@@@=                       @@@@         .-#@@@@.                  
                 #@@@@@*@@@:                       *@@+            +@@%                   
                 %@@@%  *%@@+                      @@@.            -@@@                   
                 =@@@:    +@@%                    -@@@.            :@@@:                  
                 *@@@      *@@%                   *@@@=            :@@@-                  
                -@@@#      =@@@#                 :@@@@@            #@@@@.                 
                #@@@@.     .###=                 .++++-           .*%%##:                 
                %@@@@.                                                                    
               .*%%%*                                                                     
                      
***********************************************************************************************/

#include "../include/Morphometrics.h"

Morphometrics::TPS::TPS(vtkPoints* templateSliders): m_templateSliders(templateSliders){
   Initialize();
}

void Morphometrics::TPS::GetNeighbour(vtkPolyData* mesh, int id, vtkIdList* output){
   // get all cells that vertex 'id' is a part of
   vtkNew<vtkIdList> cellIdList;
   mesh->GetPointCells(id, cellIdList);
   for (int i = 0; i < cellIdList->GetNumberOfIds(); i++){
      vtkNew<vtkIdList> pointIdList;
      mesh->GetCellPoints(cellIdList->GetId(i), pointIdList);
      for(int j=0; j < pointIdList->GetNumberOfIds(); j++){
         output->InsertUniqueId(pointIdList->GetId(j));
      }
   }
}

double Morphometrics::TPS::EucDist(double Ax, double Ay, double Az, double Bx, double By, double Bz){
   double dx = Ax - Bx;
   double dy = Ay - By;
   double dz = Az - Bz;
   double dist = sqrt((dx*dx) + (dy*dy) + (dz*dz)) ;
   return dist;
}

void Morphometrics::TPS::PDist(vtkPoints* points, Eigen::MatrixXd &output){
   int dim = points->GetNumberOfPoints();
   for(int i=0; i<dim; i++){
      for(int j= i+1; j<dim; j++){
         double dist = EucDist(points->GetPoint(i)[0], points->GetPoint(i)[1], points->GetPoint(i)[2],
         points->GetPoint(j)[0],points->GetPoint(j)[1],points->GetPoint(j)[2]);
         output.operator()(i,j) = dist;
      }
   }
   output.triangularView<Eigen::Lower>()=output.transpose();
   //output.operator*=(-1);
}

void Morphometrics::TPS::SetK(){
   m_K = Eigen::MatrixXd::Zero(m_templateSliders->GetNumberOfPoints(), m_templateSliders->GetNumberOfPoints());
   PDist(m_templateSliders, m_K);
}

void Morphometrics::TPS::SetQ(){
   m_Q = Eigen::MatrixXd::Ones(m_templateSliders->GetNumberOfPoints(), 4);
   for(int i =0; i < m_templateSliders->GetNumberOfPoints(); i++){
      m_Q.operator()(i, 1)= m_templateSliders->GetPoint(i)[0];
      m_Q.operator()(i, 2)= m_templateSliders->GetPoint(i)[1];
      m_Q.operator()(i, 3)= m_templateSliders->GetPoint(i)[2];
   }
}

void Morphometrics::TPS::SetL(){
   m_L.resize(0,0);
   SetK();
   SetQ();
   Eigen::MatrixXd temp1(m_templateSliders->GetNumberOfPoints() + 4, m_templateSliders->GetNumberOfPoints());
   temp1 << m_K,
            m_Q.transpose();
   
   Eigen::MatrixXd matrixO = Eigen::MatrixXd::Zero(4, 4);
   Eigen::MatrixXd temp2(m_templateSliders->GetNumberOfPoints() + 4, 4);
   temp2 << m_Q,
            matrixO;

   m_L.resize(m_templateSliders->GetNumberOfPoints() + 4, m_templateSliders->GetNumberOfPoints() + 4);
   m_L << temp1, temp2;
   matrixO.resize(0,0);
   temp1.resize(0,0);
   temp2.resize(0,0);
   m_K.resize(0,0);
   m_Q.resize(0,0);
}

void Morphometrics::TPS::BEMatrix(){
   m_LInv.resize(0,0);
   m_BEMat.resize(0,0);
   m_LK.resize(0,0);

   std::chrono::steady_clock sc;   // create an object of `steady_clock` class
   auto start = sc.now();     // start timer
   Eigen::FullPivLU <Eigen::MatrixXd> fpluSolver(m_L.selfadjointView<Eigen::Lower>());
   m_LInv = fpluSolver.inverse();
   auto end = sc.now();
   auto time_span = static_cast<std::chrono::duration<double>>(end - start);
   std::cout << "Calculating Inverse of L took: " << time_span.count() << " seconds !!!\n";
   
   m_BEMat = m_LInv.block(0, 0, m_templateSliders->GetNumberOfPoints(), m_templateSliders->GetNumberOfPoints());
   Eigen::MatrixXd temp(m_BEMat.rows()*3, m_BEMat.cols()*3);
   Eigen::MatrixXd zeros= Eigen::MatrixXd::Zero(m_BEMat.rows(), m_BEMat.cols());
   temp<< m_BEMat, zeros, zeros,
   zeros, m_BEMat, zeros,
   zeros, zeros, m_BEMat;

   m_LK = temp.sparseView();
   zeros.resize(0,0);
   temp.resize(0,0);

}

double Morphometrics::TPS::GetBE(Eigen::MatrixXd targetLandmarks){
   if(targetLandmarks.cols() == 3 && targetLandmarks.rows() == m_templateSliders->GetNumberOfPoints()){
      double BEX = targetLandmarks.col(0).transpose()*m_BEMat*targetLandmarks.col(0);
      double BEY = targetLandmarks.col(1).transpose()*m_BEMat*targetLandmarks.col(1);
      double BEZ = targetLandmarks.col(2).transpose()*m_BEMat*targetLandmarks.col(2);
      return std::abs((BEX + BEY + BEZ));
   }
   else{
      std::cout<< "Number of target and template landmarks must be equal, Debug!"<<std::endl;
      return -1;
   }
}

double Morphometrics::TPS::GetBE(vtkPoints* targetLandmarks){
   if(targetLandmarks->GetNumberOfPoints()== m_templateSliders->GetNumberOfPoints()){
      Eigen::MatrixXd targetCoords = Eigen::MatrixXd::Zero(targetLandmarks->GetNumberOfPoints(), 3);
      for(int i =0; i < targetLandmarks->GetNumberOfPoints(); i++){
         for(int j =0; j < 3; j++){
            targetCoords.operator()(i,j) = targetLandmarks->GetPoint(i)[j];
         }
      }
      double BEX = targetCoords.col(0).transpose()*m_BEMat*targetCoords.col(0);
      double BEY = targetCoords.col(1).transpose()*m_BEMat*targetCoords.col(1);
      double BEZ = targetCoords.col(2).transpose()*m_BEMat*targetCoords.col(2);
      return std::abs((BEX + BEY + BEZ));
   }
   else{
      std::cout<< "Number of target and template landmarks must be equal, Debug!"<<std::endl;
      return -1;
   }
}

Eigen::MatrixXd Morphometrics::TPS::PSeudoInverse(Eigen::MatrixXd &mat, double epsilon){ //Moore-Penrose Pseudo-Inverse
   Eigen::JacobiSVD<Eigen::MatrixXd> svd(mat, Eigen::ComputeFullU | Eigen::ComputeFullV);
   double tolerance = epsilon * std::max(mat.cols(), mat.rows()) * svd.singularValues().array().abs()(0);
   return svd.matrixV()*
   (svd.singularValues().array().abs() > tolerance).select(svd.singularValues().array().inverse(), 0).matrix().asDiagonal() * 
   svd.matrixU().adjoint();
}

void Morphometrics::TPS::Initialize(){
   SetL();
   BEMatrix();
}

void Morphometrics::TPS::SetTargetMesh(vtkPolyData* targetMesh){
   m_targetMesh = nullptr;
   m_normals.resize(0,0);
   m_targetMesh = targetMesh;

   auto normalArray = m_targetMesh->GetPointData()->GetArray("Normals");
   m_normals.resize(normalArray->GetNumberOfTuples(),3);
   for(int i=0; i < normalArray->GetNumberOfTuples(); i++){
      for(int j =0; j < 3; j++){
         m_normals.operator()(i,j) = normalArray->GetTuple(i)[j];
      }
   }
   CalculateTangents();
}

void Morphometrics::TPS::CalculateTangents(){
   m_tangentU.resize(0,0);
   m_tangentV.resize(0,0);

   m_tangentU.resize(m_targetMesh->GetNumberOfPoints(), 3);
   m_tangentV.resize(m_targetMesh->GetNumberOfPoints(), 3);

   for(int i=0; i < m_normals.rows(); i++){
      Eigen::Vector3d tempU;
      Eigen::Vector3d tempV;
      Eigen::Vector3d tempNorm(m_normals(i,0), m_normals(i,1), m_normals(i,2));
      if((tempNorm.array() == 0.0).any()){
         tempU <<0.0, 0.0, 0.0;
         for(int j=0; j < 3; j++){
            if(tempNorm(j) == 0){
               tempU.operator()(j) = 1.0;
            }
         }
         tempU.operator/=(tempU.norm());
      }
      else{
         tempU <<1.0, 1.0, -(tempNorm(0) + tempNorm(1)) / tempNorm(2);
         tempU.operator/=(tempU.norm());
      }
      m_tangentU.operator()(i, 0) = tempU(0);
      m_tangentU.operator()(i, 1) = tempU(1);
      m_tangentU.operator()(i, 2) = tempU(2);
      
      tempV = tempU.cross(tempNorm);
      tempV.operator/=(tempV.norm());
      m_tangentV.operator()(i, 0) = tempV(0);
      m_tangentV.operator()(i, 1) = tempV(1);
      m_tangentV.operator()(i, 2) = tempV(2);
   }
}

void Morphometrics::TPS::SetSliders(std::vector<int> &targetSliderIds){
   //Assert template and target Slider dimensions here!! Don't forget
   m_Y0.resize(0,0);
   m_U.resize(0,0);
   m_gamma0.resize(0,0);
   m_T.resize(0,0);

   std::vector<int> fixedVect;
   for(int i =0; i < 3; i++){
      fixedVect.push_back(i);
   }
   m_targetPreSliders.resize(targetSliderIds.size(), 3);
   
   int counter =0;
   for(int i=0; i < targetSliderIds.size(); i++){
      int ptId = targetSliderIds.at(i);
      for(int j=0; j < 3; j++){
         m_targetPreSliders.operator()(i,j) = m_targetMesh->GetPoint(ptId)[j];
      }
   }
   

   
   m_Y0 = Eigen::MatrixXd::Zero(m_targetPreSliders.rows() + 4, 3);
   for(int i =0; i < m_targetPreSliders.rows(); i++){
      for(int j=0; j < 3; j++){
         m_Y0.operator()(i,j) = m_targetPreSliders(i,j);
      }
   }

   
   int landmarkNum = m_targetPreSliders.rows();
   int sliderNum = landmarkNum - fixedVect.size();
   Eigen::MatrixXd temp = Eigen::MatrixXd::Zero(landmarkNum*3, sliderNum*2);

   m_gamma0 = m_targetPreSliders;
   //Eigen::Matrix3d rotationMat = Procrustes::Align(m_templateSliders, m_gamma0);
   m_gamma0.resize(landmarkNum*3, 1);
   //m_normals.operator*=(rotationMat);
   

   counter =0;
   for(int i =0; i <landmarkNum; i++){
      std::vector<int>::iterator it = std::find(fixedVect.begin(), fixedVect.end(), i);
      if(it==fixedVect.end()){
         temp.operator()(i, counter) = m_tangentU(targetSliderIds.at(i), 0);
         temp.operator()(i + landmarkNum, counter) = m_tangentU(targetSliderIds.at(i), 1);
         temp.operator()(i + (2*landmarkNum), counter) = m_tangentU(targetSliderIds.at(i), 2);

         temp.operator()(i, counter + sliderNum) = m_tangentV(targetSliderIds.at(i), 0);
         temp.operator()(i + landmarkNum, counter + sliderNum) = m_tangentV(targetSliderIds.at(i), 1);
         temp.operator()(i + (2*landmarkNum), counter + sliderNum) = m_tangentV(targetSliderIds.at(i), 2);
         counter +=1;
      }
   }
   m_U = temp.sparseView();
   temp.resize(0,0);
   

   Eigen::MatrixXd ULU = m_U.transpose()*m_LK*m_U;
   Eigen::MatrixXd ULG = (m_U.transpose() * m_LK * m_gamma0).sparseView();

   std::chrono::steady_clock sc;   // create an object of `steady_clock` class
   auto start = sc.now();     // start timer
   Eigen::CompleteOrthogonalDecomposition <Eigen::MatrixXd> orthoSolver(ULU.selfadjointView<Eigen::Lower>());
   m_T = orthoSolver.solve(ULG);
   auto end = sc.now();
   auto time_span = static_cast<std::chrono::duration<double>>(end - start);
   std::cout << "Calculating T took: " << time_span.count() << " seconds !!!\n";
   
   ULU.resize(0,0);
   ULG.resize(0,0);
   
   
   Eigen::MatrixXd ULUT = m_U * m_T;
   m_targetPreSliders.resize(landmarkNum*3, 1);
   m_targetPreSliders = m_targetPreSliders - ULUT;
   m_gamma0.resize(0,0);
   m_targetPreSliders.resize(landmarkNum, 3);
   
}

void Morphometrics::TPS::Solve(){
   Eigen::MatrixXd res = m_LInv * m_Y0;
   m_warpCoef = res.block(0,0,m_targetPreSliders.rows(),3);
}


void Morphometrics::TPS::GetCoords(Eigen::MatrixXd &output){
   output = m_targetPreSliders;
}

void Morphometrics::TPS::Print(){
   std::cout << *m_targetMesh<<std::endl;
}

template <typename T>
bool Morphometrics::TPS::IsNearZero(T x){
   return std::abs(x) < std::numeric_limits<T>::epsilon();
}

Morphometrics::TPS::~TPS(){

}

Morphometrics::PCA::PCA(Eigen::MatrixXd &data) : m_data(data){
   m_mean = m_data.colwise().sum()/(m_data.rows());
   m_variance = (m_data.rowwise() - m_mean.transpose()).array().pow(2).colwise().sum()/(m_data.rows()-1);
   m_std = m_variance.array().sqrt();
   Calculate();
}

void Morphometrics::PCA::Calculate(){
   Eigen::MatrixXd centeredMatrix;
   Eigen::MatrixXd covarianceMatrix;

   centeredMatrix = m_data.rowwise() - m_mean.transpose();
   //centeredMatrix.array().rowwise() /= m_std.transpose().array(); //Standardize data

   // Compute covariance matrix
	covarianceMatrix = (centeredMatrix.adjoint().operator*(centeredMatrix)) / (m_data.rows() - 1);
   Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> es;
   es.compute(covarianceMatrix);

   auto eigen_values = es.eigenvalues();
	auto eigen_vectors = es.eigenvectors();

   typedef std::pair<double, int> eigen_pair;
   std::vector<eigen_pair> ep;
   for (int i = 0 ; i < m_data.cols(); ++i) {
	   ep.push_back(std::make_pair(eigen_values(i), i));
    }
   sort(ep.begin(), ep.end()); // Ascending order by default
   // Sort them all in descending order
   m_eigenVectors = Eigen::MatrixXd::Zero(eigen_vectors.rows(), eigen_vectors.cols());
   m_eigenValues = Eigen::VectorXd::Zero(m_data.cols());
   int colnum = 0;
   for (int i = ep.size()-1; i > -1; i--) {
      m_eigenValues(colnum) = ep[i].first;
      m_eigenVectors.col(colnum) += eigen_vectors.col(ep[i].second);
      colnum++;
   }
   m_eigenScores = centeredMatrix.operator*(m_eigenVectors);
   centeredMatrix.resize(0,0);
   covarianceMatrix.resize(0,0);
}

void Morphometrics::PCA::GetEVectors(Eigen::MatrixXd& output, int order){
   if(order > m_eigenVectors.cols()){
      order = m_eigenVectors.cols();
   }
   output = m_eigenVectors.block(0,0, m_eigenVectors.rows(), order);
}

Morphometrics::PCA::~PCA(){}

Morphometrics::Procrustes::Procrustes(){}

void Morphometrics::Procrustes::Align(Eigen::MatrixXd &templateCoords, Eigen::MatrixXd &target){}

Eigen::Matrix3d Morphometrics::Procrustes::Align(vtkPoints *const templateCoords, Eigen::MatrixXd &target){
   Eigen::MatrixXd templateLm(templateCoords->GetNumberOfPoints(), 3);
   for(int i=0; i< templateCoords->GetNumberOfPoints(); i++){
      for(int j=0; j < 3; j++){
         templateLm.operator()(i,j) = templateCoords->GetPoint(i)[j];
      }
   }
   

   Eigen::Matrix< double, Eigen::Dynamic, 1, Eigen::ColMajor > templateMean = templateLm.colwise().sum()/(templateLm.rows());
   Eigen::Matrix< double, Eigen::Dynamic, 1, Eigen::ColMajor > targetMean = target.colwise().sum()/(target.rows());
   //Eigen::MatrixXd centeredTarget = target.rowwise() - targetMean.transpose();
   //Eigen::MatrixXd centeredTemplate = templateLm.rowwise() - templateMean.transpose();
   
   Eigen::Matrix< double, Eigen::Dynamic, 1, Eigen::ColMajor > templateVariance = (templateLm.rowwise() - 
   templateMean.transpose()).array().pow(2).colwise().sum()/(templateLm.rows()-1);
   Eigen::Matrix< double, Eigen::Dynamic, 1, Eigen::ColMajor > templateStd = templateVariance.array().sqrt();
   
   Eigen::Matrix< double, Eigen::Dynamic, 1, Eigen::ColMajor > targetVariance = (target.rowwise() - 
   targetMean.transpose()).array().pow(2).colwise().sum()/(target.rows()-1);
   Eigen::Matrix< double, Eigen::Dynamic, 1, Eigen::ColMajor > targetStd = targetVariance.array().sqrt();

   //centeredTemplate.array().rowwise() /= templateStd.transpose().array(); //Standardize template data
   //centeredTarget.array().rowwise() /= targetStd.transpose().array(); //Standardize target data


   // SVD
   Eigen::MatrixXd Cov = (templateLm.rowwise() - templateMean.transpose()).transpose() * (target.rowwise() - targetMean.transpose());
   Eigen::JacobiSVD<Eigen::MatrixXd> svd(Cov, Eigen::ComputeFullU | Eigen::ComputeFullV);
   Eigen::Matrix3d Rot = svd.matrixV() * svd.matrixU().transpose();
   // Find the rotation
   // calculate determinant of V*U^T to disambiguate rotation sign
   double det = Rot.determinant();
   Eigen::Vector3d e(1, 1, (det < 0)? -1 : 1);
   
   // recompute the rotation part if the determinant was negative
   if (det < 0){
      Rot.noalias() = svd.matrixV() * e.asDiagonal() * svd.matrixU().transpose();
   }
   auto scaleFactor = (1.0/(svd.singularValues().dot(e))) * targetStd; // Umeyama 1991
   Rot.transposeInPlace();
   Rot.array().rowwise().operator*=(scaleFactor.transpose().array());
   targetMean.transpose().array().rowwise().operator*=(scaleFactor.transpose().array());
   
   target.array().rowwise().operator*=(scaleFactor.transpose().array()); //scale
   target.operator*=(Rot);//rotate
   target.array().rowwise().operator-=(targetMean.transpose().array()); //translate

   return Rot;
}

Morphometrics::Procrustes::~Procrustes(){}
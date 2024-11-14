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

#ifndef MORPHOMETRICS_H
#define MORPHOMETRICS_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <Eigen/Eigen>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/Eigenvalues>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkDoubleArray.h>
#include <vtkPointLocator.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkPolyDataNormals.h>
#include <vtkIdList.h>
#include <vtkCenterOfMass.h>
#include <vtkProcrustesAlignmentFilter.h>

/* #include <meta.h>
#include <problem.h>
#include <solver/neldermeadsolver.h> */

#include <chrono>

namespace Morphometrics{
    class TPS {
    private:
        vtkPoints* m_templateSliders;
        vtkPolyData* m_targetMesh;
        Eigen::MatrixXd m_targetPreSliders;
        Eigen::MatrixXd m_normals;
        Eigen::MatrixXd m_tangentU;
        Eigen::MatrixXd m_tangentV;
        Eigen::MatrixXd m_K;
        Eigen::MatrixXd m_Q;
        Eigen::MatrixXd m_L;
        Eigen::SparseMatrix<double> m_LK;
        Eigen::MatrixXd m_LInv;
        Eigen::MatrixXd m_BEMat;
        Eigen::MatrixXd m_Y0;
        Eigen::MatrixXd m_warpCoef;
        Eigen::SparseMatrix<double> m_U;
        Eigen::MatrixXd m_gamma0;
        Eigen::MatrixXd m_T;
        std::vector<double> m_cMass;
        void SetK();
        void SetQ();
        void SetL();
        void BEMatrix();
        Eigen::MatrixXd PSeudoInverse(Eigen::MatrixXd &mat, double epsilon = std::numeric_limits<double>::epsilon());
        void Initialize();
        void CalculateTangents();
        template <typename T>
        bool IsNearZero(T x);
        void Solve();
        static void GetNeighbour(vtkPolyData* mesh, int id, vtkIdList* output);
        double EucDist(double Ax, double Ay, double Az, double Bx, double By, double Bz);
        void PDist(vtkPoints* points, Eigen::MatrixXd &output);

    public:
        TPS(vtkPoints* templateSliders);
        void SetTargetMesh(vtkPolyData* targetMesh);
        void SetSliders(std::vector<int> &targetSliderIds);
        void GetCoords(Eigen::MatrixXd &output);
        void Print();
        double GetBE(Eigen::MatrixXd targetLandmarks);
        double GetBE(vtkPoints* targetLandmarks);
        
        ~TPS();
    };

    class PCA{
    private:
        Eigen::MatrixXd m_data;
        Eigen::Matrix< double, Eigen::Dynamic, 1, Eigen::ColMajor > m_std;
        Eigen::Matrix< double, Eigen::Dynamic, 1, Eigen::ColMajor > m_variance;
        Eigen::Matrix< double, Eigen::Dynamic, 1, Eigen::ColMajor > m_mean;
        Eigen::VectorXd m_eigenValues;
        Eigen::MatrixXd m_eigenVectors;
        Eigen::MatrixXd m_eigenScores;

    public:
    PCA(Eigen::MatrixXd &data);
    void Calculate();
    void GetEVectors(Eigen::MatrixXd& output, int order=2);
    ~PCA();

    };

    class Procrustes{
    private:
        
    public:
        Procrustes();
        void static Align(Eigen::MatrixXd &templateCoords, Eigen::MatrixXd &target);
        Eigen::Matrix3d static Align(vtkPoints *const templateCoords, Eigen::MatrixXd &target);
        ~Procrustes();

    };

}



















#endif
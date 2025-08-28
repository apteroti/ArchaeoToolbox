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

#include "UpdateContributionThread.h"

UpdateContributionThread::UpdateContributionThread(
    vtkPolyData* landmarksPoly, vtkPolyData* meshData, vtkDoubleArray* scalars,
    vtkDoubleArray* outputScalars, QObject* parent)
    : QThread(parent),
      m_landmarksPoly(landmarksPoly),
      m_meshData(meshData),
      m_scalars(scalars),
      m_outputScalars(outputScalars) {}
void UpdateContributionThread::run() {
    if (!m_landmarksPoly || !m_meshData || !m_scalars) {
        emit finished();
        return;
    }

    vtkIdType N = m_landmarksPoly->GetNumberOfPoints();
    vtkIdType M = m_meshData->GetNumberOfPoints();
    if (m_scalars->GetNumberOfTuples() != N) {
        emit finished();
        return;
    }

    qint64 bytesNeeded = static_cast<qint64>(N + 4) *
                         static_cast<qint64>(N + 4) * sizeof(double);
    // Hard safety cap (1 GB)
    qint64 hardLimit = 1LL * 1024 * 1024 * 1024;

    // Runtime available memory
    qint64 avail = GetAvailableMemoryBytes();

    if (bytesNeeded > hardLimit || (avail > 0 && bytesNeeded > avail / 2)) {
        // If > 1 GB or > 50% of free memory, abort
        QString msg =
            QString(
                "Landmark system too large: N=%1 requires ~%2 MB, available ~%3 MB")
                .arg(N)
                .arg(bytesNeeded / (1024.0 * 1024.0), 0, 'f', 1)
                .arg(avail > 0 ? avail / (1024 * 1024) : -1);
        emit failed(msg);
        return;
    }

    // Step 1: Extract landmark positions and scalar values
    Eigen::MatrixXd X(N, 3);
    Eigen::VectorXd Y(N);
    for (vtkIdType i = 0; i < N; ++i) {
        double p[3];
        m_landmarksPoly->GetPoint(i, p);
        X.row(i) << p[0], p[1], p[2];
        Y(i) = m_scalars->GetTuple1(i);
    }

    // Step 2: Build TPS kernel matrix
    Eigen::MatrixXd K(N, N);
    for (vtkIdType i = 0; i < N; ++i) {
        for (vtkIdType j = 0; j < N; ++j) {
            double r = (X.row(i) - X.row(j)).norm();
            K(i, j) = (r > 1e-10) ? (r * r * std::log(r)) : 0.0;
        }
    }

    // Step 3: Build matrix P
    Eigen::MatrixXd P(N, 4);
    P.col(0) = Eigen::VectorXd::Ones(N);
    P.block(0, 1, N, 3) = X;

    // Step 4: Build system
    Eigen::MatrixXd L(N + 4, N + 4);
    L.setZero();
    L.block(0, 0, N, N) = K;
    L.block(0, N, N, 4) = P;
    L.block(N, 0, 4, N) = P.transpose();

    Eigen::VectorXd rhs(N + 4);
    rhs.setZero();
    rhs.head(N) = Y;

    Eigen::VectorXd coeffs = L.fullPivLu().solve(rhs);
    Eigen::VectorXd w = coeffs.head(N);
    Eigen::VectorXd a = coeffs.tail(4);

    // Step 5: Interpolate mesh
    m_outputScalars->Reset();
    m_outputScalars->SetName("Contribution");
    m_outputScalars->SetNumberOfComponents(1);
    m_outputScalars->SetNumberOfTuples(M);

    for (vtkIdType i = 0; i < M; ++i) {
        double p[3];
        m_meshData->GetPoint(i, p);
        Eigen::Vector3d x(p[0], p[1], p[2]);

        double value = a(0) + a.tail(3).dot(x);
        for (vtkIdType j = 0; j < N; ++j) {
            double r = (x - X.row(j).transpose()).norm();
            if (r > 1e-10) value += w(j) * r * r * std::log(r);
        }
        m_outputScalars->SetTuple1(i, value);
    }

    emit finished();
}

// returns available physical memory in bytes, or -1 on failure
qint64 UpdateContributionThread::GetAvailableMemoryBytes() {
#ifdef _WIN32
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    if (GlobalMemoryStatusEx(&status)) {
        return static_cast<qint64>(status.ullAvailPhys);
    }
    return -1;
#elif defined(__linux__)
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        return static_cast<qint64>(info.freeram) * info.mem_unit;
    }
    return -1;
#else
    return -1;  // not implemented
#endif
}

UpdateContributionThread::~UpdateContributionThread() {}
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

#include "../include/BlueNoiseThread.h"

BlueNoiseThread::BlueNoiseThread(vtkPolyData* inputPoly, double area, int resol,
                                 vtkPoints* output, QMutex* mutex)
    : m_resolution(resol),
      m_initRes(resol),
      m_output(output),
      m_meshData(inputPoly),
      m_mutex(mutex) {
    m_output->Initialize();

    m_area = area / (double)resol;
    int M = m_meshData->GetNumberOfPoints();
    double ratio = (double)resol / (double)M;
    m_rMax = 2.0 * (std::sqrt(m_area / (2.0 * std::sqrt(3.0))));
    m_rMin = (m_rMax * (1.0 - std::pow(ratio, m_gamma)) * m_beta);
    m_lmBuffer = m_rMin * 0.25;

    m_kdTree = vtkSmartPointer<vtkStaticPointLocator>::New();
    m_kdTree->SetDataSet(m_meshData);
    m_kdTree->BuildLocator();

    // Initialize weights vector
    m_weights.resize(m_meshData->GetNumberOfPoints());
    for (vtkIdType i = 0; i < m_meshData->GetNumberOfPoints(); ++i) {
        m_weights[i].ptId = i;
        m_weights[i].weight = 0.0;
        m_weights[i].active = true;
    }
}

BlueNoiseThread::BlueNoiseThread(vtkPolyData* inputPoly, vtkPoints* fixedLm,
                                 vtkPoints* curveLm, double area, int resol,
                                 vtkPoints* output, QMutex* mutex)
    : m_resolution(resol),
      m_initRes(resol),
      m_output(output),
      m_fixedLm(fixedLm),
      m_curveLm(curveLm),
      m_meshData(inputPoly),
      m_mutex(mutex) {
    m_output->Initialize();

    m_area = area / (double)resol;
    int M = m_meshData->GetNumberOfPoints();
    double ratio = (double)resol / (double)M;
    m_rMax = 2.0 * (std::sqrt(m_area / (2.0 * std::sqrt(3.0))));
    m_rMin = (m_rMax * (1.0 - std::pow(ratio, m_gamma)) * m_beta);
    m_lmBuffer = m_rMin * 0.25;

    m_kdTree = vtkSmartPointer<vtkStaticPointLocator>::New();
    m_kdTree->SetDataSet(m_meshData);
    m_kdTree->BuildLocator();

    // Initialize weights vector
    m_weights.resize(m_meshData->GetNumberOfPoints());
    for (vtkIdType i = 0; i < m_meshData->GetNumberOfPoints(); ++i) {
        m_weights[i].ptId = i;
        m_weights[i].weight = 0.0;
        m_weights[i].active = true;
    }
}

void BlueNoiseThread::IntegrateLandmarks(vtkPoints* fixedLm,
                                         vtkPoints* curveSliders) {
    //  Handling landmarks if they were digitised
    vtkNew<vtkIdList> lmNeighborSet;
    if (fixedLm) {
        for (int i = 0; i < fixedLm->GetNumberOfPoints(); i++) {
            vtkNew<vtkIdList> tempNeighborSet;
            m_kdTree->FindPointsWithinRadius(m_lmBuffer, fixedLm->GetPoint(i),
                                             tempNeighborSet);
            for (int j = 0; j < tempNeighborSet->GetNumberOfIds(); j++) {
                lmNeighborSet->InsertUniqueId(tempNeighborSet->GetId(j));
            }
        }
    }

    if (curveSliders) {
        for (int i = 0; i < curveSliders->GetNumberOfPoints(); i++) {
            vtkNew<vtkIdList> tempNeighborSet;
            m_kdTree->FindPointsWithinRadius(
                m_lmBuffer, curveSliders->GetPoint(i), tempNeighborSet);

            for (int j = 0; j < tempNeighborSet->GetNumberOfIds(); j++) {
                lmNeighborSet->InsertUniqueId(tempNeighborSet->GetId(j));
            }
        }
    }

    lmNeighborSet->Modified();
    for (vtkIdType i = 0; i < lmNeighborSet->GetNumberOfIds(); i++) {
        vtkIdType ptId = lmNeighborSet->GetId(i);
        m_weights[ptId].active = false;

        vtkNew<vtkIdList> neighborSet;
        m_kdTree->FindPointsWithinRadius(m_rMax, m_meshData->GetPoint(ptId),
                                         neighborSet);

        for (vtkIdType j = 0; j < neighborSet->GetNumberOfIds(); j++) {
            vtkIdType nbrId = neighborSet->GetId(j);
            if (m_weights[nbrId].active) {
                double dist = EucDist(m_meshData->GetPoint(ptId)[0],
                                      m_meshData->GetPoint(ptId)[1],
                                      m_meshData->GetPoint(ptId)[2],
                                      m_meshData->GetPoint(nbrId)[0],
                                      m_meshData->GetPoint(nbrId)[1],
                                      m_meshData->GetPoint(nbrId)[2]);

                double dHat = (dist > m_rMin) ? std::min(dist, m_rMax) : m_rMin;
                double tempW = std::pow(1.0 - (dHat / m_rMax), 8);
                m_weights[nbrId].weight -= tempW;
            }
        }
    }
    m_resolution += lmNeighborSet->GetNumberOfIds();
}

void BlueNoiseThread::CalculateWeight() {
    for (int i = 0; i < m_meshData->GetNumberOfPoints(); i++) {
        if (!m_weights[i].active) continue;

        vtkNew<vtkIdList> tempPtsId;
        m_kdTree->FindPointsWithinRadius(m_rMax, m_meshData->GetPoint(i),
                                         tempPtsId);
        double weight = 0.0;

        for (int j = 0; j < tempPtsId->GetNumberOfIds(); j++) {
            vtkIdType nbrId = tempPtsId->GetId(j);
            if (!m_weights[nbrId].active) continue;

            double dist = EucDist(
                m_meshData->GetPoint(i)[0], m_meshData->GetPoint(i)[1],
                m_meshData->GetPoint(i)[2], m_meshData->GetPoint(nbrId)[0],
                m_meshData->GetPoint(nbrId)[1], m_meshData->GetPoint(nbrId)[2]);

            double dHat = (dist > m_rMin) ? std::min(dist, m_rMax) : m_rMin;
            weight += std::pow(1.0 - (dHat / m_rMax), 8);
        }
        m_weights[i].weight = weight;
    }
}

double BlueNoiseThread::EucDist(double Ax, double Ay, double Az, double Bx,
                                double By, double Bz) {
    double dx = Ax - Bx;
    double dy = Ay - By;
    double dz = Az - Bz;
    double dist = sqrt((dx * dx) + (dy * dy) + (dz * dz));
    return dist;
}

int BlueNoiseThread::FindIndexOfLargest() {
    if (m_meshData->GetNumberOfPoints() > m_parallelThreshold) {
        struct MaxInfo {
            double weight = -std::numeric_limits<double>::max();
            int index = -1;
        } globalMax;

#pragma omp parallel
        {
            MaxInfo threadLocalMax;  // Each thread tracks its own max

// Parallel loop (no false sharing)
#pragma omp for nowait
            for (int i = 0; i < static_cast<int>(m_weights.size()); i++) {
                if (m_weights[i].active &&
                    m_weights[i].weight > threadLocalMax.weight) {
                    threadLocalMax.weight = m_weights[i].weight;
                    threadLocalMax.index = i;
                }
            }

// Merge thread-local results (minimal critical section)
#pragma omp critical
            {
                if (threadLocalMax.weight > globalMax.weight) {
                    globalMax = threadLocalMax;
                }
            }
        }
        return globalMax.index;
    } else {
        double maxVal = -std::numeric_limits<double>::max();
        vtkIdType maxPtId = -1;

        for (const auto& entry : m_weights) {
            if (entry.active && entry.weight > maxVal) {
                maxVal = entry.weight;
                maxPtId = entry.ptId;
            }
        }
        return maxPtId;
    }
}

void BlueNoiseThread::PerformCalculations(int targetId) {
    if (targetId < 0) return;
    m_weights[targetId].active = false;

    vtkNew<vtkIdList> neighborSet;
    m_kdTree->FindPointsWithinRadius(m_rMax, m_meshData->GetPoint(targetId),
                                     neighborSet);
    if (m_meshData->GetNumberOfPoints() > m_parallelThreshold) {
        // Each thread gets its own buffer
        std::vector<double> weightUpdates(neighborSet->GetNumberOfIds(), 0.0);

#pragma omp parallel for
        for (int i = 0; i < neighborSet->GetNumberOfIds(); i++) {
            vtkIdType nbrId = neighborSet->GetId(i);
            if (m_weights[nbrId].active) {
                double dist = EucDist(m_meshData->GetPoint(targetId)[0],
                                      m_meshData->GetPoint(targetId)[1],
                                      m_meshData->GetPoint(targetId)[2],
                                      m_meshData->GetPoint(nbrId)[0],
                                      m_meshData->GetPoint(nbrId)[1],
                                      m_meshData->GetPoint(nbrId)[2]);
                double dHat = (dist > m_rMin) ? std::min(dist, m_rMax) : m_rMin;
                weightUpdates[i] = std::pow(1.0 - (dHat / m_rMax), 8);
            }
        }

        // Single-threaded merge
        for (int i = 0; i < neighborSet->GetNumberOfIds(); i++) {
            vtkIdType nbrId = neighborSet->GetId(i);
            if (m_weights[nbrId].active) {
                m_weights[nbrId].weight -= weightUpdates[i];
            }
        }
    } else {
        for (int i = 0; i < neighborSet->GetNumberOfIds(); i++) {
            vtkIdType nbrId = neighborSet->GetId(i);

            if (m_weights[nbrId].active) {
                double dist = EucDist(m_meshData->GetPoint(targetId)[0],
                                      m_meshData->GetPoint(targetId)[1],
                                      m_meshData->GetPoint(targetId)[2],
                                      m_meshData->GetPoint(nbrId)[0],
                                      m_meshData->GetPoint(nbrId)[1],
                                      m_meshData->GetPoint(nbrId)[2]);

                double dHat = (dist > m_rMin) ? std::min(dist, m_rMax) : m_rMin;
                double tempW = std::pow(1.0 - (dHat / m_rMax), 8);
                m_weights[nbrId].weight -= tempW;
            }
        }
    }
}

void BlueNoiseThread::run() {
    CalculateWeight();
    if (m_fixedLm || m_curveLm) {
        IntegrateLandmarks(m_fixedLm, m_curveLm);
    }
    int target = m_weights.size() - m_resolution;

    for (int i = 0; i < target; i++) {
        int targetId = FindIndexOfLargest();
        if (targetId >= 0) {
            PerformCalculations(targetId);
        }
    }

    int count = 0;
    for (const auto& w : m_weights) {
        if (w.active == true) {
            ++count;
        }
    }

    if (count == m_initRes) {
        m_mutex->lock();
        m_output->Initialize();
        for (const auto& entry : m_weights) {
            if (entry.active) {
                m_output->InsertNextPoint(m_meshData->GetPoint(entry.ptId));
            }
        }
        m_output->Modified();
        m_mutex->unlock();
    } else {
        std::cout << "Problem in Blue Noise Thread, Debug" << std::endl;
        std::cout << "Expected: " << m_initRes << ", Found: " << count
                  << std::endl;
    }

    SamplingIsDone();
}

BlueNoiseThread::~BlueNoiseThread() {}
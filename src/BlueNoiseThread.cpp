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

// Constructor for basic blue noise sampling without landmarks
BlueNoiseThread::BlueNoiseThread(vtkPolyData* inputPoly, double area, int resol,
                                 vtkPoints* output, QMutex* mutex)
    : m_resolution(resol),
      m_initRes(resol),
      m_output(output),
      m_meshData(inputPoly),
      m_mutex(mutex) {
    m_output->Initialize();

    // Calculate area per sample point
    m_area = area / (double)resol;
    int M = m_meshData->GetNumberOfPoints();
    double ratio = (double)resol / (double)M;
    
    // Calculate maximum radius based on desired sample density
    m_rMax = 2.0 * (std::sqrt(m_area / (2.0 * std::sqrt(3.0))));
    // Calculate minimum radius with gamma correction for point distribution
    m_rMin = (m_rMax * (1.0 - std::pow(ratio, m_gamma)) * m_beta);
    // Buffer zone around landmarks
    m_lmBuffer = m_rMin * 0.25;

    // Initialize KD-tree for efficient spatial queries
    m_kdTree = vtkSmartPointer<vtkStaticPointLocator>::New();
    m_kdTree->SetDataSet(m_meshData);
    m_kdTree->BuildLocator();

    // Initialize weights vector for all mesh points
    m_weights.resize(m_meshData->GetNumberOfPoints());
    for (vtkIdType i = 0; i < m_meshData->GetNumberOfPoints(); ++i) {
        m_weights[i].ptId = i;
        m_weights[i].weight = 0.0;
        m_weights[i].active = true;  // All points start as candidates
    }
}

// Constructor with landmarks support
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

    // Same initialization as basic constructor
    m_area = area / (double)resol;
    int M = m_meshData->GetNumberOfPoints();
    double ratio = (double)resol / (double)M;
    m_rMax = 2.0 * (std::sqrt(m_area / (2.0 * std::sqrt(3.0))));
    m_rMin = (m_rMax * (1.0 - std::pow(ratio, m_gamma)) * m_beta);
    m_lmBuffer = m_rMin * 0.25;

    m_kdTree = vtkSmartPointer<vtkStaticPointLocator>::New();
    m_kdTree->SetDataSet(m_meshData);
    m_kdTree->BuildLocator();

    m_weights.resize(m_meshData->GetNumberOfPoints());
    for (vtkIdType i = 0; i < m_meshData->GetNumberOfPoints(); ++i) {
        m_weights[i].ptId = i;
        m_weights[i].weight = 0.0;
        m_weights[i].active = true;
    }
}

// Integrate landmarks into the sampling process
void BlueNoiseThread::IntegrateLandmarks(vtkPoints* fixedLm,
                                         vtkPoints* curveSliders) {
    // Find all mesh points near landmarks
    vtkNew<vtkIdList> lmNeighborSet;
    
    // Process fixed landmarks if they exist
    if (fixedLm) {
        for (int i = 0; i < fixedLm->GetNumberOfPoints(); i++) {
            vtkNew<vtkIdList> tempNeighborSet;
            // Find mesh points within buffer distance of landmark
            m_kdTree->FindPointsWithinRadius(m_lmBuffer, fixedLm->GetPoint(i),
                                             tempNeighborSet);
            // Add to combined neighbor set
            for (int j = 0; j < tempNeighborSet->GetNumberOfIds(); j++) {
                lmNeighborSet->InsertUniqueId(tempNeighborSet->GetId(j));
            }
        }
    }

    // Process curve landmarks if they exist
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
    
    // Process all points near landmarks
    for (vtkIdType i = 0; i < lmNeighborSet->GetNumberOfIds(); i++) {
        vtkIdType ptId = lmNeighborSet->GetId(i);
        m_weights[ptId].active = false;  // Exclude from sampling

        // Find neighbors of landmark-adjacent points
        vtkNew<vtkIdList> neighborSet;
        m_kdTree->FindPointsWithinRadius(m_rMax, m_meshData->GetPoint(ptId),
                                         neighborSet);

        // Update weights of neighbors to enforce exclusion zone
        for (vtkIdType j = 0; j < neighborSet->GetNumberOfIds(); j++) {
            vtkIdType nbrId = neighborSet->GetId(j);
            if (m_weights[nbrId].active) {
                // Calculate Euclidean distance
                double dist = EucDist(m_meshData->GetPoint(ptId)[0],
                                      m_meshData->GetPoint(ptId)[1],
                                      m_meshData->GetPoint(ptId)[2],
                                      m_meshData->GetPoint(nbrId)[0],
                                      m_meshData->GetPoint(nbrId)[1],
                                      m_meshData->GetPoint(nbrId)[2]);

                // Apply distance-based weighting
                double dHat = (dist > m_rMin) ? std::min(dist, m_rMax) : m_rMin;
                double tempW = std::pow(1.0 - (dHat / m_rMax), 8);
                m_weights[nbrId].weight -= tempW;  // Reduce selection probability
            }
        }
    }
    // Adjust target resolution to account for landmark points
    m_resolution += lmNeighborSet->GetNumberOfIds();
}

// Calculate initial weights for all points
void BlueNoiseThread::CalculateWeight() {
    for (int i = 0; i < m_meshData->GetNumberOfPoints(); i++) {
        if (!m_weights[i].active) continue;  // Skip inactive points

        vtkNew<vtkIdList> tempPtsId;
        // Find neighbors within maximum radius
        m_kdTree->FindPointsWithinRadius(m_rMax, m_meshData->GetPoint(i),
                                         tempPtsId);
        double weight = 0.0;

        // Sum influence from all active neighbors
        for (int j = 0; j < tempPtsId->GetNumberOfIds(); j++) {
            vtkIdType nbrId = tempPtsId->GetId(j);
            if (!m_weights[nbrId].active) continue;

            double dist = EucDist(
                m_meshData->GetPoint(i)[0], m_meshData->GetPoint(i)[1],
                m_meshData->GetPoint(i)[2], m_meshData->GetPoint(nbrId)[0],
                m_meshData->GetPoint(nbrId)[1], m_meshData->GetPoint(nbrId)[2]);

            // Apply distance-based weighting function
            double dHat = (dist > m_rMin) ? std::min(dist, m_rMax) : m_rMin;
            weight += std::pow(1.0 - (dHat / m_rMax), 8);
        }
        m_weights[i].weight = weight;
    }
}

// Euclidean distance calculation between two 3D points
double BlueNoiseThread::EucDist(double Ax, double Ay, double Az, double Bx,
                                double By, double Bz) {
    double dx = Ax - Bx;
    double dy = Ay - By;
    double dz = Az - Bz;
    double dist = sqrt((dx * dx) + (dy * dy) + (dz * dz));
    return dist;
}

// Find the point with maximum weight (parallel version for large datasets)
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
        // Sequential version for smaller datasets
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

// Perform calculations when a point is selected as a sample
void BlueNoiseThread::PerformCalculations(int targetId) {
    if (targetId < 0) return;
    m_weights[targetId].active = false;  // Mark as excluded

    vtkNew<vtkIdList> neighborSet;
    m_kdTree->FindPointsWithinRadius(m_rMax, m_meshData->GetPoint(targetId),
                                     neighborSet);
                                     
    // Parallel version for large datasets
    if (m_meshData->GetNumberOfPoints() > m_parallelThreshold) {
        // Each thread gets its own buffer to avoid race conditions
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

        // Single-threaded merge of weight updates
        for (int i = 0; i < neighborSet->GetNumberOfIds(); i++) {
            vtkIdType nbrId = neighborSet->GetId(i);
            if (m_weights[nbrId].active) {
                m_weights[nbrId].weight -= weightUpdates[i];
            }
        }
    } else {
        // Sequential version for smaller datasets
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
                m_weights[nbrId].weight -= tempW;  // Reduce neighbor weights
            }
        }
    }
}

// Main execution thread function
void BlueNoiseThread::run() {
    // === 1. INITIAL WEIGHT CALCULATION ===
    // Compute initial weights for all points based on neighbor density.
    // Uses a kernel function to penalize proximity, encouraging dispersion.
    CalculateWeight();

    // === 2. LANDMARK INTEGRATION (OPTIONAL) ===
    // If landmarks exist, force them into the sample set and suppress their neighbors.
    // This preserves user-defined features while maintaining blue noise properties.
    if (m_fixedLm || m_curveLm) {
        IntegrateLandmarks(m_fixedLm, m_curveLm);
    }

    // === 3. MAIN SAMPLING LOOP ===
    // Determine how many points to eliminate (target = total points - desired samples).
    // The loop eliminates points iteratively, leaving only the optimally spaced samples.
    int target = m_weights.size() - m_resolution;
    for (int i = 0; i < target; i++) {
        // 3.1. Select the point with the highest weight:
        // - High weight = "under-sampled" region (far from existing samples).
        // - Parallelized for large datasets via FindIndexOfLargest().
        int targetId = FindIndexOfLargest();
        
        if (targetId >= 0) {
            // 3.2. Eliminate the selected point from future consideration:
            // - Marks it as inactive (it will *not* be part of the final output).
            // - Updates weights of its neighbors to suppress clustering.
            PerformCalculations(targetId);
        }
    }

    // === 4. RESULT VALIDATION & OUTPUT ===
    // Count remaining active points (should match desired resolution).
    int count = 0;
    for (const auto& w : m_weights) {
        if (w.active == true) {
            ++count;
        }
    }

    // 4.1. Success case: Copy active points to output.
    if (count == m_initRes) {
        m_mutex->lock();
        m_output->Initialize();
        for (const auto& entry : m_weights) {
            if (entry.active) {
                // Active points = optimally spaced blue noise samples.
                m_output->InsertNextPoint(m_meshData->GetPoint(entry.ptId));
            }
        }
        m_output->Modified();
        m_mutex->unlock();
    }
    // 4.2. Error handling (debugging only).
    else {
        std::cout << "Problem in Blue Noise Thread, Debug" << std::endl;
        std::cout << "Expected: " << m_initRes << ", Found: " << count
                  << std::endl;
    }

    // Signal completion.
    SamplingIsDone();
}

// Destructor
BlueNoiseThread::~BlueNoiseThread() {}
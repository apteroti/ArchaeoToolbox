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

    vtkNew<vtkDoubleArray> weights;
    weights->SetName("Weights");
    weights->SetNumberOfComponents(1);
    weights->SetNumberOfTuples(m_meshData->GetNumberOfPoints());
    for (int i = 0; i < weights->GetNumberOfTuples(); i++) {
        weights->SetValue(i, 0);
    }
    m_meshData->GetPointData()->AddArray(weights);
    m_meshData->Modified();
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

    vtkNew<vtkDoubleArray> weights;
    weights->SetName("Weights");
    weights->SetNumberOfComponents(1);
    weights->SetNumberOfTuples(m_meshData->GetNumberOfPoints());
    for (int i = 0; i < weights->GetNumberOfTuples(); i++) {
        weights->SetValue(i, 0);
    }
    m_meshData->GetPointData()->AddArray(weights);
    m_meshData->Modified();
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

    for (int i = 0; i < lmNeighborSet->GetNumberOfIds(); i++) {
        m_meshData->GetPointData()->GetArray("Weights")->SetTuple1(
            lmNeighborSet->GetId(i), -1);
        m_meshData->GetPointData()->GetArray("Weights")->Modified();
        m_meshData->Modified();
        vtkNew<vtkIdList> neighborSet;
        m_kdTree->FindPointsWithinRadius(
            m_rMax, m_meshData->GetPoint(lmNeighborSet->GetId(i)), neighborSet);
        for (int j = 0; j < neighborSet->GetNumberOfIds(); j++) {
            double w =
                m_meshData->GetPointData()->GetArray("Weights")->GetTuple1(
                    neighborSet->GetId(j));
            if (w >= 0) {
                double dist =
                    EucDist(m_meshData->GetPoint(lmNeighborSet->GetId(i))[0],
                            m_meshData->GetPoint(lmNeighborSet->GetId(i))[1],
                            m_meshData->GetPoint(lmNeighborSet->GetId(i))[2],
                            m_meshData->GetPoint(neighborSet->GetId(j))[0],
                            m_meshData->GetPoint(neighborSet->GetId(j))[1],
                            m_meshData->GetPoint(neighborSet->GetId(j))[2]);
                double dHat = 0.0;
                if (dist > m_rMin) {
                    dHat = std::min(dist, m_rMax);
                } else {
                    dHat = m_rMin;
                }
                double tempW = std::pow(1.0 - (dHat / m_rMax), 8);
                double updatedWeight = w - tempW;

                m_meshData->GetPointData()->GetArray("Weights")->SetTuple1(
                    neighborSet->GetId(j), updatedWeight);
                m_meshData->GetPointData()->GetArray("Weights")->Modified();
                m_meshData->Modified();
            }
        }
    }
    m_resolution += lmNeighborSet->GetNumberOfIds();
}

void BlueNoiseThread::CalculateWeight(vtkPolyData* inputPoly,
                                      vtkStaticPointLocator* tree, double rMax,
                                      double rMin, int res) {
    for (int i = 0; i < inputPoly->GetNumberOfPoints(); i++) {
        vtkNew<vtkIdList> tempPtsId;
        tree->FindPointsWithinRadius(rMax, inputPoly->GetPoint(i), tempPtsId);
        double weight = 0.0;
        for (int j = 0; j < tempPtsId->GetNumberOfIds(); j++) {
            double dist =
                EucDist(inputPoly->GetPoint(i)[0], inputPoly->GetPoint(i)[1],
                        inputPoly->GetPoint(i)[2],
                        inputPoly->GetPoint(tempPtsId->GetId(j))[0],
                        inputPoly->GetPoint(tempPtsId->GetId(j))[1],
                        inputPoly->GetPoint(tempPtsId->GetId(j))[2]);
            double dHat = 0.0;
            if (dist > rMin) {
                dHat = std::min(dist, rMax);
            } else {
                dHat = rMin;
            }
            weight += std::pow(1.0 - (dHat / rMax), 8);
        }
        inputPoly->GetPointData()->GetArray("Weights")->SetComponent(i, 0,
                                                                     weight);
        inputPoly->GetPointData()->GetArray("Weights")->Modified();
        inputPoly->Modified();
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

int BlueNoiseThread::FindIndexOfLargest(vtkPolyData* poly,
                                        std::string arrName) {
    double maxVal =
        poly->GetPointData()->GetArray(arrName.data())->GetRange()[1];
    vtkDoubleArray* tempArr = vtkDoubleArray::SafeDownCast(
        poly->GetPointData()->GetArray(arrName.data()));

    auto begin = tempArr->GetPointer(0);
    double* end = begin + tempArr->GetNumberOfValues();
    double* it = std::find(begin, end, maxVal);
    int targetId = std::distance(begin, it);
    return targetId;
}

void BlueNoiseThread::PerformCalculations(vtkPolyData* poly,
                                          vtkStaticPointLocator* tree,
                                          std::string arrName, int targetId,
                                          double rMin, double rMax) {
    poly->GetPointData()
        ->GetArray(arrName.data())
        ->SetComponent(targetId, 0, -1);
    poly->GetPointData()->GetArray(arrName.data())->Modified();
    poly->Modified();
    vtkNew<vtkIdList> neighborSet;
    tree->FindPointsWithinRadius(rMax, poly->GetPoint(targetId), neighborSet);
    for (int i = 0; i < neighborSet->GetNumberOfIds(); i++) {
        double w = poly->GetPointData()
                       ->GetArray(arrName.data())
                       ->GetTuple1(neighborSet->GetId(i));
        if (w >= 0.0) {
            double dist = EucDist(poly->GetPoint(targetId)[0],
                                  poly->GetPoint(targetId)[1],
                                  poly->GetPoint(targetId)[2],
                                  poly->GetPoint(neighborSet->GetId(i))[0],
                                  poly->GetPoint(neighborSet->GetId(i))[1],
                                  poly->GetPoint(neighborSet->GetId(i))[2]);
            double dHat = 0.0;
            if (dist > rMin) {
                dHat = std::min(dist, rMax);
            } else {
                dHat = rMin;
            }
            double tempW = std::pow(1.0 - (dHat / rMax), 8);
            double updatedWeight = w - tempW;

            poly->GetPointData()
                ->GetArray(arrName.data())
                ->SetTuple1(neighborSet->GetId(i), updatedWeight);
            poly->GetPointData()->GetArray(arrName.data())->Modified();
            poly->Modified();
        }
    }
}

void BlueNoiseThread::run() {
    CalculateWeight(m_meshData, m_kdTree, m_rMax, m_rMin, m_resolution);
    if (m_fixedLm || m_curveLm) {
        IntegrateLandmarks(m_fixedLm, m_curveLm);
    }
    int remaining = m_meshData->GetNumberOfPoints();
    int target = remaining - m_resolution;

    //auto start = std::chrono::high_resolution_clock::now();
    //#pragma omp parallel for 
    for (int i = 0; i < target; i++) {
        m_mutex->lock();
        int targetId = FindIndexOfLargest(m_meshData, "Weights");
        
        PerformCalculations(m_meshData, m_kdTree, "Weights", targetId, m_rMin,
                            m_rMax);
        m_mutex->unlock();
    }
    /* auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Elapsed time: " << duration.count() << " seconds\n"; */

    m_mutex->lock();
    vtkNew<vtkThresholdPoints> threshold;
    threshold->SetInputData(m_meshData);
    threshold->ThresholdByUpper(0.0);
    threshold->SetInputArrayToProcess(
        0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "Weights");
    threshold->Update();
    vtkPoints* tempPts = threshold->GetOutput()->GetPoints();

    m_meshData->GetPointData()->RemoveArray("Weights");
    m_meshData->Modified();
    m_mutex->unlock();
    if (tempPts->GetNumberOfPoints() == m_initRes) {
        m_mutex->lock();
        m_output->DeepCopy(tempPts);
        m_output->Modified();
        m_mutex->unlock();
    } else {
        std::cout << "Problem in Blue Noise Thread, Debug" << std::endl;
        std::cout << tempPts->GetNumberOfPoints() << std::endl;
    }

    /* vtkNew<vtkPoints> outputPts;
    outputPts->DeepCopy(threshold->GetOutput()->GetPoints()); */

    SamplingIsDone();
}

BlueNoiseThread::~BlueNoiseThread() {}
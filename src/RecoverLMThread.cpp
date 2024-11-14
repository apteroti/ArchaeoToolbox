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

#include "../include/RecoverLMThread.h"

#include "../include/MainWindow.h"

RecoverLMThread::RecoverLMThread(MainWindow* parent,
                                 std::vector<std::vector<std::string>>& content,
                                 std::string& path, QMutex* mutex)
    : m_parent(parent), m_content(content), m_path(path), m_mutex(mutex) {
    m_dataBase = m_parent->GetDataBase();
    m_typeINOL = m_parent->GetTypeINOL();
    m_curveNOS = m_parent->GetCurveNOS();
    m_curveNOC = m_parent->GetNumberOfCurves();
    m_surfaceNOS = m_parent->GetSurfaceNOS();
    m_surfacePatchUNOS = m_parent->GetURes();
    m_surfacePatchVNOS = m_parent->GetVRes();
    m_surfacePatchNOP = m_parent->GetNumberOfPatches();
}

void RecoverLMThread::run() {
    for (int i = 0; i < m_content.size(); i++) {
        if (!m_content[i].empty()) {
            std::string sampleName = m_content[i][0];
            if (sampleName != "Template") {
                std::string meshName = m_path + sampleName + ".obj";
                vtkNew<vtkOBJReader> objReader;
                objReader->SetFileName(meshName.c_str());
                objReader->Update();
                if (objReader->GetOutput()->GetNumberOfPoints() > 0) {
                    vtkNew<vtkCleanPolyData> cleanFilter;
                    cleanFilter->SetInputData(objReader->GetOutput());
                    cleanFilter->Update();
                    m_mutex->lock();
                    while (m_dataBase->CheckMembership(sampleName)) {
                        sampleName += "_Duplicate";
                    }
                    m_dataBase->AddNode(sampleName, cleanFilter->GetOutput(),
                                        "OBJ");
                    m_mutex->unlock();
                    // check number of landmarks
                    vtkNew<vtkPoints> fixedPts;
                    vtkNew<vtkPoints> curvePts;
                    vtkNew<vtkPoints> surfacePatchPts;
                    vtkNew<vtkPoints> surfacePts;
                    int numCurveSlider = m_curveNOS * m_curveNOC;
                    int numSurfPatchSlider = m_surfacePatchNOP *
                                             m_surfacePatchUNOS *
                                             m_surfacePatchVNOS;
                    int startPoint = 1;
                    int endPoint = (m_typeINOL * 3) + 1;
                    if (endPoint <= m_content[i].size()) {
                        for (int j = startPoint; j < endPoint; j += 3) {
                            if (!m_content[i][j].empty() &&
                                !m_content[i][j + 1].empty() &&
                                !m_content[i][j + 2].empty()) {
                                if (IsNumber(m_content[i][j]) &&
                                    IsNumber(m_content[i][j + 1]) &&
                                    IsNumber(m_content[i][j + 2])) {
                                    double x =
                                        std::stod(m_content[i][j].c_str());
                                    double y =
                                        std::stod(m_content[i][j + 1].c_str());
                                    double z =
                                        std::stod(m_content[i][j + 2].c_str());
                                    fixedPts->InsertNextPoint(x, y, z);
                                } else {
                                    std::cout << "Non-digit "
                                                 "fixed row"
                                              << std::endl;
                                    break;
                                }
                            } else {
                                std::cout << "Corrupted row" << std::endl;
                                break;
                            }
                        }
                    } else {
                        std::cout << "fixed length missmatch" << std::endl;
                        break;
                    }
                    startPoint = endPoint;
                    endPoint = 1 + (m_typeINOL * 3) + (numCurveSlider * 3);
                    if (endPoint <= m_content[i].size()) {
                        for (int j = startPoint; j < endPoint; j += 3) {
                            if (!m_content[i][j].empty() &&
                                !m_content[i][j + 1].empty() &&
                                !m_content[i][j + 2].empty()) {
                                if (IsNumber(m_content[i][j]) &&
                                    IsNumber(m_content[i][j + 1]) &&
                                    IsNumber(m_content[i][j + 2])) {
                                    double x =
                                        std::stod(m_content[i][j].c_str());
                                    double y =
                                        std::stod(m_content[i][j + 1].c_str());
                                    double z =
                                        std::stod(m_content[i][j + 2].c_str());
                                    curvePts->InsertNextPoint(x, y, z);
                                } else {
                                    std::cout << "Non-digit "
                                                 "curve row"
                                              << std::endl;
                                    break;
                                }
                            } else {
                                std::cout << "Corrupted curve row" << std::endl;
                                break;
                            }
                        }
                    } else {
                        std::cout << "Curve length missmatch" << std::endl;
                        break;
                    }
                    startPoint = endPoint;
                    if (numSurfPatchSlider == 0 && m_surfaceNOS != 0) {
                        endPoint = 1 + (m_typeINOL * 3) + (numCurveSlider * 3) +
                                   (m_surfaceNOS * 3);
                        if (endPoint <= m_content[i].size()) {
                            for (int j = startPoint; j < endPoint; j += 3) {
                                if (!m_content[i][j].empty() &&
                                    !m_content[i][j + 1].empty() &&
                                    !m_content[i][j + 2].empty()) {
                                    if (IsNumber(m_content[i][j]) &&
                                        IsNumber(m_content[i][j + 1]) &&
                                        IsNumber(m_content[i][j + 2])) {
                                        double x =
                                            std::stod(m_content[i][j].c_str());
                                        double y = std::stod(
                                            m_content[i][j + 1].c_str());
                                        double z = std::stod(
                                            m_content[i][j + 2].c_str());
                                        surfacePts->InsertNextPoint(x, y,
                                                                         z);
                                    } else {
                                        std::cout << "Non-digit "
                                                     "surface-patch "
                                                     "row"
                                                  << std::endl;
                                        break;
                                    }
                                } else {
                                    std::cout << "Corrupted "
                                                 "Surface-patch row"
                                              << std::endl;
                                    break;
                                }
                            }
                        } else {
                            std::cout << "Surface row missmatch" << std::endl;
                            break;
                        }
                    }
                    if (numSurfPatchSlider != 0 && m_surfaceNOS == 0) {
                        endPoint = 1 + (m_typeINOL * 3) + (numCurveSlider * 3) +
                                   (numSurfPatchSlider * 3);
                        if (endPoint <= m_content[i].size()) {
                            for (int j = startPoint; j < endPoint; j += 3) {
                                if (!m_content[i][j].empty() &&
                                    !m_content[i][j + 1].empty() &&
                                    !m_content[i][j + 2].empty()) {
                                    if (IsNumber(m_content[i][j]) &&
                                        IsNumber(m_content[i][j + 1]) &&
                                        IsNumber(m_content[i][j + 2])) {
                                        double x =
                                            std::stod(m_content[i][j].c_str());
                                        double y = std::stod(
                                            m_content[i][j + 1].c_str());
                                        double z = std::stod(
                                            m_content[i][j + 2].c_str());
                                        surfacePatchPts->InsertNextPoint(x, y,
                                                                         z);
                                    } else {
                                        std::cout << "Non-digit "
                                                     "surface-patch "
                                                     "row"
                                                  << std::endl;
                                        break;
                                    }
                                } else {
                                    std::cout << "Corrupted "
                                                 "Surface-patch row"
                                              << std::endl;
                                    break;
                                }
                            }
                        } else {
                            std::cout << "Surface row missmatch" << std::endl;
                            break;
                        }
                    }

                    if (fixedPts->GetNumberOfPoints() == m_typeINOL &&
                        curvePts->GetNumberOfPoints() == numCurveSlider &&
                        surfacePts->GetNumberOfPoints() == m_surfaceNOS &&
                        surfacePatchPts->GetNumberOfPoints() ==
                            numSurfPatchSlider) {
                        m_mutex->lock();
                        m_dataBase->DeleteAllLandmarks(sampleName);
                        m_dataBase->InsertTypeI(sampleName, fixedPts);
                        m_dataBase->InsertCurveSliders(sampleName, curvePts);
                        m_dataBase->InsertSurfaceSliders(sampleName,
                                                        surfacePts);
                        if(numSurfPatchSlider != 0 && m_surfaceNOS == 0){
                            m_dataBase->InsertSurfaceSliders(sampleName,
                                                            surfacePatchPts);
                        }
                        
                        m_mutex->unlock();
                        emit TreeObjectChanged(sampleName);
                        emit DataBaseChanged(sampleName);
                    } else {
                        std::cout << fixedPts->GetNumberOfPoints() << std::endl;
                        std::cout << curvePts->GetNumberOfPoints() << std::endl;
                        std::cout << surfacePatchPts->GetNumberOfPoints()
                                  << std::endl;
                        std::cout << sampleName << std::endl;
                        std::cout << "doesn't match the template" << std::endl;
                        std::cout << "Template has" << std::endl;
                        std::cout << numSurfPatchSlider << std::endl;
                        std::cout << "Surface Sliders" << std::endl;
                    }
                }
                else{
                    std::cout<< "Couldn't find Mesh File:"<<std::endl;
                    std::cout<< meshName<<std::endl;
                    break;
                }
            } 
            else {
                // Template name is protected
            }
        } else {
            break;
        }
    }
    RecoveryIsDone();
}

bool RecoverLMThread::IsNumber(const std::string& s) {
    char* end = nullptr;
    double val = strtod(s.c_str(), &end);
    return end != s.c_str() && *end == '\0' && val != HUGE_VAL;
}

RecoverLMThread::~RecoverLMThread() {}
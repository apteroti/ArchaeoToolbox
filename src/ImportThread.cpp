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

#include "../include/ImportThread.h"

#include "../include/MainWindow.h"

ImportThread::ImportThread(MainWindow* parent, QString& path, QMutex* mutex)
    : m_parent(parent), m_path(path), m_mutex(mutex) {
    // m_dataBase = m_parent->GetDataBase();
}
ImportThread::ImportThread(MainWindow* parent) : m_parent(parent) {
    // m_dataBase = m_parent->GetDataBase();
}

void ImportThread::run() {
    m_mutex->lock();
    bool hasTemplate = m_parent->GetDataBase()->CheckMembership("Template");
    m_mutex->unlock();
    if (hasTemplate) {
        m_mutex->lock();
        int numFixedLM =
            m_parent->GetDataBase()->GetTypeI("Template")->GetNumberOfPoints();
        int numCurveLM = m_parent->GetDataBase()
                             ->GetCurveSliders("Template")
                             ->GetNumberOfPoints();
        int numSurfaceLM = m_parent->GetDataBase()
                               ->GetSurfaceSliders("Template")
                               ->GetNumberOfPoints();
        auto nameList = m_parent->GetDataBase()->GetNodeNames();
        m_mutex->unlock();
        vtkNew<vtkPoints> tempFixed;
        vtkNew<vtkPoints> tempCurve;
        vtkNew<vtkPoints> tempSurface;
        vtkNew<vtkPoints> tempVertex;
        vtkNew<vtkCellArray> tempPolyCells;
        vtkNew<vtkPolyData> tempPoly;

        std::string fname = m_path.toStdString();
        std::vector<std::string> row;
        std::string line, word;
        fstream file(fname, ios::in);
        std::string name = "";
        int tempNumFixed = 0;
        int tempNumCurve = 0;
        int tempNumSurf = 0;
        int numPoly = 0;
        int numVert = 0;
        std::string type = "";
        if (file.is_open()) {
            while (getline(file, line)) {
                row.clear();
                std::stringstream str(line);
                while (getline(str, word, ',')) {
                    row.push_back(word);
                }
                if (row[0] == "Specimen") {
                    name = row[1];
                }
                if (name != "Template") {
                    while (std::find(nameList.begin(), nameList.end(), name) !=
                           nameList.end()) {
                        name += "_Duplicate";
                    }

                    if (row[0] == "Type") {
                        type = row[1];
                    }

                    if (row[0] == "Number of Fixed Landmarks") {
                        tempNumFixed = std::stoi(row[1]);
                        tempFixed->Initialize();
                        tempFixed->Modified();
                    }
                    if (row[0] == "Fixed Coords") {
                        if (IsNumber(row[1]) && IsNumber(row[2]) &&
                            IsNumber(row[3])) {
                            tempFixed->InsertNextPoint(std::stod(row[1]),
                                                       std::stod(row[2]),
                                                       std::stod(row[3]));
                            tempFixed->Modified();
                        } else {
                            std::cout << "Non numerical value, debug!"
                                      << std::endl;
                            ImportIsDone();
                            return;
                        }
                    }
                    if (row[0] == "Number of Curve Landmarks") {
                        tempNumCurve = std::stoi(row[1]);
                        tempCurve->Initialize();
                        tempCurve->Modified();
                    }
                    if (row[0] == "Curve Coords") {
                        if (IsNumber(row[1]) && IsNumber(row[2]) &&
                            IsNumber(row[3])) {
                            tempCurve->InsertNextPoint(std::stod(row[1]),
                                                       std::stod(row[2]),
                                                       std::stod(row[3]));
                            tempCurve->Modified();
                        } else {
                            std::cout << "Non numerical value, debug!"
                                      << std::endl;
                            ImportIsDone();
                            return;
                        }
                    }
                    if (row[0] == "Number of Surface Landmarks") {
                        tempNumSurf = std::stoi(row[1]);
                        tempSurface->Initialize();
                        tempSurface->Modified();
                    }
                    if (row[0] == "Surface Coords") {
                        if (IsNumber(row[1]) && IsNumber(row[2]) &&
                            IsNumber(row[3])) {
                            tempSurface->InsertNextPoint(std::stod(row[1]),
                                                         std::stod(row[2]),
                                                         std::stod(row[3]));
                            tempSurface->Modified();
                        } else {
                            std::cout << "Non numerical value, debug!"
                                      << std::endl;
                            ImportIsDone();
                            return;
                        }
                    }
                    if (row[0] == "Number of Vertices") {
                        tempVertex->Initialize();
                        tempVertex->Modified();
                        numVert = std::stoi(row[1]);
                    }
                    if (row[0] == "Vertex Coords") {
                        if (IsNumber(row[1]) && IsNumber(row[2]) &&
                            IsNumber(row[3])) {
                            tempVertex->InsertNextPoint(std::stod(row[1]),
                                                        std::stod(row[2]),
                                                        std::stod(row[3]));
                            tempVertex->Modified();
                        } else {
                            std::cout << "Non numerical value, debug!"
                                      << std::endl;
                            ImportIsDone();
                            return;
                        }
                    }
                    if (row[0] == "Number of Poly") {
                        tempPolyCells->Initialize();
                        tempPolyCells->Modified();
                        numPoly = std::stoi(row[1]);
                    }
                    if (row[0] == "Connectivity") {
                        int dim = 0;
                        if (IsNumber(row[1])) {
                            dim = std::stoi(row[1]);
                        } else {
                            std::cout << "Non numerical value, debug!"
                                      << std::endl;
                            ImportIsDone();
                            return;
                        }
                        vtkNew<vtkPolygon> tempCell;
                        tempCell->GetPointIds()->SetNumberOfIds(dim);
                        for (int i = 0; i < dim; i++) {
                            if (IsNumber(row[2 + i])) {
                                tempCell->GetPointIds()->SetId(
                                    i, std::stoi(row[2 + i]));
                            } else {
                                std::cout << "Non numerical value, debug!"
                                          << std::endl;
                                ImportIsDone();
                                return;
                            }
                        }
                        tempPolyCells->InsertNextCell(tempCell);
                    }
                    if (row[0] == "Next Specimen") {
                        if (tempVertex->GetNumberOfPoints() > 0 &&
                            tempPolyCells->GetNumberOfCells() > 0) {
                            if (numVert == tempVertex->GetNumberOfPoints() &&
                                numPoly == tempPolyCells->GetNumberOfCells()) {
                                nameList.push_back(name);
                                tempPoly->Initialize();
                                tempPoly->SetPoints(tempVertex);
                                tempPoly->SetPolys(tempPolyCells);
                                tempPoly->Modified();
                                m_mutex->lock();
                                if(type == ""){
                                    type = "Mesh"; // for legacy project files
                                }
                                m_parent->GetDataBase()->AddNode(name, tempPoly, type);
                                m_mutex->unlock();
                                if (tempNumFixed == numFixedLM &&
                                    tempNumCurve == numCurveLM &&
                                    tempNumSurf == numSurfaceLM) {
                                    m_mutex->lock();
                                    m_parent->GetDataBase()->InsertTypeI(name, tempFixed);
                                    m_parent->GetDataBase()->InsertCurveSliders(name,
                                                                 tempCurve);
                                    m_parent->GetDataBase()->InsertSurfaceSliders(name,
                                                                   tempSurface);
                                    m_mutex->unlock();
                                }
                                else{
                                    //std::cout<< "No landmarks to register"<<std::endl;
                                }

                                emit TreeObjectChanged(name);
                                emit DataBaseChanged(name);
                            } else {
                                std::cout << "Geometry is corrputed!"
                                          << std::endl;
                            }
                        } else {
                            std::cout << "Geometry is Empty!" << std::endl;
                        }
                        // std::cout << "Updating the database" << std::endl;
                    }
                }
            }

        } else {
            std::cout << "Could not open the .atp file\n";
        }
    } else {
        std::cout << "Template is not set!\n";
    }
    // std::cout<< "Done!"<<std::endl;
    ImportIsDone();
}

void ImportThread::Pause(bool state) { m_pauseMe = state; }

bool ImportThread::IsNumber(const std::string& s) {
    char* end = nullptr;
    double val = strtod(s.c_str(), &end);
    return end != s.c_str() && *end == '\0' && val != HUGE_VAL;
}

ImportThread::~ImportThread() {}
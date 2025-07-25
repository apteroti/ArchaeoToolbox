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

#include "../include/DataBase.h"

DataBase::DataBase() : head(nullptr), curr(nullptr), temp(nullptr) {}

DataBase::DataBase(const DataBase& db) {
    // Improved copy constructor with proper deep copy
    if (db.head) {
        head = std::make_shared<node>(*db.head);
        curr = head;
        temp = head;
        
        auto srcNode = db.head->next;
        while (srcNode) {
            curr->next = std::make_shared<node>(*srcNode);
            curr = curr->next;
            srcNode = srcNode->next;
        }
    }
}

DataBase& DataBase::operator=(const DataBase& rhs) {
    if (this != &rhs) {
        DataBase temp(rhs);
        std::swap(head, temp.head);
        std::swap(curr, temp.curr);
        std::swap(this->temp, temp.temp);
    }
    return *this;
}

void DataBase::AddNode(std::string name, vtkPolyData* poly, std::string dataType) {
    auto n = std::make_shared<node>();
    n->next = nullptr;
    n->nodeName = name;
    n->geometryType = dataType;
    if (poly) {
        n->nodePoly->DeepCopy(poly);
    }

    if (!head) {
        head = n;
        return;
    }

    curr = head;
    while (curr->next) {
        curr = curr->next;
    }
    curr->next = n;
}

void DataBase::ChangePoly(std::string name, vtkPolyData* poly) {
    if (!poly) return;

    curr = head;
    while (curr && curr->nodeName != name) {
        curr = curr->next;
    }

    if (curr) {
        curr->nodePoly->Initialize();
        curr->nodePoly->DeepCopy(poly);
    }
}

void DataBase::AddNode(std::string name, vtkStructuredGrid* grid) {
    if (!grid) return;

    auto n = std::make_shared<node>();
    n->next = nullptr;
    n->nodeName = name;
    n->nodeDICOM->DeepCopy(grid);

    if (!head) {
        head = n;
        return;
    }

    curr = head;
    while (curr->next) {
        curr = curr->next;
    }
    curr->next = n;
}

void DataBase::RenameNode(std::string name, std::string newName) {
    curr = head;
    while (curr && curr->nodeName != name) {
        curr = curr->next;
    }

    if (curr) {
        curr->nodeName = newName;
    }
}

void DataBase::DeleteNode(std::string name) {
    if (!head) return;

    if (head->nodeName == name) {
        head = head->next;
        return;
    }

    temp = head;
    curr = head->next;
    
    while (curr && curr->nodeName != name) {
        temp = curr;
        curr = curr->next;
    }

    if (curr) {
        temp->next = curr->next;
    }
}

void DataBase::InsertTypeI(std::string name, vtkPoints* type1) {
    if (!type1) return;

    curr = head;
    while (curr && curr->nodeName != name) {
        curr = curr->next;
    }

    if (curr) {
        curr->typeI->Initialize();
        curr->typeI->DeepCopy(type1);
        UpdateDataBase(name);
    }
}

void DataBase::InsertCurveSliders(std::string name, vtkPoints* sliders) {
    if (!sliders) return;

    curr = head;
    while (curr && curr->nodeName != name) {
        curr = curr->next;
    }

    if (curr) {
        curr->curveSlider->Initialize();
        curr->curveSlider->DeepCopy(sliders);
        UpdateDataBase(name);
    }
}

void DataBase::InsertSurfaceSliders(std::string name, vtkPoints* sliders) {
    if (!sliders) return;

    curr = head;
    while (curr && curr->nodeName != name) {
        curr = curr->next;
    }

    if (curr) {
        curr->surfaceSlider->Initialize();
        curr->surfaceSlider->DeepCopy(sliders);
        UpdateDataBase(name);
    }
}

void DataBase::SetLandMarks(std::string name, vtkPolyData* landmarks) {
    if (!landmarks) return;

    curr = head;
    while (curr && curr->nodeName != name) {
        curr = curr->next;
    }

    if (curr) {
        curr->totalLM->Initialize();
        curr->totalLM->DeepCopy(landmarks);
    }
}

void DataBase::SetProcDistance(std::string name, vtkDoubleArray* magnitudeArray) {
    if (!magnitudeArray) return;

    curr = head;
    while (curr && curr->nodeName != name) {
        curr = curr->next;
    }

    if (curr) {
        curr->procDistance->Initialize();
        curr->procDistance->DeepCopy(magnitudeArray);
    }
}

void DataBase::DeleteTypeI(std::string name) {
    curr = head;
    while (curr && curr->nodeName != name) {
        curr = curr->next;
    }

    if (curr) {
        curr->typeI->Initialize();
        curr->procDistance->Initialize();
        curr->nodePoly->GetPointData()->RemoveArray("ProcrustesResidualMagnitude");
        curr->nodePoly->GetPointData()->RemoveArray("ProcrustesResidualVector");
        curr->nodePoly->Modified();
    }
}

void DataBase::DeleteSliders(std::string name) {
    curr = head;
    while (curr && curr->nodeName != name) {
        curr = curr->next;
    }

    if (curr) {
        curr->curveSlider->Initialize();
        curr->surfaceSlider->Initialize();
        curr->procDistance->Initialize();
        curr->nodePoly->GetPointData()->RemoveArray("ProcrustesResidualMagnitude");
        curr->nodePoly->GetPointData()->RemoveArray("ProcrustesResidualVector");
        curr->nodePoly->Modified();
    }
}

void DataBase::DeleteAllLandmarks(std::string name) {
    curr = head;
    while (curr && curr->nodeName != name) {
        curr = curr->next;
    }

    if (curr) {
        curr->curveSlider->Initialize();
        curr->surfaceSlider->Initialize();
        curr->typeI->Initialize();
        curr->totalLM->Initialize();
        curr->procDistance->Initialize();
        curr->nodePoly->GetPointData()->RemoveArray("ProcrustesResidualMagnitude");
        curr->nodePoly->GetPointData()->RemoveArray("ProcrustesResidualVector");
        curr->nodePoly->Modified();
    }
}

void DataBase::DeleteWarpMagnitude(std::string name) {
    curr = head;
    while (curr && curr->nodeName != name) {
        curr = curr->next;
    }

    if (curr) {
        curr->procDistance->Initialize();
    }
}

vtkPoints* DataBase::GetTypeI(std::string name)const {
    auto current = head.get();
    while (current && current->nodeName != name) {
        current = current->next.get();
    }
    return current ? current->typeI : nullptr;
}

vtkPoints* DataBase::GetCurveSliders(std::string name)const {
    auto current = head.get();
    while (current && current->nodeName != name) {
        current = current->next.get();
    }
    return current ? current->curveSlider : nullptr;
}

vtkPoints* DataBase::GetSurfaceSliders(std::string name)const {
    auto current = head.get();
    while (current && current->nodeName != name) {
        current = current->next.get();
    }
    return current ? current->surfaceSlider : nullptr;
}

vtkPolyData* DataBase::GetPolyNode(std::string name)const {
    auto current = head.get();
    while (current && current->nodeName != name) {
        current = current->next.get();
    }
    return current ? current->nodePoly : nullptr;
}

vtkPolyData* DataBase::GetTotalLandmarks(std::string name)const {
    auto current = head.get();
    while (current && current->nodeName != name) {
        current = current->next.get();
    }
    return current ? current->totalLM : nullptr;
}

vtkStructuredGrid* DataBase::GetGridNode(std::string name)const {
    auto current = head.get();
    while (current && current->nodeName != name) {
        current = current->next.get();
    }
    return current ? current->nodeDICOM : nullptr;
}

vtkDoubleArray* DataBase::GetProcDistance(std::string name)const {
    auto current = head.get();
    while (current && current->nodeName != name) {
        current = current->next.get();
    }
    return current ? current->procDistance : nullptr;
}

std::string DataBase::GetGeometryType(std::string name)const {
    auto current = head.get();
    while (current && current->nodeName != name) {
        current = current->next.get();
    }
    return current ? current->geometryType : std::string();
}

int DataBase::GetNumberOfTypeI(std::string name)const {
    auto current = head.get();
    while (current && current->nodeName != name) {
        current = current->next.get();
    }
    return current ? current->typeI->GetNumberOfPoints() : 0;
}

int DataBase::GetNumberOfCurveSliders(std::string name)const {
    auto current = head.get();
    while (current && current->nodeName != name) {
        current = current->next.get();
    }
    return current ? current->curveSlider->GetNumberOfPoints() : 0;
}

int DataBase::GetNumberOfSurfaceSliders(std::string name)const {
    auto current = head.get();
    while (current && current->nodeName != name) {
        current = current->next.get();
    }
    return current ? current->surfaceSlider->GetNumberOfPoints() : 0;
}

void DataBase::PrintNode()const {
    auto current = head.get();
    while (current) {
        std::cout << current->nodeName << std::endl;
        current = current->next.get();
    }
}

std::vector<std::string> DataBase::GetNodeNames()const {
    std::vector<std::string> nameList;
    auto current = head.get();
    while (current) {
        nameList.push_back(current->nodeName);
        current = current->next.get();
    }
    return nameList;
}

int DataBase::GetNumberOfNodes()const noexcept {
    int numNodes = 0;
    auto current = head.get();
    while (current) {
        numNodes++;
        current = current->next.get();
    }
    return numNodes;
}

bool DataBase::CheckMembership(std::string name)const noexcept {
    auto current = head.get();
    while (current && current->nodeName != name) {
        current = current->next.get();
    }
    return current != nullptr;
}

void DataBase::UpdateDataBase(std::string name) {
    curr = head;
    while (curr && curr->nodeName != name) {
        curr = curr->next;
    }

    if (!curr) return;

    vtkNew<vtkPoints> tempTotalLM;
    if (curr->typeI) {
        for (int i = 0; i < curr->typeI->GetNumberOfPoints(); i++) {
            tempTotalLM->InsertNextPoint(curr->typeI->GetPoint(i));
        }
    }
    if (curr->curveSlider) {
        for (int i = 0; i < curr->curveSlider->GetNumberOfPoints(); i++) {
            tempTotalLM->InsertNextPoint(curr->curveSlider->GetPoint(i));
        }
    }
    if (curr->surfaceSlider) {
        for (int i = 0; i < curr->surfaceSlider->GetNumberOfPoints(); i++) {
            tempTotalLM->InsertNextPoint(curr->surfaceSlider->GetPoint(i));
        }
    }

    vtkNew<vtkPolyData> tempTotalLMPoly;
    tempTotalLMPoly->SetPoints(tempTotalLM);
    curr->totalLM->Initialize();
    curr->totalLM->DeepCopy(tempTotalLMPoly);
}

DataBase::~DataBase() {
    while (head) {
        auto next = head->next;
        head->next.reset();
        head = next;
    }
}
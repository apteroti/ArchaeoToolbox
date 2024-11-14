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


DataBase::DataBase(){
    head = nullptr;
    curr = nullptr;
    temp = nullptr;
}

DataBase::DataBase(const DataBase& db){
    //Copy constructor
    std::cout<< "Copy constructor" <<std::endl;
    head = db.head;
    curr = db.curr;
    temp = db.temp;
    
}

DataBase& DataBase::operator=(const DataBase& t){
    //std::cout<< "Assignment constructor" <<std::endl;
    return *this;
}

void DataBase::AddNode(string name, vtkPolyData* poly, string dataType){
    std::shared_ptr<node> n 
    = std::shared_ptr<node>(new node);
    n->next = nullptr;
    n->nodeName = name;
    n->geometryType = dataType;
    n->nodePoly->DeepCopy(poly);
    //n->nodePoly = poly;
    if(head != nullptr){
        curr = head;
        while (curr->next != nullptr)
        {
            curr = curr->next;
        }
        curr->next = n;
    }
    else{
        head = n;
    }
}

void DataBase::ChangePoly(string name, vtkPolyData* poly){
    curr = head;
    while (curr != nullptr && curr->nodeName != name)
    {
        curr = curr->next;
    }
    if(curr == nullptr){
        std::cout<<"Can't change the poly, Node Not found\n";
    }
    else{
        curr->nodePoly->Initialize();
        curr->nodePoly->DeepCopy(poly);
    }
}

void DataBase::AddNode(string name, vtkStructuredGrid* grid){
    std::shared_ptr<node> n 
    = std::shared_ptr<node>(new node);
    n->next = nullptr;
    n->nodeName = name;
    n->nodeDICOM =  grid;
    
    if(head != nullptr){
        curr = head;
        while (curr->next != nullptr)
        {
            curr = curr->next;
        }
        curr->next = n;
    }
    else{
        head = n;
    }
}

void DataBase::RenameNode(string name, string newName){
    curr = head;
    while (curr != nullptr && curr->nodeName != name)
    {
        curr = curr->next;
    }
    if(curr == nullptr){
        std::cout<<"Can't rename, Node Not found\n";
    }
    else{
        curr->nodeName = newName;
    }
}

void DataBase::DeleteNode(string name){
    std::shared_ptr<node> delPtr = nullptr;
    temp = head;
    curr = head;
    while (curr != nullptr && curr->nodeName != name)
    {
        temp = curr;
        curr = curr->next;
    }
    if(curr == nullptr){
        std::cout<<"Can't delete, Node not found\n";
    }
    else{
        if(curr == head){
            delPtr = head;
            head = head->next;
            delPtr = nullptr;
        }
        else{
            delPtr = curr;
            curr = curr->next;
            temp->next = curr;
            delPtr = nullptr;
        }
    }
}

void DataBase::InsertTypeI(string name, vtkPoints* type1){
    curr = head;
    while (curr != nullptr && curr->nodeName != name)
    {
        curr = curr->next;
    }
    if(curr == nullptr){
        std::cout<<"Can't insert T1, Node not found\n";
    }
    else{
        curr->typeI->Initialize();
        curr->typeI->DeepCopy(type1);
        //curr->typeI = type1;
        UpdateDataBase(name);
    }
}

/* void DataBase::InsertSliders(string name, vtkPoints* sliders){
    curr = head;
    while (curr != nullptr && curr->nodeName != name)
    {
        curr = curr->next;
    }
    if(curr == nullptr){
        std::cout<<"Can't insert Sliders, Node not found\n";
    }
    else{
        curr->slider->Initialize();
        curr->slider->DeepCopy(sliders);
        //curr->slider = sliders;
        UpdateDataBase(name);
    }
    
} */
void DataBase::InsertCurveSliders(string name, vtkPoints* sliders){
    curr = head;
    while (curr != nullptr && curr->nodeName != name)
    {
        curr = curr->next;
    }
    if(curr == nullptr){
        std::cout<<"Can't insert Sliders, Node not found\n";
    }
    else{
        curr->curveSlider->Initialize();
        curr->curveSlider->DeepCopy(sliders);
        UpdateDataBase(name);
    }
    
}

void DataBase::InsertSurfaceSliders(string name, vtkPoints* sliders){
    curr = head;
    while (curr != nullptr && curr->nodeName != name)
    {
        curr = curr->next;
    }
    if(curr == nullptr){
        std::cout<<"Can't insert Sliders, Node not found\n";
    }
    else{
        curr->surfaceSlider->Initialize();
        curr->surfaceSlider->DeepCopy(sliders);
        UpdateDataBase(name);
    }
    
}

void DataBase::SetLandMarks(string name, vtkPolyData* landmarks){
    curr = head;
    while (curr != nullptr && curr->nodeName != name)
    {
        curr = curr->next;
    }
    if(curr == nullptr){
        std::cout<<"Can't insert Landmarks, Node not found\n";
    }
    else{
        curr->totalLM->Initialize();
        curr->totalLM->DeepCopy(landmarks);
    }
}

void DataBase::SetProcDistance(string name, vtkDoubleArray* magnitudeArray){
    curr = head;
    while (curr != nullptr && curr->nodeName != name)
    {
        curr = curr->next;
    }
    if(curr == nullptr){
        std::cout<<"Can't insert Landmarks, Node not found\n";
    }
    else{
        curr->procDistance->Initialize();
        curr->procDistance->DeepCopy(magnitudeArray);
    }
}

void DataBase::DeleteTypeI(string name){
    curr = head;
    while (curr != nullptr && curr->nodeName != name)
    {
        curr = curr->next;
    }
    if(curr == nullptr){
        std::cout<<"Node not found\n";
    }
    else{
        curr->typeI->Initialize();
        curr->procDistance->Initialize();
        curr->nodePoly->GetPointData()->RemoveArray("ProcrustesResidualMagnitude");
        curr->nodePoly->GetPointData()->RemoveArray("ProcrustesResidualVector");
        curr->nodePoly->Modified();
    }
}

void DataBase::DeleteSliders(string name){
    curr = head;
    while (curr != nullptr && curr->nodeName != name)
    {
        curr = curr->next;
    }
    if(curr == nullptr){
        std::cout<<"Node not found\n";
    }
    else{
        //curr->slider->Initialize();
        curr->curveSlider->Initialize();
        curr->surfaceSlider->Initialize();
        curr->procDistance->Initialize();
        curr->nodePoly->GetPointData()->RemoveArray("ProcrustesResidualMagnitude");
        curr->nodePoly->GetPointData()->RemoveArray("ProcrustesResidualVector");
        curr->nodePoly->Modified();
    }
}

void::DataBase::DeleteAllLandmarks(string name){
    curr = head;
    while (curr != nullptr && curr->nodeName != name){
        curr = curr->next;
    }
    if(curr == nullptr){
        std::cout<<"Node not found\n";
    }
    else{
        //curr->slider->Initialize();
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

void::DataBase::DeleteWarpMagnitude(string name){
    curr = head;
    while (curr != nullptr && curr->nodeName != name){
        curr = curr->next;
    }
    if(curr == nullptr){
        std::cout<<"Can't delete warp array, Node not found\n";
    }
    else{
        curr->procDistance->Initialize();
    }
}

vtkPoints* DataBase::GetTypeI(string name){
    curr = head;
    while (curr != nullptr && curr->nodeName != name)
    {
        curr = curr->next;
    }
    if(curr == nullptr){
        std::cout<<"Can't Get T1, Node not found\n";
        return nullptr;
    }
    else{
        if(curr->typeI != nullptr){
            return curr->typeI;
        }
        else{
            std::cout<<"TypeI not found\n";
            return nullptr;
        }
    }
}

/* vtkPoints* DataBase::GetSliders(string name){
    curr = head;
    while (curr != nullptr && curr->nodeName != name)
    {
        curr = curr->next;
    }
    if(curr == nullptr){
        std::cout<<"Can't Get Sliders,Node not found\n";
        return nullptr;
    }
    else{
        if(curr->slider != nullptr){
            return curr->slider;
        }
        else{
            std::cout<<"Slider not found\n";
            return nullptr;
        }
    }
} */

vtkPoints* DataBase::GetCurveSliders(string name){
    curr = head;
    while (curr != nullptr && curr->nodeName != name)
    {
        curr = curr->next;
    }
    if(curr == nullptr){
        std::cout<<"Can't Get Sliders,Node not found\n";
        return nullptr;
    }
    else{
        if(curr->curveSlider != nullptr){
            return curr->curveSlider;
        }
        else{
            std::cout<<"Curve Slider not found\n";
            return nullptr;
        }
    }
}

vtkPoints* DataBase::GetSurfaceSliders(string name){
    curr = head;
    while (curr != nullptr && curr->nodeName != name)
    {
        curr = curr->next;
    }
    if(curr == nullptr){
        std::cout<<"Can't Get Sliders,Node not found\n";
        return nullptr;
    }
    else{
        if(curr->surfaceSlider != nullptr){
            return curr->surfaceSlider;
        }
        else{
            std::cout<<"Surface Slider not found\n";
            return nullptr;
        }
    }
}

vtkPolyData* DataBase::GetPolyNode(string name){
    curr = head;
    while (curr != nullptr && curr->nodeName != name){
        curr = curr->next;
    }
    if(curr == nullptr){
        std::cout<<"Can't Get The poly,Node not found\n";
        return nullptr;
    }
    else{
        return curr->nodePoly;
    }
}

vtkPolyData* DataBase::GetTotalLandmarks(string name){
    curr = head;
    while (curr != nullptr && curr->nodeName != name){
        curr = curr->next;
    }
    if(curr == nullptr){
        std::cout<<"Can't Get the Total Landmarks,Node not found\n";
        return nullptr;
    }
    else{
        return curr->totalLM;
    }
}

vtkStructuredGrid* DataBase::GetGridNode(string name){
    curr = head;
    while (curr != nullptr && curr->nodeName != name)
    {
        curr = curr->next;
    }
    if(curr == nullptr){
        std::cout<<"Can't Get The Grid, Node not found\n";
        return nullptr;
    }
    else{
        return curr->nodeDICOM;
    }
}

vtkDoubleArray* DataBase::GetProcDistance(string name){
    curr = head;
    while (curr != nullptr && curr->nodeName != name)
    {
        curr = curr->next;
    }
    if(curr == nullptr){
        std::cout<<"Can't Get The Warp Magnitude, Node not found\n";
        return nullptr;
    }
    else{
        return curr->procDistance;
    }
}

string DataBase::GetGeometryType(string name){
    curr = head;
    while (curr != nullptr && curr->nodeName != name)
    {
        curr = curr->next;
    }
    if(curr == nullptr){
        std::cout<<"Can't Get T1, Node not found\n";
        return nullptr;
    }
    else{
        if(curr->typeI != nullptr){
            return curr->geometryType;
        }
        else{
            std::cout<<"TypeI not found\n";
            return nullptr;
        }
    }
}

void DataBase::PrintNode(){
    curr = head;
    while (curr != nullptr)
    {
        std::cout << curr->nodeName << std::endl;
        curr = curr->next;
    }
}

std::vector<std::string> DataBase::GetNodeNames(){
    std::vector<std::string> nameList;
    curr = head;
    while (curr != nullptr)
    {
        nameList.push_back(curr->nodeName);
        curr = curr->next;
    }
    return nameList;
}

bool DataBase::CheckMembership(string name){
    curr = head;
    while (curr != nullptr && curr->nodeName != name)
    {
        curr = curr->next;
    }
    if(curr == nullptr){
        return 0;
    }
    else{
        return 1;
    }
}

void DataBase::UpdateDataBase(string name){
    curr = head;
    while (curr != nullptr && curr->nodeName != name)
    {
        curr = curr->next;
    }
    if(curr == nullptr){
        std::cout<<"Can't Update Landmarks, Node not found\n";
    }
    else{
        vtkNew<vtkPoints> tempTotalLM;
        vtkPoints* tempTypeI = curr->typeI;
        vtkPoints* tempCurveSliders = curr->curveSlider;
        vtkPoints* tempSurfaceSliders = curr->surfaceSlider;

        for(int i = 0; i < tempTypeI->GetNumberOfPoints(); i++){
            tempTotalLM->InsertNextPoint(tempTypeI->GetPoint(i));
        }
        for(int i = 0; i < tempCurveSliders->GetNumberOfPoints(); i++){
            tempTotalLM->InsertNextPoint(tempCurveSliders->GetPoint(i));
        }
        for(int i = 0; i < tempSurfaceSliders->GetNumberOfPoints(); i++){
            tempTotalLM->InsertNextPoint(tempSurfaceSliders->GetPoint(i));
        }
        vtkNew<vtkPolyData> tempTotalLMPoly;
        tempTotalLMPoly->SetPoints(tempTotalLM);
        tempTotalLMPoly->Modified();
        curr->totalLM->Initialize();
        curr->totalLM->DeepCopy(tempTotalLMPoly);
    }
}

DataBase::~DataBase(){
}
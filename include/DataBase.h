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

#ifndef DATABASE_H
#define DATABASE_H
#include <map>
//#include <boost/variant.hpp>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkStructuredGrid.h>
#include <vtkSmartPointer.h>
#include <vtkDoubleArray.h>


using std::string;


class DataBase
{
private:
    struct node
    {
        string nodeName;
        string geometryType;
        vtkSmartPointer<vtkPolyData> nodePoly 
        = vtkSmartPointer<vtkPolyData>::New();
        vtkSmartPointer<vtkStructuredGrid> nodeDICOM 
        = vtkSmartPointer<vtkStructuredGrid>::New();
        vtkSmartPointer<vtkPoints> typeI 
        = vtkSmartPointer<vtkPoints>::New();
        /* vtkSmartPointer<vtkPoints> slider 
        = vtkSmartPointer<vtkPoints>::New(); */
        vtkSmartPointer<vtkPoints> curveSlider 
        = vtkSmartPointer<vtkPoints>::New();
        vtkSmartPointer<vtkPoints> surfaceSlider 
        = vtkSmartPointer<vtkPoints>::New();
        vtkSmartPointer<vtkPolyData> totalLM 
        = vtkSmartPointer<vtkPolyData>::New();
        vtkSmartPointer<vtkDoubleArray> procDistance 
        = vtkSmartPointer<vtkDoubleArray>::New();

        std::shared_ptr<node> next;
    };
    std::shared_ptr<node> head;
    std::shared_ptr<node> curr;
    std::shared_ptr<node> temp;
    void UpdateDataBase(string name);

public:
    DataBase();
    DataBase(const DataBase& db);
    DataBase& operator=(const DataBase& db);
    void AddNode(string name, vtkPolyData* poly, string dataType);
    void ChangePoly(string name, vtkPolyData* poly);
    void AddNode(string name, vtkStructuredGrid* poly);
    void InsertTypeI(string name, vtkPoints* type1);
    //void InsertSliders(string name, vtkPoints* sliders);
    void InsertCurveSliders(string name, vtkPoints* sliders);
    void InsertSurfaceSliders(string name, vtkPoints* sliders);
    void SetLandMarks(string name, vtkPolyData* landmarks);
    void SetProcDistance(string name, vtkDoubleArray* magnitudeArray);
    void DeleteTypeI(string name);
    void DeleteSliders(string name);
    void DeleteAllLandmarks(string name);
    void DeleteWarpMagnitude(string name);
    vtkPoints* GetTypeI(string name);
    //vtkPoints* GetSliders(string name);
    vtkPoints* GetCurveSliders(string name);
    vtkPoints* GetSurfaceSliders(string name);
    vtkPolyData* GetPolyNode(string name);
    vtkPolyData* GetTotalLandmarks(string name);
    vtkStructuredGrid* GetGridNode(string name);
    vtkDoubleArray* GetProcDistance(string name);
    string GetGeometryType(string name);
    void DeleteNode(string name);
    void RenameNode(string name, string newName);
    void PrintNode();
    std::vector<std::string> GetNodeNames();
    bool CheckMembership(string name);
    
    ~DataBase();
};





#endif
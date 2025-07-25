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
#include <memory>
#include <string>
#include <vector>

#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkStructuredGrid.h>
#include <vtkSmartPointer.h>
#include <vtkDoubleArray.h>


class DataBase
{
private:
    struct node
    {
        std::string nodeName;
        std::string geometryType;
        vtkSmartPointer<vtkPolyData> nodePoly = vtkSmartPointer<vtkPolyData>::New();
        vtkSmartPointer<vtkStructuredGrid> nodeDICOM = vtkSmartPointer<vtkStructuredGrid>::New();
        vtkSmartPointer<vtkPoints> typeI = vtkSmartPointer<vtkPoints>::New();
        vtkSmartPointer<vtkPoints> curveSlider = vtkSmartPointer<vtkPoints>::New();
        vtkSmartPointer<vtkPoints> surfaceSlider = vtkSmartPointer<vtkPoints>::New();
        vtkSmartPointer<vtkPolyData> totalLM = vtkSmartPointer<vtkPolyData>::New();
        vtkSmartPointer<vtkDoubleArray> procDistance = vtkSmartPointer<vtkDoubleArray>::New();

        std::shared_ptr<node> next;
    };
    
    std::shared_ptr<node> head;
    std::shared_ptr<node> curr;
    std::shared_ptr<node> temp;
    
    void UpdateDataBase(std::string name);

public:
    DataBase();
    DataBase(const DataBase& db);
    DataBase& operator=(const DataBase& db);
    ~DataBase();

    // Node management
    void AddNode(std::string name, vtkPolyData* poly, std::string dataType);
    void AddNode(std::string name, vtkStructuredGrid* grid);
    void ChangePoly(std::string name, vtkPolyData* poly);
    void DeleteNode(std::string name);
    void RenameNode(std::string name, std::string newName);
    
    // Landmark management
    void InsertTypeI(std::string name, vtkPoints* type1);
    void InsertCurveSliders(std::string name, vtkPoints* sliders);
    void InsertSurfaceSliders(std::string name, vtkPoints* sliders);
    void SetLandMarks(std::string name, vtkPolyData* landmarks);
    void SetProcDistance(std::string name, vtkDoubleArray* magnitudeArray);
    
    // Deletion methods
    void DeleteTypeI(std::string name);
    void DeleteSliders(std::string name);
    void DeleteAllLandmarks(std::string name);
    void DeleteWarpMagnitude(std::string name);
    
    // Getters
    vtkPoints* GetTypeI(std::string name)const;
    vtkPoints* GetCurveSliders(std::string name)const;
    vtkPoints* GetSurfaceSliders(std::string name)const;
    vtkPolyData* GetPolyNode(std::string name)const;
    vtkPolyData* GetTotalLandmarks(std::string name)const;
    vtkStructuredGrid* GetGridNode(std::string name)const;
    vtkDoubleArray* GetProcDistance(std::string name) const;
    std::string GetGeometryType(std::string name) const;
    
    // Utility functions
    std::vector<std::string> GetNodeNames() const;
    int GetNumberOfNodes() const noexcept;
    bool CheckMembership(std::string name) const noexcept;
    void PrintNode() const;
};
#endif
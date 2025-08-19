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

#ifndef CUTTINGTHREAD_H
#define CUTTINGTHREAD_H

#include <omp.h>
#include <vtkAppendPolyData.h>
#include <vtkCell.h>
#include <vtkCellArray.h>
#include <vtkCleanPolyData.h>
#include <vtkConnectivityFilter.h>
#include <vtkDataArray.h>
#include <vtkGeometryFilter.h>
#include <vtkIdList.h>
#include <vtkMath.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPointLocator.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkThreshold.h>
#include <vtkDijkstraGraphGeodesicPath.h>
#include <vtkStaticPointLocator.h>

#include <QThread>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <queue>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class CuttingThread : public QThread {
    Q_OBJECT
   public:
    CuttingThread(vtkPolyData* inputMesh, vtkPoints* curvePoints,
                  vtkPolyData* outputCutMesh, QObject* parent = nullptr);

    ~CuttingThread();

   signals:
    void finished();

   public:
    void run() override;

   private:
    vtkSmartPointer<vtkPolyData> m_inputMesh;
    vtkSmartPointer<vtkPoints> m_curvePoints;
    vtkSmartPointer<vtkPolyData> m_outputCutMesh;

    // Internal helpers
    static void CutMeshWithCurve(vtkPolyData* inputMesh, vtkPoints* curvePoints,
                                 vtkPolyData* outputCutMesh);

    static void GetCutterCurve(vtkPolyData* Poly, vtkPoints* curvePts,
                               vtkIdList* outCurveIds);

    struct AStarNode {
        vtkIdType vertexId;
        double fScore;
        bool operator>(const AStarNode& other) const {
            return fScore > other.fScore;
        }
    };

    static void AStarEdgeSearch(vtkPolyData* mesh, vtkPolyData* closedCurve,
                                vtkIdList* edgePointIds);
    static void DijkstraEdgeSearch(vtkPolyData* mesh, vtkPolyData* closedCurve,
                                   vtkIdList* edgePointIds);
};

#endif  // CUTTINGTHREAD_H

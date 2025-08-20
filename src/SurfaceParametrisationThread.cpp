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

#include "../include/SurfaceParametrisationThread.h"

SurfaceParametrisationThread::SurfaceParametrisationThread(
    vtkPolyData* maskMesh, vtkPoints* inputPts, vtkPolyData* outputMesh,
    int uRes, int vRes, QObject* parent)
    : QThread(parent),
      m_mesh(maskMesh),
      m_inputPts(inputPts),
      m_output(outputMesh),
      m_uRes(uRes),
      m_vRes(vRes) {}

void SurfaceParametrisationThread::run() {
    m_output->Initialize();

    // 1. Create initial plane from first 3 points of the curve
    if (m_inputPts->GetNumberOfPoints() < 3) {
        return;
    }

    // 2. Build RectSlimMapper and sample the grid
    RectSlimMapper slim(m_mesh, m_inputPts, m_output);
    slim.Sample(m_uRes, m_vRes);
    emit finished();
}

SurfaceParametrisationThread::~SurfaceParametrisationThread() {
    wait();
}
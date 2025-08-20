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

#ifndef SURFACE_PARAMETRISATION_THREAD
#define SURFACE_PARAMETRISATION_THREAD

#include <vtkPoints.h>
#include <vtkPolyData.h>

#include <QThread>

#include "RectSlimMapper.h"

class SurfaceParametrisationThread : public QThread {
    Q_OBJECT
   private:
    vtkPolyData* m_mesh;
    vtkPolyData* m_output;
    vtkPoints* m_inputPts;
    int m_uRes = 0;
    int m_vRes = 0;

   public:
    SurfaceParametrisationThread(vtkPolyData* maskMesh, vtkPoints* inputPts,
                                 vtkPolyData* outputMesh, int uRes, int vRes,
                                 QObject* parent = nullptr);
    ~SurfaceParametrisationThread();
    void run() override;

   signals:
    void finished();
};



#endif
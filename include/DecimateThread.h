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

#ifndef DECIMATETHREAD_H
#define DECIMATETHREAD_H

#include <vcg/complex/complex.h>
#include <vcg/space/point3.h>
#include <vcg/space/texcoord2.h>
#include <vcg/math/quadric.h>
#include <vcg/complex/algorithms/clean.h>
#include <vcg/container/simple_temporary_data.h>
#include <vcg/complex/algorithms/local_optimization.h>
#include <vcg/complex/algorithms/local_optimization/tri_edge_collapse_quadric.h>

#include <QtWidgets/QDialog>
#include <QtGui/QPainter>
#include <QtCore/QTimer>
#include <QtWidgets/QVBoxLayout>
#include <QtCore/QThread>
#include <QtWidgets/QLabel>
#include <QtCore/QDebug>

#include "VCGDefinitions.h"


class DecimateWorker : public QObject {
    Q_OBJECT
private: 
void Decimate(MyMesh& m, float reductionRatio){
  // Validate input
    int startFaces = m.face.size();
    if (reductionRatio <= 0 || reductionRatio >= 1.0f) {
        qWarning() << "Invalid reduction ratio. Must be between 0 and 1.";
        return;
    }
    if (startFaces == 0) {
        qWarning() << "Mesh has no faces to decimate.";
        return;
    }
    
    // Update mesh properties
    vcg::tri::UpdateBounding<MyMesh>::Box(m);
    vcg::tri::UpdateNormal<MyMesh>::PerVertexNormalized(m);
    
    // Prepare mesh
    vcg::tri::Clean<MyMesh>::RemoveDegenerateFace(m);
    vcg::tri::Clean<MyMesh>::RemoveDuplicateFace(m);
    vcg::tri::Clean<MyMesh>::RemoveDuplicateVertex(m);
    vcg::tri::Clean<MyMesh>::RemoveUnreferencedVertex(m);
    vcg::tri::UpdateTopology<MyMesh>::FaceFace(m);
    vcg::tri::Clean<MyMesh>::RemoveNonManifoldFace(m);
    vcg::tri::UpdateTopology<MyMesh>::FaceFace(m);
    vcg::tri::Clean<MyMesh>::RemoveNonManifoldVertex(m);
    vcg::tri::UpdateTopology<MyMesh>::FaceFace(m);
    vcg::tri::UpdateBounding<MyMesh>::Box (m);
    vcg::tri::UpdateTopology<MyMesh>::VertexFace(m);
    vcg::tri::UpdateFlags<MyMesh>::VertexBorderFromNone(m);

    // Set up parameters
    vcg::tri::TriEdgeCollapseQuadricParameter pp;
    pp.QualityThr = 0.3;         // Quality threshold
    pp.PreserveBoundary = true;  // Preserve mesh boundaries
    pp.PreserveTopology = true;
    pp.OptimalPlacement = true;
    pp.QualityCheck = true;
    pp.NormalCheck = true;

    if (pp.NormalCheck) {
        pp.NormalThrRad = M_PI/4.0;  // 45 degrees
    }

    // Set up quadric temporary data
    vcg::math::Quadric<double> QZero;
    QZero.SetZero();
    vcg::tri::QuadricTemp TD(m.vert, QZero);
    vcg::tri::QHelper::TDp() = &TD;
    
    // Create optimizer
    vcg::LocalOptimization<MyMesh> deciSession(m, &pp);

    // Calculate target faces
    int targetFaces = static_cast<int>(m.fn * (1.0f - reductionRatio));
    if (targetFaces < 4) targetFaces = 4;

    // Get initial deleted face count
    int facesToDelete = startFaces - targetFaces;

    // Initialize
    deciSession.Init<vcg::tri::MyTriEdgeCollapse>();
    
    // Set target faces
    deciSession.SetTargetSimplices(targetFaces);
    
    deciSession.DoOptimization();
    // Finalize
    deciSession.Finalize<vcg::tri::MyTriEdgeCollapse>();

    // Clean up temporary data
    vcg::tri::QHelper::TDp() = nullptr;

    vcg::tri::Clean<MyMesh>::RemoveDegenerateFace(m);
    vcg::tri::Clean<MyMesh>::RemoveDuplicateFace(m);
    vcg::tri::Clean<MyMesh>::RemoveDuplicateVertex(m);
    vcg::tri::Clean<MyMesh>::RemoveUnreferencedVertex(m);
    vcg::tri::UpdateTopology<MyMesh>::FaceFace(m);
    vcg::tri::Clean<MyMesh>::RemoveNonManifoldFace(m);
    vcg::tri::UpdateTopology<MyMesh>::FaceFace(m);
    vcg::tri::Clean<MyMesh>::RemoveNonManifoldVertex(m);
    vcg::tri::UpdateTopology<MyMesh>::FaceFace(m);
    vcg::tri::UpdateBounding<MyMesh>::Box (m);
    vcg::tri::UpdateTopology<MyMesh>::VertexFace(m);
    vcg::tri::UpdateFlags<MyMesh>::VertexBorderFromNone(m);

    // Final mesh updates
    vcg::tri::Allocator<MyMesh>::CompactEveryVector(m);
    vcg::tri::UpdateNormal<MyMesh>::PerVertexNormalized(m);
    vcg::tri::UpdateBounding<MyMesh>::Box(m);
}
public:
    MyMesh* mesh;
    float ratio;
public slots:
    void run() {
        Decimate(*mesh, ratio);
        emit finished();
    }
signals:
    void finished();
};


#endif
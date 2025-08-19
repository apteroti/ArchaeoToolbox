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

#ifndef VCGDEFINITIONS_H
#define VCGDEFINITIONS_H

#include <vcg/complex/complex.h>
#include <vcg/space/point3.h>
#include <vcg/space/texcoord2.h>
#include <vcg/math/quadric.h>
#include <vcg/complex/algorithms/clean.h>
#include <vcg/container/simple_temporary_data.h>
#include <vcg/complex/algorithms/local_optimization.h>
#include <vcg/complex/algorithms/local_optimization/tri_edge_collapse_quadric.h>

// Forward declarations
class MyVertex;
class MyFace;
class MyEdge;

// Define the types
struct MyUsedTypes : public vcg::UsedTypes<
    vcg::Use<MyVertex>::AsVertexType,
    vcg::Use<MyFace>::AsFaceType,
    vcg::Use<MyEdge>::AsEdgeType
> {};

// Vertex definition
class MyVertex : public vcg::Vertex<
    MyUsedTypes,
    vcg::vertex::Coord3f,        // Position (3D float)
    vcg::vertex::Normal3f,        // Normal (3D float)
    vcg::vertex::BitFlags,        // Flags storage
    vcg::vertex::TexCoord2f,      // Texture coordinates
    vcg::vertex::VFAdj,           // Vertex-Face adjacency
    vcg::vertex::Qualityf,        // Per-vertex quality
    vcg::vertex::Mark,            // Per-vertex mark
    vcg::vertex::Color4b          // Per-vertex color
> {};

// Face definition
class MyFace : public vcg::Face<
    MyUsedTypes,
    vcg::face::VertexRef,         // Vertex references
    vcg::face::Normal3f,          // Face normal
    vcg::face::BitFlags,          // Flags storage
    vcg::face::VFAdj,             // Face-Vertex adjacency
    vcg::face::FFAdj,             // Face-Face adjacency
    vcg::face::Mark,              // Per-face mark
    vcg::face::Color4b,           // Per-face color
    vcg::face::Qualityf           // Per-face quality
> {};

// Edge definition (minimal requirements)
class MyEdge : public vcg::Edge<MyUsedTypes> {};

// Mesh definition
class MyMesh : public vcg::tri::TriMesh<
    std::vector<MyVertex>,
    std::vector<MyFace>,
    std::vector<MyEdge>
> {};

// Quadric helper class
namespace vcg {
namespace tri {

typedef SimpleTempData<MyMesh::VertContainer, math::Quadric<double>> QuadricTemp;

class QHelper {
public:
  QHelper() {}
  static void Init() {}
  static math::Quadric<double>& Qd(MyVertex& v) { return TD()[v]; }
  static math::Quadric<double>& Qd(MyVertex* v) { return TD()[*v]; }
  static MyVertex::ScalarType W(MyVertex*) { return 1.0; }
  static MyVertex::ScalarType W(MyVertex&) { return 1.0; }
  static void Merge(MyVertex&, MyVertex const&) {}
  static QuadricTemp*& TDp() { static QuadricTemp* td; return td; }
  static QuadricTemp& TD() { return *TDp(); }
};

typedef BasicVertexPair<MyVertex> VertexPair;

class MyTriEdgeCollapse : public TriEdgeCollapseQuadric<MyMesh, VertexPair, MyTriEdgeCollapse, QHelper> {
public:
  typedef TriEdgeCollapseQuadric<MyMesh, VertexPair, MyTriEdgeCollapse, QHelper> TECQ;
  inline MyTriEdgeCollapse(const VertexPair& p, int i, BaseParameterClass* pp) : TECQ(p, i, pp) {}
};

} // namespace tri
} // namespace vcg

#endif
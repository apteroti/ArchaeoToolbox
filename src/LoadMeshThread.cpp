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

#include "LoadMeshThread.h"

LoadMeshThread::LoadMeshThread(const QString& fileName, vtkPolyData* outputPoly, QObject* parent)
    : QThread(parent), m_fileName(fileName), m_outputPoly(outputPoly) {}

void LoadMeshThread::run() {
    if (!(m_fileName.endsWith(".obj", Qt::CaseInsensitive) ||
          m_fileName.endsWith(".ply", Qt::CaseInsensitive))) {
        emit failed("Unsupported file format");
        return;
    }

    vtkNew<vtkCleanPolyData> cleanFilter;
    cleanFilter->PointMergingOn();
    cleanFilter->SetTolerance(0.0001);
    cleanFilter->ConvertLinesToPointsOn();
    cleanFilter->ConvertPolysToLinesOn();
    cleanFilter->ConvertStripsToPolysOn();

    if (m_fileName.endsWith(".obj", Qt::CaseInsensitive)) {
        vtkNew<vtkOBJReader> objReader;
        objReader->SetFileName(m_fileName.toLocal8Bit().data());
        objReader->Update();
        cleanFilter->SetInputData(objReader->GetOutput());
    } else if (m_fileName.endsWith(".ply", Qt::CaseInsensitive)) {
        vtkNew<vtkPLYReader> plyReader;
        plyReader->SetFileName(m_fileName.toLocal8Bit().data());
        plyReader->Update();
        cleanFilter->SetInputData(plyReader->GetOutput());
    }

    cleanFilter->Update();

    if (cleanFilter->GetOutput()->GetNumberOfPoints() == 0) {
        emit failed("File contained no points");
        return;
    }

    vtkNew<vtkTriangleFilter> triangleFilter;
    triangleFilter->SetInputConnection(cleanFilter->GetOutputPort());
    triangleFilter->Update();

    MyMesh vcgMesh;
    ConvertVTKToVCG(triangleFilter->GetOutput(), vcgMesh);

    // VCG cleaning pipeline
    vcg::tri::Clean<MyMesh>::RemoveDegenerateFace(vcgMesh);
    vcg::tri::Clean<MyMesh>::RemoveDuplicateFace(vcgMesh);
    vcg::tri::Clean<MyMesh>::RemoveDuplicateVertex(vcgMesh);
    vcg::tri::Clean<MyMesh>::RemoveUnreferencedVertex(vcgMesh);
    vcg::tri::UpdateTopology<MyMesh>::FaceFace(vcgMesh);
    vcg::tri::Clean<MyMesh>::RemoveNonManifoldFace(vcgMesh);
    vcg::tri::UpdateTopology<MyMesh>::FaceFace(vcgMesh);
    vcg::tri::Clean<MyMesh>::RemoveNonManifoldVertex(vcgMesh);
    vcg::tri::UpdateTopology<MyMesh>::FaceFace(vcgMesh);
    vcg::tri::UpdateBounding<MyMesh>::Box(vcgMesh);
    vcg::tri::UpdateTopology<MyMesh>::VertexFace(vcgMesh);
    vcg::tri::UpdateFlags<MyMesh>::VertexBorderFromNone(vcgMesh);

    m_outputPoly->Initialize();
    ConvertVCGToVTK(vcgMesh, m_outputPoly);
    emit finished();
}


void LoadMeshThread::ConvertVTKToVCG(vtkPolyData* polyData, MyMesh& vcgMesh) {
    // Clear existing mesh
    vcgMesh.Clear();

    // Validate input
    if (!polyData || !polyData->GetPoints()) {
        std::cerr << "Invalid VTK polydata input"<<std::endl;
        return;
    }

    vtkPoints* points = polyData->GetPoints();
    vtkIdType numPoints = points->GetNumberOfPoints();
    
    // Add vertices
    for (vtkIdType i = 0; i < numPoints; ++i) {
        double p[3];
        points->GetPoint(i, p);
        vcg::tri::Allocator<MyMesh>::AddVertex(vcgMesh, vcg::Point3f(p[0], p[1], p[2]));
    }

    // Add faces 
    vtkCellArray* polys = polyData->GetPolys();
    polys->InitTraversal();
    vtkIdType npts, *pts;
    
    while (polys->GetNextCell(npts, pts)) {
        if (npts != 3) continue;  // Only triangles
        
        // Pass vertex pointers to AddFace
        vcg::tri::Allocator<MyMesh>::AddFace(
            vcgMesh,
            &vcgMesh.vert[pts[0]],
            &vcgMesh.vert[pts[1]],
            &vcgMesh.vert[pts[2]]
        );
    }

    // Transfer normals if available
    vtkFloatArray* normals =
        vtkFloatArray::SafeDownCast(polyData->GetPointData()->GetNormals());
    if (normals && normals->GetNumberOfTuples() == numPoints) {
        for (vtkIdType i = 0; i < numPoints; ++i) {
            vcgMesh.vert[i].N() =
                vcg::Point3f(normals->GetTypedComponent(i, 0),
                             normals->GetTypedComponent(i, 1),
                             normals->GetTypedComponent(i, 2));
        }
    }

    // Transfer colors if available
    vtkUnsignedCharArray* colors = vtkUnsignedCharArray::SafeDownCast(
        polyData->GetPointData()->GetScalars());
    if (colors && colors->GetNumberOfComponents() == 3 &&
        colors->GetNumberOfTuples() == numPoints) {
        for (vtkIdType i = 0; i < numPoints; ++i) {
            unsigned char c[3];
            colors->GetTypedTuple(i, c);
            vcgMesh.vert[i].C() = vcg::Color4b(c[0], c[1], c[2], 255);
        }
    }

    // Transfer texture coordinates if available
    vtkFloatArray* texCoords =
        vtkFloatArray::SafeDownCast(polyData->GetPointData()->GetTCoords());
    if (texCoords && texCoords->GetNumberOfComponents() >= 2 &&
        texCoords->GetNumberOfTuples() == numPoints) {
        for (vtkIdType i = 0; i < numPoints; ++i) {
            float uv[2];
            texCoords->GetTypedTuple(i, uv);
            vcgMesh.vert[i].T() = vcg::TexCoord2f(uv[0], uv[1]);
        }
    }

    // CRITICAL: Update topology and flags
    vcg::tri::UpdateTopology<MyMesh>::VertexFace(vcgMesh);
    vcg::tri::UpdateTopology<MyMesh>::FaceFace(vcgMesh);
    vcg::tri::UpdateFlags<MyMesh>::VertexBorderFromNone(vcgMesh);
    
    // Update normals and bounding box
    vcg::tri::UpdateNormal<MyMesh>::PerVertexNormalized(vcgMesh);
    vcg::tri::UpdateBounding<MyMesh>::Box(vcgMesh);
}

void LoadMeshThread::ConvertVCGToVTK(MyMesh& vcgMesh, vtkPolyData* polyData) {
    if (!polyData) return;

    // Ensure compact vertex/face arrays
    vcg::tri::Allocator<MyMesh>::CompactVertexVector(vcgMesh);
    vcg::tri::Allocator<MyMesh>::CompactFaceVector(vcgMesh);

    auto points = vtkSmartPointer<vtkPoints>::New();
    auto polys  = vtkSmartPointer<vtkCellArray>::New();

    // Add vertices
    points->SetNumberOfPoints(vcgMesh.vert.size());
    for (size_t i = 0; i < vcgMesh.vert.size(); ++i) {
        auto& v = vcgMesh.vert[i];
        points->SetPoint(static_cast<vtkIdType>(i), v.P()[0], v.P()[1], v.P()[2]);
    }

    // Add faces
    for (size_t i = 0; i < vcgMesh.face.size(); ++i) {
        auto& f = vcgMesh.face[i];
        vtkIdType pts[3] = {
            static_cast<vtkIdType>(vcg::tri::Index(vcgMesh, f.V(0))),
            static_cast<vtkIdType>(vcg::tri::Index(vcgMesh, f.V(1))),
            static_cast<vtkIdType>(vcg::tri::Index(vcgMesh, f.V(2)))};

        if (pts[0] < 0 || pts[1] < 0 || pts[2] < 0) continue; // skip invalid
        polys->InsertNextCell(3, pts);
    }

    polyData->SetPoints(points);
    polyData->SetPolys(polys);

    // Create GroupIds array (per-cell)
    auto groupIds = vtkSmartPointer<vtkFloatArray>::New();
    groupIds->SetNumberOfComponents(1);
    groupIds->SetName("GroupIds");
    groupIds->SetNumberOfTuples(polyData->GetNumberOfCells());
    for (vtkIdType i = 0; i < polyData->GetNumberOfCells(); ++i) {
        groupIds->SetValue(i, 0.0f);
    }
    polyData->GetCellData()->AddArray(groupIds);
    // Normals
    if (!vcgMesh.vert.empty() && vcgMesh.vert[0].IsNormalEnabled()) {
        auto normals = vtkSmartPointer<vtkFloatArray>::New();
        normals->SetNumberOfComponents(3);
        normals->SetName("Normals");

        for (size_t i = 0; i < vcgMesh.vert.size(); ++i) {
            auto& n = vcgMesh.vert[i].N();
            normals->InsertNextTuple3(n[0], n[1], n[2]);
        }
        polyData->GetPointData()->SetNormals(normals);
    }

    // Colors
    if (!vcgMesh.vert.empty() && vcgMesh.vert[0].IsColorEnabled()) {
        auto colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
        colors->SetNumberOfComponents(3);
        colors->SetName("Colors");

        for (size_t i = 0; i < vcgMesh.vert.size(); ++i) {
            auto& c = vcgMesh.vert[i].C();
            unsigned char rgb[3] = {c[0], c[1], c[2]};
            colors->InsertNextTypedTuple(rgb);
        }
        polyData->GetPointData()->SetScalars(colors);
    }

    // Texture coordinates
    if (!vcgMesh.vert.empty() && vcgMesh.vert[0].IsTexCoordEnabled()) {
        auto texCoords = vtkSmartPointer<vtkFloatArray>::New();
        texCoords->SetNumberOfComponents(2);
        texCoords->SetName("TexCoords");

        for (size_t i = 0; i < vcgMesh.vert.size(); ++i) {
            auto& t = vcgMesh.vert[i].T();
            texCoords->InsertNextTuple2(t.U(), t.V());
        }
        polyData->GetPointData()->SetTCoords(texCoords);
    }
   polyData->Modified();
}

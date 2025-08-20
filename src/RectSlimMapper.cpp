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
#include "include/RectSlimMapper.h"

// -------------------- Constructor --------------------
RectSlimMapper::RectSlimMapper(vtkPolyData* maskMesh, vtkPoints* inputPts,
                               vtkPolyData* outputMesh)
    : m_curvePts(inputPts), m_output(outputMesh) {
    if (maskMesh->GetNumberOfPoints() == 0 ||
        m_curvePts->GetNumberOfPoints() == 0 || !m_output) {
        std::cerr << "RectSlimMapper: null input(s)" << std::endl;
        return;
    } else {
        m_flatMask = vtkSmartPointer<vtkPolyData>::New();
        m_mask = vtkSmartPointer<vtkPolyData>::New();
        m_mask->DeepCopy(maskMesh);
        /**/
        // Uniformly Remesh the mask
        MyMesh vcgMesh;
        ConvertVTKToVCG(m_mask, vcgMesh);
        float baseSize = vcgMesh.bbox.Diag() * 0.01f;  // 1% of diagonal
        float cellSize = baseSize * 20;
        vcg::tri::IsotropicRemeshing<MyMesh>::Params params;
        params.cleanFlag = true;
        params.smoothFlag = true;
        params.maxSurfDist = 0.0;
        params.adapt = false;
        params.SetTargetLen(cellSize);
        params.iter = 10;
        vcg::tri::IsotropicRemeshing<MyMesh>::Do(vcgMesh, params);
        m_mask->Initialize();
        ConvertVCGToVTK(vcgMesh, m_mask);
        m_mask->Modified();
        ComputeHarmonicToPlane(m_mask, m_flatMask);
        vtkIdType nPoints = m_flatMask->GetNumberOfPoints();
        m_UV.assign((size_t)nPoints, Vector2d::Zero());
        InitialUV(m_flatMask, m_UV);
    }
}

void RectSlimMapper::ComputeHarmonicToPlane(vtkPolyData* mesh,
                                            vtkPolyData* outFlatMesh) {
    // 1) Extract ordered boundary loop
    std::vector<vtkIdType> boundaryLoop;
    ExtractSingleBoundaryLoop(m_mask, m_curvePts, boundaryLoop);
    if (boundaryLoop.empty()) {
        std::cerr << "No boundary found for mask mesh." << std::endl;
        return;
    }

    const vtkIdType nPts = mesh->GetNumberOfPoints();
    vtkNew<vtkPoints> flatPts;
    flatPts->SetNumberOfPoints(nPts);

    // 2) Mark boundary vertices
    std::vector<bool> isBoundary(nPts, false);
    for (auto id : boundaryLoop) {
        isBoundary[id] = true;
    }

    // 3) Map boundary to square perimeter (arc-length parameterization)
    std::vector<double> cumLength(boundaryLoop.size() + 1, 0.0);
    double totalLength = 0.0;

    // Compute cumulative arc length
    for (size_t i = 0; i < boundaryLoop.size(); i++) {
        size_t next_i = (i + 1) % boundaryLoop.size();

        double p0[3], p1[3];
        mesh->GetPoint(boundaryLoop[i], p0);
        mesh->GetPoint(boundaryLoop[next_i], p1);

        double dx = p1[0] - p0[0];
        double dy = p1[1] - p0[1];
        double dz = p1[2] - p0[2];
        double d = std::sqrt(dx * dx + dy * dy + dz * dz);
        totalLength += d;
        cumLength[i + 1] = totalLength;
    }

    // Map boundary points to square perimeter
    for (size_t i = 0; i < boundaryLoop.size(); i++) {
        double t = cumLength[i] / totalLength;
        double u = 0.0, v = 0.0;

        if (t < 0.25) {
            u = 4 * t;
            v = 0.0;
        } else if (t < 0.5) {
            u = 1.0;
            v = 4 * (t - 0.25);
        } else if (t < 0.75) {
            u = 1.0 - 4 * (t - 0.5);
            v = 1.0;
        } else {
            u = 0.0;
            v = 1.0 - 4 * (t - 0.75);
        }

        flatPts->SetPoint(boundaryLoop[i], u, v, 0.0);
    }

    // Initialize interior points to centroid (0.5, 0.5)
    for (vtkIdType i = 0; i < nPts; i++) {
        if (!isBoundary[i]) {
            flatPts->SetPoint(i, 0.5, 0.5, 0.0);
        }
    }

    // 4) Compute cotangent weights and build Laplacian matrix
    typedef Eigen::Triplet<double> T;
    std::vector<T> triplets;
    triplets.reserve(16 * nPts);  // Estimate: 16 non-zeros per point
    Eigen::VectorXd rhsU = Eigen::VectorXd::Zero(nPts);
    Eigen::VectorXd rhsV = Eigen::VectorXd::Zero(nPts);

    // Map for storing edge weights (key: min(i,j), max(i,j))
    std::unordered_map<vtkIdType, std::unordered_map<vtkIdType, double>>
        edgeWeights;

    // Iterate over all triangles using VTK 8.2 API
    vtkCellArray* polys = mesh->GetPolys();
    vtkIdType numCells = polys->GetNumberOfCells();

    vtkNew<vtkIdList> cellPointIds;
    polys->InitTraversal();

    for (vtkIdType cellId = 0; cellId < numCells; cellId++) {
        polys->GetNextCell(cellPointIds);
        vtkIdType numPoints = cellPointIds->GetNumberOfIds();
        if (numPoints != 3) continue;  // Skip non-triangle cells

        vtkIdType i = cellPointIds->GetId(0);
        vtkIdType j = cellPointIds->GetId(1);
        vtkIdType k = cellPointIds->GetId(2);

        double pi[3], pj[3], pk[3];
        mesh->GetPoint(i, pi);
        mesh->GetPoint(j, pj);
        mesh->GetPoint(k, pk);

        // Compute edge vectors
        double v_ij[3] = {pj[0] - pi[0], pj[1] - pi[1], pj[2] - pi[2]};
        double v_ik[3] = {pk[0] - pi[0], pk[1] - pi[1], pk[2] - pi[2]};

        // Compute cross product for area
        double normal[3];
        vtkMath::Cross(v_ij, v_ik, normal);
        double norm = vtkMath::Norm(normal);
        if (norm < 1e-12) continue;  // Skip degenerate triangles

        // Compute vectors for other angles
        double v_ji[3] = {pi[0] - pj[0], pi[1] - pj[1], pi[2] - pj[2]};
        double v_jk[3] = {pk[0] - pj[0], pk[1] - pj[1], pk[2] - pj[2]};

        double v_ki[3] = {pi[0] - pk[0], pi[1] - pk[1], pi[2] - pk[2]};
        double v_kj[3] = {pj[0] - pk[0], pj[1] - pk[1], pj[2] - pk[2]};

        // Compute dot products
        double dot_i = vtkMath::Dot(v_ij, v_ik);
        double dot_j = vtkMath::Dot(v_ji, v_jk);
        double dot_k = vtkMath::Dot(v_ki, v_kj);

        // Compute cotangents (using norm instead of area)
        double cot_i = (norm > 1e-12) ? (dot_i / norm) : 0.0;
        double cot_j = (norm > 1e-12) ? (dot_j / norm) : 0.0;
        double cot_k = (norm > 1e-12) ? (dot_k / norm) : 0.0;

        // Add weights for each edge (using opposite angles)
        auto addWeight = [&](vtkIdType a, vtkIdType b, double weight) {
            vtkIdType minId = std::min(a, b);
            vtkIdType maxId = std::max(a, b);
            edgeWeights[minId][maxId] += weight;
        };

        // Add weights for all edges in the triangle
        addWeight(j, k,
                  cot_i / 2.0);  // Weight for edge jk (opposite to vertex i)
        addWeight(k, i,
                  cot_j / 2.0);  // Weight for edge ki (opposite to vertex j)
        addWeight(i, j,
                  cot_k / 2.0);  // Weight for edge ij (opposite to vertex k)
    }

    // Build Laplacian matrix from edge weights
    std::vector<double> diag(nPts, 0.0);
    for (const auto& i_map : edgeWeights) {
        vtkIdType i = i_map.first;
        for (const auto& j_weight : i_map.second) {
            vtkIdType j = j_weight.first;
            double w = j_weight.second;

            triplets.push_back(T(i, j, -w));
            triplets.push_back(T(j, i, -w));
            diag[i] += w;
            diag[j] += w;
        }
    }

    // Add diagonal entries
    for (vtkIdType i = 0; i < nPts; i++) {
        triplets.push_back(T(i, i, diag[i]));
    }

    // 5) Apply boundary conditions
    for (vtkIdType i = 0; i < nPts; i++) {
        if (isBoundary[i]) {
            // Overwrite row for boundary vertex
            for (auto it = triplets.begin(); it != triplets.end();) {
                if (it->row() == i) {
                    if (it->col() == i) {
                        it = triplets.erase(it);
                    } else {
                        it = triplets.erase(it);
                        continue;
                    }
                }
                if (it != triplets.end()) ++it;
            }
            triplets.push_back(T(i, i, 1.0));

            // Set right-hand side
            double uv[3];
            flatPts->GetPoint(i, uv);
            rhsU(i) = uv[0];
            rhsV(i) = uv[1];
        }
    }

    // 6) Build and solve linear systems
    Eigen::SparseMatrix<double> L(nPts, nPts);
    L.setFromTriplets(triplets.begin(), triplets.end());

    // Factorize matrix
    Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;
    solver.compute(L);

    if (solver.info() != Eigen::Success) {
        std::cerr << "Laplacian matrix factorization failed" << std::endl;
        return;
    }

    // Solve for U and V coordinates
    Eigen::VectorXd U = solver.solve(rhsU);
    Eigen::VectorXd V = solver.solve(rhsV);

    // 7) Assign parameterization coordinates
    for (vtkIdType i = 0; i < nPts; i++) {
        flatPts->SetPoint(i, U(i), V(i), 0.0);
    }

    outFlatMesh->SetPoints(flatPts);
    outFlatMesh->SetPolys(mesh->GetPolys());

    // 8) Debug: Save boundary loop as a polyline
    vtkNew<vtkPoints> loopPts;
    vtkNew<vtkCellArray> lines;

    for (vtkIdType id : boundaryLoop) {
        double pt[3];
        mesh->GetPoint(id, pt);
        loopPts->InsertNextPoint(pt);
    }

    // Create a single polyline cell
    vtkNew<vtkPolyLine> polyLine;
    polyLine->GetPointIds()->SetNumberOfIds(boundaryLoop.size());
    for (vtkIdType i = 0; i < static_cast<vtkIdType>(boundaryLoop.size());
         ++i) {
        polyLine->GetPointIds()->SetId(i, i);  // Local indices
    }
    lines->InsertNextCell(polyLine);

    // Build polydata
    vtkNew<vtkPolyData> loopPolyData;
    loopPolyData->SetPoints(loopPts);
    loopPolyData->SetLines(lines);

    // Save to file
    // vtkNew<vtkXMLPolyDataWriter> writer;
    // writer->SetFileName("boundaryLoop.vtp");
    // writer->SetInputData(loopPolyData);
    // writer->Write();
}

// -------------------- Extract boundary --------------------
bool RectSlimMapper::ExtractSingleBoundaryLoop(vtkPolyData* mesh,
                                               vtkPoints* curvePts,
                                               std::vector<vtkIdType>& loop) {
    if (!mesh || mesh->GetNumberOfPoints() == 0 ||
        mesh->GetNumberOfCells() == 0)
        return false;
    // Step 1: Preserve original point IDs
    vtkNew<vtkIdFilter> idFilter;
    idFilter->SetInputData(m_mask);
    idFilter->SetPointIds(true);
    idFilter->SetCellIds(false);
    idFilter->SetIdsArrayName("OriginalIds");
    idFilter->Update();

    // Step 2: Extract boundary edges
    vtkNew<vtkFeatureEdges> maskBoundaryFilter;
    maskBoundaryFilter->SetInputConnection(idFilter->GetOutputPort());
    maskBoundaryFilter->BoundaryEdgesOn();
    maskBoundaryFilter->FeatureEdgesOff();
    maskBoundaryFilter->NonManifoldEdgesOff();
    maskBoundaryFilter->ManifoldEdgesOff();
    maskBoundaryFilter->Update();

    // Step 3: Split boundaries into connected pieces
    vtkNew<vtkConnectivityFilter> conn;
    conn->SetInputConnection(maskBoundaryFilter->GetOutputPort());
    conn->SetExtractionModeToAllRegions();
    conn->ColorRegionsOn();
    conn->Update();

    vtkPolyData* boundaries = vtkPolyData::SafeDownCast(conn->GetOutput());
    vtkIntArray* regionIds = vtkIntArray::SafeDownCast(
        boundaries->GetCellData()->GetArray("RegionId"));
    if (!regionIds) {
        regionIds = vtkIntArray::SafeDownCast(
            boundaries->GetPointData()->GetArray("RegionId"));
    }
    // Step 4: Find longest region by edge length
    std::map<int, double> regionLength;
    for (vtkIdType i = 0; i < boundaries->GetNumberOfCells(); ++i) {
        vtkCell* cell = boundaries->GetCell(i);
        double length = 0.0;
        for (vtkIdType j = 0; j < cell->GetNumberOfPoints() - 1; ++j) {
            double p0[3], p1[3];
            boundaries->GetPoint(cell->GetPointId(j), p0);
            boundaries->GetPoint(cell->GetPointId(j + 1), p1);
            length += sqrt(vtkMath::Distance2BetweenPoints(p0, p1));
        }

        int region = regionIds ? regionIds->GetValue(i) : 0;
        regionLength[region] += length;
    }
    // Find region with maximum length
    auto maxRegion = std::max_element(
        regionLength.begin(), regionLength.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; });
    int longestRegion = maxRegion->first;

    // Step 5: Extract longest boundary
    vtkNew<vtkConnectivityFilter> extractLongest;
    extractLongest->SetInputConnection(maskBoundaryFilter->GetOutputPort());
    extractLongest->SetExtractionModeToSpecifiedRegions();
    extractLongest->AddSpecifiedRegion(longestRegion);
    extractLongest->Update();
    auto meshBoundary = vtkPolyData::SafeDownCast(extractLongest->GetOutput());
    vtkDataArray* originalIds =
        meshBoundary->GetPointData()->GetArray("OriginalIds");

    // Build adjacency list
    std::unordered_map<vtkIdType, std::vector<vtkIdType>> adj;
    vtkIdType nCells = meshBoundary->GetNumberOfCells();
    for (vtkIdType ci = 0; ci < nCells; ++ci) {
        vtkCell* c = meshBoundary->GetCell(ci);
        if (!c || c->GetNumberOfPoints() != 2) continue;
        vtkIdType a = c->GetPointId(0);
        vtkIdType b = c->GetPointId(1);
        adj[a].push_back(b);
        adj[b].push_back(a);
    }

    if (adj.empty()) return false;

    vtkNew<vtkIdList> maskResamplePtIds;
    vtkNew<vtkPointLocator> ptLocator;
    ptLocator->SetDataSet(meshBoundary);
    ptLocator->BuildLocator();

    for (vtkIdType i = 0; i < curvePts->GetNumberOfPoints(); ++i) {
        double queryPoint[3];
        curvePts->GetPoint(i, queryPoint);

        vtkIdType closestPointId = ptLocator->FindClosestPoint(queryPoint);
        maskResamplePtIds->InsertNextId(closestPointId);
    }
    auto startVertex = maskResamplePtIds->GetId(0);
    auto dirStartVertex = maskResamplePtIds->GetId(0);
    auto dirEndVertex = maskResamplePtIds->GetId(1);

    // Determine start vertex
    vtkIdType start = startVertex;
    if (startVertex < 0 || adj.find(startVertex) == adj.end()) {
        start = adj.begin()->first;
    }

    // Calculate direction vector from two vertices
    double directionVector[3] = {0, 0, 0};
    bool useDirection = false;
    if (dirStartVertex >= 0 && dirEndVertex >= 0) {
        double p1[3], p2[3];
        meshBoundary->GetPoint(dirStartVertex, p1);
        meshBoundary->GetPoint(dirEndVertex, p2);
        directionVector[0] = p2[0] - p1[0];
        directionVector[1] = p2[1] - p1[1];
        directionVector[2] = p2[2] - p1[2];
        useDirection = (vtkMath::Norm(directionVector) > 1e-8);
    }

    // Normalize direction vector if valid
    double normalizedDir[3] = {0, 0, 0};
    if (useDirection) {
        vtkMath::Normalize(directionVector);
        for (int i = 0; i < 3; ++i) {
            normalizedDir[i] = directionVector[i];
        }
    }

    loop.clear();
    std::unordered_set<vtkIdType> visited;
    vtkIdType curr = start, prev = -1;

    for (;;) {
        loop.push_back(curr);
        visited.insert(curr);
        const auto& nbrs = adj[curr];
        vtkIdType next = -1;

        // Special handling at start with valid direction vector
        if (prev == -1 && useDirection) {
            double startPoint[3];
            meshBoundary->GetPoint(curr, startPoint);
            double maxCosine = -VTK_DOUBLE_MAX;
            for (auto nb : nbrs) {
                double nbPoint[3];
                meshBoundary->GetPoint(nb, nbPoint);
                double edgeVec[3] = {nbPoint[0] - startPoint[0],
                                     nbPoint[1] - startPoint[1],
                                     nbPoint[2] - startPoint[2]};
                double edgeNorm = vtkMath::Norm(edgeVec);
                if (edgeNorm <= 0) continue;
                // Normalize edge vector and compute cosine similarity
                double normalizedEdge[3] = {edgeVec[0] / edgeNorm,
                                            edgeVec[1] / edgeNorm,
                                            edgeVec[2] / edgeNorm};
                double cosine = vtkMath::Dot(normalizedEdge, normalizedDir);
                if (cosine > maxCosine) {
                    maxCosine = cosine;
                    next = nb;
                }
            }
        } else {
            // Original neighbor selection logic
            for (auto nb : nbrs) {
                if (nb == prev) continue;
                next = nb;
                if (!visited.count(nb)) break;
            }
        }

        if (next < 0) break;  // No valid next vertex

        prev = curr;
        curr = next;

        // Check loop completion
        if (curr == start) {
            loop.push_back(curr);  // Close the loop
            break;
        }

        // Prevent infinite loops
        if (loop.size() > adj.size() * 5) {
            loop.clear();
            return false;
        }
    }

    // Validate loop
    if (loop.size() < 4 || loop.front() != loop.back()) {
        loop.clear();
        return false;
    }

    if (!originalIds) {
        loop.clear();
        return false;
    }

    for (vtkIdType& id : loop) {
        id = static_cast<vtkIdType>(originalIds->GetTuple1(id));
    }
    return true;
}

void RectSlimMapper::InitialUV(vtkPolyData* flatMesh,
                               std::vector<Vector2d>& UV) {
    vtkIdType n = flatMesh->GetNumberOfPoints();
    UV.resize(static_cast<size_t>(n));

    for (vtkIdType i = 0; i < n; ++i) {
        double P[3];
        flatMesh->GetPoint(i, P);  // P[0] = U, P[1] = V, P[2] = 0
        UV[static_cast<size_t>(i)] = Vector2d(P[0], P[1]);
    }
}

// -------------------- Sample flattened mesh --------------------
void RectSlimMapper::Sample(int uRes, int vRes) {
    if (m_mask->GetNumberOfPoints() == 0 || m_flatMask->GetNumberOfPoints() == 0 || !m_output ||
        m_UV.empty()) {
        return;
    }

    int U = std::max(1, uRes);
    int V = std::max(1, vRes);

    vtkSmartPointer<vtkPoints> outPts = vtkSmartPointer<vtkPoints>::New();
    outPts->SetNumberOfPoints(U * V);

    vtkSmartPointer<vtkFloatArray> tcoords =
        vtkSmartPointer<vtkFloatArray>::New();
    tcoords->SetNumberOfComponents(2);
    tcoords->SetNumberOfTuples(U * V);
    tcoords->SetName("UV");

    // Simple regular grid in UV bounds
    double minU = m_UV[0].x(), minV = m_UV[0].y();
    double maxU = minU, maxV = minV;
    for (auto& uv : m_UV) {
        minU = std::min(minU, uv.x());
        minV = std::min(minV, uv.y());
        maxU = std::max(maxU, uv.x());
        maxV = std::max(maxV, uv.y());
    }
    double du = (maxU - minU) / (U - 1);
    double dv = (maxV - minV) / (V - 1);

    for (int vv = 0; vv < V; ++vv) {
        for (int uu = 0; uu < U; ++uu) {
            double u = minU + uu * du;
            double v = minV + vv * dv;

            // nearest mask vertex
            double bestDist2 = std::numeric_limits<double>::infinity();
            vtkIdType bestId = 0;
            for (vtkIdType i = 0; i < (vtkIdType)m_UV.size(); ++i) {
                Vector2d d = m_UV[(size_t)i] - Vector2d(u, v);
                double d2 = d.squaredNorm();
                if (d2 < bestDist2) {
                    bestDist2 = d2;
                    bestId = i;
                }
            }

            double P[3];
            m_mask->GetPoint(bestId, P);
            vtkIdType id = vv * U + uu;
            outPts->SetPoint(id, P);
            float uv2[2] = {(float)((u - minU) / (maxU - minU)),
                            (float)((v - minV) / (maxV - minV))};
            tcoords->SetTuple(id, uv2);
        }
    }

    vtkSmartPointer<vtkPolyData> result = vtkSmartPointer<vtkPolyData>::New();
    result->SetPoints(outPts);
    result->GetPointData()->SetTCoords(tcoords);

    vtkSmartPointer<vtkCellArray> polys = vtkSmartPointer<vtkCellArray>::New();
    for (int v = 0; v < V - 1; ++v)
        for (int u = 0; u < U - 1; ++u) {
            vtkIdType pts[4] = {v * U + u, v * U + u + 1, (v + 1) * U + u + 1,
                                (v + 1) * U + u};
            polys->InsertNextCell(4, pts);
        }
    result->SetPolys(polys);
    m_output->DeepCopy(result);
}

void RectSlimMapper::DebugMesh(const std::string& folder) {
    // Ensure folder string ends with "/" or "\"
    std::string f = folder;
    if (!f.empty() && f.back() != '/' && f.back() != '\\') f += "/";

    // 1. Save original mask
    {
        vtkNew<vtkXMLPolyDataWriter> writer;
        writer->SetFileName((f + "maskMesh.vtp").c_str());
        writer->SetInputData(m_mask);
        writer->Write();
    }

    // 2. Save flattened mask

    vtkNew<vtkXMLPolyDataWriter> writer;
    writer->SetFileName((f + "flattenedMask.vtp").c_str());
    writer->SetInputData(m_flatMask);
    writer->Write();
}

void RectSlimMapper::ConvertVTKToVCG(vtkPolyData* polyData, MyMesh& vcgMesh) {
    // Clear existing mesh
    vcgMesh.Clear();

    // Validate input
    if (!polyData || !polyData->GetPoints()) {
        std::cout << "Invalid VTK polydata input" << std::endl;
        return;
    }

    vtkPoints* points = polyData->GetPoints();
    vtkIdType numPoints = points->GetNumberOfPoints();

    // Add vertices
    for (vtkIdType i = 0; i < numPoints; ++i) {
        double p[3];
        points->GetPoint(i, p);
        vcg::tri::Allocator<MyMesh>::AddVertex(vcgMesh,
                                               vcg::Point3f(p[0], p[1], p[2]));
    }

    // Add faces
    vtkCellArray* polys = polyData->GetPolys();
    polys->InitTraversal();
    vtkIdType npts, *pts;

    while (polys->GetNextCell(npts, pts)) {
        if (npts != 3) continue;  // Only triangles

        // Pass vertex pointers to AddFace
        vcg::tri::Allocator<MyMesh>::AddFace(vcgMesh, &vcgMesh.vert[pts[0]],
                                             &vcgMesh.vert[pts[1]],
                                             &vcgMesh.vert[pts[2]]);
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

void RectSlimMapper::ConvertVCGToVTK(MyMesh& vcgMesh, vtkPolyData* polyData) {
    if (!polyData) return;

    // Ensure compact vertex/face arrays
    vcg::tri::Allocator<MyMesh>::CompactVertexVector(vcgMesh);
    vcg::tri::Allocator<MyMesh>::CompactFaceVector(vcgMesh);

    auto points = vtkSmartPointer<vtkPoints>::New();
    auto polys = vtkSmartPointer<vtkCellArray>::New();

    // Add vertices
    points->SetNumberOfPoints(vcgMesh.vert.size());
    for (size_t i = 0; i < vcgMesh.vert.size(); ++i) {
        auto& v = vcgMesh.vert[i];
        points->SetPoint(static_cast<vtkIdType>(i), v.P()[0], v.P()[1],
                         v.P()[2]);
    }

    // Add faces
    for (size_t i = 0; i < vcgMesh.face.size(); ++i) {
        auto& f = vcgMesh.face[i];
        vtkIdType pts[3] = {
            static_cast<vtkIdType>(vcg::tri::Index(vcgMesh, f.V(0))),
            static_cast<vtkIdType>(vcg::tri::Index(vcgMesh, f.V(1))),
            static_cast<vtkIdType>(vcg::tri::Index(vcgMesh, f.V(2)))};

        if (pts[0] < 0 || pts[1] < 0 || pts[2] < 0) continue;  // skip invalid
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
    polyData->Modified();
}
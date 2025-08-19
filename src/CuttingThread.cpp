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

#include "CuttingThread.h"

CuttingThread::CuttingThread(vtkPolyData* inputMesh,
                             vtkPoints* curvePoints,
                             vtkPolyData* outputCutMesh,
                             QObject* parent)
    : QThread(parent),
      m_inputMesh(inputMesh),
      m_curvePoints(curvePoints),
      m_outputCutMesh(outputCutMesh)
{
}

CuttingThread::~CuttingThread() {
    wait();
}

void CuttingThread::run() {
    if (!m_inputMesh || !m_curvePoints || !m_outputCutMesh) {
        std::cerr << "CuttingThread: Invalid inputs" << std::endl;
        return;
    }
    CutMeshWithCurve(m_inputMesh, m_curvePoints, m_outputCutMesh);
    emit finished();
}

// === Implementation of helpers === //

void CuttingThread::CutMeshWithCurve(vtkPolyData* inputMesh,
                                         vtkPoints* curvePoints,
                                         vtkPolyData* outputCutMesh) {
    if (!inputMesh || !curvePoints || curvePoints->GetNumberOfPoints() < 3) {
        std::cerr << "Invalid mesh or curve points." << std::endl;
        return;
    }

    // --- Step 0: Fast region extraction ---
    vtkNew<vtkConnectivityFilter> preSelector;
    preSelector->SetInputData(inputMesh);
    preSelector->SetExtractionModeToAllRegions();
    preSelector->ColorRegionsOn();
    preSelector->Update();

    vtkPolyData* labeledMesh =
        vtkPolyData::SafeDownCast(preSelector->GetOutput());
    vtkDataArray* regionArray = labeledMesh->GetPointData()->GetScalars();
    if (!regionArray) {
        std::cerr << "Missing region scalars." << std::endl;
        return;
    }

    // --- Step 0.1: Identify unique region IDs ---
    std::unordered_set<int> touchedRegions;
    vtkSmartPointer<vtkPointLocator> bodyLocator =
        vtkSmartPointer<vtkPointLocator>::New();
    bodyLocator->SetDataSet(labeledMesh);
    bodyLocator->BuildLocator();

    vtkIdType nCheck =
        std::min(static_cast<vtkIdType>(3), curvePoints->GetNumberOfPoints());
    double pt[3];
    for (int i = 0; i < nCheck; ++i) {
        curvePoints->GetPoint(i, pt);
        vtkIdType id = bodyLocator->FindClosestPoint(pt);
        int regionId = static_cast<int>(regionArray->GetComponent(id, 0));
        touchedRegions.insert(regionId);
    }

    // --- Step 0.2: Append regions ---
    vtkNew<vtkAppendPolyData> regionCombiner;
    for (int regionId : touchedRegions) {
        vtkNew<vtkThreshold> regionThreshold;
        regionThreshold->SetInputData(labeledMesh);
        regionThreshold->SetInputArrayToProcess(
            0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "RegionId");
        regionThreshold->ThresholdBetween(regionId, regionId);

        vtkNew<vtkGeometryFilter> converter;
        converter->SetInputConnection(regionThreshold->GetOutputPort());

        regionCombiner->AddInputConnection(converter->GetOutputPort());
    }
    regionCombiner->Update();

    vtkNew<vtkPolyData> relevantBodies;
    relevantBodies->DeepCopy(regionCombiner->GetOutput());
    relevantBodies->GetPointData()->RemoveArray("RegionId");

    // --- Step 1: Close curve if not closed ---
    // 
    // --- Step 2: cut faces using the curve ---
    vtkNew<vtkIdList> cutterIdList;
    GetCutterCurve(relevantBodies, curvePoints, cutterIdList);
    for (vtkIdType i = 0; i < cutterIdList->GetNumberOfIds(); i++) {
        relevantBodies->DeleteCell(cutterIdList->GetId(i));
    }
    relevantBodies->RemoveDeletedCells();

    // --- Step 3: Extract largest region only ---
    vtkNew<vtkConnectivityFilter> selector;
    selector->SetInputData(relevantBodies);
    selector->SetExtractionModeToLargestRegion();
    selector->ColorRegionsOn();
    selector->Update();

    vtkPolyData* coloredRegions =
        vtkPolyData::SafeDownCast(selector->GetOutput());
    vtkDataArray* regionColors = coloredRegions->GetPointData()->GetScalars();
    if (!regionColors) {
        std::cerr << "No region color scalars found." << std::endl;
        return;
    }

    // --- Step 4: Inverse selection array ---
    vtkNew<vtkIntArray> inverseSelection;
    inverseSelection->SetName("InverseSelection");
    inverseSelection->SetNumberOfComponents(1);
    inverseSelection->SetNumberOfTuples(relevantBodies->GetNumberOfPoints());
    inverseSelection->Fill(1);

    // Use batched spatial query via locator
    vtkNew<vtkPointLocator> locator;
    locator->SetDataSet(coloredRegions);
    locator->BuildLocator();

    const vtkIdType nPts = relevantBodies->GetNumberOfPoints();
#pragma omp parallel for shared(inverseSelection) schedule(static)
    for (vtkIdType i = 0; i < nPts; ++i) {
        double pt[3];
        relevantBodies->GetPoint(i, pt);
        vtkIdType closest = locator->FindClosestPoint(pt);
        if (regionColors->GetComponent(closest, 0) == 1.0) {
            inverseSelection->SetValue(i, 0);
        }
    }

    relevantBodies->GetPointData()->AddArray(inverseSelection);

    // --- Step 5: Threshold by InverseSelection ---
    vtkNew<vtkThreshold> threshold;
    threshold->SetInputData(relevantBodies);
    threshold->SetInputArrayToProcess(
        0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "InverseSelection");
    threshold->ThresholdBetween(0, 0);

    vtkNew<vtkGeometryFilter> geometryFilter;
    geometryFilter->SetInputConnection(threshold->GetOutputPort());

    vtkNew<vtkCleanPolyData> cleaner;
    cleaner->SetInputConnection(geometryFilter->GetOutputPort());
    cleaner->Update();

    outputCutMesh->ShallowCopy(cleaner->GetOutput());
    outputCutMesh->GetPointData()->RemoveArray("InverseSelection");
}

void CuttingThread::GetCutterCurve(vtkPolyData* Poly, vtkPoints* curvePts,
                                       vtkIdList* outCurveIds) {
    outCurveIds->Initialize();

    // Build point locator ONCE
    vtkNew<vtkPointLocator> curvePtLocator;
    curvePtLocator->SetDataSet(Poly);
    curvePtLocator->BuildLocator();

    // Snap curve points to surface
    vtkNew<vtkPoints> snappedPts;
    for (vtkIdType i = 0; i < curvePts->GetNumberOfPoints(); i++) {
        double p[3];
        curvePts->GetPoint(i, p);
        vtkIdType closestId = curvePtLocator->FindClosestPoint(p);
        Poly->GetPoint(closestId, p);  // Get actual coordinate
        snappedPts->InsertNextPoint(p);
    }

    // Build closed line cell
    vtkNew<vtkCellArray> line;
    vtkIdType n = snappedPts->GetNumberOfPoints();
    line->InsertNextCell(n + 1);
    for (vtkIdType i = 0; i < n; i++) {
        line->InsertCellPoint(i);
    }
    line->InsertCellPoint(0);

    vtkNew<vtkPolyData> curvePoly;
    curvePoly->SetPoints(snappedPts);
    curvePoly->SetLines(line);

    // Call optimized Astar
    vtkNew<vtkIdList> edgePointIds;
    AStarEdgeSearch(Poly, curvePoly, edgePointIds);
    //DijkstraEdgeSearch(Poly, curvePoly, edgePointIds);
    
    // --- Optimization: Build point-to-cell map ---
    std::unordered_map<vtkIdType, std::vector<vtkIdType>> pointToCells;
    for (vtkIdType cellId = 0; cellId < Poly->GetNumberOfCells(); ++cellId) {
        vtkCell* cell = Poly->GetCell(cellId);
        vtkIdList* ids = cell->GetPointIds();
        for (vtkIdType j = 0; j < ids->GetNumberOfIds(); ++j) {
            pointToCells[ids->GetId(j)].push_back(cellId);
        }
    }

    std::unordered_set<vtkIdType> visitedCells;
    for (vtkIdType i = 0; i < edgePointIds->GetNumberOfIds(); ++i) {
        vtkIdType ptId = edgePointIds->GetId(i);
        auto it = pointToCells.find(ptId);
        if (it != pointToCells.end()) {
            for (vtkIdType cellId : it->second) {
                visitedCells.insert(cellId);
            }
        }
    }

    for (vtkIdType cellId : visitedCells) {
        outCurveIds->InsertNextId(cellId);
    }
}

void CuttingThread::AStarEdgeSearch(vtkPolyData* mesh,
                                        vtkPolyData* closedCurve,
                                        vtkIdList* edgePointIds) {
    edgePointIds->Initialize();

    vtkPoints* points = mesh->GetPoints();
    if (!points) return;

    vtkIdType numVertices = points->GetNumberOfPoints();
    if (numVertices == 0) return;

    // Build adjacency list: vertexId -> vector of connected vertexIds with edge
    // length
    std::vector<std::vector<std::pair<vtkIdType, double>>> adjacency(
        numVertices);

    for (vtkIdType cellId = 0; cellId < mesh->GetNumberOfCells(); ++cellId) {
        vtkCell* cell = mesh->GetCell(cellId);
        vtkIdList* ids = cell->GetPointIds();

        vtkIdType n = ids->GetNumberOfIds();
        for (vtkIdType i = 0; i < n; ++i) {
            vtkIdType v0 = ids->GetId(i);
            vtkIdType v1 = ids->GetId((i + 1) % n);

            double p0[3], p1[3];
            points->GetPoint(v0, p0);
            points->GetPoint(v1, p1);
            double dist = sqrt(vtkMath::Distance2BetweenPoints(p0, p1));

            adjacency[v0].emplace_back(v1, dist);
            adjacency[v1].emplace_back(v0, dist);
        }
    }

    vtkIdType nLoopPts = closedCurve->GetNumberOfPoints();
    if (nLoopPts < 2) return;

    // Create and build vtkPointLocator for mesh points
    vtkSmartPointer<vtkPointLocator> pointLocator =
        vtkSmartPointer<vtkPointLocator>::New();
    pointLocator->SetDataSet(mesh);
    pointLocator->BuildLocator();

    // Use vtkPointLocator to find closest mesh points for closedCurve points
    std::vector<std::pair<vtkIdType, vtkIdType>> pathSegments(nLoopPts);
    for (vtkIdType i = 0; i < nLoopPts; ++i) {
        double p0[3], p1[3];
        closedCurve->GetPoint(i, p0);
        closedCurve->GetPoint((i + 1) % nLoopPts, p1);
        pathSegments[i].first = pointLocator->FindClosestPoint(p0);
        pathSegments[i].second = pointLocator->FindClosestPoint(p1);
    }

    // Lambda: A* search for one start-goal pair
    auto AStarSearch = [&](vtkIdType start,
                           vtkIdType goal) -> std::vector<vtkIdType> {
        std::vector<double> gScore(numVertices,
                                   std::numeric_limits<double>::infinity());
        std::vector<double> fScore(numVertices,
                                   std::numeric_limits<double>::infinity());
        std::vector<vtkIdType> cameFrom(numVertices, -1);
        std::vector<bool> closedSet(numVertices, false);

        auto heuristic = [&](vtkIdType a, vtkIdType b) -> double {
            double pa[3], pb[3];
            points->GetPoint(a, pa);
            points->GetPoint(b, pb);
            return sqrt(vtkMath::Distance2BetweenPoints(pa, pb));
        };

        std::priority_queue<AStarNode, std::vector<AStarNode>,
                            std::greater<AStarNode>>
            openSet;

        gScore[start] = 0.0;
        fScore[start] = heuristic(start, goal);
        openSet.push({start, fScore[start]});

        while (!openSet.empty()) {
            AStarNode current = openSet.top();
            openSet.pop();

            if (current.vertexId == goal) {
                // Reconstruct path
                std::vector<vtkIdType> path;
                vtkIdType cur = goal;
                while (cur != -1) {
                    path.push_back(cur);
                    cur = cameFrom[cur];
                }
                std::reverse(path.begin(), path.end());
                return path;
            }

            if (closedSet[current.vertexId]) continue;
            closedSet[current.vertexId] = true;

            for (const auto& neighbor : adjacency[current.vertexId]) {
                vtkIdType neighborId = neighbor.first;
                double edgeWeight = neighbor.second;
                if (closedSet[neighborId]) continue;

                double tentative_gScore = gScore[current.vertexId] + edgeWeight;
                if (tentative_gScore < gScore[neighborId]) {
                    cameFrom[neighborId] = current.vertexId;
                    gScore[neighborId] = tentative_gScore;
                    fScore[neighborId] =
                        tentative_gScore + heuristic(neighborId, goal);
                    openSet.push({neighborId, fScore[neighborId]});
                }
            }
        }

        return {};  // no path found
    };

    // Create per-thread containers for thread safety
    int maxThreads = omp_get_max_threads();
    std::vector<vtkSmartPointer<vtkIdList>> threadResults(maxThreads);
    for (int i = 0; i < maxThreads; ++i) {
        threadResults[i] = vtkSmartPointer<vtkIdList>::New();
    }

    // Parallel loop over curve segments
#pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < static_cast<int>(nLoopPts); ++i) {
        int tid = omp_get_thread_num();

        vtkIdType start = pathSegments[i].first;
        vtkIdType goal = pathSegments[i].second;

        std::vector<vtkIdType> path = AStarSearch(start, goal);
        for (vtkIdType v : path) {
            threadResults[tid]->InsertNextId(v);
        }
    }

    // Merge thread-local results into edgePointIds (optional: remove duplicates
    // here)
    std::set<vtkIdType> uniqueIds;
    for (auto& list : threadResults) {
        for (vtkIdType i = 0; i < list->GetNumberOfIds(); ++i) {
            vtkIdType id = list->GetId(i);
            if (uniqueIds.insert(id).second) {
                edgePointIds->InsertNextId(id);
            }
        }
    }
}

void CuttingThread::DijkstraEdgeSearch(vtkPolyData* mesh,
                                           vtkPolyData* closedCurve,
                                           vtkIdList* edgePointIds) {
    mesh->BuildLinks();  // Required by Dijkstra

    vtkNew<vtkDijkstraGraphGeodesicPath> edgeSearch;
    edgeSearch->StopWhenEndReachedOn();
    edgeSearch->SetInputData(mesh);

    vtkNew<vtkStaticPointLocator> locator;
    locator->SetDataSet(mesh);
    locator->BuildLocator();

    vtkIdType n = closedCurve->GetNumberOfPoints();
    double p0[3], p1[3];
    closedCurve->GetPoint(0, p0);
    vtkIdType id0 = locator->FindClosestPoint(p0);

    for (vtkIdType i = 1; i <= n; ++i) {
        closedCurve->GetPoint(i % n, p1);
        vtkIdType id1 = locator->FindClosestPoint(p1);

        edgeSearch->SetStartVertex(id0);
        edgeSearch->SetEndVertex(id1);
        edgeSearch->Update();

        vtkPolyData* path = edgeSearch->GetOutput();
        for (vtkIdType j = 0; j < path->GetNumberOfPoints(); ++j) {
            double x[3];
            path->GetPoint(j, x);
            vtkIdType closest = locator->FindClosestPoint(x);
            edgePointIds->InsertUniqueId(closest);  // prevent duplicates
        }

        std::copy(std::begin(p1), std::end(p1), std::begin(p0));
        id0 = id1;
    }
}
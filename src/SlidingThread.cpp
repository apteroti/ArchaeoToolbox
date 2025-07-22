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

#include "../include/SlidingThread.h"

// SlidingThread::SlidingThread(){}

SlidingThread::SlidingThread(
    vtkPolyData* meshData, int typeINOL, int curveNOS, int curveNOC,
    std::vector<int>* curveType, int surfaceNOS, int surfacePatchUNOS,
    int surfacePatchVNOS, int surfacePatchNOP,
    vtkMultiBlockDataSet* curvePolyLineBlock,
    vtkMultiBlockDataSet* surfaceMaskBlock,
    const Eigen::Ref<const Eigen::MatrixXd>& templateCoordinates,
    Eigen::MatrixXd& coordinates, int mode)
    : m_meshData(meshData),
    //m_templateMesh(templateMesh),
    m_typeINOL(typeINOL),
    m_curveNOS(curveNOS),
    m_curveNOC(curveNOC),
    m_curveType(curveType),
    m_surfaceNOS(surfaceNOS),
    m_surfacePatchUNOS(surfacePatchUNOS),
    m_surfacePatchVNOS(surfacePatchVNOS),
    m_surfacePatchNOP(surfacePatchNOP),
    m_curvePolyLineBlock(curvePolyLineBlock),
    m_surfaceMaskBlock(surfaceMaskBlock),
    m_coordinates(coordinates) {
    vtkNew<vtkPoints> tempTargetPts;
    vtkNew<vtkPoints> tempTemplatePts;
    vtkNew<vtkPolyData> tempTemplatePtsPoly;
    for (int i = 0; i < m_coordinates.rows(); i++) {
        tempTargetPts->InsertNextPoint(m_coordinates(i, 0), m_coordinates(i, 1),
            m_coordinates(i, 2));
    }
    for (int i = 0; i < templateCoordinates.rows(); i++) {
        tempTemplatePts->InsertNextPoint(templateCoordinates(i, 0),
            templateCoordinates(i, 1),
            templateCoordinates(i, 2));
    }
    tempTemplatePtsPoly->SetPoints(tempTemplatePts);
    vtkNew<vtkLandmarkTransform> linearLMTransform;
    linearLMTransform->SetTargetLandmarks(tempTargetPts);
    linearLMTransform->SetSourceLandmarks(tempTemplatePts);
    linearLMTransform->SetModeToSimilarity();
    linearLMTransform->Update();

    vtkNew<vtkTransformPolyDataFilter> linearLMTransformFilter;
    linearLMTransformFilter->SetInputData(tempTemplatePtsPoly);
    linearLMTransformFilter->SetTransform(linearLMTransform);
    linearLMTransformFilter->Update();
    vtkPoints* imposedTemplatePts =
        linearLMTransformFilter->GetOutput()->GetPoints();
    m_templateCoordinates.resize(imposedTemplatePts->GetNumberOfPoints(), 3);
    for (int i = 0; i < imposedTemplatePts->GetNumberOfPoints(); i++) {
        m_templateCoordinates.operator()(i, 0) =
            imposedTemplatePts->GetPoint(i)[0];
        m_templateCoordinates.operator()(i, 1) =
            imposedTemplatePts->GetPoint(i)[1];
        m_templateCoordinates.operator()(i, 2) =
            imposedTemplatePts->GetPoint(i)[2];
    }
    tempTargetPts->Initialize();
    tempTemplatePts->Initialize();
    tempTemplatePtsPoly->Initialize();
    // Precompute invariant quantities
    AssembleQ(m_templateCoordinates, m_Q);
    AssembleK(m_templateCoordinates, m_K);
    AssembleL(m_Q, m_K, m_L);
    BEMatrix(m_L, m_Q.rows(), m_SMat, m_BEMat);
}

void SlidingThread::run() {
    // U should be sparse due to selector-like structure
    Eigen::SparseMatrix<double> Uboundary;
    AssembleU(m_coordinates, Uboundary);
    
    int numTotalLndmrks = m_coordinates.rows();
    Eigen::MatrixXd reserveLndmrks = m_coordinates;

    double minBE = std::numeric_limits<double>::max();
    Eigen::MatrixXd bestCoords = m_coordinates;
    m_noImprovementCounter = 0;

    while ((!m_abort)) {
        // Compute current Bending Energy
        double BEXBefore = m_coordinates.col(0).transpose() * m_BEMat * m_coordinates.col(0);
        double BEYBefore = m_coordinates.col(1).transpose() * m_BEMat * m_coordinates.col(1);
        double BEZBefore = m_coordinates.col(2).transpose() * m_BEMat * m_coordinates.col(2);
        double BEInitial = std::abs(BEXBefore + BEYBefore + BEZBefore);

        // Flatten coordinates for solving
        Eigen::MatrixXd gamma0 = m_coordinates;
        gamma0.resize(numTotalLndmrks * 3, 1);

        // Construct sparse matrix products
        Eigen::SparseMatrix<double> USU =
            Uboundary.transpose() * m_SMat * Uboundary;
        Eigen::MatrixXd USG =
            Uboundary.transpose() * m_SMat * gamma0;  // keep dense

        Eigen::SimplicialLLT<Eigen::SparseMatrix<double>> llt;
        llt.compute(USU);
        Eigen::MatrixXd T;
        

        if (llt.info() == Eigen::Success) {
            m_solverType = "Cholesky";
            T = llt.solve(USG);  // USG can be dense, this is okay
        } else {
            m_solverType = "Orthogonal";
            Eigen::MatrixXd denseUSU = Eigen::MatrixXd(USU);
            Eigen::CompleteOrthogonalDecomposition<Eigen::MatrixXd> orthoSolver(
                denseUSU);
            T = orthoSolver.solve(USG);  // already dense
        }

        // Back project solution to full coordinates
        Eigen::MatrixXd USUT = Uboundary * T;
        GSSRefinement(m_coordinates, USUT, numTotalLndmrks);

        // Recalculate Bending Energy
        double BEXAfter = m_coordinates.col(0).transpose() * m_BEMat * m_coordinates.col(0);
        double BEYAfter = m_coordinates.col(1).transpose() * m_BEMat * m_coordinates.col(1);
        double BEZAfter = m_coordinates.col(2).transpose() * m_BEMat * m_coordinates.col(2);
        m_BEUpdated = std::abs(BEXAfter + BEYAfter + BEZAfter);

        // Check for improvement
        if (m_BEUpdated < minBE) {
            minBE = m_BEUpdated;
            bestCoords = m_coordinates;
            m_noImprovementCounter = 0;
            m_improvement = 0;
        } else {
            m_noImprovementCounter++;
            m_improvement = 1;
        }

        // Exit condition: no improvement for long
        if (m_noImprovementCounter >= m_maxNoImprovementCount) {
            m_coordinates = bestCoords;
            m_improvement = 0;
            m_BEUpdated = minBE;  // Always report best BE
            break;
        }

        reserveLndmrks = m_coordinates;

        // Update coordinates-dependent matrices only
        SuperImpose(m_coordinates, m_templateCoordinates);
        AssembleU(m_coordinates, Uboundary);

        emit CoordinateNotChanged(m_coordinates);
    }

    if (!m_abort) {
        emit CoordinateChanged(m_coordinates);
    }
}

// Golden Section Search is a derivative-free optimization method ideal for
// finding the minimum of a unimodal function on a bounded interval.
void SlidingThread::GSSRefinement(Eigen::MatrixXd& coordinates,
                                    const Eigen::MatrixXd& USUT,
                                    int numLNDMRK) {
    auto bendingEnergyAtScale = [&](double s) -> std::pair<double, Eigen::MatrixXd> {
        Eigen::MatrixXd tempCoord = coordinates;
        Eigen::MatrixXd tempUSUT = USUT * s;

        tempCoord.resize(numLNDMRK * 3, 1);
        tempCoord -= tempUSUT;
        tempCoord.resize(numLNDMRK, 3);

        ClampPointsToSurface(tempCoord);  // Clamp to mesh/curve

        double BEX = tempCoord.col(0).transpose() * m_BEMat * tempCoord.col(0);
        double BEY = tempCoord.col(1).transpose() * m_BEMat * tempCoord.col(1);
        double BEZ = tempCoord.col(2).transpose() * m_BEMat * tempCoord.col(2);
        double totalBE = std::abs(BEX + BEY + BEZ);

        return std::make_pair(totalBE, tempCoord);
    };

    // Golden Section constants
    const double gr = (std::sqrt(5.0) + 1.0) / 2.0;
    double a = 0.1;
    double b = 1.0;
    double tol = 1e-3;

    double c = b - (b - a) / gr;
    double d = a + (b - a) / gr;

    auto [fc, coordsC] = bendingEnergyAtScale(c);
    auto [fd, coordsD] = bendingEnergyAtScale(d);

    while (std::abs(b - a) > tol) {
        if (fc < fd) {
            b = d;
            d = c;
            fd = fc;
            coordsD = coordsC;
            c = b - (b - a) / gr;
            std::tie(fc, coordsC) = bendingEnergyAtScale(c);
        } else {
            a = c;
            c = d;
            fc = fd;
            coordsC = coordsD;
            d = a + (b - a) / gr;
            std::tie(fd, coordsD) = bendingEnergyAtScale(d);
        }
    }

    double optimalScale = (fc < fd) ? c : d;
    coordinates = (fc < fd) ? coordsC : coordsD;
    m_scaleFactor = optimalScale;
}

// Helper function to clamp points back to mesh/curves
void SlidingThread::ClampPointsToSurface(Eigen::MatrixXd& coords) {
    int numCurveSliders = 0;
    int numSurfaceSliders = 0;

    if (m_curveNOS != 0) {
        numCurveSliders = m_curveNOS * m_curveNOC;
        int start = m_typeINOL;
        for (int j = 0; j < m_curveNOC; ++j) {
            vtkPolyData* curveRef = dynamic_cast<vtkPolyData*>(m_curvePolyLineBlock->GetBlock(j));
            vtkNew<vtkCellLocator> pointTree;
            pointTree->SetDataSet(curveRef);
            pointTree->BuildLocator();
            pointTree->Update();

            for (int k = 0; k < m_curveNOS; ++k) {
                int idx = start + j * m_curveNOS + k;
                double pt[3] = { coords(idx, 0), coords(idx, 1), coords(idx, 2) };
                double closestPoint[3];
                vtkIdType cellId = -1;
                int subId = -1;
                double dist = -1;
                pointTree->FindClosestPoint(pt, closestPoint, cellId, subId, dist);
                // Snap to closest point on curve
                coords(idx, 0) = closestPoint[0];
                coords(idx, 1) = closestPoint[1];
                coords(idx, 2) = closestPoint[2];
            }
        }
    }

    if (m_surfacePatchUNOS != 0 && m_surfacePatchVNOS != 0 && m_surfaceNOS == 0) {
        numSurfaceSliders = (m_surfacePatchUNOS * m_surfacePatchVNOS) * m_surfacePatchNOP;
        int start = m_typeINOL + numCurveSliders;
        for (int j = 0; j < m_surfacePatchNOP; ++j) {
            vtkPolyData* surfaceRef = dynamic_cast<vtkPolyData*>(m_surfaceMaskBlock->GetBlock(j));
            vtkNew<vtkCellLocator> pointTree;
            pointTree->SetDataSet(surfaceRef);
            pointTree->BuildLocator();
            pointTree->Update();

            for (int k = 0; k < (m_surfacePatchUNOS * m_surfacePatchVNOS); ++k) {
                int idx = start + j * (m_surfacePatchUNOS * m_surfacePatchVNOS) + k;
                double pt[3] = { coords(idx, 0), coords(idx, 1), coords(idx, 2) };
                double closestPoint[3];
                vtkIdType cellId = -1;
                int subId = -1;
                double dist = -1;
                pointTree->FindClosestPoint(pt, closestPoint, cellId, subId, dist);
                coords(idx, 0) = closestPoint[0];
                coords(idx, 1) = closestPoint[1];
                coords(idx, 2) = closestPoint[2];
            }
        }
    }

    if (m_surfacePatchUNOS == 0 && m_surfacePatchVNOS == 0 && m_surfaceNOS != 0) {
        numSurfaceSliders = m_surfaceNOS;
        int start = m_typeINOL + numCurveSliders;
        vtkNew<vtkCellLocator> pointTree;
        pointTree->SetDataSet(m_meshData);
        pointTree->BuildLocator();
        pointTree->Update();

        for (int i = 0; i < m_surfaceNOS; ++i) {
            int idx = start + i;
            double pt[3] = { coords(idx, 0), coords(idx, 1), coords(idx, 2) };
            double closestPoint[3];
            vtkIdType cellId = -1;
            int subId = -1;
            double dist = -1;
            pointTree->FindClosestPoint(pt, closestPoint, cellId, subId, dist);
            coords(idx, 0) = closestPoint[0];
            coords(idx, 1) = closestPoint[1];
            coords(idx, 2) = closestPoint[2];
        }
    }
}

double SlidingThread::GetBE() { return m_BEUpdated; }
double SlidingThread::GetScalingFactor() { return m_scaleFactor; }
double SlidingThread::GetRefinementLoop() { return m_noImprovementCounter; }
std::string SlidingThread::GetSolverType() { return m_solverType; }
bool SlidingThread::GetImprovement(){ return m_improvement;}
int SlidingThread::GetImprovementLoop(){return m_noImprovementCounter;}

void SlidingThread::SuperImpose(Eigen::MatrixXd& templatePts,
    Eigen::MatrixXd& targetPts) {
    vtkNew<vtkPoints> templateVtkPts;
    vtkNew<vtkPoints> targetVtkPts;
    vtkNew<vtkPolyData> targetVtkPtsPoly;
    for (int i = 0; i < templatePts.rows(); i++) {
        templateVtkPts->InsertNextPoint(templatePts(i, 0), templatePts(i, 1),
            templatePts(i, 2));
    }
    templateVtkPts->Modified();

    for (int i = 0; i < targetPts.rows(); i++) {
        targetVtkPts->InsertNextPoint(targetPts(i, 0), targetPts(i, 1),
            targetPts(i, 2));
    }
    targetVtkPts->Modified();
    targetVtkPtsPoly->SetPoints(targetVtkPts);

    vtkNew<vtkLandmarkTransform> linearLMTransform;
    linearLMTransform->SetTargetLandmarks(templateVtkPts);
    linearLMTransform->SetSourceLandmarks(targetVtkPts);
    linearLMTransform->SetModeToSimilarity();
    linearLMTransform->Update();

    vtkNew<vtkTransformPolyDataFilter> linearLMTransformFilter;
    linearLMTransformFilter->SetInputData(targetVtkPtsPoly);
    linearLMTransformFilter->SetTransform(linearLMTransform);
    linearLMTransformFilter->Update();

    vtkPolyData* outputPoly = linearLMTransformFilter->GetOutput();

    targetPts.resize(0, 0);
    targetPts.resize(outputPoly->GetNumberOfPoints(), 3);
    for (int i = 0; i < outputPoly->GetNumberOfPoints(); i++) {
        targetPts.operator()(i, 0) = outputPoly->GetPoint(i)[0];
        targetPts.operator()(i, 1) = outputPoly->GetPoint(i)[1];
        targetPts.operator()(i, 2) = outputPoly->GetPoint(i)[2];
    }
}

void SlidingThread::DebugPrintMatrix(Eigen::MatrixXd matrix) {
    Eigen::IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");
    std::string sep = "\n----------------------------------------\n";
    std::cout << matrix.format(CleanFmt) << sep;
}

void SlidingThread::KillNow() { m_abort = true; }

bool SlidingThread::Killing() { return m_abort; }

void SlidingThread::AssembleU(const Eigen::MatrixXd& targetLndmrks,
                              Eigen::SparseMatrix<double>& outputU) {
    int numCurveSliders = 0;
    int numSurfaceSliders = 0;

    if (m_curveNOS != 0) numCurveSliders = m_curveNOS * m_curveNOC;

    if (m_surfacePatchUNOS != 0 && m_surfacePatchVNOS != 0 && m_surfaceNOS == 0)
        numSurfaceSliders =
            m_surfacePatchUNOS * m_surfacePatchVNOS * m_surfacePatchNOP;

    if (m_surfacePatchUNOS == 0 && m_surfacePatchVNOS == 0 && m_surfaceNOS != 0)
        numSurfaceSliders = m_surfaceNOS;

    int numSliders = numCurveSliders + numSurfaceSliders;
    int totalNumLandmarks = numSliders + m_typeINOL;

    std::vector<Eigen::Triplet<double>> triplets;

    if (numSliders != 0) {
        int sliderCounter = 0;

        if (numCurveSliders != 0) {
            Eigen::MatrixXd curveCoordsBlock =
                targetLndmrks.block(m_typeINOL, 0, numCurveSliders, 3);
            Eigen::MatrixXd curveTangent =
                Eigen::MatrixXd::Zero(numCurveSliders, 3);

            for (int i = 0; i < m_curveNOC; i++) {
                Eigen::MatrixXd tangent;
                CalculateCurveTangent(m_curvePolyLineBlock, i, tangent);
                int start = i * m_curveNOS;
                auto curveSubBlock =
                    curveCoordsBlock.block(start, 0, m_curveNOS, 3);
                vtkPolyData* pd = dynamic_cast<vtkPolyData*>(
                    m_curvePolyLineBlock->GetBlock(i));
                vtkNew<vtkPointLocator> pointTree;
                pointTree->SetDataSet(pd);
                pointTree->BuildLocator();
                for (int j = 0; j < m_curveNOS; j++) {
                    int ptId = pointTree->FindClosestPoint(curveSubBlock(j, 0),
                                                           curveSubBlock(j, 1),
                                                           curveSubBlock(j, 2));
                    curveTangent.row(start + j) = tangent.row(ptId);
                }
            }

            int curveFirstIndx = m_typeINOL;
            for (int i = 0; i < numCurveSliders; i++) {
                triplets.emplace_back(i + curveFirstIndx, sliderCounter,
                                      curveTangent(i, 0));
                triplets.emplace_back(i + curveFirstIndx + totalNumLandmarks,
                                      sliderCounter, curveTangent(i, 1));
                triplets.emplace_back(
                    i + curveFirstIndx + 2 * totalNumLandmarks, sliderCounter,
                    curveTangent(i, 2));
                sliderCounter++;
            }
        }

        if (numSurfaceSliders != 0) {
            Eigen::MatrixXd UVector, VVector;
            CalculateTangent(m_meshData, UVector, VVector);

            int surfaceFirstIndx = m_typeINOL + numCurveSliders;
            for (int i = 0; i < numSurfaceSliders; i++) {
                triplets.emplace_back(i + surfaceFirstIndx, sliderCounter,
                                      UVector(i, 0));
                triplets.emplace_back(i + surfaceFirstIndx + totalNumLandmarks,
                                      sliderCounter, UVector(i, 1));
                triplets.emplace_back(
                    i + surfaceFirstIndx + 2 * totalNumLandmarks, sliderCounter,
                    UVector(i, 2));

                triplets.emplace_back(i + surfaceFirstIndx,
                                      sliderCounter + numSliders,
                                      VVector(i, 0));
                triplets.emplace_back(i + surfaceFirstIndx + totalNumLandmarks,
                                      sliderCounter + numSliders,
                                      VVector(i, 1));
                triplets.emplace_back(
                    i + surfaceFirstIndx + 2 * totalNumLandmarks,
                    sliderCounter + numSliders, VVector(i, 2));

                sliderCounter++;
            }
        }
    }

    outputU.resize(totalNumLandmarks * 3, numSliders * 2);
    outputU.setFromTriplets(triplets.begin(), triplets.end());
}

void SlidingThread::AssembleQ(const Eigen::MatrixXd& templatePoints,
                              Eigen::MatrixXd& Q) {
    Q.resize(templatePoints.rows(), 4);
    Q.col(0).setOnes();
    Q.block(0, 1, templatePoints.rows(), 3) = templatePoints;
}

void SlidingThread::AssembleK(const Eigen::MatrixXd& templatePoints,
                              Eigen::MatrixXd& K) {
    K.setZero(templatePoints.rows(), templatePoints.rows());
    PDist(templatePoints, K);  // This could potentially be made faster if reused
}

void SlidingThread::AssembleL(const Eigen::MatrixXd& Q, const Eigen::MatrixXd& K,
                              Eigen::MatrixXd& OutputL) {
    OutputL.resize(0, 0);
    Eigen::MatrixXd temp1(Q.rows() + 4, Q.rows());
    temp1 << K, Q.transpose();
    Eigen::MatrixXd matrixO = Eigen::MatrixXd::Zero(4, 4);
    Eigen::MatrixXd temp2(Q.rows() + 4, 4);
    temp2 << Q, matrixO;
    OutputL.resize(Q.rows() + 4, Q.rows() + 4);
    OutputL << temp1, temp2;
}

void SlidingThread::BEMatrix(const Eigen::MatrixXd& L, int numOfLandmarks,
                              Eigen::SparseMatrix<double>& outputSMat,
                              Eigen::MatrixXd& outputBEMat) {
    Eigen::MatrixXd LInv;
    bool usedLLT = false;

    // Try LLT first (fastest)
    Eigen::LLT<Eigen::MatrixXd> llt(L.selfadjointView<Eigen::Lower>());
    if (llt.info() == Eigen::Success) {
        LInv = llt.solve(Eigen::MatrixXd::Identity(L.rows(), L.cols()));
        usedLLT = true;
    } else {
        Eigen::FullPivLU<Eigen::MatrixXd> fpluSolver(L);
        LInv = fpluSolver.inverse();
        if (!fpluSolver.isInvertible()) {
            QMessageBox::warning(nullptr, 
                                 "Warning",
                                 "Matrix L may be singular!"
            );
        }
    }

    // Extract BEMat and store output
    Eigen::MatrixXd BEMat = LInv.block(0, 0, numOfLandmarks, numOfLandmarks);
    outputBEMat = BEMat;

    // Fill triplets for sparse block diagonal matrix
    std::vector<Eigen::Triplet<double>> triplets;
    int n = BEMat.rows();
    for (int d = 0; d < 3; ++d)
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                if (BEMat(i, j) != 0.0)
                    triplets.emplace_back(i + d * n, j + d * n, BEMat(i, j));

    outputSMat.resize(n * 3, n * 3);
    outputSMat.setFromTriplets(triplets.begin(), triplets.end());
}

double SlidingThread::EucDist(double Ax, double Ay, double Az, double Bx,
    double By, double Bz) {
    double dx = Ax - Bx;
    double dy = Ay - By;
    double dz = Az - Bz;
    double dist = sqrt((dx * dx) + (dy * dy) + (dz * dz));
    return dist;
}

void SlidingThread::PDist(vtkPoints* points, Eigen::MatrixXd& output) {
    int dim = points->GetNumberOfPoints();
    for (int i = 0; i < dim; i++) {
        for (int j = i + 1; j < dim; j++) {
            double dist =
                EucDist(points->GetPoint(i)[0], points->GetPoint(i)[1],
                    points->GetPoint(i)[2], points->GetPoint(j)[0],
                    points->GetPoint(j)[1], points->GetPoint(j)[2]);
            output.operator()(i, j) = dist;
        }
    }
    output.triangularView<Eigen::Lower>() = output.transpose();
}

void SlidingThread::PDist(const Eigen::MatrixXd& points, Eigen::MatrixXd& output) {
    int dim = points.rows();
    for (int i = 0; i < dim; i++) {
        for (int j = i + 1; j < dim; j++) {
            double dist = EucDist(points(i, 0), points(i, 1), points(i, 2),
                points(j, 0), points(j, 1), points(j, 2));
            output.operator()(i, j) = dist;
        }
    }
    output.triangularView<Eigen::Lower>() = output.transpose();
}

void SlidingThread::PGeoDist(vtkPolyData* mesh, Eigen::MatrixXd& points, Eigen::MatrixXd& output) {
    int dim = points.rows();
    for (int i = 0; i < dim; i++) {
        for (int j = i + 1; j < dim; j++) {
        }
    }
}

void SlidingThread::CalculateTangent(vtkPolyData* polyMesh,
    Eigen::MatrixXd& tangentU,
    Eigen::MatrixXd& tangentV) {
    vtkNew<vtkPolyDataNormals> normalsFilter;
    normalsFilter->SetInputData(polyMesh);
    normalsFilter->ConsistencyOn();
    normalsFilter->ComputePointNormalsOn();
    normalsFilter->Update();
    auto normalArray =
        normalsFilter->GetOutput()->GetPointData()->GetArray("Normals");

    Eigen::MatrixXd normalMatrix;
    normalMatrix.resize(normalArray->GetNumberOfTuples(), 3);
    for (int i = 0; i < normalArray->GetNumberOfTuples(); i++) {
        for (int j = 0; j < 3; j++) {
            normalMatrix.operator()(i, j) = normalArray->GetTuple(i)[j];
        }
    }
    tangentU.resize(0, 0);
    tangentV.resize(0, 0);
    tangentU.resize(normalsFilter->GetOutput()->GetNumberOfPoints(), 3);
    tangentV.resize(normalsFilter->GetOutput()->GetNumberOfPoints(), 3);

    for (int i = 0; i < normalMatrix.rows(); i++) {
        Eigen::Vector3d tempU;
        Eigen::Vector3d tempV;
        Eigen::Vector3d tempNorm(normalMatrix(i, 0), normalMatrix(i, 1),
            normalMatrix(i, 2));
        if ((tempNorm.array() == 0.0).any()) {
            tempU << 0.0, 0.0, 0.0;
            for (int j = 0; j < 3; j++) {
                if (tempNorm(j) == 0) {
                    tempU.operator()(j) = 1.0;
                }
            }
            tempU.operator/=(tempU.norm());
        }
        else {
            tempU << 1.0, 1.0, -(tempNorm(0) + tempNorm(1)) / tempNorm(2);
            tempU.operator/=(tempU.norm());
        }
        tangentU.operator()(i, 0) = tempU(0);
        tangentU.operator()(i, 1) = tempU(1);
        tangentU.operator()(i, 2) = tempU(2);
        tempV = tempU.cross(tempNorm);
        tempV.operator/=(tempV.norm());
        tangentV.operator()(i, 0) = tempV(0);
        tangentV.operator()(i, 1) = tempV(1);
        tangentV.operator()(i, 2) = tempV(2);
    }
}

void SlidingThread::CalculateCurveTangent(vtkMultiBlockDataSet* inputCurve,
    int id, Eigen::MatrixXd& outputU) {
    outputU.resize(0, 0);
    vtkDataObject* dso = inputCurve->GetBlock(id);
    vtkPolyData* pd = dynamic_cast<vtkPolyData*>(dso);
    if (pd->GetNumberOfPoints() > 0) {
        outputU.resize(pd->GetNumberOfPoints(), 3);
        if (m_curveType->at(id) == 1) {  // closed curve
            for (int i = 0; i < pd->GetNumberOfPoints(); i++) {
                if (i == pd->GetNumberOfPoints() - 1) {
                    double x2 = pd->GetPoint(0)[0];
                    double y2 = pd->GetPoint(0)[1];
                    double z2 = pd->GetPoint(0)[2];
                    double x1 = pd->GetPoint(i)[0];
                    double y1 = pd->GetPoint(i)[1];
                    double z1 = pd->GetPoint(i)[2];
                    outputU.operator()(i, 0) = x2 - x1;
                    outputU.operator()(i, 1) = y2 - y1;
                    outputU.operator()(i, 2) = z2 - z1;
                }
                else {
                    double x2 = pd->GetPoint(i + 1)[0];
                    double y2 = pd->GetPoint(i + 1)[1];
                    double z2 = pd->GetPoint(i + 1)[2];
                    double x1 = pd->GetPoint(i)[0];
                    double y1 = pd->GetPoint(i)[1];
                    double z1 = pd->GetPoint(i)[2];
                    outputU.operator()(i, 0) = x2 - x1;
                    outputU.operator()(i, 1) = y2 - y1;
                    outputU.operator()(i, 2) = z2 - z1;
                }
            }
        }
        else if (m_curveType->at(id) == 0) {  // open curve
            for (int i = 0; i < pd->GetNumberOfPoints(); i++) {
                if (i == pd->GetNumberOfPoints() - 1) {
                    double x2 = pd->GetPoint(i - 1)[0];
                    double y2 = pd->GetPoint(i - 1)[1];
                    double z2 = pd->GetPoint(i - 1)[2];
                    double x1 = pd->GetPoint(i)[0];
                    double y1 = pd->GetPoint(i)[1];
                    double z1 = pd->GetPoint(i)[2];
                    outputU.operator()(i, 0) = x2 - x1;
                    outputU.operator()(i, 1) = y2 - y1;
                    outputU.operator()(i, 2) = z2 - z1;
                }
                else {
                    double x2 = pd->GetPoint(i + 1)[0];
                    double y2 = pd->GetPoint(i + 1)[1];
                    double z2 = pd->GetPoint(i + 1)[2];
                    double x1 = pd->GetPoint(i)[0];
                    double y1 = pd->GetPoint(i)[1];
                    double z1 = pd->GetPoint(i)[2];
                    outputU.operator()(i, 0) = x2 - x1;
                    outputU.operator()(i, 1) = y2 - y1;
                    outputU.operator()(i, 2) = z2 - z1;
                }
            }
        }
    }
}

SlidingThread::~SlidingThread() {}
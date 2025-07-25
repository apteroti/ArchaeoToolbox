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

/*
 * SlidingThread implements sliding semi-landmarks algorithm for geometric
 * morphometrics Key concepts:
 * - Semi-landmarks: points that can slide along curves or surfaces
 * - Bending energy: measures deformation between template and target shapes
 * - Thin-plate spline (TPS): used for interpolation and regularization
 */

#include "../include/SlidingThread.h"
// Constructor initializes all data structures and performs initial alignment
SlidingThread::SlidingThread(
    vtkPolyData* meshData, int typeINOL, int curveNOS, int curveNOC,
    std::vector<int>* curveType, int surfaceNOS, int surfacePatchUNOS,
    int surfacePatchVNOS, int surfacePatchNOP,
    vtkMultiBlockDataSet* curvePolyLineBlock,
    vtkMultiBlockDataSet* surfaceMaskBlock,
    const Eigen::Ref<const Eigen::MatrixXd>& templateCoordinates,
    Eigen::MatrixXd& coordinates, int mode)
    : m_meshData(meshData),
      m_typeINOL(typeINOL),      // Number of fixed landmarks
      m_curveNOS(curveNOS),      // Number of sliding points per curve
      m_curveNOC(curveNOC),      // Number of curves
      m_curveType(curveType),    // Type of each curve (open/closed)
      m_surfaceNOS(surfaceNOS),  // Number of surface sliding points
      m_surfacePatchUNOS(surfacePatchUNOS),      // U-direction points per patch
      m_surfacePatchVNOS(surfacePatchVNOS),      // V-direction points per patch
      m_surfacePatchNOP(surfacePatchNOP),        // Number of surface patches
      m_curvePolyLineBlock(curvePolyLineBlock),  // Curves data
      m_surfaceMaskBlock(surfaceMaskBlock),      // Surface patches data
      m_coordinates(coordinates) {               // Target coordinates

    // Convert points to VTK format for initial alignment
    vtkNew<vtkPoints> tempTargetPts;
    vtkNew<vtkPoints> tempTemplatePts;
    vtkNew<vtkPolyData> tempTemplatePtsPoly;

    // Load target points
    for (int i = 0; i < m_coordinates.rows(); i++) {
        tempTargetPts->InsertNextPoint(m_coordinates(i, 0), m_coordinates(i, 1),
                                       m_coordinates(i, 2));
    }

    // Load template points
    for (int i = 0; i < templateCoordinates.rows(); i++) {
        tempTemplatePts->InsertNextPoint(templateCoordinates(i, 0),
                                         templateCoordinates(i, 1),
                                         templateCoordinates(i, 2));
    }
    tempTemplatePtsPoly->SetPoints(tempTemplatePts);

    // Compute similarity transform to align template to target
    vtkNew<vtkLandmarkTransform> linearLMTransform;
    linearLMTransform->SetTargetLandmarks(tempTargetPts);
    linearLMTransform->SetSourceLandmarks(tempTemplatePts);
    linearLMTransform->SetModeToSimilarity();  // Scale + Rotation + Translation
    linearLMTransform->Update();

    // Apply the transform to template points
    vtkNew<vtkTransformPolyDataFilter> linearLMTransformFilter;
    linearLMTransformFilter->SetInputData(tempTemplatePtsPoly);
    linearLMTransformFilter->SetTransform(linearLMTransform);
    linearLMTransformFilter->Update();

    // Store transformed template coordinates
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

    // Clear temporary VTK objects
    tempTargetPts->Initialize();
    tempTemplatePts->Initialize();
    tempTemplatePtsPoly->Initialize();

    // Precompute invariant quantities for TPS:
    // Q - affine basis functions (1,x,y,z)
    // K - pairwise distances matrix
    // L - combined matrix [K Q; Q' 0]
    // SMat - sparse block diagonal bending energy matrix
    // BEMat - bending energy matrix
    AssembleQ(m_templateCoordinates, m_Q);
    AssembleK(m_templateCoordinates, m_K);
    AssembleL(m_Q, m_K, m_L);
    BEMatrix(m_L, m_Q.rows(), m_SMat, m_BEMat);
}

// Main sliding algorithm loop
void SlidingThread::run() {
    // U matrix encodes tangent directions for sliding
    Eigen::SparseMatrix<double> Uboundary;
    AssembleU(m_coordinates, Uboundary);

    int numTotalLndmrks = m_coordinates.rows();
    Eigen::MatrixXd reserveLndmrks = m_coordinates;

    double minBE = std::numeric_limits<double>::max();
    Eigen::MatrixXd bestCoords = m_coordinates;
    m_noImprovementCounter = 0;

    // Main optimization loop
    while ((!m_abort)) {
        // Compute current bending energy (measure of deformation)
        double BEXBefore =
            m_coordinates.col(0).transpose() * m_BEMat * m_coordinates.col(0);
        double BEYBefore =
            m_coordinates.col(1).transpose() * m_BEMat * m_coordinates.col(1);
        double BEZBefore =
            m_coordinates.col(2).transpose() * m_BEMat * m_coordinates.col(2);
        double BEInitial = std::abs(BEXBefore + BEYBefore + BEZBefore);

        // Flatten coordinates for solving (stack x,y,z components)
        Eigen::MatrixXd gamma0 = m_coordinates;
        gamma0.resize(numTotalLndmrks * 3, 1);

        // Construct sparse matrix products for linear system:
        // USU * T = USG
        // Where T are the tangent coefficients we're solving for
        Eigen::SparseMatrix<double> USU =
            Uboundary.transpose() * m_SMat * Uboundary;
        Eigen::MatrixXd USG = Uboundary.transpose() * m_SMat * gamma0;

        // Try Cholesky first (fastest for SPD matrices)
        Eigen::SimplicialLLT<Eigen::SparseMatrix<double>> llt;
        llt.compute(USU);
        Eigen::MatrixXd T;

        if (llt.info() == Eigen::Success) {
            m_solverType = "Cholesky";
            T = llt.solve(USG);
        } else {
            // Fall back to QR decomposition if Cholesky fails
            m_solverType = "Orthogonal";
            Eigen::MatrixXd denseUSU = Eigen::MatrixXd(USU);
            Eigen::CompleteOrthogonalDecomposition<Eigen::MatrixXd> orthoSolver(
                denseUSU);
            T = orthoSolver.solve(USG);
        }

        // Back project solution to full coordinates
        Eigen::MatrixXd USUT = Uboundary * T;

        // Refine solution using Golden Section Search
        GSSRefinement(m_coordinates, USUT, numTotalLndmrks);

        // Recalculate bending energy after update
        double BEXAfter =
            m_coordinates.col(0).transpose() * m_BEMat * m_coordinates.col(0);
        double BEYAfter =
            m_coordinates.col(1).transpose() * m_BEMat * m_coordinates.col(1);
        double BEZAfter =
            m_coordinates.col(2).transpose() * m_BEMat * m_coordinates.col(2);
        m_BEUpdated = std::abs(BEXAfter + BEYAfter + BEZAfter);

        // Check for improvement in bending energy
        if (m_BEUpdated < minBE) {
            minBE = m_BEUpdated;
            bestCoords = m_coordinates;
            m_noImprovementCounter = 0;
            m_improvement = 0;
        } else {
            m_noImprovementCounter++;
            m_improvement = 1;
        }

        // Exit condition: no improvement for several iterations
        if (m_noImprovementCounter >= m_maxNoImprovementCount) {
            m_coordinates = bestCoords;
            m_improvement = 0;
            m_BEUpdated = minBE;
            break;
        }

        reserveLndmrks = m_coordinates;

        // Update tangent directions based on new coordinates
        SuperImpose(m_coordinates, m_templateCoordinates);
        AssembleU(m_coordinates, Uboundary);

        emit CoordinateNotChanged(m_coordinates);
    }

    if (!m_abort) {
        emit CoordinateChanged(m_coordinates);
    }
}

/*
 * Golden Section Search refinement
 * Finds optimal scaling factor for the tangent update that minimizes bending
 * energy while keeping points constrained to their respective curves/surfaces
 */
void SlidingThread::GSSRefinement(Eigen::MatrixXd& coordinates,
                                  const Eigen::MatrixXd& USUT, int numLNDMRK) {
    // Lambda computes bending energy for a given scale factor
    auto bendingEnergyAtScale =
        [&](double s) -> std::pair<double, Eigen::MatrixXd> {
        Eigen::MatrixXd tempCoord = coordinates;
        Eigen::MatrixXd tempUSUT = USUT * s;

        tempCoord.resize(numLNDMRK * 3, 1);
        tempCoord -= tempUSUT;  // Apply tangent update
        tempCoord.resize(numLNDMRK, 3);

        // Project points back to curves/surfaces
        ClampPointsToSurface(tempCoord);

        // Compute bending energy
        double BEX = tempCoord.col(0).transpose() * m_BEMat * tempCoord.col(0);
        double BEY = tempCoord.col(1).transpose() * m_BEMat * tempCoord.col(1);
        double BEZ = tempCoord.col(2).transpose() * m_BEMat * tempCoord.col(2);
        double totalBE = std::abs(BEX + BEY + BEZ);

        return std::make_pair(totalBE, tempCoord);
    };

    // Golden Section Search parameters
    const double gr = (std::sqrt(5.0) + 1.0) / 2.0;  // Golden ratio
    double a = 0.1;                                  // Lower bound
    double b = 1.0;                                  // Upper bound
    double tol = 1e-3;                               // Tolerance

    // Initial points
    double c = b - (b - a) / gr;
    double d = a + (b - a) / gr;

    auto [fc, coordsC] = bendingEnergyAtScale(c);
    auto [fd, coordsD] = bendingEnergyAtScale(d);

    // Golden section search loop
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

    // Select best solution
    double optimalScale = (fc < fd) ? c : d;
    coordinates = (fc < fd) ? coordsC : coordsD;
    m_scaleFactor = optimalScale;
}

/*
 * Projects sliding points back to their respective curves/surfaces
 * Uses VTK locators to find closest points
 */
void SlidingThread::ClampPointsToSurface(Eigen::MatrixXd& coords) {
    int numCurveSliders = 0;
    int numSurfaceSliders = 0;

    // Handle curve points
    if (m_curveNOS != 0) {
        numCurveSliders = m_curveNOS * m_curveNOC;
        int start = m_typeINOL;  // Start after fixed landmarks
        for (int j = 0; j < m_curveNOC; ++j) {
            vtkPolyData* curveRef =
                dynamic_cast<vtkPolyData*>(m_curvePolyLineBlock->GetBlock(j));
            vtkNew<vtkCellLocator> pointTree;
            pointTree->SetDataSet(curveRef);
            pointTree->BuildLocator();
            pointTree->Update();

            // Project each sliding point to curve
            for (int k = 0; k < m_curveNOS; ++k) {
                int idx = start + j * m_curveNOS + k;
                double pt[3] = {coords(idx, 0), coords(idx, 1), coords(idx, 2)};
                double closestPoint[3];
                vtkIdType cellId = -1;
                int subId = -1;
                double dist = -1;
                pointTree->FindClosestPoint(pt, closestPoint, cellId, subId,
                                            dist);
                // Snap to closest point on curve
                coords(idx, 0) = closestPoint[0];
                coords(idx, 1) = closestPoint[1];
                coords(idx, 2) = closestPoint[2];
            }
        }
    }

    // Handle surface patch points (parameterized grid)
    if (m_surfacePatchUNOS != 0 && m_surfacePatchVNOS != 0 &&
        m_surfaceNOS == 0) {
        numSurfaceSliders =
            (m_surfacePatchUNOS * m_surfacePatchVNOS) * m_surfacePatchNOP;
        int start = m_typeINOL + numCurveSliders;
        for (int j = 0; j < m_surfacePatchNOP; ++j) {
            vtkPolyData* surfaceRef =
                dynamic_cast<vtkPolyData*>(m_surfaceMaskBlock->GetBlock(j));
            vtkNew<vtkCellLocator> pointTree;
            pointTree->SetDataSet(surfaceRef);
            pointTree->BuildLocator();
            pointTree->Update();

            // Project each grid point to surface
            for (int k = 0; k < (m_surfacePatchUNOS * m_surfacePatchVNOS);
                 ++k) {
                int idx =
                    start + j * (m_surfacePatchUNOS * m_surfacePatchVNOS) + k;
                double pt[3] = {coords(idx, 0), coords(idx, 1), coords(idx, 2)};
                double closestPoint[3];
                vtkIdType cellId = -1;
                int subId = -1;
                double dist = -1;
                pointTree->FindClosestPoint(pt, closestPoint, cellId, subId,
                                            dist);
                coords(idx, 0) = closestPoint[0];
                coords(idx, 1) = closestPoint[1];
                coords(idx, 2) = closestPoint[2];
            }
        }
    }

    // Handle general surface points
    if (m_surfacePatchUNOS == 0 && m_surfacePatchVNOS == 0 &&
        m_surfaceNOS != 0) {
        numSurfaceSliders = m_surfaceNOS;
        int start = m_typeINOL + numCurveSliders;
        vtkNew<vtkCellLocator> pointTree;
        pointTree->SetDataSet(m_meshData);
        pointTree->BuildLocator();
        pointTree->Update();

        // Project each point to surface
        for (int i = 0; i < m_surfaceNOS; ++i) {
            int idx = start + i;
            double pt[3] = {coords(idx, 0), coords(idx, 1), coords(idx, 2)};
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

/*
 * The U matrix encodes tangent directions for sliding:
 * - For curve points: single tangent direction per point
 * - For surface points: two tangent directions (u and v) per point
 * The matrix has a block structure to handle x,y,z components separately
 */
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

    // Use triplets for efficient sparse matrix construction
    std::vector<Eigen::Triplet<double>> triplets;

    if (numSliders != 0) {
        int sliderCounter = 0;

        // Handle curve points
        if (numCurveSliders != 0) {
            Eigen::MatrixXd curveCoordsBlock =
                targetLndmrks.block(m_typeINOL, 0, numCurveSliders, 3);
            Eigen::MatrixXd curveTangent =
                Eigen::MatrixXd::Zero(numCurveSliders, 3);

            // Compute tangent directions for each curve
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

                // Find closest point on curve to get tangent direction
                for (int j = 0; j < m_curveNOS; j++) {
                    int ptId = pointTree->FindClosestPoint(curveSubBlock(j, 0),
                                                           curveSubBlock(j, 1),
                                                           curveSubBlock(j, 2));
                    curveTangent.row(start + j) = tangent.row(ptId);
                }
            }

            // Add curve tangent entries to U matrix
            int curveFirstIndx = m_typeINOL;
            for (int i = 0; i < numCurveSliders; i++) {
                // Block structure: x,y,z components handled separately
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

        // Handle surface points
        if (numSurfaceSliders != 0) {
            Eigen::MatrixXd UVector, VVector;
            // Compute tangent directions for surface points
            CalculateTangent(m_meshData, UVector, VVector);

            int surfaceFirstIndx = m_typeINOL + numCurveSliders;
            for (int i = 0; i < numSurfaceSliders; i++) {
                // Add u tangent direction
                triplets.emplace_back(i + surfaceFirstIndx, sliderCounter,
                                      UVector(i, 0));
                triplets.emplace_back(i + surfaceFirstIndx + totalNumLandmarks,
                                      sliderCounter, UVector(i, 1));
                triplets.emplace_back(
                    i + surfaceFirstIndx + 2 * totalNumLandmarks, sliderCounter,
                    UVector(i, 2));

                // Add v tangent direction (in second block)
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

    // Build sparse U matrix from triplets
    outputU.resize(totalNumLandmarks * 3, numSliders * 2);
    outputU.setFromTriplets(triplets.begin(), triplets.end());
}

/*
 * Assembles Q matrix for TPS:
 * Q = [1 x y z] - affine basis functions
 */
void SlidingThread::AssembleQ(const Eigen::MatrixXd& templatePoints,
                              Eigen::MatrixXd& Q) {
    Q.resize(templatePoints.rows(), 4);
    Q.col(0).setOnes();  // First column is 1 (constant term)
    Q.block(0, 1, templatePoints.rows(), 3) =
        templatePoints;  // x,y,z coordinates
}

/*
 * Assembles K matrix for TPS:
 * K_ij = ||x_i - x_j|| - pairwise Euclidean distances
 */
void SlidingThread::AssembleK(const Eigen::MatrixXd& templatePoints,
                              Eigen::MatrixXd& K) {
    K.setZero(templatePoints.rows(), templatePoints.rows());
    PDist(templatePoints, K);  // Compute pairwise distances
}

/*
 * Assembles L matrix for TPS:
 * L = [K Q; Q' 0] - combined matrix for TPS system
 */
void SlidingThread::AssembleL(const Eigen::MatrixXd& Q,
                              const Eigen::MatrixXd& K,
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

/*
 * Computes bending energy matrix for TPS:
 * BEMat = L^-1[1..n,1..n] (top-left block of inverse L)
 * SMat is sparse block diagonal version for x,y,z components
 */
void SlidingThread::BEMatrix(const Eigen::MatrixXd& L, int numOfLandmarks,
                             Eigen::SparseMatrix<double>& outputSMat,
                             Eigen::MatrixXd& outputBEMat) {
    Eigen::MatrixXd LInv;
    bool usedLLT = false;

    // Try Cholesky first (fastest for symmetric positive definite)
    Eigen::LLT<Eigen::MatrixXd> llt(L.selfadjointView<Eigen::Lower>());
    if (llt.info() == Eigen::Success) {
        LInv = llt.solve(Eigen::MatrixXd::Identity(L.rows(), L.cols()));
        usedLLT = true;
    } else {
        // Fall back to LU decomposition
        Eigen::FullPivLU<Eigen::MatrixXd> fpluSolver(L);
        LInv = fpluSolver.inverse();
        if (!fpluSolver.isInvertible()) {
            QMessageBox::warning(nullptr, "Warning",
                                 "Matrix L may be singular!");
        }
    }

    // Extract bending energy matrix (top-left block of inverse)
    Eigen::MatrixXd BEMat = LInv.block(0, 0, numOfLandmarks, numOfLandmarks);
    outputBEMat = BEMat;

    // Create sparse block diagonal version for x,y,z components
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

// Helper function to compute Euclidean distance
double SlidingThread::EucDist(double Ax, double Ay, double Az, double Bx,
                              double By, double Bz) {
    double dx = Ax - Bx;
    double dy = Ay - By;
    double dz = Az - Bz;
    double dist = sqrt((dx * dx) + (dy * dy) + (dz * dz));
    return dist;
}

// Computes pairwise distances between points (VTK version)
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
    // Fill lower triangle
    output.triangularView<Eigen::Lower>() = output.transpose();
}

// Computes pairwise distances between points (Eigen version)
void SlidingThread::PDist(const Eigen::MatrixXd& points,
                          Eigen::MatrixXd& output) {
    int dim = points.rows();
    for (int i = 0; i < dim; i++) {
        for (int j = i + 1; j < dim; j++) {
            double dist = EucDist(points(i, 0), points(i, 1), points(i, 2),
                                  points(j, 0), points(j, 1), points(j, 2));
            output.operator()(i, j) = dist;
        }
    }
    // Fill lower triangle
    output.triangularView<Eigen::Lower>() = output.transpose();
}

/*
 * Computes tangent directions for surface points:
 * - First computes normals
 * - Then computes orthogonal tangent vectors
 */
void SlidingThread::CalculateTangent(vtkPolyData* polyMesh,
                                     Eigen::MatrixXd& tangentU,
                                     Eigen::MatrixXd& tangentV) {
    // Compute normals
    vtkNew<vtkPolyDataNormals> normalsFilter;
    normalsFilter->SetInputData(polyMesh);
    normalsFilter->ConsistencyOn();
    normalsFilter->ComputePointNormalsOn();
    normalsFilter->Update();
    auto normalArray =
        normalsFilter->GetOutput()->GetPointData()->GetArray("Normals");

    // Store normals in matrix
    Eigen::MatrixXd normalMatrix;
    normalMatrix.resize(normalArray->GetNumberOfTuples(), 3);
    for (int i = 0; i < normalArray->GetNumberOfTuples(); i++) {
        for (int j = 0; j < 3; j++) {
            normalMatrix.operator()(i, j) = normalArray->GetTuple(i)[j];
        }
    }

    // Initialize tangent matrices
    tangentU.resize(0, 0);
    tangentV.resize(0, 0);
    tangentU.resize(normalsFilter->GetOutput()->GetNumberOfPoints(), 3);
    tangentV.resize(normalsFilter->GetOutput()->GetNumberOfPoints(), 3);

    // Compute tangent directions for each point
    for (int i = 0; i < normalMatrix.rows(); i++) {
        Eigen::Vector3d tempU;
        Eigen::Vector3d tempV;
        Eigen::Vector3d tempNorm(normalMatrix(i, 0), normalMatrix(i, 1),
                                 normalMatrix(i, 2));

        // Handle degenerate case (zero normal)
        if ((tempNorm.array() == 0.0).any()) {
            tempU << 0.0, 0.0, 0.0;
            for (int j = 0; j < 3; j++) {
                if (tempNorm(j) == 0) {
                    tempU.operator()(j) = 1.0;
                }
            }
            tempU.operator/=(tempU.norm());
        } else {
            // Compute first tangent orthogonal to normal
            tempU << 1.0, 1.0, -(tempNorm(0) + tempNorm(1)) / tempNorm(2);
            tempU.operator/=(tempU.norm());
        }

        // Store first tangent
        tangentU.operator()(i, 0) = tempU(0);
        tangentU.operator()(i, 1) = tempU(1);
        tangentU.operator()(i, 2) = tempU(2);

        // Compute second tangent as cross product
        tempV = tempU.cross(tempNorm);
        tempV.operator/=(tempV.norm());
        tangentV.operator()(i, 0) = tempV(0);
        tangentV.operator()(i, 1) = tempV(1);
        tangentV.operator()(i, 2) = tempV(2);
    }
}

/*
 * Computes tangent directions for curve points:
 * - For closed curves: tangent is next_point - current_point
 * - For open curves: tangent is current_point - previous_point at endpoints
 */
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
                    // Last point connects to first point
                    double x2 = pd->GetPoint(0)[0];
                    double y2 = pd->GetPoint(0)[1];
                    double z2 = pd->GetPoint(0)[2];
                    double x1 = pd->GetPoint(i)[0];
                    double y1 = pd->GetPoint(i)[1];
                    double z1 = pd->GetPoint(i)[2];
                    outputU.operator()(i, 0) = x2 - x1;
                    outputU.operator()(i, 1) = y2 - y1;
                    outputU.operator()(i, 2) = z2 - z1;
                } else {
                    // Normal point - use next point
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
        } else if (m_curveType->at(id) == 0) {  // open curve
            for (int i = 0; i < pd->GetNumberOfPoints(); i++) {
                if (i == pd->GetNumberOfPoints() - 1) {
                    // Last point - use previous point
                    double x2 = pd->GetPoint(i - 1)[0];
                    double y2 = pd->GetPoint(i - 1)[1];
                    double z2 = pd->GetPoint(i - 1)[2];
                    double x1 = pd->GetPoint(i)[0];
                    double y1 = pd->GetPoint(i)[1];
                    double z1 = pd->GetPoint(i)[2];
                    outputU.operator()(i, 0) = x2 - x1;
                    outputU.operator()(i, 1) = y2 - y1;
                    outputU.operator()(i, 2) = z2 - z1;
                } else {
                    // Normal point - use next point
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

//Getters
double SlidingThread::GetBE() { return m_BEUpdated; }
double SlidingThread::GetScalingFactor() { return m_scaleFactor; }
double SlidingThread::GetRefinementLoop() { return m_noImprovementCounter; }
std::string SlidingThread::GetSolverType() { return m_solverType; }
bool SlidingThread::GetImprovement(){ return m_improvement;}
int SlidingThread::GetImprovementLoop(){return m_noImprovementCounter;}
// Destructor
SlidingThread::~SlidingThread() {}
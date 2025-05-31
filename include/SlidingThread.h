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

#ifndef SLIDINGTHREAD_H
#define SLIDINGTHREAD_H

#include <QObject>
#include <QThread>
#include <QtConcurrent/QtConcurrentRun>
// #include <QtConcurrent/QtConcurrentMap>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkAppendArcLength.h>
#include <vtkAutoInit.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkCellLocator.h>
#include <vtkCellPicker.h>
#include <vtkCenterOfMass.h>
#include <vtkCleanPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkCompositeDataGeometryFilter.h>
#include <vtkCompositePolyDataMapper2.h>
#include <vtkConnectivityFilter.h>
#include <vtkContourFilter.h>
#include <vtkContourTriangulator.h>
#include <vtkDataObjectTreeIterator.h>
#include <vtkDataSetMapper.h>
#include <vtkDecimatePro.h>
#include <vtkDelaunay3D.h>
#include <vtkDoubleArray.h>
#include <vtkExtractEdges.h>
#include <vtkExtractGeometry.h>
#include <vtkExtractPolyDataGeometry.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkGenericRenderWindowInteractor.h>
#include <vtkGeometryFilter.h>
#include <vtkGlyph3D.h>
#include <vtkGlyph3DMapper.h>
#include <vtkImplicitSelectionLoop.h>
#include <vtkInteractorStyleImage.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLabeledDataMapper.h>
#include <vtkLandmarkTransform.h>
#include <vtkLight.h>
#include <vtkMassProperties.h>
#include <vtkMeshQuality.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOBJWriter.h>
#include <vtkPlaneSource.h>
#include <vtkPointData.h>
#include <vtkPointGaussianMapper.h>
#include <vtkPointLocator.h>
#include <vtkPointPicker.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyDataPointSampler.h>
#include <vtkPolyLine.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSelectPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkSphereSource.h>
#include <vtkSplineFilter.h>
#include <vtkSurfaceReconstructionFilter.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkThinPlateSplineTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTubeFilter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkWindowedSincPolyDataFilter.h>

#include <Eigen/Dense>
#include <Eigen/Eigen>
#include <Eigen/Eigenvalues>
#include <Eigen/Sparse>
#include <QEventLoop>
#include <QFuture>
#include <QFutureWatcher>
#include <cmath>
#include <tuple>

class SlidingThread : public QThread {
    Q_OBJECT
private:
    Eigen::MatrixXd m_SMat;
    Eigen::MatrixXd m_K;
    Eigen::MatrixXd m_L;
    Eigen::MatrixXd m_Q;
    Eigen::MatrixXd m_BEMat;
    Eigen::MatrixXd m_coordinates;
    Eigen::MatrixXd m_templateCoordinates;
    vtkPolyData* m_meshData;
    vtkPolyData* m_templateMesh;
    vtkMultiBlockDataSet* m_curvePolyLineBlock;
    vtkMultiBlockDataSet* m_surfaceMaskBlock;
    int m_typeINOL = 0;
    int m_curveNOS = 0;
    int m_curveNOC = 1;
    int m_surfaceNOS = 0;
    int m_surfacePatchUNOS = 0;
    int m_surfacePatchVNOS = 0;
    int m_surfacePatchNOP = 1;
    double m_scaleFactor = 0;
    double m_BEUpdated = 0;

    std::vector<int>* m_curveType;
    //int m_currentCurveId;
    bool m_abort = false;

public:
    SlidingThread(vtkPolyData* meshData, int typeINOL, int curveNOS,
        int curveNOC, std::vector<int>* curveType,
        int m_surfaceNOS, int surfacePatchUNOS, int surfacePatchVNOS,
        int surfacePatchNOP, vtkMultiBlockDataSet* curvePolyLineBlock,
        vtkMultiBlockDataSet* surfaceMaskBlock,
        const Eigen::Ref<const Eigen::MatrixXd>& templateCoordinates,
        Eigen::MatrixXd& coordinates, int mode = 0);
    // SlidingThread();
    void run();
    void FinalizeDigitization();
    void AssembleU(Eigen::MatrixXd& targetLndmrks, Eigen::MatrixXd& outputU);
    void AssembleQ(Eigen::MatrixXd& templatePts, Eigen::MatrixXd& Q);
    void AssembleK(Eigen::MatrixXd& templatePts, Eigen::MatrixXd& K);
    void AssembleL(Eigen::MatrixXd& Q, Eigen::MatrixXd& K,
        Eigen::MatrixXd& OutputL);
    void BEMatrix(Eigen::MatrixXd& L, int numOfLandmarks,
        Eigen::MatrixXd& outputSMat, Eigen::MatrixXd& outputBEMat);
    double EucDist(double Ax, double Ay, double Az, double Bx, double By,
        double Bz);
    void PDist(vtkPoints* points, Eigen::MatrixXd& output);
    void PDist(Eigen::MatrixXd& points, Eigen::MatrixXd& output);
    void PGeoDist(vtkPolyData* mesh, Eigen::MatrixXd& points, Eigen::MatrixXd& output);
    void CalculateTangent(vtkPolyData* polyMesh, Eigen::MatrixXd& U,
        Eigen::MatrixXd& V);
    void CalculateCurveTangent(vtkMultiBlockDataSet* inputCurve, int id,
        Eigen::MatrixXd& outputU);
    void DebugPrintMatrix(Eigen::MatrixXd matrix);
    void KillNow();
    bool Killing();
    void SuperImpose(Eigen::MatrixXd& templatePts, Eigen::MatrixXd& targetPts);
    double Optimizer(const Eigen::MatrixXd& x);
    void NaiveRefinement(Eigen::MatrixXd& coordinates,
        const Eigen::MatrixXd& USUT, int numLNDMRK);
    double GetBE();
    double GetScalingFactor();
    ~SlidingThread();
signals:
    void CoordinateChanged(Eigen::MatrixXd coordinates);
    void CoordinateNotChanged(Eigen::MatrixXd coordinates);
};

#endif
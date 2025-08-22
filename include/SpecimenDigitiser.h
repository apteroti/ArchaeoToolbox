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

#ifndef SPECIMENDIGITISER_H
#define SPECIMENDIGITISER_H

#include <QVTKOpenGLWidget.h>
#include <math.h>
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
#include <vtkLight.h>
#include <vtkMassProperties.h>
#include <vtkMeshQuality.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOBJWriter.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricSpline.h>
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
#include <vtkXMLStructuredGridWriter.h>
#include <vtkKochanekSpline.h>
#include <vtkGenericCell.h>

#include <Eigen/Dense>
#include <Eigen/Eigen>
#include <Eigen/Eigenvalues>
#include <Eigen/Sparse>
#include <QFuture>
#include <QGroupBox>
#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include <QtCore/QPropertyAnimation>
#include <QtGui/QCloseEvent>
#include <QtGui/QSurfaceFormat>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGraphicsDropShadowEffect>
#include <QtWidgets/QGraphicsOpacityEffect>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolBar>
#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <limits>
#include <queue>
#include <set>
#include <unordered_map>
#include <vector>

#include "ExclusionPaint.h"
#include "MainWindow.fwd.h"
#include "Registration.h"
#include "SlidingThread.h"
#include "StatusReporterThread.h"
#include "SpinnerDialog.h"
#include "CuttingThread.h"
#include "SurfaceParameterisationThread.h"

class SpecimenDigitiser : public QMainWindow {
   private:
    // Data
    bool m_ignoreInside = true;
    int m_typeINOL = 0;
    int m_surfaceNOS = 0;
    int m_surfacePatchUNOS = 0;
    int m_surfacePatchVNOS = 0;
    int m_surfacePatchNOP = 1;
    int m_curveNOS = 0;
    int m_curveNOC = 1;
    int m_currentCurveId = 0;
    int m_currentSurfaceId = 0;
    bool m_editableSurface = 1;
    bool m_dataDigitized = 0;
    bool m_surfaceChanged = 0;
    double m_sizeConstant = 0;
    double m_splineTension = 0.5;  // Range: -1.0 to 1.0 (higher = flatter)
    double m_diagonal = 0;
    const double m_tubeRadRatio = 0.3 / 170.304;
    const double m_arrowSizeRatio = 10 / 170.304;
    Registration *m_regPlot = nullptr;
    SlidingThread *m_slidingThread = nullptr;
    StatusReporterThread *m_slidingStatThread = nullptr;
    std::vector<int> *m_curveType = nullptr;
    vtkPolyData *m_meshData;
    vtkSmartPointer<vtkIntArray> m_fixedPtsIds;
    std::vector<std::vector<int> *> *m_surfacePtsIds = nullptr;
    std::vector<std::vector<int> *> *m_curvePtsIds = nullptr;
    // Template data
    vtkSmartPointer<vtkPolyData> m_templateMesh = nullptr;
    vtkSmartPointer<vtkPoints> m_templateSurfaceSliders = nullptr;
    vtkSmartPointer<vtkPoints> m_templateTypeI = nullptr;
    vtkSmartPointer<vtkMultiBlockDataSet> m_templatePatchSurfaceSliders =
        nullptr;
    vtkSmartPointer<vtkMultiBlockDataSet> m_templateCurveSliders = nullptr;

    MainWindow *m_parent;
    ExclusionPaint *m_exclusionPainter = nullptr;
    //
    QLineEdit *typeILineEdit;
    QLineEdit *curveLineEditNOC;
    QLineEdit *surfaceLineEditNOP;
    QLabel *counterFixedLabel;
    QLabel *counterCurveNOCLabel;
    QLabel *counterSurfaceNOPLabel;
    QHBoxLayout *counterLayout;
    // Rendering
    QVTKOpenGLWidget *m_vtkRenderWidget;
    vtkSmartPointer<vtkRenderer> m_renderer;
    vtkSmartPointer<vtkRenderWindow> m_renWin;
    vtkSmartPointer<vtkGenericRenderWindowInteractor> m_iren;
    vtkSmartPointer<vtkActor> m_meshActor;
    vtkSmartPointer<vtkActor> m_fixedPointActor;
    vtkSmartPointer<vtkVertexGlyphFilter> m_fixedVertexFilter;
    vtkSmartPointer<vtkPoints> m_fixedLandmarks;  // this is fixed landmarks
    vtkSmartPointer<vtkPolyData> m_fixedPointsPoly;
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> m_PointPickerStyle;
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> m_PointMoverStyle;
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> m_style;
    vtkSmartPointer<vtkInteractorStyleImage> m_2Dstyle;
    vtkSmartPointer<vtkPointPicker> m_grabPicker;
    vtkSmartPointer<vtkPolyData> m_cosmeticCurvePoly;
    vtkSmartPointer<vtkActor> m_largestDiamTubeActor;
    //------
    vtkSmartPointer<vtkActor> m_cutMeshActor;
    vtkSmartPointer<vtkPoints>
        m_surfaceLandmarks;  // this is for whole surface scenario
    vtkSmartPointer<vtkPoints> m_surfaceCurveHighlightCtrlPoints;
    vtkSmartPointer<vtkActor> m_surfaceCurveCtrlPointActor;
    vtkSmartPointer<vtkActor> m_surfaceCurveActor;
    vtkSmartPointer<vtkVertexGlyphFilter> m_surfaceCurveCtrlVertexFilter;
    vtkSmartPointer<vtkTubeFilter> m_surfaceCurveTubeFilter;
    vtkSmartPointer<vtkTubeFilter> m_surfaceCurveTubeFilterDeactive;
    vtkSmartPointer<vtkActor> m_surfaceCurveActorDeactive;
    vtkSmartPointer<vtkPolyData> m_surfaceCurveCtrlPointsPoly;
    vtkSmartPointer<vtkPolyData> m_surfaceCurvePoly;
    vtkSmartPointer<vtkActor> m_surfacePointActor;
    vtkSmartPointer<vtkVertexGlyphFilter> m_surfaceVertexFilter;
    vtkSmartPointer<vtkPoints> m_surfacePatchLandmarks;
    vtkSmartPointer<vtkVertexGlyphFilter> m_surfacePatchVertexFilter;
    vtkSmartPointer<vtkActor> m_surfacePatchPointActor;
    vtkSmartPointer<vtkVertexGlyphFilter> m_surfacePatchVertexDeactiveFilter;
    vtkSmartPointer<vtkActor> m_surfacePatchPointDeactiveActor;
    vtkSmartPointer<vtkVertexGlyphFilter> m_surfaceCurveVertexDeactiveFilter;
    vtkSmartPointer<vtkActor> m_surfaceCurveCtrlPtsDeactiveActor;
    vtkSmartPointer<vtkPoints> m_surfaceSliderPtsTotal;
    vtkSmartPointer<vtkPoints> m_surfaceLabelPoints;
    vtkSmartPointer<vtkVertexGlyphFilter> m_surfaceLabelVertexFilter;
    vtkSmartPointer<vtkVertexGlyphFilter> m_surfaceCtrlVertexFilter;
    vtkSmartPointer<vtkActor> m_surfaceCtrlPointActor;
    vtkSmartPointer<vtkPolyData> m_surfaceCtrlPointsPoly;
    vtkSmartPointer<vtkTubeFilter> m_surfaceTubeFilter;
    vtkSmartPointer<vtkActor> m_surfaceEdgeActor;
    vtkSmartPointer<vtkVertexGlyphFilter> m_surfacePatchVertexActiveFilter;
    vtkSmartPointer<vtkActor> m_surfacePatchPointActiveActor;
    vtkSmartPointer<vtkActor> m_surfacePointDeactiveActor;
    vtkSmartPointer<vtkVertexGlyphFilter> m_surfaceVertexDeactiveFilter;

    vtkSmartPointer<vtkActor> m_surfaceArrowActor;
    vtkSmartPointer<vtkGlyph3D> m_glyphSurfaceArrow;
    vtkSmartPointer<vtkPolyData> m_surfaceMask;

    vtkSmartPointer<vtkMultiBlockDataSet> m_surfaceBlock;
    vtkSmartPointer<vtkMultiBlockDataSet> m_surfaceCurveCtrlBlock;
    vtkSmartPointer<vtkMultiBlockDataSet> m_surfaceCurveBlock;
    vtkSmartPointer<vtkMultiBlockDataSet> m_surfaceMaskBlock;
    vtkSmartPointer<vtkMultiBlockDataSet> m_surfaceCtrlPtsBlock;
    //-------
    vtkSmartPointer<vtkActor> m_curvectrlPointActor;
    vtkSmartPointer<vtkVertexGlyphFilter> m_curvectrlVertexFilter;
    vtkSmartPointer<vtkPolyData> m_curvectrlPointsPoly;
    vtkSmartPointer<vtkPolyData> m_curvePoly;
    vtkSmartPointer<vtkActor> m_curveActor;
    vtkSmartPointer<vtkTubeFilter> m_curveTubeFilter;
    vtkSmartPointer<vtkTubeFilter> m_curveTubeFilterDeactive;
    vtkSmartPointer<vtkActor> m_curveActorDeactive;
    vtkSmartPointer<vtkActor> m_curvePointActor;
    vtkSmartPointer<vtkVertexGlyphFilter> m_curveVertexFilter;
    vtkSmartPointer<vtkPolyData> m_curvePointsPoly;
    vtkSmartPointer<vtkPoints> m_curveLabelPoints;
    vtkSmartPointer<vtkVertexGlyphFilter> m_curveLabelVertexFilter;
    vtkSmartPointer<vtkActor> m_curvePointActorDeactive;
    vtkSmartPointer<vtkVertexGlyphFilter> m_curveVertexFilterDeactive;
    vtkSmartPointer<vtkPoints> m_curveHighlightCtrlPoints;
    vtkSmartPointer<vtkPoints> m_curveLandmarks;
    vtkSmartPointer<vtkPoints> m_curveTotalLandmarks;
    vtkSmartPointer<vtkVertexGlyphFilter> m_curveVertexFilterActive;
    vtkSmartPointer<vtkActor> m_curvePointActorActive;

    vtkSmartPointer<vtkActor> m_curveArrowActor;
    vtkSmartPointer<vtkGlyph3D> m_glyphCurveArrow;

    vtkSmartPointer<vtkMultiBlockDataSet> m_curveCtrlBlock;
    vtkSmartPointer<vtkMultiBlockDataSet> m_curveBlock;
    vtkSmartPointer<vtkMultiBlockDataSet> m_curvePolyLineBlock;
    vtkSmartPointer<vtkActor> m_meshBoundActor;
    vtkSmartPointer<vtkPolyData> m_meshBoundaries;

    // Toolbar
    QToolBar *mainToolbar;
    QToolBar *fixedLandmarkToolbar;
    QToolBar *surfacePatchToolbar;
    QToolBar *curveToolbar;
    QLabel *statusLabel;
    QLabel *progressLabel;
    QGroupBox *counterGroup;
    QLineEdit *progressLineEdit;
    // box
    QComboBox *curveSelectComboBox;
    QComboBox *surfaceSelectComboBox;
    QComboBox *fromSurfaceComboBox;
    QComboBox *fromCurveComboBox;
    QSpinBox *pointSizeSpinBox;
    QSpinBox *lineSizeSpinBox;
    QCheckBox *showDiameterBox;
    QCheckBox *pickFromBoundariesBox;
    // Buttons
    QPushButton *landmarkButton;
    QPushButton *surfaceSliderButton;
    QPushButton *curveSliderButton;
    QPushButton *curveAddButton;
    QPushButton *curveTypeButton;
    QPushButton *curveClearButton;
    QPushButton *surfaceAddButton;
    QPushButton *surfaceLockButton;
    QPushButton *surfaceClearButton;
    QPushButton *surfaceGrabButton;
    QPushButton *slidingButton;
    QPushButton *surfaceInterpolateButton;
    QPropertyAnimation *m_interpolationAnimation;
    QPropertyAnimation *m_grabAnimation;
    QPropertyAnimation *m_slidingAnimation;
    // Functions
    
    void PrepareSliding();
    // Interaction
    void PickFunc(vtkObject *caller, long unsigned int eventId, void *callData);
    void resetLeftClck(vtkObject *caller, long unsigned int eventId,
                       void *callData);
    void MoveFunc(vtkObject *caller, long unsigned int eventId, void *callData);
    void resetMouseMove(vtkObject *caller, long unsigned int eventId,
                        void *callData);
    void CoordinateFunc(vtkObject *caller, long unsigned int eventId,
                        void *callData);
    void DebugPrintMatrix(Eigen::MatrixXd matrix);

   public:
    SpecimenDigitiser(vtkPolyData *data, MainWindow *parent);
    void ResetCurveScene();
    void ResetSurfaceScene();
    void UpdateCurveScene(int id);
    void UpdateSurfaceScene(int id);
    void FinalizeCurveScene();
    void FinalizeSurfaceScene();
    void UpdateCurveData(vtkPoints *pts, vtkPolyData *outputLine,
                         vtkPoints *outputPoints, vtkPolyData *baseMesh);
    void ConstructSurfaceData(vtkPoints *pts, vtkPoints *outputSliders,
                              vtkPolyData *outputCtrlPtsPoly);
    void ConstructSurfaceData(vtkPolyData *CtrlPtsPoly,
                              vtkPoints *outputSliders);
    void Plot();
    double EucDist(double Ax, double Ay, double Az, double Bx, double By,
                   double Bz);
    void PDist(vtkPoints *points, Eigen::MatrixXd &output);
    void PDist(Eigen::MatrixXd &points, Eigen::MatrixXd &output);
    void ProjectOnMesh(vtkPoints *points, vtkPolyData *mask);
    void ProjectOnMesh(vtkPolyData *Poly, vtkPolyData *mask,
                       std::vector<int> *ids = nullptr);
    void MakeArrow(vtkPolyData *inputMesh,
                   vtkMultiBlockDataSet *inputCurveBlock, int liftScale,
                   vtkPolyData *output);
    void FinalizeDigitization(Eigen::MatrixXd &Lndmrks, bool sendOffData);
    void CosmeticCurve(vtkPoints *ctrlPts, vtkPolyData *outputCurve);
    void MeshCutter(vtkPoints *pts);
    void OutlineIdFinder(int u, int v, std::vector<int> *output);
    void MakeCage(vtkPoints *pts, vtkPolyData *outPlanePoly);
    void SlidingStatus();
    void DrawDiameter(vtkPoints *meshPoints);
    void SetSurfaceSlider(vtkPoints *pts);
    void FlipSurfaceButton();
    int GetTemplateNOL();
    vtkPoints *GetTemplateTypeI();
    vtkPoints *GetTypeI();
    void GetTemplateCurveSliders(vtkPoints *Output);
    void GetCurveSliders(vtkPoints *Output);
    bool GetIgnorSetting();
    void GetPlaneBoundaryPoints(vtkPolyData *plane, vtkPoints *boundaryPoints);
    void UpdateSurfaceDirection();
    void ResetPatch();
    ~SpecimenDigitiser();

   public slots:
    void OnCoordinateChanged(Eigen::MatrixXd sendOffData);
    void OnCoordinateNotChanged(Eigen::MatrixXd sendOffData);
    void OnStatusChanged(int status);
    void TypeITool();
    void SurfaceTool();
    void CurveTool();
    void ShowLargestDiameter();
    void AddCurve();
    void AddSurface();
    void ChangeCurveType();
    void ChangeSurfaceLock();
    void ChangeCurve(int index);
    void PickFromBoundaries();
    void ClearCurve();
    void ClearSurface();
    void MakeSlide();
    void DrapeToSurface();
    void InterpolateSurface();
    void ChangeSurface(int index);
    void ChangeCurveSource(int index);
    void ChangeSurfaceSource(int index);
    void ChangePointSize(int index);
    void ChangeLineSize(int index);

   protected:
    void keyReleaseEvent(QKeyEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
};

#endif
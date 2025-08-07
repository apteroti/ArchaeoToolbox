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

#ifndef TEMPLATEDIGITISER_H
#define TEMPLATEDIGITISER_H

#include <QVTKOpenGLWidget.h>
#include <QtWidgets/qstatusbar.h>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkAppendArcLength.h>
#include <vtkAppendPolyData.h>
#include <vtkArrowSource.h>
#include <vtkAutoInit.h>
#include <vtkBooleanOperationPolyDataFilter.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkCellIterator.h>
#include <vtkCellLocator.h>
#include <vtkCellPicker.h>
#include <vtkCenterOfMass.h>
#include <vtkCleanPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkColorSeries.h>
#include <vtkColorTransferFunction.h>
#include <vtkCompositeDataGeometryFilter.h>
#include <vtkCompositePolyDataMapper2.h>
#include <vtkConnectivityFilter.h>
#include <vtkContourFilter.h>
#include <vtkContourTriangulator.h>
#include <vtkDataObjectTreeIterator.h>
#include <vtkDataSetMapper.h>
#include <vtkDecimatePro.h>
#include <vtkDelaunay3D.h>
#include <vtkDijkstraGraphGeodesicPath.h>
#include <vtkDoubleArray.h>
#include <vtkExtractEdges.h>
#include <vtkExtractGeometry.h>
#include <vtkExtractPolyDataGeometry.h>
#include <vtkFeatureEdges.h>
#include <vtkFloatArray.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkGenericRenderWindowInteractor.h>
#include <vtkGeometryFilter.h>
#include <vtkGlyph3D.h>
#include <vtkGlyph3DMapper.h>
#include <vtkImplicitPolyDataDistance.h>
#include <vtkImplicitSelectionLoop.h>
#include <vtkInformation.h>
#include <vtkIntArray.h>
#include <vtkInteractorStyleImage.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkIntersectionPolyDataFilter.h>
#include <vtkKdTreePointLocator.h>
#include <vtkLabeledDataMapper.h>
#include <vtkLight.h>
#include <vtkLine.h>
#include <vtkLineSource.h>
#include <vtkLookupTable.h>
#include <vtkMaskPoints.h>
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
#include <vtkPointDensityFilter.h>
#include <vtkPointGaussianMapper.h>
#include <vtkPointLocator.h>
#include <vtkPointPicker.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyDataPointSampler.h>
#include <vtkPolyLine.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSCurveSpline.h>
#include <vtkSelectEnclosedPoints.h>
#include <vtkSelectPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkSortDataArray.h>
#include <vtkSphereSource.h>
#include <vtkSplineFilter.h>
#include <vtkStaticPointLocator.h>
#include <vtkStripper.h>
#include <vtkSurfaceReconstructionFilter.h>
#include <vtkTable.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkThinPlateSplineTransform.h>
#include <vtkThreshold.h>
#include <vtkThresholdPoints.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTubeFilter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkXMLMultiBlockDataWriter.h>
#include <vtkXMLPolyDataWriter.h>

#include <Eigen/Dense>
#include <Eigen/Eigen>
#include <QGroupBox>
#include <QProgressBar>
#include <QtCore/QPropertyAnimation>
#include <QtGui/QCloseEvent>
#include <QtGui/QSurfaceFormat>
#include <QtWidgets/QAction>
#include <QtWidgets/QCheckBox>
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
#include <QtWidgets/QShortcut>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolBar>
#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <limits>
#include <queue>
#include <random>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "BlueNoiseThread.h"
#include "ProSetMenu.fwd.h"
#include "StatusReporterThread.h"

class TemplateDigitiser : public QMainWindow {
   private:
    struct AStarNode {
        vtkIdType vertexId;
        double fScore;
        bool operator>(const AStarNode &other) const {
            return fScore > other.fScore;
        }
    };
    // Data
    QMutex *m_mutex;
    bool m_ignoreInside = 1;
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
    bool m_surfaceChanged = 0;

    vtkSmartPointer<vtkIntArray> m_maskArray;
    int m_brushSize = 1;
    bool m_mouseIsClicked = 0;

    std::vector<int> *m_curveType = nullptr;
    vtkPolyData *m_meshData;
    vtkSmartPointer<vtkIntArray> m_fixedPtsIds;
    std::vector<std::vector<int> *> *m_surfacePtsIds = nullptr;

    BlueNoiseThread *m_BlueNoiseThread = nullptr;
    StatusReporterThread *m_statThread = nullptr;

    ProSetMenu *m_parent;
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
    vtkSmartPointer<vtkPoints>
        m_fixedLandmarks;  // this is type I or II landmarks
    vtkSmartPointer<vtkPolyData> m_fixedPointsPoly;
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> m_PointPickerStyle;
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> m_PointPainterStyle;
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> m_PointMoverStyle;
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> m_style;
    vtkSmartPointer<vtkInteractorStyleImage> m_2Dstyle;
    vtkSmartPointer<vtkPointPicker> m_grabPicker;
    vtkSmartPointer<vtkActor> m_largestDiamTubeActor;

    vtkSmartPointer<vtkPolyData> m_cosmeticCurvePoly;
    //------
    vtkSmartPointer<vtkActor> m_cutMeshActor;
    vtkSmartPointer<vtkPoints>
        m_surfaceLandmarks;  // this is for whole surface scenario
    vtkSmartPointer<vtkPoints> m_surfaceCurveHighlightCtrlPoints;
    vtkSmartPointer<vtkActor> m_surfaceCurveCtrlPointActor;
    vtkSmartPointer<vtkActor> m_surfaceCtrlPointActor;
    vtkSmartPointer<vtkActor> m_surfaceCurveActor;
    vtkSmartPointer<vtkVertexGlyphFilter> m_surfaceCtrlVertexFilter;
    vtkSmartPointer<vtkVertexGlyphFilter> m_surfaceCurveCtrlVertexFilter;
    vtkSmartPointer<vtkTubeFilter> m_surfaceTubeFilter;
    vtkSmartPointer<vtkTubeFilter> m_surfaceCurveTubeFilter;
    vtkSmartPointer<vtkPolyData> m_surfaceCtrlPointsPoly;
    vtkSmartPointer<vtkPolyData> m_surfaceCurveCtrlPointsPoly;
    vtkSmartPointer<vtkPolyData> m_surfaceCurvePoly;
    vtkSmartPointer<vtkPolyData> m_surfaceCtrlEdgesPoly;
    vtkSmartPointer<vtkActor> m_surfacePointActor;
    vtkSmartPointer<vtkActor> m_surfaceEdgeActor;
    vtkSmartPointer<vtkVertexGlyphFilter> m_surfaceVertexFilter;
    vtkSmartPointer<vtkPolyData> m_surfacePointsPoly;
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
    vtkSmartPointer<vtkTubeFilter> m_surfaceCurveTubeFilterDeactive;
    vtkSmartPointer<vtkActor> m_surfaceCurveActorDeactive;
    vtkSmartPointer<vtkPolyData> m_surfaceMask;
    vtkSmartPointer<vtkMultiBlockDataSet> m_surfaceMaskBlock;
    vtkSmartPointer<vtkActor> m_surfaceArrowActor;
    vtkSmartPointer<vtkGlyph3D> m_glyphSurfaceArrow;

    vtkSmartPointer<vtkMultiBlockDataSet> m_surfaceBlock;
    vtkSmartPointer<vtkMultiBlockDataSet> m_surfaceCtrlPtsBlock;
    vtkSmartPointer<vtkMultiBlockDataSet> m_surfaceCurveCtrlBlock;
    vtkSmartPointer<vtkMultiBlockDataSet> m_surfaceCurveBlock;
    //-------
    vtkSmartPointer<vtkActor> m_curvectrlPointActor;
    vtkSmartPointer<vtkVertexGlyphFilter> m_curvectrlVertexFilter;
    vtkSmartPointer<vtkPolyData> m_curvectrlPointsPoly;
    vtkSmartPointer<vtkPolyData> m_curvePoly;
    vtkSmartPointer<vtkActor> m_curveActor;
    vtkSmartPointer<vtkTubeFilter> m_curveTubeFilter;
    vtkSmartPointer<vtkActor> m_curvePointActor;
    vtkSmartPointer<vtkVertexGlyphFilter> m_curveVertexFilter;
    vtkSmartPointer<vtkPolyData> m_curvePointsPoly;
    vtkSmartPointer<vtkPoints> m_curveLabelPoints;
    vtkSmartPointer<vtkVertexGlyphFilter> m_curveLabelVertexFilter;
    vtkSmartPointer<vtkActor> m_curvePointActorDeactive;
    vtkSmartPointer<vtkVertexGlyphFilter> m_curveVertexFilterDeactive;
    vtkSmartPointer<vtkPoints> m_curveHighlightCtrlPoints;
    vtkSmartPointer<vtkPoints> m_curveLandmarks;
    vtkSmartPointer<vtkPoints>
        m_curveTotalLandmarks;  // this is the total curve semi-landmarks
    vtkSmartPointer<vtkActor> m_curveActorDeactive;
    vtkSmartPointer<vtkTubeFilter> m_curveTubeFilterDeactive;
    vtkSmartPointer<vtkActor> m_curveArrowActor;
    vtkSmartPointer<vtkGlyph3D> m_glyphCurveArrow;
    vtkSmartPointer<vtkMultiBlockDataSet> m_curveCtrlBlock;
    vtkSmartPointer<vtkMultiBlockDataSet> m_curveBlock;
    vtkSmartPointer<vtkMultiBlockDataSet> m_curvePolyLineBlock;
    vtkSmartPointer<vtkActor> m_meshBoundActor;
    vtkSmartPointer<vtkPolyData> m_meshBoundaries;

    double m_sizeConstant = 0;

    // Toolbar
    QToolBar *mainToolbar;
    QToolBar *fixedLandmarkToolbar;
    QToolBar *surfacePatchToolbar;
    QToolBar *surfaceToolbar;
    QToolBar *curveToolbar;
    QLabel *statusLabel;
    QLabel *progressLabel;
    // QProgressBar *statusProgressBar;
    QGroupBox *counterGroup;
    // box
    QComboBox *curveSelectComboBox;
    QComboBox *surfaceSelectComboBox;
    QComboBox *curvePickSourceComboBox;
    QComboBox *surfacePickSourceComboBox;
    QSpinBox *pointSizeSpinBox;
    QSpinBox *lineSizeSpinBox;
    QSpinBox *brushSizeSpinBox;
    QCheckBox *showDiameterBox;
    QCheckBox *pickFromBoundariesBox;
    QCheckBox *ignoreIntCheckbox;
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
    QPushButton *surfaceIronButton;
    QPushButton *surfaceAnchorButton;
    QPushButton *surfacePaintButton;
    QPushButton *surfaceResampleButton;
    QPushButton *surfaceInterpolateButton;
    QPropertyAnimation *m_interpolationAnimation;
    QPropertyAnimation *m_ironAnimation;
    // Functions
    void TypeITool();
    void SurfaceTool();
    void CurveTool();
    // Interaction
    void PickFunc(vtkObject *caller, long unsigned int eventId, void *callData);
    void resetLeftClck(vtkObject *caller, long unsigned int eventId,
                       void *callData);
    void MoveFunc(vtkObject *caller, long unsigned int eventId, void *callData);
    void PaintFunc(vtkObject *caller, long unsigned int eventId,
                   void *callData);
    void resetMouseMove(vtkObject *caller, long unsigned int eventId,
                        void *callData);
    void CoordinateFunc(vtkObject *caller, long unsigned int eventId,
                        void *callData);
    void MakeCage(vtkPoints *pts, vtkPolyData *outPlanePoly);

    void NeighborFinder(int initId, int brushSize, std::vector<int> &outList);

   public:
    TemplateDigitiser(ProSetMenu *parent);
    void SetPoly(vtkPolyData *poly);
    void ResetCurveScene();
    void ResetSurfaceScene();
    void AddCurve();
    void AddSurface();
    void ChangeCurve(int index);
    void ChangeCurveSource(int index);
    void ChangeSurfaceSource(int index);
    void ChangeSurface(int index);
    void ChangeCurveType();
    void ChangeCurveTypeAuto();
    void ChangeSurfaceLock();
    void ClearCurve();
    void ClearSurface();
    void DrapeToSurface();
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
    void Resample(int resolution, vtkPolyData *mesh);
    double GetMeshCellArea(std::vector<double> *probab,
                           vtkPolyData *inputMresh);
    void RandomChoice(vtkPolyData *mesh, int outputSize,
                      std::vector<double> *probab, std::vector<int> *idList,
                      vtkPoints *points);
    double RandomFloat(double maximum);
    double EucDist(double Ax, double Ay, double Az, double Bx, double By,
                   double Bz);
    void ProjectOnMesh(vtkPoints *points);
    void ProjectOnMesh(vtkPoints *points, vtkPolyData *mask);
    void ProjectOnMesh(vtkPolyData *Poly, vtkPolyData *mask,
                       std::vector<int> *ids = nullptr);
    void MakeArrow(vtkPolyData *inputMesh,
                   vtkMultiBlockDataSet *inputCurveBlock, int liftScale,
                   vtkPolyData *output);
    void CosmeticCurve(vtkPoints *ctrlPts, vtkPolyData *outputCurve);
    void MeshCutter(vtkPoints *pts);
    void CutMeshWithCurve(vtkPolyData *inputMesh, vtkPoints *curvePoints,
                          vtkPolyData *outputCutMesh);
    void GetCutterCurve(vtkPolyData *Poly, vtkPoints *curvePts,
                        vtkIdList *outCurveIds);
    void DijkstraEdgeSearch(vtkPolyData *mesh, vtkPolyData *closedCurve,
                            vtkIdList *edgePointIds);
    void AStarEdgeSearch(vtkPolyData *mesh, vtkPolyData *closedCurve,
                         vtkIdList *edgePointIds);
    void ChangePointSize(int index);
    void ChangeLineSize(int index);
    void OutlineIdFinder(int u, int v, std::vector<int> *output);
    void ShowLargestDiameter();
    void DrawDiameter(vtkPoints *meshPoints);
    void PDist(vtkPoints *points, Eigen::MatrixXd &output);
    void DebugPrintMatrix(Eigen::MatrixXd matrix);
    void PickFromBoundaries();
    void BrushTool();
    void ChangeBrushSize(int index);
    void PaintMouseClicked();
    void PaintMouseReleased();
    void SelectAll();
    void DeSelectAll();
    void IgnoreInternalStat();
    void PoissonDisk(vtkPolyData *inputPoly, vtkPoints *fixedLm,
                     vtkPoints *curveSliders, double area, int resol,
                     vtkPoints *outPutPts);
    void OnSamplingIsDone();
    void PrepareBNSampling(bool stat);
    void RunStatThread(QThread *thread);
    void OnStatusChanged(int status);
    void PrepareClosing(QCloseEvent *event);
    void GetPlaneBoundaryPoints(vtkPolyData *plane, vtkPoints *boundaryPoints);
    void InterpolateSurface();
    ~TemplateDigitiser();

   protected:
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
};

#endif
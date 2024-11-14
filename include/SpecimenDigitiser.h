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

#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QAction>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QAction>
#include <QtGui/QCloseEvent>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QHBoxLayout>
#include <QtGui/QSurfaceFormat>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QGraphicsDropShadowEffect>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QVTKOpenGLWidget.h>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QThread>
#include <QGroupBox>

#include <vtkGenericRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkLight.h>
#include <vtkCamera.h>
#include <vtkActor2D.h>
#include <vtkTextActor.h>
#include <vtkProperty2D.h>
#include <vtkTextProperty.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkInteractorStyleImage.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkNew.h>
#include <vtkCellPicker.h>
#include <vtkPointPicker.h>
#include <vtkPointGaussianMapper.h>
#include <vtkGlyph3DMapper.h>
#include <vtkGlyph3D.h>
#include <vtkSphereSource.h>
#include <vtkMassProperties.h>
#include <vtkLabeledDataMapper.h>
#include <vtkDataSetMapper.h>
#include <vtkNamedColors.h>
#include <vtkSurfaceReconstructionFilter.h>
#include <vtkContourFilter.h>
#include <vtkCleanPolyData.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkSelectPolyData.h>
#include <vtkContourTriangulator.h>
#include <vtkClipPolyData.h>
#include <vtkMeshQuality.h>
#include <vtkDoubleArray.h>
#include <vtkCellData.h>
#include <vtkPolyDataPointSampler.h>
#include <vtkDecimatePro.h>
#include <vtkPointLocator.h>
#include <vtkDelaunay3D.h>
#include <vtkUnstructuredGrid.h>
#include <vtkGeometryFilter.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkCompositeDataGeometryFilter.h>
#include <vtkExtractEdges.h>
#include <vtkDataObjectTreeIterator.h>
#include <vtkCellArray.h>
#include <vtkCompositePolyDataMapper2.h>
#include <vtkPlaneSource.h>
#include <vtkAutoInit.h>
#include <vtkOBJWriter.h>
#include <vtkImplicitSelectionLoop.h>
#include <vtkExtractPolyDataGeometry.h>
#include <vtkAppendArcLength.h>
#include <vtkTubeFilter.h>
#include <vtkPolyLine.h>
#include <vtkConnectivityFilter.h>
#include <vtkCellLocator.h>
#include <vtkThinPlateSplineTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkCenterOfMass.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkSplineFilter.h>
#include <vtkExtractGeometry.h>
#include <vtkPolyDataNormals.h>
#include <vtkParametricSpline.h>
#include <vtkParametricFunctionSource.h>
#include <vtkXMLStructuredGridWriter.h>

#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <Eigen/Eigen>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/Eigenvalues>
#include "MainWindow.fwd.h"
#include "SlidingThread.h"
#include "StatusReporterThread.h"
#include "Registration.h"

#include <TColgp_HArray1OfPnt.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <STEPControl_Writer.hxx>
#include <Interface_EntityIterator.hxx>
#include <TopoDS_Compound.hxx>
#include <BRep_Builder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <BRepFill_Filling.hxx>



class SpecimenDigitiser : public QMainWindow
{
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
    int m_nurbsResolution = 4;
    bool m_editableSurface = 1;
    bool m_dataDigitized = 0;
    bool m_surfaceChanged = 0;
    Registration *m_regPlot = nullptr;
    SlidingThread *m_slidingThread = nullptr;
    StatusReporterThread *m_slidingStatThread = nullptr;
    
    vtkSmartPointer<vtkPolyData> m_cutMeshData;
    //int m_dynamicRes =0;
    
    std::vector<int> *m_curveType = nullptr;
    vtkPolyData *m_meshData;
    vtkSmartPointer<vtkIntArray> m_fixedPtsIds;
    // vtkSmartPointer<vtkIntArray> m_curvePtsIds;
    std::vector<std::vector<int> *> *m_surfacePtsIds = nullptr;
    // Template data
    vtkSmartPointer<vtkPolyData> m_templateMesh = nullptr;
    vtkSmartPointer<vtkPoints> m_templateSurfaceSliders = nullptr;
    vtkSmartPointer<vtkPoints> m_templateTypeI = nullptr;
    vtkSmartPointer<vtkMultiBlockDataSet> m_templatePatchSurfaceSliders = nullptr;
    vtkSmartPointer<vtkMultiBlockDataSet> m_templateCurveSliders = nullptr;

    MainWindow *m_parent;
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
    vtkSmartPointer<vtkPoints> m_fixedHighlightPoints; //this is fixed landmarks
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
    vtkSmartPointer<vtkPoints> m_surfaceHighlightPoints; // this is for whole surface scenario
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
    vtkSmartPointer<vtkPoints> m_surfacePatchHighlightPoints;
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
    vtkSmartPointer<vtkPolyData> m_surfaceMaskPoly;

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
    vtkSmartPointer<vtkPoints> m_curveHighlightPoints;
    vtkSmartPointer<vtkPoints> m_curveHighlightPointsTotal;
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
    //QToolBar *typeIToolbar;
    QToolBar *landmarkToolbar;
    QToolBar *surfaceToolbar;
    QToolBar *curveToolbar;
    QLabel *statusLabel;
    QLabel *progressLabel;
    QGroupBox *counterGroup;
    QLineEdit *progressLineEdit;
    // box
    QComboBox *curveSelectComboBox;
    QComboBox *surfaceSelectComboBox;
    QComboBox *curvePickSourceComboBox;
    QComboBox *surfacePickSourceComboBox;
    QComboBox *cyclePatchesComboBox;
    QSpinBox * pointSizeSpinBox;
    QSpinBox * lineSizeSpinBox;
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
    QPushButton *surfaceIronButton;
    QPushButton *slidingButton;
    // Functions
    void TypeITool();
    void SurfaceTool();
    void CurveTool();
    void PrepareSliding();
    void DrapeToSurface();
    void MakeSlide();
    
    // Interaction
    void PickFunc(vtkObject *caller, long unsigned int eventId, void *callData);
    void resetLeftClck(vtkObject *caller, long unsigned int eventId, void *callData);
    void MoveFunc(vtkObject *caller, long unsigned int eventId, void *callData);
    void resetMouseMove(vtkObject *caller, long unsigned int eventId, void *callData);
    void CoordinateFunc(vtkObject *caller, long unsigned int eventId, void *callData);
    void DebugPrintMatrix(Eigen::MatrixXd matrix);

public:
    SpecimenDigitiser(vtkPolyData *data, MainWindow *parent);
    void ResetCurveScene();
    void ResetSurfaceScene();
    void AddCurve();
    void AddSurface();
    void ChangeCurve(int index);
    void ChangeCurveSource(int index);
    void ChangeSurface(int index);
    void ChangeSurfaceSource(int index);
    void ChangeCurveType();
    void ChangeSurfaceLock();
    void ClearCurve();
    void ClearSurface();
    void UpdateCurveScene(int id);
    void UpdateSurfaceScene(int id);
    void FinalizeCurveScene();
    void FinalizeSurfaceScene();
    void UpdateCurveData(vtkPoints *pts, vtkPolyData *outputLine, vtkPoints *outputPoints, vtkPolyData *baseMesh);
    void ConstructSurfaceData(vtkPoints *pts, vtkPoints *outputSliders, vtkPolyData *outputCtrlPtsPoly, vtkPolyData *outputCurvePoly, int resolution);
    void ConstructSurfaceData(vtkPolyData *CtrlPtsPoly, vtkPoints *outputSliders);
    void Plot();
    double EucDist(double Ax, double Ay, double Az, double Bx, double By, double Bz);
    void PDist(vtkPoints *points, Eigen::MatrixXd &output);
    void PDist(Eigen::MatrixXd &points, Eigen::MatrixXd &output);
    void ProjectOnMesh(vtkPoints *points, vtkPolyData *mask);
    void ProjectOnMesh(vtkPolyData *Poly, vtkPolyData *mask, std::vector<int> *ids = nullptr);
    void MakeArrow(vtkPolyData *inputMesh, vtkMultiBlockDataSet *inputCurveBlock, int liftScale, vtkPolyData *output);
    void FinalizeDigitization(Eigen::MatrixXd &Lndmrks, bool sendOffData);
    void CosmeticCurve(vtkPoints *ctrlPts, vtkPolyData *outputCurve);
    void DijkstraEdgeSearch(vtkPolyData* mesh, vtkPolyData* closedCurve, vtkIdList* edgePointIds);
    void MeshCutter(vtkPolyData *Poly, vtkPoints *curvePts, vtkPolyData* outMask);
    void GetCutterCurve(vtkPolyData *poly, vtkPoints *curvePts, vtkIdList* outCurveIds);
    void CycleThroughPatches(int index);
    void ChangePointSize(int index);
    void ChangeLineSize(int index);
    //void MakeSurface(vtkPolyData* mask, vtkPoints* curve, vtkPolyData* plane, int& res);
    void OutlineIdFinder(int u, int v, std::vector<int>* output);
    void MakeCage(vtkPoints* pts, vtkPolyData* outPlanePoly);
    void SlidingStatus();
    void ShowLargestDiameter();
    void DrawDiameter(vtkPoints* meshPoints);
    void PickFromBoundaries();
    void CleanUp();
    void SetSurfaceSlider(vtkPoints* pts);
    void FlipSurfaceButton();
    int GetTemplateNOL();
    vtkPoints* GetTemplateTypeI();
    vtkPoints* GetTypeI();
    void GetTemplateCurveSliders(vtkPoints* Output);
    void GetCurveSliders(vtkPoints* Output);
    bool GetIgnorSetting();
    void DoSomeTest(vtkPolyData* targetMesh, TColgp_Array2OfPnt* plane, vtkPoints* ctrlPts);
    ~SpecimenDigitiser();

public Q_SLOTS:
    void OnCoordinateChanged(Eigen::MatrixXd sendOffData);
    void OnCoordinateNotChanged(Eigen::MatrixXd sendOffData);
    void OnStatusChanged(int status);

protected:
    void keyReleaseEvent(QKeyEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
};

#endif
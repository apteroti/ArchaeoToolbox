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
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGraphicsDropShadowEffect>
#include <QtWidgets/qstatusbar.h>
#include <QVTKOpenGLWidget.h>
#include <QProgressBar>
#include <QGroupBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QShortcut>

#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <Geom_BezierSurface.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <Geom_BSplineCurve.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <IVtkTools_ShapeDataSource.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <GeomConvert_CompBezierSurfacesToBSplineSurface.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <BRepOffsetAPI_MakeFilling.hxx>
#include <IGESControl_Controller.hxx>
#include <IGESControl_Writer.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <Geom_BSplineSurface.hxx>
#include <BRepBuilderAPI_NurbsConvert.hxx>
#include <BRepLib_FindSurface.hxx>
#include <GeomConvert.hxx>
#include <StdPrs_ToolRFace.hxx>
#include <BRepAdaptor_Surface.hxx>
//#include <BRepAdaptor_HSurface.hxx>
#include <Adaptor2d_Curve2d.hxx>
#include <Hatch_Hatcher.hxx>
#include <Adaptor3d_IsoCurve.hxx>

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
#include <vtkCellLocator.h>
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
#include <vtkArrowSource.h>
#include <vtkSelectEnclosedPoints.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkImplicitPolyDataDistance.h>
#include <vtkFeatureEdges.h>
#include <vtkThreshold.h>
#include <vtkXMLMultiBlockDataWriter.h>
#include <vtkSCurveSpline.h>
#include <vtkParametricSpline.h>
#include <vtkParametricFunctionSource.h>
#include <vtkStripper.h>
#include <vtkIntersectionPolyDataFilter.h>
#include <vtkBooleanOperationPolyDataFilter.h>
#include <vtkDijkstraGraphGeodesicPath.h>
#include <vtkGeometryFilter.h>
#include <vtkColorSeries.h>
#include <vtkIntArray.h>
#include <vtkTable.h>
#include <vtkLookupTable.h>
#include <vtkColorTransferFunction.h>
#include <vtkLine.h>
#include <vtkLineSource.h>
#include <vtkMaskPoints.h>
#include <vtkInformation.h>
#include <vtkCellIterator.h>
#include <vtkThreshold.h>
#include <vtkCallbackCommand.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkStaticPointLocator.h>
#include <vtkThresholdPoints.h>
#include <vtkSortDataArray.h>


#include "ProSetMenu.fwd.h"
#include "BlueNoiseThread.h"
#include "StatusReporterThread.h"

#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <Eigen/Eigen>
#include <Eigen/Dense>

class TemplateDigitiser:public QMainWindow
{
private:
    //Data
    QMutex* m_mutex;
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
    int m_nurbsResolution = 4;
    bool m_editableSurface = 1;
    bool m_surfaceChanged = 0;
    //bool m_curveAnchored = 0;
    //bool m_editableCurve = 1;
    vtkSmartPointer<vtkIntArray> m_maskArray;
    int m_brushSize = 1;
    bool m_mouseIsClicked = 0;
    //std::vector<int> m_maskIdList;
    std::vector<int> *m_curveType = nullptr;
    vtkPolyData* m_meshData;
    vtkSmartPointer<vtkIntArray> m_fixedPtsIds;
    //vtkSmartPointer<vtkIntArray> m_curvePtsIds;
    std::vector<std::vector <int>* > *m_surfacePtsIds = nullptr;
    
    vtkSmartPointer<vtkPolyData> m_cutMeshData;

    BlueNoiseThread* m_BlueNoiseThread = nullptr;
    StatusReporterThread* m_statThread = nullptr;
    
    //int m_dynamicRes =0;
    
    ProSetMenu* m_parent;
    //
    QLineEdit *typeILineEdit;
    QLineEdit *curveLineEditNOC;
    QLineEdit *surfaceLineEditNOP;
    QLabel *counterFixedLabel;
    QLabel *counterCurveNOCLabel;
    QLabel *counterSurfaceNOPLabel;
    QHBoxLayout *counterLayout;
    //Rendering
    QVTKOpenGLWidget *m_vtkRenderWidget;
    vtkSmartPointer<vtkRenderer> m_renderer;
    vtkSmartPointer<vtkRenderWindow> m_renWin;
    vtkSmartPointer<vtkGenericRenderWindowInteractor> m_iren;
    vtkSmartPointer<vtkActor> m_meshActor;
    vtkSmartPointer<vtkActor> m_fixedPointActor;
    vtkSmartPointer<vtkVertexGlyphFilter> m_fixedVertexFilter;
    vtkSmartPointer<vtkPoints> m_fixedHighlightPoints; // this is type I or II landmarks
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
    vtkSmartPointer<vtkPoints> m_surfaceHighlightPoints; // this is for whole surface scenario
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
    //vtkSmartPointer<IVtkTools_ShapeDataSource> m_curvePoly;
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
    vtkSmartPointer<vtkPoints> m_curveHighlightPoints;
    vtkSmartPointer<vtkPoints> m_curveHighlightPointsTotal; // this is the total curve semi-landmarks
    vtkSmartPointer<vtkActor> m_curveActorDeactive;
    vtkSmartPointer<vtkTubeFilter> m_curveTubeFilterDeactive;
    vtkSmartPointer<vtkActor> m_curveArrowActor;
    vtkSmartPointer<vtkGlyph3D> m_glyphCurveArrow;
    //vtkSmartPointer<vtkColorSeries> m_curveLMColorsSeries;

    vtkSmartPointer<vtkMultiBlockDataSet> m_curveCtrlBlock;
    vtkSmartPointer<vtkMultiBlockDataSet> m_curveBlock;
    vtkSmartPointer<vtkMultiBlockDataSet> m_curvePolyLineBlock;
    //std::vector<int>* m_curveAnchorIdList;
    vtkSmartPointer<vtkActor> m_meshBoundActor;
    vtkSmartPointer<vtkPolyData> m_meshBoundaries;

    double m_area =0;
    double m_sizeConstant = 0;

    //Toolbar
    QToolBar *mainToolbar;
    QToolBar *landmarkToolbar;
    QToolBar *surfacePatchToolbar;
    QToolBar *surfaceToolbar;
    QToolBar *curveToolbar;
    QLabel *statusLabel;
    QLabel *progressLabel;
    //QProgressBar *statusProgressBar;
    QGroupBox *counterGroup;
    //box
    QComboBox *curveSelectComboBox;
    QComboBox *surfaceSelectComboBox;
    QComboBox *curvePickSourceComboBox;
    QComboBox *surfacePickSourceComboBox;
    QComboBox *cyclePatchesComboBox;
    QSpinBox *pointSizeSpinBox;
    QSpinBox *lineSizeSpinBox;
    QSpinBox *brushSizeSpinBox;
    QCheckBox *showDiameterBox;
    QCheckBox *pickFromBoundariesBox;
    QCheckBox *ignoreIntCheckbox;
    //Buttons
    QPushButton *landmarkButton;
    QPushButton *surfaceSliderButton;
    QPushButton *curveSliderButton;
    QPushButton *curveAddButton;
    QPushButton *curveTypeButton;
    //QPushButton *curveLockButton;
    //QPushButton *curveAnchorButton;
    QPushButton *curveClearButton;
    QPushButton *surfaceAddButton;
    QPushButton *surfaceLockButton;
    QPushButton *surfaceClearButton;
    QPushButton *surfaceIronButton;
    QPushButton *surfaceAnchorButton;
    QPushButton *surfacePaintButton;
    QPushButton *surfaceResampleButton;
    //Functions
    void TypeITool();
    void SurfaceTool();
    void CurveTool();
    //Interaction
    void PickFunc(vtkObject* caller, long unsigned int eventId, void* callData);
    void resetLeftClck(vtkObject* caller, long unsigned int eventId, void* callData);
    void MoveFunc(vtkObject* caller, long unsigned int eventId, void* callData);
    void PaintFunc(vtkObject* caller, long unsigned int eventId, void* callData);
    void resetMouseMove(vtkObject* caller, long unsigned int eventId, void* callData);
    void CoordinateFunc(vtkObject* caller, long unsigned int eventId, void* callData);
    void MakeCage(vtkPoints* pts, vtkPolyData* outPlanePoly);
    void NeighborFinder(int initId, int brushSize, std::vector<int> &outList);
    

public:
    TemplateDigitiser(ProSetMenu* parent);
    void SetPoly(vtkPolyData* poly);
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
    //void PickCurveAnchor();
    void ChangeSurfaceLock();
    //void ChangeCurveLock();
    void ClearCurve();
    void ClearSurface();
    void DrapeToSurface();
    void UpdateCurveScene(int id);
    void UpdateSurfaceScene(int id);
    void FinalizeCurveScene();
    void FinalizeSurfaceScene();
    void UpdateCurveData(vtkPoints* pts, vtkPolyData* outputLine, vtkPoints *outputPoints, vtkPolyData *baseMesh);
    void ConstructSurfaceData(vtkPoints* pts, vtkPoints *outputSliders, vtkPolyData *outputCtrlPtsPoly, vtkPolyData *outputCurvePoly, int resolution);
    void ConstructSurfaceData(vtkPolyData *CtrlPtsPoly, vtkPoints *outputSliders);
    void Plot();
    void Resample(int resolution, vtkPolyData* mesh);
    double GetMeshCellArea(std::vector<double> *probab, vtkPolyData* inputMresh);
    void RandomChoice(vtkPolyData* mesh, int outputSize,std::vector<double> *probab,std::vector<int> *idList, vtkPoints* points);
    double RandomFloat(double maximum);
    double EucDist(double Ax, double Ay, double Az, double Bx, double By, double Bz);
    void ProjectOnMesh(vtkPoints *points);
    void ProjectOnMesh(vtkPoints *points, vtkPolyData *mask);
    void ProjectOnMesh(vtkPolyData *Poly, vtkPolyData* mask, std::vector<int>* ids = nullptr);
    void MakeArrow(vtkPolyData* inputMesh, vtkMultiBlockDataSet* inputCurveBlock,int liftScale, vtkPolyData* output);
    void CosmeticCurve(vtkPoints* ctrlPts, vtkPolyData* outputCurve);
    void DijkstraEdgeSearch(vtkPolyData* mesh, vtkPolyData* closedCurve, vtkIdList* edgePointIds);
    void MeshCutter(vtkPolyData *Poly, vtkPoints *curvePts, vtkPolyData* outMask);
    void GetCutterCurve(vtkPolyData *poly, vtkPoints *curvePts, vtkIdList* outCurveIds);
    void CycleThroughPatches(int index);
    void ChangePointSize(int index);
    void ChangeLineSize(int index);
    void OutlineIdFinder(int u, int v, std::vector<int>* output);
    void ShowLargestDiameter();
    void DrawDiameter(vtkPoints* meshPoints);
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
    void PoissonDisk(vtkPolyData* inputPoly, vtkPoints* fixedLm, vtkPoints* curveSliders, double area, int resol, vtkPoints* outPutPts);
    void OnSamplingIsDone();
    void PrepareBNSampling(bool stat);
    void RunStatThread(QThread* thread);
    void OnStatusChanged(int status);
    void PrepareClosing(QCloseEvent* event);
    ~TemplateDigitiser();

protected:
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
};

#endif
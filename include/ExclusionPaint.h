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

#ifndef EXCLUSIONPAINT_H
#define EXCLUSIONPAINT_H

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


class ExclusionPaint:public QMainWindow
{
  Q_OBJECT
private:
    vtkPolyData *m_meshData;
    vtkSmartPointer<vtkIntArray> m_maskArray;
    int m_brushSize = 1;
    bool m_mouseIsClicked = 0;
    bool m_forceClose = false;
    // Rendering
    QVTKOpenGLWidget *m_vtkRenderWidget;
    vtkSmartPointer<vtkRenderer> m_renderer;
    vtkSmartPointer<vtkRenderWindow> m_renWin;
    vtkSmartPointer<vtkGenericRenderWindowInteractor> m_iren;
    vtkSmartPointer<vtkActor> m_meshActor;
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> m_PointPainterStyle;
    //vtkSmartPointer<vtkInteractorStyleTrackballCamera> m_style;
    vtkSmartPointer<vtkInteractorStyleImage> m_2Dstyle;

    //Toolbar
    QToolBar *mainToolbar;
    QPushButton *surfacePaintButton;
    QPushButton* closeButton;
    QSpinBox *brushSizeSpinBox;

public:
    ExclusionPaint(vtkPolyData *data);
    void Plot();
    void BrushTool();
    void ChangeBrushSize(int index);
    void PaintMouseClicked();
    void PaintMouseReleased();
    void SelectAll();
    void DeSelectAll();
    void PaintFunc(vtkObject* caller, long unsigned int eventId, void* callData);
    void NeighborFinder(int initId, int brushSize, std::vector<int> &outList);
    void HandleCloseButton();
    ~ExclusionPaint();
signals:
    void windowClosed();  // custom signal

protected:
    void closeEvent(QCloseEvent* event) override;
};


















#endif
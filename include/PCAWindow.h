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

#ifndef PCAWINDOW_H
#define PCAWINDOW_H

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
#include <vtkThinPlateSplineTransform.h>
#include <vtkLandmarkTransform.h>
#include <vtkChartXY.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkContextActor.h>
#include <vtkPlotPoints.h>
#include <vtkFloatArray.h>
#include <vtkTable.h>
#include <vtkLookupTable.h>
#include <vtkColorTransferFunction.h>
#include <vtkScalarBarActor.h>
#include <vtkStringArray.h>
#include <vtkTooltipItem.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkContextMouseEvent.h>
#include <vtkAxis.h>
#include <vtkTextMapper.h>
#include <vtkTextActor3D.h>
#include <vtkBillboardTextActor3D.h>
#include <vtkTransform.h>


#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QComboBox>
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
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QGraphicsDropShadowEffect>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QGraphicsLayout>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QLineEdit>
#include <QVTKOpenGLWidget.h>
#include <QtCore/QTimer>
#include <QtWidgets/QFrame>

#include <Eigen/Eigen>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/Eigenvalues>
#include <array>

#include "DataBase.h"

using namespace Eigen;

class PCAWindow : public QMainWindow{

private:
    DataBase* m_parentDataBase;
    Eigen::MatrixXd m_dataMatrix;
    vtkPolyData* m_meshData;
    vtkPolyData* m_landmarksPoly;
    Eigen::MatrixXd m_eigenScores;
    Eigen::MatrixXd m_eigenVectors;
    Eigen::MatrixXd m_eigenValues;
    Eigen::MatrixXd m_expVariance;
    std::vector<std::string> m_nameList;
    int m_x = 1;
    int m_y = 2;
    QFrame* m_plotPlaceholder;
    QFrame* m_meshPlaceholder;
    QGridLayout* m_layout;


    //Rendering
    QVTKOpenGLWidget *m_meshRenderWidget;
    vtkSmartPointer<vtkRenderer> m_meshRenderer;
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> m_style;
    vtkSmartPointer<vtkGenericRenderWindowInteractor> m_meshIren;
    vtkSmartPointer<vtkActor> m_meshActor;
    vtkSmartPointer<vtkDataSetMapper> m_meshMapper;

    vtkSmartPointer<vtkVertexGlyphFilter> m_lmVertexFilter;
    vtkSmartPointer<vtkActor> m_lmActor;
    vtkSmartPointer<vtkGlyph3DMapper> m_lmMapper;
    vtkSmartPointer<vtkScalarBarActor> m_scalarBar;
    vtkSmartPointer<vtkActor2D> m_lmLabelActor;    

    QVTKOpenGLWidget *m_graphRenderWidget;
    vtkRenderWindow *m_graphRenWin;
    vtkSmartPointer<vtkContextView> m_scatterChartView;
    vtkSmartPointer<vtkChartXY> m_scatterChart;
    
    vtkSmartPointer <vtkStringArray> m_graphLabelArray;
    vtkSmartPointer<vtkBillboardTextActor3D>m_MILLabelActor[1];
    vtkSmartPointer<vtkColorTransferFunction> m_ctf;

    // toolbar etc
    QToolBar *mainToolbar;
    QSpinBox *select1stPCsSpinBox;
    QSpinBox *select2ndPCsSpinBox;
    QLabel *m_meshRenLabel;
    QDockWidget *m_dockedToolbar;
    QTreeWidget *specimenTreeWidget;
    QCheckBox *showPtsIdsBox;
    QLineEdit * milLineEdit;
    // Buttons
    QPushButton *exportButton;

public:
    PCAWindow(DataBase* parentDB);
    void Calculate(Eigen::MatrixXd &data, bool standardise = 0);
    void UpdateScatter(int x, int y);
    void UpdateContribution(int pc);
    template <typename M>
    M LoadCSV(const std::string &path); //For debugging
    void DebugPrintMatrix(Eigen::MatrixXd matrix); //For debugging
    void InterpolateTPSContributionToMesh(vtkDoubleArray* scalars);
    void Plot();
    void DelayedPlotter();
    ~PCAWindow();
public slots:
    void Export2Csv();
    void Update1stPC(int PC1);
    void Update2ndPC(int PC2);
    void ShowPtsIds();
protected:
    //void closeEvent(QCloseEvent *event) override;
};


#endif
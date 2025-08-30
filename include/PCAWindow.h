/***********************************************************************************************

************************************************************************************************
* ArchaeoToolbox *
* Geometric Morphometrics Software *
* *
* Copyright(C) 2023 *
* Kaveh Yousef Pouran *
* Laboratori d’Arqueozoologia, Universitat Autònoma de Barcelona *
* *
* All rights reserved. *
* *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or *
* (at your option) any later version. *
* *
* This program is distributed in the hope that it will be useful, *
* but WITHOUT ANY WARRANTY; without even the implied warranty of *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt) *
* for more details. *
* *

 ***********************************************************************************************
                                                                               .
                                                  . =: # +*
                                                 ## %@.
                                                =@@ #@%
                                               .@@* @@@:
                                               %@@* #@@@=
                                               =@@@#- .:+#@@@#
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

#include <QVTKOpenGLWidget.h>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkAppendArcLength.h>
#include <vtkAutoInit.h>
#include <vtkAxis.h>
#include <vtkBillboardTextActor3D.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkCellLocator.h>
#include <vtkCellPicker.h>
#include <vtkCenterOfMass.h>
#include <vtkChartLegend.h>
#include <vtkChartXY.h>
#include <vtkCleanPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkColorTransferFunction.h>
#include <vtkCompositeDataGeometryFilter.h>
#include <vtkCompositePolyDataMapper2.h>
#include <vtkConnectivityFilter.h>
#include <vtkContextActor.h>
#include <vtkContextMouseEvent.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
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
#include <vtkFloatArray.h>
#include <vtkGL2PSExporter.h>
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
#include <vtkLookupTable.h>
#include <vtkMassProperties.h>
#include <vtkMeshQuality.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOBJWriter.h>
#include <vtkPlaneSource.h>
#include <vtkPlotPoints.h>
#include <vtkPointData.h>
#include <vtkPointGaussianMapper.h>
#include <vtkPointLocator.h>
#include <vtkPointPicker.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataPointSampler.h>
#include <vtkPolyLine.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkScalarBarActor.h>
#include <vtkSelectPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkSphereSource.h>
#include <vtkSplineFilter.h>
#include <vtkStringArray.h>
#include <vtkSurfaceReconstructionFilter.h>
#include <vtkTable.h>
#include <vtkTextActor.h>
#include <vtkTextActor3D.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>
#include <vtkThinPlateSplineTransform.h>
#include <vtkTooltipItem.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTubeFilter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVariant.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkWindowedSincPolyDataFilter.h>


#include <Eigen/Dense>
#include <Eigen/Eigen>
#include <Eigen/Eigenvalues>
#include <Eigen/Sparse>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QValueAxis>
#include <QtCore/QTextStream>
#include <QtCore/QTimer>
#include <QtGui/QCloseEvent>
#include <QtGui/QSurfaceFormat>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QColorDialog>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGraphicsDropShadowEffect>
#include <QtWidgets/QGraphicsLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeWidget>
#include <array>

#include "DataBase.h"
#include "SpinnerDialog.h"
#include "UpdateContributionThread.h"

using namespace Eigen;

class PCAWindow : public QMainWindow {
    Q_OBJECT
   private:
    DataBase *m_parentDataBase;
    Eigen::MatrixXd m_dataMatrix;
    vtkPolyData *m_meshData;
    vtkPolyData *m_landmarksPoly;
    Eigen::MatrixXd m_eigenScores;
    Eigen::MatrixXd m_eigenVectors;
    Eigen::MatrixXd m_eigenValues;
    Eigen::MatrixXd m_expVariance;
    std::vector<std::string> m_nameList;
    std::vector<std::pair<std::string, std::string>> m_pairedList;
    std::map<QString, std::pair<QColor, int>>
        m_categoryStyles;  // category -> (color, shape)
    int m_x = 1;
    int m_y = 2;
    QFrame *m_plotPlaceholder;
    QFrame *m_meshPlaceholder;
    QGridLayout *m_layout;
    QTableWidget *m_specimenTable;

    // Rendering
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

    vtkSmartPointer<vtkStringArray> m_graphLabelArray;
    vtkSmartPointer<vtkBillboardTextActor3D> m_MILLabelActor[1];
    vtkSmartPointer<vtkColorTransferFunction> m_ctf;

    // toolbar etc
    QToolBar *mainToolbar;
    QSpinBox *select1stPCsSpinBox;
    QSpinBox *select2ndPCsSpinBox;
    QLabel *m_meshRenLabel;
    QDockWidget *m_dockedToolbar;

    QCheckBox *showPtsIdsBox;
    QLineEdit *milLineEdit;
    // Buttons
    QPushButton *exportButton;
    QPushButton *m_importCatBtn;
    QPushButton *m_canvasBtn;
    QPushButton *m_saveScatterBtn;

   public:
    PCAWindow(DataBase *parentDB);
    void Calculate(Eigen::MatrixXd &data, bool standardise = 0);
    void UpdateScatter(int x, int y);
    void UpdateContribution(int pc);
    template <typename M>
    M LoadCSV(const std::string &path);             // For debugging
    void DebugPrintMatrix(Eigen::MatrixXd matrix);  // For debugging
    void InterpolateTPSContributionToMesh(vtkDoubleArray *scalars);
    void Plot();
    void DelayedPlotter();
    void UpdateTableFromPairedList();
    ~PCAWindow();
   public slots:
    void Export2Csv();
    void Update1stPC(int PC1);
    void Update2ndPC(int PC2);
    void ShowPtsIds();
    void ImportCategories();
    void OnCategoryEdited(QTableWidgetItem *item);
    void SetShapeColor();
    void SaveScatter();

   protected:
    // void closeEvent(QCloseEvent *event) override;
};

#endif
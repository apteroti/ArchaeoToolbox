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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <omp.h>
#include <vtkAlgorithmOutput.h>
#include <vtkAxesActor.h>
#include <vtkCaptionActor2D.h>
#include <vtkCellArray.h>
#include <vtkCleanPolyData.h>
#include <vtkColorSeries.h>
#include <vtkColorTransferFunction.h>
#include <vtkDoubleArray.h>
#include <vtkGaussianKernel.h>
#include <vtkLandmarkTransform.h>
#include <vtkLinearExtrusionFilter.h>
#include <vtkLinearSubdivisionFilter.h>
#include <vtkLookupTable.h>
#include <vtkMultiBlockDataGroupFilter.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkOBJReader.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPLYReader.h>
#include <vtkPointInterpolator.h>
#include <vtkPolyDataNormals.h>
#include <vtkProcrustesAlignmentFilter.h>
#include <vtkScalarBarActor.h>
#include <vtkSmartPointer.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkStructuredGrid.h>
#include <vtkTable.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkTriangleFilter.h>
#include <vtkVectorText.h>

#include <Eigen/Dense>
#include <Eigen/Eigen>
#include <Eigen/Eigenvalues>
#include <Eigen/Sparse>
#include <QFuture>
#include <QTreeWidgetItemIterator>
#include <QtConcurrent/QtConcurrent>
#include <QtCore/QTextStream>
#include <QtGui/QCloseEvent>
#include <QtGui/QTextDocument>
#include <QtHelp/QHelpContentWidget>
#include <QtHelp/QHelpEngine>
#include <QtHelp/QHelpIndexWidget>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrinter>
#include <QtTest/QSignalSpy>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QGraphicsDropShadowEffect>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeWidget>
#include <cmath>
#include <iostream>
#include <thread>
#include <tuple>

#include "DataBase.h"
#include "DecimateThread.h"
#include "DecimationDialog.h"
#include "ExportDialogue.h"
#include "HelpBrowser.h"
#include "ImportThread.h"
#include "LoadMeshThread.h"
#include "PCAWindow.h"
#include "ProjectSetMenu.h"
#include "RecoverLMThread.h"
#include "SpecimenDigitiser.h"
#include "SpinnerDialog.h"
#include "StatusReporterThread.h"
#include "SuperImposition.h"
#include "SuperImpositionThread.h"
#include "TemplateViewer.h"
#include "VCGDefinitions.h"
#include "WindowUtils.h"

using std::string;

enum class STATUS { outdated = 0, neutral = 1, reset = 2 };

// typedef boost::variant<vtkPolyData*, vtkStructuredGrid*> dataType;
class MainWindow : public QMainWindow {
    Q_OBJECT

   private:
    QMutex* m_mutex;
    // treewidget
    QTreeWidget* m_treeWidget = nullptr;
    QTreeWidgetItem* m_treeItem = nullptr;
    QTreeWidgetItem* m_typeIBranch = nullptr;
    QTreeWidgetItem* m_curveSliderBranch = nullptr;
    QTreeWidgetItem* m_surfaceSliderBranch = nullptr;
    QTableWidget* mainTable;
    QVTKOpenGLWidget* mainRenderWindowWidget;
    QTableWidget* supImposedTable;
    QTableWidget* procResTable;
    QTabWidget* mainTabWidget;
    std::string m_oldName;
    std::string m_newName;
    // Help engine
    QTabWidget* helpTab;
    QHelpEngine* helpEngine;
    HelpBrowser* textViewer;
    QSplitter* horizSplitter;
    QMainWindow* helpWindow;

    // for exporting the results
    bool m_lm, m_si, m_pv, m_pm = 0;
    ExportDialogue* m_exportDial = nullptr;

    // Data
    bool TemplateIsSet = 0;
    int m_typeINOL;
    int m_surfaceNOS;
    int m_surfacePatchNOP;
    int m_surfacePatchUNOS;
    int m_surfacePatchVNOS;
    int m_curveNOS;
    int m_curveNOC;
    int m_TableRowNum = 1000;
    int m_TableColNum = 500;
    double m_LmOpacity = 0.6;
    std::string m_templateMeshType = "";
    DataBase* m_dataBase;
    SpecimenDigitiser* m_meshPlot = nullptr;
    SuperImposition* m_SIMP = nullptr;
    TemplateViewer* m_templateView = nullptr;
    std::string m_lastSpecimenName = "";
    vtkSmartPointer<vtkPolyData> m_currentMesh;
    vtkSmartPointer<vtkStructuredGrid> m_currentGrid;
    STATUS m_status = STATUS::neutral;
    vtkSmartPointer<vtkPolyData> m_templateMesh;
    vtkSmartPointer<vtkPoints> m_templateSurfaceSliders;
    vtkSmartPointer<vtkPoints> m_templateTypeI;
    vtkSmartPointer<vtkMultiBlockDataSet> m_templatePatchSurfaceSliders;
    vtkSmartPointer<vtkMultiBlockDataSet> m_templatePatchSurfaceCurve;
    vtkSmartPointer<vtkMultiBlockDataSet> m_templateCurveSliders;
    vtkSmartPointer<vtkMultiBlockDataSet> m_templateCurvePointsPoly;

    PCAWindow* m_pcaWindow;
    StatusReporterThread* m_statThread = nullptr;
    RecoverLMThread* m_recoveryThread = nullptr;
    ImportThread* m_importThread = nullptr;
    // Menus
    QMenu* fileMenu;
    QMenu* helpMenu;
    QMenu* m_toolMenu;
    QMenu nodePopUpMenu;
    QMenu branchPopUpMenu;
    ProjectSetMenu* projectSettings;
    // Actions
    QAction* quitAction;
    QAction* aboutAction;
    QAction* helpAction;
    QAction* exportCSVAction;
    QAction* exportVTKAction;
    QAction* projectSettingAction;
    QAction* plotToolbarAction;
    QAction* meshPlotToolbarAction;
    QAction* superImpositionToolbarAction;
    QAction* templatePlotToolbarAction;
    QAction* importMeshAction;
    QAction* m_showConsoleAction;
    QAction* renameNodeAction;
    QAction* exportGeometryAction;
    QAction* deleteNodeAction;
    QAction* deleteBranchAction;
    QAction* pcaToolbarAction;
    QAction* recoverActionLegacy;
    QAction* openProjectAction;
    QAction* saveProjectAction;
    QAction* helpToolbarAction;

    // Toolbar
    QToolBar* toolbar;
    QDockWidget* dockedToolbar;
    QLabel* statusLabel;
    QLabel* progressLabel;
    // Visualization
    vtkSmartPointer<vtkRenderer> m_mainRenderer;
    vtkSmartPointer<vtkActor> m_mainMeshActor;
    vtkSmartPointer<vtkPolyDataMapper> m_mainMeshMapper;

    vtkSmartPointer<vtkVertexGlyphFilter> m_fixedLmVertexFilter;
    vtkSmartPointer<vtkActor> m_fixedLmActor;

    vtkSmartPointer<vtkVertexGlyphFilter> m_curveLmVertexFilter;
    vtkSmartPointer<vtkActor> m_curveLmActor;

    vtkSmartPointer<vtkVertexGlyphFilter> m_surfaceLmVertexFilter;
    vtkSmartPointer<vtkActor> m_surfaceLmActor;

    vtkSmartPointer<vtkScalarBarActor> m_scalarBar;

   public:
    MainWindow();
    void ContentTree(QDockWidget*);
    void UpdateNodeName();
    void UpdateDataBase();
    void PDist(vtkPoints* points, Eigen::MatrixXd& output);
    double EucDist(double Ax, double Ay, double Az, double Bx, double By,
                   double Bz);
    void SetStatus(STATUS status);
    void SetTemplateMeshType(std::string type);
    void SetTemplateMesh(vtkPolyData* mesh);
    void SetTemplateTypeI(vtkPoints* pts);
    void SetTemplateCurveSliders(vtkMultiBlockDataSet* ptsPolyBlock,
                                 vtkMultiBlockDataSet* curvePtsPolyBlock,
                                 int numOfSliders);
    void SetTemplateSurfaceSliders(vtkPoints* pts);
    void SetTemplatePatchSurfaceSliders(vtkMultiBlockDataSet* ptsPolyBlock,
                                        vtkMultiBlockDataSet* surfaceCurve,
                                        int Ures, int Vres);
    void SetTypeI(vtkPoints* pts);
    void SetSliders(vtkPoints* fixedPts, vtkPoints* curveSliderPts,
                    vtkPoints* surfaceSliderPts);
    void SetSliders(vtkPoints* fixedPts, vtkPoints* curveSliderPts,
                    vtkPoints* surfaceSliderPts, std::string name);
    void SetLandmarkHeaders(QTableWidget* table, int colCount);
    void SetNumberOfCPUCores(int num);
    void ResetLandmarks();
    void TemplateStatus(bool status);
    void DebugPrintMatrix(Eigen::MatrixXd matrix);
    void PlotLandmarks(vtkPoints* fixedLandmarks, vtkPoints* curveLandmarks,
                       vtkPoints* surfaceLandmarks, vtkPolyData* mesh);
    void PlotLandmarks(vtkPoints* fixedLandmarks, vtkPoints* curveLandmarks,
                       vtkPoints* surfaceLandmarks, vtkPolyData* mesh,
                       std::string landmarkType);
    void PaintMesh(std::string name);
    void ResetLMData(std::string name);
    void ResetImposition();
    void FinaliseImposition();
    void OnSuperImposed(std::string name);
    bool IsNumber(const std::string& s);
    void LockTheWindow();
    void UnlockTheWindow();
    void ReadLMDataFromFile();
    void ReadProjectFromFile();
    void OnSuperImpositionIsDone();
    void RunStatThread(QThread* thread);
    void SetToExport(bool lm, bool si, bool pv, bool pm);
    void DoExport();
    vtkMultiBlockDataSet* GetTemplateCurveSliders();
    vtkMultiBlockDataSet* GetTemplateCurvePoly();
    vtkMultiBlockDataSet* GetTemplateSurfacePatchCurve();
    vtkPolyData* GetTemplateTotalLandmarks();
    vtkPolyData* GetTemplateMesh();
    vtkPoints* GetTemplateTypeI();
    vtkPoints* GetTemplateSurfaceSliders();
    vtkMultiBlockDataSet* GetTemplateSurfacePatchSliders();
    DataBase* GetDataBase();
    bool GetTemplateStatus();
    int GetTypeINOL();
    int GetCurveNOS();
    int GetSurfaceNOS();
    int GetNumberOfCurves();
    int GetNumberOfPatches();
    int GetURes();
    int GetVRes();
    bool GetIgnorSetting();
    QMutex* GetMutex();
    void DecimateWithAnimatedDialog(MyMesh& m, float reductionRatio);
    void ConvertVTKToVCG(vtkPolyData* polyData, MyMesh& vcgMesh);
    void ConvertVCGToVTK(MyMesh& vcgMesh, vtkPolyData* polyData);
    ~MainWindow();
   public slots:
    void about();
    void PrintHelp();
    void LoadMesh();
    void ExportCSV();
    void ExportGeometry();
    void SaveProject();
    void RecoverDigitisedLM();
    void RecoverProject();
    void DICOMPlot();  // Not implemented
    void SuperImpose();
    void TemplatePlot();
    void meshPlot();
    void SettingMenu();
    void UpdateActiveData();
    void ShowContextMenu(const QPoint& pos);
    void PCA();
    void RenameDataNode();
    void DeleteDataNode();
    void DeleteDataBranch();
    void OnTreeObjectChanged(std::string name);
    void OnDataBaseChanged(std::string name);
    void OnRecoveryIsDone();
    void OnStatusChanged(int status);
    void ShowCmdl();

   protected:
    void closeEvent(QCloseEvent* event) override;
    void customQuit();
   signals:
};

#endif
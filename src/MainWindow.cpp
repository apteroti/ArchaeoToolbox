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

#include "../include/MainWindow.h"

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
#define ENDL Qt::endl
#else
#define ENDL endl
#endif

MainWindow::MainWindow() {
    // qRegisterMetaType<vtkMultiBlockDataSet*>("vtkPolyData*");
    m_mutex = new QMutex();
    m_currentMesh = vtkSmartPointer<vtkPolyData>::New();
    m_currentGrid = vtkSmartPointer<vtkStructuredGrid>::New();
    m_dataBase = new DataBase();
    mainRenderWindowWidget = new QVTKOpenGLWidget();

    helpEngine = new QHelpEngine(QApplication::applicationDirPath() +
                                 "/documents/helpdocuments.qhc");
    helpEngine->setupData();

    helpTab = new QTabWidget();
    helpTab->setMaximumWidth(200);
    helpTab->addTab(helpEngine->contentWidget(), "Contents");
    helpTab->addTab(helpEngine->indexWidget(), "Index");

    textViewer = new HelpBrowser(helpEngine);
    textViewer->setSource(
        QUrl("qthelp://archaeotoolbox.helpdocument/doc/index.html"));

    connect(helpEngine->contentWidget(), SIGNAL(linkActivated(QUrl)),
            textViewer, SLOT(setSource(QUrl)));

    connect(helpEngine->indexWidget(), SIGNAL(linkActivated(QUrl, QString)),
            textViewer, SLOT(setSource(QUrl)));
    horizSplitter = new QSplitter(Qt::Horizontal);
    horizSplitter->insertWidget(0, helpTab);
    horizSplitter->insertWidget(1, textViewer);

    helpWindow = new QMainWindow(this);
    helpWindow->setCentralWidget(horizSplitter);
    helpWindow->hide();

    mainTable = new QTableWidget();
    supImposedTable = new QTableWidget();
    procResTable = new QTableWidget();
    mainTabWidget = new QTabWidget();
    mainTabWidget->tabBar()->setDocumentMode(0);
    mainTabWidget->tabBar()->setExpanding(0);
    mainTabWidget->tabBar()->setStyleSheet(
        "QTabBar::tab {"
        "background: white;"
        "color: black;"
        "}"
        "QTabBar::tab:selected {"
        "background: lightblue;"
        "}");

    m_treeWidget = new QTreeWidget();
    m_typeINOL = 0;
    m_surfaceNOS = 0;
    m_surfacePatchNOP = 1;
    m_surfacePatchUNOS = 0;
    m_surfacePatchVNOS = 0;
    m_curveNOS = 0;
    m_curveNOC = 1;
    m_templateMesh = vtkSmartPointer<vtkPolyData>::New();
    m_templateSurfaceSliders = vtkSmartPointer<vtkPoints>::New();
    m_templateTypeI = vtkSmartPointer<vtkPoints>::New();
    m_templatePatchSurfaceSliders =
        vtkSmartPointer<vtkMultiBlockDataSet>::New();
    m_templatePatchSurfaceCurve = vtkSmartPointer<vtkMultiBlockDataSet>::New();
    m_templateCurveSliders = vtkSmartPointer<vtkMultiBlockDataSet>::New();
    m_templateCurvePointsPoly = vtkSmartPointer<vtkMultiBlockDataSet>::New();
    setWindowTitle("ArchaeoToolbox");
    resize(800, 500);

    //--------------
    vtkNew<vtkNamedColors> colors;
    m_mainRenderer = vtkSmartPointer<vtkRenderer>::New();
    m_mainMeshActor = vtkSmartPointer<vtkActor>::New();
    m_mainMeshMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    m_scalarBar = vtkSmartPointer<vtkScalarBarActor>::New();
    vtkNew<vtkGenericRenderWindowInteractor> mainIRen;
    vtkNew<vtkInteractorStyleTrackballCamera> renderStyle;
    vtkRenderWindow* mainRenWin = mainRenderWindowWidget->GetRenderWindow();
    mainRenWin->AddRenderer(m_mainRenderer);
    renderStyle->SetCurrentRenderer(m_mainRenderer);
    mainIRen->SetInteractorStyle(renderStyle);
    mainIRen->SetRenderWindow(mainRenWin);

    double windowWidth = 800;
    double windowHeight = 800;
    m_scalarBar->SetTitle("Procrustes Residuals (Extrapolated)");
    m_scalarBar->UnconstrainedFontSizeOn();
    m_scalarBar->SetNumberOfLabels(5);
    m_scalarBar->SetMaximumWidthInPixels(windowWidth / 10);
    m_scalarBar->SetMaximumHeightInPixels(windowHeight / 3);

    // Create a dummy vector text
    vtkNew<vtkVectorText> vecText;
    vecText->SetText("Welcome!");
    vtkNew<vtkLinearExtrusionFilter> extrude;
    extrude->SetInputConnection(vecText->GetOutputPort());
    extrude->SetExtrusionTypeToNormalExtrusion();
    extrude->SetVector(0, 0, 1);
    extrude->SetScaleFactor(0.5);
    extrude->Update();
    m_mainMeshMapper->SetInputData(extrude->GetOutput());
    m_mainMeshMapper->ScalarVisibilityOn();
    m_mainMeshActor->SetMapper(m_mainMeshMapper);
    m_mainMeshActor->GetProperty()->SetColor(1, 0.992, 0.815);
    m_mainRenderer->AddActor(m_mainMeshActor);
    //--------------
    // Point properties and color etc

    vtkNew<vtkSphereSource> sphereSource;
    m_fixedLmVertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    m_fixedLmActor = vtkSmartPointer<vtkActor>::New();

    m_curveLmVertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    m_curveLmActor = vtkSmartPointer<vtkActor>::New();

    m_surfaceLmVertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    m_surfaceLmActor = vtkSmartPointer<vtkActor>::New();

    vtkNew<vtkPolyData> tempEmptyPoly;
    // Fixed Landmarks
    m_fixedLmVertexFilter->SetInputData(tempEmptyPoly);
    m_fixedLmVertexFilter->Update();
    vtkNew<vtkGlyph3DMapper> fixedLmMapper;
    fixedLmMapper->SetInputData(m_fixedLmVertexFilter->GetOutput());
    fixedLmMapper->SetSourceConnection(sphereSource->GetOutputPort());
    fixedLmMapper->ScalingOff();
    fixedLmMapper->ScalarVisibilityOff();

    m_fixedLmActor->GetProperty()->SetColor(1.0, 0.0, 0.0);
    m_fixedLmActor->GetProperty()->SetDiffuse(0.6);
    m_fixedLmActor->GetProperty()->SetAmbient(0.4);
    m_fixedLmActor->GetProperty()->SetSpecular(0.0);
    m_fixedLmActor->GetProperty()->SetSpecularPower(1);
    m_fixedLmActor->SetMapper(fixedLmMapper);
    m_mainRenderer->AddActor(m_fixedLmActor);
    // Curve Landmarks
    m_curveLmVertexFilter->SetInputData(tempEmptyPoly);
    m_curveLmVertexFilter->Update();
    vtkNew<vtkGlyph3DMapper> curveLmMapper;
    curveLmMapper->SetInputData(m_curveLmVertexFilter->GetOutput());
    curveLmMapper->SetSourceConnection(sphereSource->GetOutputPort());
    curveLmMapper->ScalingOff();
    curveLmMapper->ScalarVisibilityOff();

    m_curveLmActor->GetProperty()->SetColor(0.0, 1.0, 0.0);
    m_curveLmActor->GetProperty()->SetDiffuse(0.6);
    m_curveLmActor->GetProperty()->SetAmbient(0.4);
    m_curveLmActor->GetProperty()->SetSpecular(0.0);
    m_curveLmActor->GetProperty()->SetSpecularPower(1);
    m_curveLmActor->SetMapper(curveLmMapper);
    m_mainRenderer->AddActor(m_curveLmActor);

    // Surface Landmarks
    m_surfaceLmVertexFilter->SetInputData(tempEmptyPoly);
    m_surfaceLmVertexFilter->Update();
    vtkNew<vtkGlyph3DMapper> surfaceLmMapper;
    surfaceLmMapper->SetInputData(m_surfaceLmVertexFilter->GetOutput());
    surfaceLmMapper->SetSourceConnection(sphereSource->GetOutputPort());
    surfaceLmMapper->ScalingOff();
    surfaceLmMapper->ScalarVisibilityOff();

    m_surfaceLmActor->GetProperty()->SetColor(0.0, 0.0, 1.0);
    m_surfaceLmActor->GetProperty()->SetDiffuse(0.6);
    m_surfaceLmActor->GetProperty()->SetAmbient(0.4);
    m_surfaceLmActor->GetProperty()->SetSpecular(0.0);
    m_surfaceLmActor->GetProperty()->SetSpecularPower(1);
    m_surfaceLmActor->SetMapper(surfaceLmMapper);
    m_mainRenderer->AddActor(m_surfaceLmActor);

    // Ambient properties
    m_mainRenderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
    m_mainRenderer->ResetCamera();
    m_mainRenderer->GetActiveCamera()->Zoom(0.5);

    mainRenWin->Render();
    mainIRen->Start();
    //--------------
    m_pcaWindow = nullptr;
    //--------------
    mainTable->setRowCount(TableRowNum);
    mainTable->setColumnCount(TableColNum);
    SetLandmarkHeaders(mainTable);

    mainTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainTable->setFocusPolicy(Qt::NoFocus);
    mainTable->setSelectionMode(QAbstractItemView::NoSelection);

    supImposedTable->setRowCount(TableRowNum);
    supImposedTable->setColumnCount(TableColNum);
    SetLandmarkHeaders(supImposedTable);
    supImposedTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    supImposedTable->setFocusPolicy(Qt::NoFocus);
    supImposedTable->setSelectionMode(QAbstractItemView::NoSelection);

    procResTable->setRowCount(TableRowNum);
    procResTable->setColumnCount(TableColNum);
    SetLandmarkHeaders(procResTable);
    procResTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    procResTable->setFocusPolicy(Qt::NoFocus);
    procResTable->setSelectionMode(QAbstractItemView::NoSelection);

    mainTabWidget->addTab(mainRenderWindowWidget, tr("Renderer"));
    mainTabWidget->addTab(mainTable, tr("Digitised"));
    mainTabWidget->addTab(supImposedTable, tr("SuperImposed"));
    mainTabWidget->addTab(procResTable, tr("Procrustes Residual"));

    setCentralWidget(mainTabWidget);
    // Setting up File menu
    fileMenu = menuBar()->addMenu("&File");
    importMeshAction = new QAction("Import Geometry (OBJ/PLY)", this);
    importMeshAction->setShortcut(QKeySequence("Ctrl+I"));
    importMeshAction->setStatusTip("Import .OBJ/PLY mesh");

    exportCSVAction = new QAction("Export Data", this);
    exportCSVAction->setStatusTip("Export Data as .CSV");

    exportVTKAction = new QAction("Export Geometry", this);
    exportVTKAction->setStatusTip("Export Geometry as .VTK");

    recoverActionLegacy = new QAction("Recover (Legacy)", this);
    recoverActionLegacy->setStatusTip("Recover via file");

    openProjectAction = new QAction("Open", this);
    openProjectAction->setStatusTip("Open a saved project");

    saveProjectAction = new QAction("Save the Project", this);
    saveProjectAction->setStatusTip("Save the Project");

    quitAction = new QAction("Quit", this);
    quitAction->setShortcut(QKeySequence::Quit);

    fileMenu->addAction(importMeshAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exportCSVAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exportVTKAction);
    fileMenu->addSeparator();
    fileMenu->addAction(saveProjectAction);
    fileMenu->addAction(openProjectAction);
    fileMenu->addSeparator();
    fileMenu->addAction(recoverActionLegacy);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAction);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAction);

    helpMenu = menuBar()->addMenu("&Help");
    aboutAction = new QAction("About", this);
    helpMenu->addAction(aboutAction);

    helpAction = new QAction("Documentation", this);
    helpAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_H));
    helpMenu->addAction(helpAction);
    //--------------------------------------------------------

    // Setting up Toolbar for content tree
    dockedToolbar = new QDockWidget;
    dockedToolbar->setFeatures(QDockWidget::DockWidgetFloatable |
                               QDockWidget::DockWidgetMovable);
    addDockWidget(Qt::LeftDockWidgetArea, dockedToolbar);
    //---------------------------------------
    toolbar = addToolBar("Main Toolbar");
    toolbar->setMovable(true);

    projectSettingAction = toolbar->addAction(
        QIcon(":/icons/graphics/icons/cog.png"), "Project Settings");
    plotToolbarAction = toolbar->addAction(
        QIcon(":/icons/graphics/icons/DICOMPlot.png"), "Digitize DICOM");
    plotToolbarAction->setEnabled(0);
    meshPlotToolbarAction = toolbar->addAction(
        QIcon(":/icons/graphics/icons/digitize.svg"), "Digitize Mesh");
    meshPlotToolbarAction->setEnabled(0);
    superImpositionToolbarAction =
        toolbar->addAction(QIcon(":/icons/graphics/icons/superimposition.svg"),
                           "Super Imposition");
    superImpositionToolbarAction->setEnabled(0);
    pcaToolbarAction =
        toolbar->addAction(QIcon(":/icons/graphics/icons/PCAicon.svg"),
                           "Principal Component Analysis");
    pcaToolbarAction->setEnabled(0);
    templatePlotToolbarAction = toolbar->addAction(
        QIcon(":/icons/graphics/icons/template.svg"), "Plot Template");
    templatePlotToolbarAction->setEnabled(0);

    helpToolbarAction =
        toolbar->addAction(QIcon(":/icons/graphics/icons/Help.svg"), "Help");

    m_templateView = new TemplateViewer(this);
    m_templateView->hide();
    projectSettings = new ProSetMenu(this);
    projectSettings->hide();

    statusLabel = new QLabel(this);
    statusLabel->setText("Status: Idle");
    progressLabel = new QLabel(this);
    progressLabel->setPixmap(QPixmap(":/icons/graphics/icons/idle.svg"));

    this->statusBar()->addPermanentWidget(statusLabel, 0);
    this->statusBar()->addPermanentWidget(progressLabel, 0);

    // Setting up signals and slots
    connect(aboutAction, &QAction::triggered, this, &MainWindow::about);
    connect(helpAction, &QAction::triggered, this, &MainWindow::PrintHelp);
    connect(importMeshAction, &QAction::triggered, this, &MainWindow::LoadMesh);
    connect(exportCSVAction, &QAction::triggered, this, &MainWindow::ExportCSV);
    connect(exportVTKAction, &QAction::triggered, this,
            &MainWindow::ExportGeometry);
    connect(saveProjectAction, &QAction::triggered, this,
            &MainWindow::SaveProject);
    connect(recoverActionLegacy, &QAction::triggered, this,
            &MainWindow::RecoverDigitisedLM);
    connect(openProjectAction, &QAction::triggered, this,
            &MainWindow::RecoverProject);
    // connect(quitAction, &QAction::triggered, this, &QApplication::quit);
    connect(quitAction, &QAction::triggered, this, &MainWindow::customQuit);
    connect(plotToolbarAction, &QAction::triggered, this,
            &MainWindow::DICOMPlot);
    connect(superImpositionToolbarAction, &QAction::triggered, this,
            &MainWindow::SuperImpose);
    connect(templatePlotToolbarAction, &QAction::triggered, this,
            &MainWindow::TemplatePlot);
    connect(meshPlotToolbarAction, &QAction::triggered, this,
            &MainWindow::meshPlot);
    connect(projectSettingAction, &QAction::triggered, this,
            &MainWindow::SettingMenu);
    connect(m_treeWidget, &QTreeWidget::itemSelectionChanged, this,
            &MainWindow::UpdateActiveData);
    connect(m_treeWidget, &QTreeWidget::customContextMenuRequested, this,
            &MainWindow::ShowContextMenu);
    connect(pcaToolbarAction, &QAction::triggered, this, &MainWindow::PCA);
    connect(helpToolbarAction, &QAction::triggered, this,
            &MainWindow::PrintHelp);
    //-----------------------------------------------------------
    ContentTree(dockedToolbar);
}

void MainWindow::customQuit() {
    if (QMessageBox::Yes ==
        QMessageBox::question(this, "Close Confirmation",
                              "Do you Want to quit?",
                              QMessageBox::Yes | QMessageBox::No)) {
        QApplication::quit();
    }
}

void MainWindow::closeEvent(QCloseEvent* event) {
    event->ignore();
    if (QMessageBox::Yes ==
        QMessageBox::question(this, "Close Confirmation",
                              "Do you Want to quit?",
                              QMessageBox::Yes | QMessageBox::No)) {
        delete m_meshPlot;
        m_meshPlot = nullptr;
        delete m_SIMP;
        m_SIMP = nullptr;
        delete projectSettings;
        projectSettings = nullptr;
        delete m_templateView;
        m_templateView = nullptr;
        delete m_pcaWindow;
        m_pcaWindow = nullptr;
        delete m_recoveryThread;
        m_recoveryThread = nullptr;
        delete m_importThread;
        m_importThread = nullptr;
        delete m_exportDial;
        m_exportDial = nullptr;
        event->accept();
    }
}

void MainWindow::ContentTree(QDockWidget* parent) {
    m_treeWidget->setHeaderLabels(QStringList{"Name", "Type"});
    parent->setWidget(m_treeWidget);
    m_treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    m_treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    renameNodeAction = nodePopUpMenu.addAction("Rename");
    connect(renameNodeAction, &QAction::triggered, this,
            &MainWindow::RenameDataNode);
    connect(m_treeWidget, &QTreeWidget::itemChanged, this,
            &MainWindow::UpdateNodeName);
    nodePopUpMenu.addSeparator();
    deleteNodeAction = nodePopUpMenu.addAction("Delete");
    connect(deleteNodeAction, &QAction::triggered, this,
            &MainWindow::DeleteDataNode);
    deleteBranchAction = branchPopUpMenu.addAction("Delete Landmarks");
    connect(deleteBranchAction, &QAction::triggered, this,
            &MainWindow::DeleteDataBranch);
    nodePopUpMenu.addSeparator();
    exportGeometryAction = nodePopUpMenu.addAction("Export Geometry");
    connect(exportGeometryAction, &QAction::triggered, this,
            &MainWindow::ExportGeometry);
}

void MainWindow::SettingMenu() { projectSettings->show(); }

void MainWindow::about() {
    QMessageBox::about(
        this, tr("About ArchaeoToolbox"),
        tr("<p align='center'> <b>ArchaeoToolbox</b> <br> 2.2"
           "<br> <b>Reference:</b> <br> Kaveh Yousef Pouran, Maria Saña, Juan "
           "Anton Barceló, 2023. Biomechanics, behaviour dynamics and "
           "archaeology: Integrative attempts to study animal domestication "
           "and husbandry. Universitat Autònoma de Barcelona. <br> "
           "Copyright (c) Kaveh Yousef Poran <br> 2023"));
}

void MainWindow::LoadMesh() {
    // we could add this option {QFileDialog::DontUseNativeDialog}
    QString fileName = QFileDialog::getOpenFileName(
        this, "Mesh Files", QDir::homePath(),
        "OBJ Files (*.obj);;PLY Files (*.ply);;All Files (*)", nullptr,
        QFileDialog::DontUseNativeDialog);
    if (fileName.isEmpty()) {
    } else {
        if (fileName.endsWith(".obj") || fileName.endsWith(".ply")) {
            vtkNew<vtkCleanPolyData> cleanFilter;
            cleanFilter->PointMergingOn();
            cleanFilter->SetTolerance(0.0001);
            cleanFilter->ConvertLinesToPointsOn();
            cleanFilter->ConvertPolysToLinesOn();
            cleanFilter->ConvertStripsToPolysOn();
            if (fileName.endsWith(".obj")) {
                vtkNew<vtkOBJReader> objReader;
                objReader->SetFileName(fileName.toLocal8Bit().data());
                objReader->Update();
                cleanFilter->SetInputData(objReader->GetOutput());
                cleanFilter->Update();
            } else if (fileName.endsWith(".ply")) {
                vtkNew<vtkPLYReader> plyReader;
                plyReader->SetFileName(fileName.toLocal8Bit().data());
                plyReader->Update();
                cleanFilter->SetInputData(plyReader->GetOutput());
                cleanFilter->Update();
            }
            vtkNew <vtkTriangleFilter> triangleFilter;
            triangleFilter->SetInputConnection(cleanFilter->GetOutputPort());
            triangleFilter->Update();

            string realName = QFileInfo(fileName).baseName().toStdString();
            // name template is reserved
            if (realName == "Template") {
                realName += "1";
            }
            while (m_dataBase->CheckMembership(realName)) {
                realName += "_Duplicate";
            }

            m_dataBase->AddNode(realName, triangleFilter->GetOutput(), "Mesh");
            m_treeItem = new QTreeWidgetItem();
            m_treeItem->setText(0, QString::fromStdString(realName));
            m_treeItem->setText(1, "Mesh");
            m_treeWidget->addTopLevelItem(m_treeItem);
            if (m_treeWidget->selectedItems().size() == 0 &&
                m_treeWidget->topLevelItemCount()) {
                m_treeWidget
                    ->topLevelItem(m_treeWidget->topLevelItemCount() - 1)
                    ->setSelected(true);
            }
            if (meshPlotToolbarAction->isEnabled() != 1) {
                meshPlotToolbarAction->setEnabled(1);
                // exportVTKAction->setEnabled(1);
            }
            if (!superImpositionToolbarAction->isEnabled()) {
                superImpositionToolbarAction->setEnabled(1);
            }
            if (!pcaToolbarAction->isEnabled()) {
                pcaToolbarAction->setEnabled(1);
            }
        }
        if (!fileName.endsWith(".obj") && !fileName.endsWith(".ply")) {
            auto errorDialogue = QMessageBox(this);
            errorDialogue.setIcon(QMessageBox::Critical);
            errorDialogue.setWindowTitle("Error");
            errorDialogue.setText("No Suitable file was Selected");
            errorDialogue.exec();
        }
    }
}

void MainWindow::ExportCSV() {
    if (m_dataBase->CheckMembership("Template")) {
        delete m_exportDial;
        m_exportDial = new ExportDialogue(this);
        m_exportDial->show();
    } else {
        auto errorDialogue = QMessageBox(this);
        errorDialogue.setIcon(QMessageBox::Critical);
        errorDialogue.setWindowTitle("Error");
        errorDialogue.setText("Nothing to Export!");
        errorDialogue.exec();
    }
}

void MainWindow::DoExport() {
    if (m_lm) {
        auto filter = "csv(*.csv)";
        QString filename = QFileDialog::getSaveFileName(
            this, "Save Raw Landmarks", "", filter, nullptr,
            QFileDialog::DontUseNativeDialog);
        QFileInfo fi(filename);
        QString ext = fi.completeSuffix();
        if (filename.isEmpty()) {
            return;
        }
        if (ext != "csv") {
            filename += ".csv";
        }
        int numLM =
            m_dataBase->GetTotalLandmarks("Template")->GetNumberOfPoints();
        int numCols = (numLM * 3) + 1;
        QFile f(filename);
        if (f.open(QFile::WriteOnly | QFile::Truncate)) {
            QStringList textItem;
            QTextStream data(&f);
            textItem += "Name,";
            for (int i = 0; i < numLM; i++) {
                if (i == numLM - 1) {
                    textItem += "LM" + QString::number(i) + "X" + ",";
                    textItem += "LM" + QString::number(i) + "Y" + ",";
                    textItem += "LM" + QString::number(i) + "Z";
                } else {
                    textItem += "LM" + QString::number(i) + "X" + ",";
                    textItem += "LM" + QString::number(i) + "Y" + ",";
                    textItem += "LM" + QString::number(i) + "Z" + ",";
                }
            }
            data << textItem.join("") << ENDL;
            textItem.clear();
            for (int i = 0; i < mainTable->rowCount(); i++) {
                for (int j = 0; j < numCols; j++) {
                    QTableWidgetItem* item = mainTable->item(i, j);
                    if (item && !item->text().isEmpty()) {
                        if (j == numCols - 1) {
                            textItem += item->text();
                        } else {
                            textItem += item->text() + ",";
                        }
                    }
                }
                data << textItem.join("") << ENDL;
                textItem.clear();
            }
            f.close();
        }
    } else if (m_si) {
        auto filter = "csv(*.csv)";
        QString filename = QFileDialog::getSaveFileName(
            this, "Save SuperImposed Landmarks", "", filter, nullptr,
            QFileDialog::DontUseNativeDialog);
        QFileInfo fi(filename);
        QString ext = fi.completeSuffix();
        if (filename.isEmpty()) {
            return;
        }
        if (ext != "csv") {
            filename += ".csv";
        }
        int numLM =
            m_dataBase->GetTotalLandmarks("Template")->GetNumberOfPoints();
        int numCols = (numLM * 3) + 1;
        QFile f(filename);
        if (f.open(QFile::WriteOnly | QFile::Truncate)) {
            QStringList textItem;
            QTextStream data(&f);

            textItem += "Name,";
            for (int i = 0; i < numLM; i++) {
                if (i == numLM - 1) {
                    textItem += "LM" + QString::number(i) + "X" + ",";
                    textItem += "LM" + QString::number(i) + "Y" + ",";
                    textItem += "LM" + QString::number(i) + "Z";
                } else {
                    textItem += "LM" + QString::number(i) + "X" + ",";
                    textItem += "LM" + QString::number(i) + "Y" + ",";
                    textItem += "LM" + QString::number(i) + "Z" + ",";
                }
            }
            data << textItem.join("") << ENDL;
            textItem.clear();

            for (int i = 0; i < supImposedTable->rowCount(); i++) {
                for (int j = 0; j < numCols; j++) {
                    QTableWidgetItem* item = supImposedTable->item(i, j);
                    if (item && !item->text().isEmpty()) {
                        if (j == numCols - 1) {
                            textItem += item->text();
                        } else {
                            textItem += item->text() + ",";
                        }
                    }
                }
                data << textItem.join("") << ENDL;
                textItem.clear();
            }
            f.close();
        }
    } else if (m_pv) {
        auto filter = "csv(*.csv)";
        QString filename = QFileDialog::getSaveFileName(
            this, "Save Procrustes Residual Vectors", "", filter, nullptr,
            QFileDialog::DontUseNativeDialog);
        QFileInfo fi(filename);
        QString ext = fi.completeSuffix();
        if (filename.isEmpty()) {
            return;
        }
        if (ext != "csv") {
            filename += ".csv";
        }
        QFile f(filename);
        int numLM =
            m_dataBase->GetTotalLandmarks("Template")->GetNumberOfPoints();
        int numCols = (numLM * 3) + 1;
        if (f.open(QFile::WriteOnly | QFile::Truncate)) {
            QStringList textItem;
            QTextStream data(&f);
            textItem += "Name,";
            for (int i = 0; i < numLM; i++) {
                if (i == numLM - 1) {
                    textItem += "PV" + QString::number(i) + "X" + ",";
                    textItem += "PV" + QString::number(i) + "Y" + ",";
                    textItem += "PV" + QString::number(i) + "Z";
                } else {
                    textItem += "PV" + QString::number(i) + "X" + ",";
                    textItem += "PV" + QString::number(i) + "Y" + ",";
                    textItem += "PV" + QString::number(i) + "Z" + ",";
                }
            }
            data << textItem.join("") << ENDL;
            textItem.clear();
            for (int i = 0; i < procResTable->rowCount(); i++) {
                for (int j = 0; j < procResTable->columnCount(); j++) {
                    QTableWidgetItem* item = procResTable->item(i, j);
                    if (item && !item->text().isEmpty()) {
                        if (j == numCols - 1) {
                            textItem += item->text();
                        } else {
                            textItem += item->text() + ",";
                        }
                    }
                }
                data << textItem.join("") << ENDL;
                textItem.clear();
            }
            f.close();
        }
    } else if (m_pm) {
        auto filter = "csv(*.csv)";
        QString filename = QFileDialog::getSaveFileName(
            this, "Save Procrustes Residual Magnitudes", "", filter, nullptr,
            QFileDialog::DontUseNativeDialog);
        QFileInfo fi(filename);
        QString ext = fi.completeSuffix();
        if (filename.isEmpty()) {
            return;
        }
        if (ext != "csv") {
            filename += ".csv";
        }
        QFile f(filename);
        int numLM =
            m_dataBase->GetTotalLandmarks("Template")->GetNumberOfPoints();
        int numCols = (numLM * 3) + 1;
        if (f.open(QFile::WriteOnly | QFile::Truncate)) {
            QStringList textItem;
            QTextStream data(&f);
            textItem += "Name,";
            for (int i = 0; i < numLM; i++) {
                if (i == numLM - 1) {
                    textItem += "PR" + QString::number(i);
                } else {
                    textItem += "PR" + QString::number(i) + ",";
                }
            }
            data << textItem.join("") << ENDL;
            textItem.clear();
            std::vector<double>* doubleList = new std::vector<double>;
            for (int i = 0; i < procResTable->rowCount(); i++) {
                for (int j = 0; j < numCols; j++) {
                    QTableWidgetItem* item = procResTable->item(i, j);
                    if (item && !item->text().isEmpty()) {
                        if (j == 0) {
                            textItem += item->text() + ",";
                        } else {
                            std::string textNum = item->text().toStdString();
                            double num = 0;
                            bool convertable = 1;
                            try {
                                num = stod(textNum);
                            } catch (const std::exception& e) {
                                convertable = 0;
                            }
                            if (convertable) {
                                doubleList->push_back(num);
                            }
                        }
                    }
                }
                if (doubleList->size() % 3 == 0) {
                    for (int j = 0; j < doubleList->size(); j += 3) {
                        double x = doubleList->at(j);
                        double y = doubleList->at(j + 1);
                        double z = doubleList->at(j + 2);
                        double mag = sqrt((x * x) + (y * y) + (z * z));
                        QString item = QString::number(mag);
                        if (j == doubleList->size() - 1) {
                            textItem += item;
                        } else {
                            textItem += item + ",";
                        }
                    }
                } else {
                    std::cout << "Number of coordinates are not devisible by "
                                 "3; Debug!"
                              << std::endl;
                    std::cout << doubleList->size() << std::endl;
                }

                data << textItem.join("") << ENDL;
                textItem.clear();
                doubleList->clear();
            }
            f.close();
            delete doubleList;
        }
    }
}

void MainWindow::ExportGeometry() {
    // Check if any item is selected
    if (m_treeWidget->selectedItems().empty()) {
        QMessageBox errorDialogue(this);
        errorDialogue.setIcon(QMessageBox::Critical);
        errorDialogue.setWindowTitle("Error");
        errorDialogue.setText("Nothing to export!");
        errorDialogue.exec();
        return;
    }

    // Ask user for filename
    QString filter = "VTK PolyData (*.vtp)";
    QString filename =
        QFileDialog::getSaveFileName(this, "Save file", "", filter, nullptr,
                                     QFileDialog::DontUseNativeDialog);

    // If user cancels, filename is empty
    if (filename.isEmpty()) {
        return;
    }

    // Ensure .vtp extension
    QFileInfo fi(filename);
    if (fi.suffix().toLower() != "vtp") {
        filename += ".vtp";
    }

    // Get name from selected tree item
    QTreeWidgetItem* selectedItem = m_treeWidget->selectedItems()[0];
    QString itemName = selectedItem->text(0);

    // If child item, get parent's name instead
    if (selectedItem->parent()) {
        itemName = selectedItem->parent()->text(0);
    }

    std::string name = itemName.toStdString();
    std::string dType = m_dataBase->GetGeometryType(name);

    if (dType == "Mesh") {
        vtkPolyData* tempPoly = m_dataBase->GetPolyNode(name);
        if (!tempPoly) {
            QMessageBox::warning(this, "Export Failed",
                                 "Failed to access geometry.");
            return;
        }

        vtkNew<vtkXMLPolyDataWriter> writer;
        writer->SetInputData(tempPoly);
        writer->SetFileName(filename.toStdString().c_str());
        writer->SetDataModeToBinary();  // optional: smaller file
        writer->Write();
    } else {
        QMessageBox::warning(
            this, "Unsupported Format",
            "Only OBJ/PLY-based geometries are supported for export.");
    }
}

void MainWindow::DICOMPlot() { cout << "Plotting DICOM" << endl; }

void MainWindow::SuperImpose() {
    if (TemplateIsSet) {
        if (m_dataBase->GetNodeNames().size() > 1) {
            LockTheWindow();
            statusLabel->setText("Status: Busy");
            progressLabel->setPixmap(
                QPixmap(":/icons/graphics/icons/vBusy.svg"));
            delete m_SIMP;
            m_SIMP = new SuperImposition(m_dataBase, this, m_mutex);
        } else {
            auto errorDialogue = QMessageBox(this);
            errorDialogue.setIcon(QMessageBox::Critical);
            errorDialogue.setWindowTitle("Error");
            errorDialogue.setText("Digitise a mesh, first!");
            errorDialogue.exec();
            superImpositionToolbarAction->setEnabled(0);
        }
    } else {
        auto errorDialogue = QMessageBox(this);
        // QPixmap Quim(":/icons/graphics/icons/Quim.png");
        errorDialogue.setIcon(QMessageBox::Critical);
        // errorDialogue.setIconPixmap(Quim);
        errorDialogue.setWindowTitle("Error");
        errorDialogue.setText(
            "Project setting was not found! \n If the setting is already "
            "configured,\n the REGISTER button has not been pushed!");
        errorDialogue.exec();
    }
}

void MainWindow::ResetImposition() {
    supImposedTable->clear();
    supImposedTable->setColumnCount(TableColNum);
    supImposedTable->setRowCount(TableRowNum);
    SetLandmarkHeaders(supImposedTable);
    procResTable->clear();
    procResTable->setColumnCount(TableColNum);
    procResTable->setRowCount(TableRowNum);
    SetLandmarkHeaders(procResTable);
}

void MainWindow::FinaliseImposition() {
    UnlockTheWindow();
    statusLabel->setText("Status: Idle");
    progressLabel->setPixmap(QPixmap(":/icons/graphics/icons/idle.svg"));
}

void MainWindow::OnSuperImposed(std::string name) {
    m_mutex->lock();
    vtkPolyData* coordsPoly = m_dataBase->GetTotalLandmarks(name);
    int freeRow;
    for (int i = 0; i < supImposedTable->rowCount(); i++) {
        if (!supImposedTable->item(i, 0) ||
            supImposedTable->item(i, 0)->text().isEmpty()) {
            freeRow = i;
            break;
        }
    }

    supImposedTable->setItem(
        freeRow, 0, new QTableWidgetItem(QString::fromStdString(name)));
    for (int i = 0; i < coordsPoly->GetNumberOfPoints(); i++) {
        for (int j = 0; j < 3; j++) {
            double item = coordsPoly->GetPoint(i)[j];
            int index = (3 * i) + j + 1;
            std::stringstream stream;
            stream << std::fixed << std::setprecision(4) << item;
            std::string convString = stream.str();
            supImposedTable->setItem(
                freeRow, index,
                new QTableWidgetItem(QString::fromStdString(convString)));
        }
    }

    for (int i = 0; i < procResTable->rowCount(); i++) {
        if (!procResTable->item(i, 0) ||
            procResTable->item(i, 0)->text().isEmpty()) {
            freeRow = i;
            break;
        }
    }

    procResTable->setItem(freeRow, 0,
                          new QTableWidgetItem(QString::fromStdString(name)));
    for (int j = 0; j < coordsPoly->GetNumberOfPoints(); j++) {
        for (int k = 0; k < 3; k++) {
            double item = 0;
            if (name != "Template") {
                item = coordsPoly->GetPointData()
                           ->GetArray("ProcrustesVector")
                           ->GetTuple3(j)[k];
            } else if (name == "Template") {
                item = 0;
            }
            int index = (3 * j) + k + 1;
            std::stringstream stream;
            stream << std::fixed << std::setprecision(4) << item;
            std::string convString = stream.str();
            procResTable->setItem(
                freeRow, index,
                new QTableWidgetItem(QString::fromStdString(convString)));
        }
    }
    m_mutex->unlock();
    std::string dataName;
    if (m_treeWidget->selectedItems()[0]->parent()) {
        dataName =
            m_treeWidget->selectedItems()[0]->parent()->text(0).toStdString();
    } else {
        dataName = m_treeWidget->selectedItems()[0]->text(0).toStdString();
    }
}

void MainWindow::PDist(vtkPoints* points, Eigen::MatrixXd& output) {
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
    output.triangularView<Eigen::Lower>() = output.transpose();
}

double MainWindow::EucDist(double Ax, double Ay, double Az, double Bx,
                           double By, double Bz) {
    double dx = Ax - Bx;
    double dy = Ay - By;
    double dz = Az - Bz;
    double dist = sqrt((dx * dx) + (dy * dy) + (dz * dz));
    return dist;
}

void MainWindow::DebugPrintMatrix(Eigen::MatrixXd matrix) {
    Eigen::IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");
    std::string sep = "\n----------------------------------------\n";
    std::cout << matrix.format(CleanFmt) << sep;
}

void MainWindow::TemplatePlot() { m_templateView->show(); }

void MainWindow::meshPlot() {
    if (!TemplateIsSet) {
        auto errorDialogue = QMessageBox(this);
        errorDialogue.setIcon(QMessageBox::Critical);
        errorDialogue.setWindowTitle("Error");
        errorDialogue.setText(
            "Project setting was not found! \n If the setting is already "
            "configured,\n the REGISTER button has not been pushed!");
        errorDialogue.exec();
    } else {
        if (!m_treeWidget->selectedItems().isEmpty()) {
            auto name = m_treeWidget->selectedItems()[0]->text(0).toStdString();
            if (m_treeWidget->selectedItems()[0]->parent()) {
                name = m_treeWidget->selectedItems()[0]
                           ->parent()
                           ->text(0)
                           .toStdString();
            }
            std::string dType = m_dataBase->GetGeometryType(name);
            m_currentMesh = m_dataBase->GetPolyNode(name);

            if (dType == "Mesh" && m_currentMesh != nullptr) {
                m_treeWidget->setSelectionMode(QAbstractItemView::NoSelection);
                if (m_treeWidget->selectedItems()[0]->parent()) {
                    m_treeWidget->blockSignals(true);
                    m_treeWidget->selectedItems()[0]->parent()->setIcon(
                        0, QIcon(":/icons/graphics/icons/hourglass.png"));
                    m_treeWidget->blockSignals(false);
                } else {
                    m_treeWidget->blockSignals(true);
                    m_treeWidget->selectedItems()[0]->setIcon(
                        0, QIcon(":/icons/graphics/icons/hourglass.png"));
                    m_treeWidget->blockSignals(false);
                }
                LockTheWindow();
                statusLabel->setText("Status: Busy");
                progressLabel->setPixmap(
                    QPixmap(":/icons/graphics/icons/vBusy.svg"));
                delete m_meshPlot;
                m_meshPlot = new SpecimenDigitiser(m_currentMesh, this);
            }
        } else {
            if (m_treeWidget->topLevelItemCount() > 0) {
                auto errorDialogue = QMessageBox(this);
                errorDialogue.setIcon(QMessageBox::Critical);
                errorDialogue.setWindowTitle("Error");
                errorDialogue.setText("Select a geometry to proceed!");
                errorDialogue.exec();
            } else {
                auto errorDialogue = QMessageBox(this);
                errorDialogue.setIcon(QMessageBox::Critical);
                errorDialogue.setWindowTitle("Error");
                errorDialogue.setText("Import a mesh, first!");
                errorDialogue.exec();
                meshPlotToolbarAction->setEnabled(0);
            }
        }
    }
}

void MainWindow::UpdateActiveData() {
    if (m_treeWidget->selectedItems().empty()) {
        m_currentMesh->Initialize();
        m_currentGrid->Initialize();
    } else {
        if (!m_treeWidget->selectedItems()[0]->parent()) {
            auto name = m_treeWidget->selectedItems()[0]->text(0).toStdString();
            std::string dType = m_dataBase->GetGeometryType(name);
            if (dType == "Mesh") {
                m_currentMesh = m_dataBase->GetPolyNode(name);
                PaintMesh(name);
                m_mainMeshActor->SetMapper(m_mainMeshMapper);
                m_mainMeshActor->Modified();

                vtkPoints* fixedLM = m_dataBase->GetTypeI(name);
                vtkPoints* curveLM = m_dataBase->GetCurveSliders(name);
                vtkPoints* surfaceLM = m_dataBase->GetSurfaceSliders(name);

                PlotLandmarks(fixedLM, curveLM, surfaceLM, m_currentMesh);
                m_mainRenderer->ResetCamera();
                m_mainRenderer->GetRenderWindow()->Render();
            }
            if (dType == "DICOM") {
                m_currentGrid = m_dataBase->GetGridNode(name);
            }
        } else {
            auto name = m_treeWidget->selectedItems()[0]
                            ->parent()
                            ->text(0)
                            .toStdString();
            auto branchName = m_treeWidget->selectedItems()[0]->text(0)
                            .toStdString();
            std::string dType = m_dataBase->GetGeometryType(name);
            if (dType == "Mesh") {
                m_currentMesh = m_dataBase->GetPolyNode(name);
                PaintMesh(name);
                m_mainMeshActor->SetMapper(m_mainMeshMapper);
                m_mainMeshActor->Modified();

                vtkPoints* fixedLM = m_dataBase->GetTypeI(name);
                vtkPoints* curveLM = m_dataBase->GetCurveSliders(name);
                vtkPoints* surfaceLM = m_dataBase->GetSurfaceSliders(name);
                PlotLandmarks(fixedLM, curveLM, surfaceLM, m_currentMesh, branchName);
                m_mainRenderer->ResetCamera();
                m_mainRenderer->GetRenderWindow()->Render();
            }
            if (dType == "DICOM") {
                m_currentGrid = m_dataBase->GetGridNode(name);
            }
        }
    }
}

void MainWindow::ShowContextMenu(const QPoint& pos) {
    auto index = m_treeWidget->selectedItems();
    if (index.count() > 0) {
        if (m_treeWidget->selectedItems()[0]->parent()) {
            branchPopUpMenu.exec(m_treeWidget->mapToGlobal(pos));
        } else {
            nodePopUpMenu.exec(m_treeWidget->mapToGlobal(pos));
        }
    }
}

void MainWindow::RenameDataNode() {
    if (m_treeWidget->selectedItems()[0]->parent()) {
    } else {
        m_oldName = m_treeWidget->selectedItems()[0]->text(0).toStdString();
        m_treeWidget->blockSignals(1);
        m_treeWidget->selectedItems()[0]->setFlags(
            m_treeWidget->selectedItems()[0]->flags() | Qt::ItemIsEditable);
        m_treeWidget->editItem(m_treeWidget->selectedItems()[0]);
        m_treeWidget->selectedItems()[0]->setFlags(
            m_treeWidget->selectedItems()[0]->flags() & ~Qt::ItemIsEditable);
        m_treeWidget->blockSignals(0);
    }
}

void MainWindow::DeleteDataNode() {
    auto choice = QMessageBox::question(this, "Warning",
                                        "Do you want to DELETE this object?",
                                        QMessageBox::Yes | QMessageBox::No);

    if (choice == QMessageBox::Yes) {
        QTreeWidgetItem* root = m_treeWidget->invisibleRootItem();
        auto name = m_treeWidget->selectedItems()[0]->text(0).toStdString();
        m_dataBase->DeleteNode(name);
        while (m_treeWidget->selectedItems()[0]->childCount()) {
            QTreeWidgetItem* c = m_treeWidget->selectedItems()[0]->child(0);
            m_treeWidget->selectedItems()[0]->removeChild(c);
            delete c;
        }
        for (auto items : m_treeWidget->selectedItems()) {
            root->removeChild(items);
            delete items;
        }
        //-----------------------------------
        QList oldItem = mainTable->findItems(QString::fromStdString(name),
                                             Qt::MatchExactly);
        if (oldItem.count() > 0) {
            int row = oldItem.at(0)->row();
            mainTable->removeRow(row);
        }
        oldItem = supImposedTable->findItems(QString::fromStdString(name),
                                             Qt::MatchExactly);
        if (oldItem.count() > 0) {
            int row = oldItem.at(0)->row();
            supImposedTable->removeRow(row);
        }
        oldItem = procResTable->findItems(QString::fromStdString(name),
                                          Qt::MatchExactly);
        if (oldItem.count() > 0) {
            int row = oldItem.at(0)->row();
            procResTable->removeRow(row);
        }
        //-----------------------------------
        if (m_treeWidget->topLevelItemCount() == 0) {
            meshPlotToolbarAction->setEnabled(0);
            superImpositionToolbarAction->setEnabled(0);
            pcaToolbarAction->setEnabled(0);
            // exportVTKAction->setEnabled(0);

            vtkNew<vtkVectorText> vecText;
            vecText->SetText("Nothing to plot for the moment!");
            vtkNew<vtkLinearExtrusionFilter> extrude;
            extrude->SetInputConnection(vecText->GetOutputPort());
            extrude->SetExtrusionTypeToNormalExtrusion();
            extrude->SetVector(0, 0, 1);
            extrude->SetScaleFactor(0.5);
            extrude->Update();
            m_mainMeshMapper->SetInputData(extrude->GetOutput());
            m_mainMeshActor->SetMapper(m_mainMeshMapper);
            m_mainMeshActor->Modified();
            m_mainRenderer->RemoveActor(m_scalarBar);
            m_mainRenderer->RemoveActor(m_fixedLmActor);
            m_mainRenderer->RemoveActor(m_curveLmActor);
            m_mainRenderer->RemoveActor(m_surfaceLmActor);
            m_mainRenderer->ResetCamera();
            m_mainRenderer->GetRenderWindow()->Render();
        }
    }
}

void MainWindow::DeleteDataBranch() {
    auto choice = QMessageBox::question(
        this, "Warning",
        "This will delete the digitised landmarks! \n Do you want to continue?",
        QMessageBox::Yes | QMessageBox::No);
    if (choice == QMessageBox::Yes) {
        auto parent = m_treeWidget->selectedItems()[0]->parent();
        auto nodeName =
            m_treeWidget->selectedItems()[0]->parent()->text(0).toStdString();
        auto name = m_treeWidget->selectedItems()[0]->text(0).toStdString();
        while (parent->childCount()) {
            QTreeWidgetItem* c = parent->child(0);
            parent->removeChild(c);
            delete c;
        }
        if (name == "Type I-II" || name == "Curve Slider" ||
            name == "Surface Slider") {
            m_dataBase->DeleteAllLandmarks(nodeName);

            QList oldItem = mainTable->findItems(
                QString::fromStdString(nodeName), Qt::MatchExactly);
            if (oldItem.count() > 0) {
                int row = oldItem.at(0)->row();
                mainTable->removeRow(row);
            }
            oldItem = supImposedTable->findItems(
                QString::fromStdString(nodeName), Qt::MatchExactly);
            if (oldItem.count() > 0) {
                int row = oldItem.at(0)->row();
                supImposedTable->removeRow(row);
            }
            oldItem = procResTable->findItems(QString::fromStdString(nodeName),
                                              Qt::MatchExactly);
            if (oldItem.count() > 0) {
                int row = oldItem.at(0)->row();
                procResTable->removeRow(row);
            }
            // Update rendering stuff
            ResetLMData(nodeName);
        } else {
            std::cout << "This shouldn't happen" << std::endl;
        }
    }
}

void MainWindow::ResetLMData(std::string name) {
    PaintMesh(name);
    m_mainMeshActor->SetMapper(m_mainMeshMapper);
    m_mainMeshActor->Modified();

    vtkPoints* fixedLM = m_dataBase->GetTypeI(name);
    vtkPoints* curveLM = m_dataBase->GetCurveSliders(name);
    vtkPoints* surfaceLM = m_dataBase->GetSurfaceSliders(name);
    vtkPolyData* tempPoly = m_dataBase->GetPolyNode(name);
    PlotLandmarks(fixedLM, curveLM, surfaceLM, tempPoly);
    m_mainRenderer->GetRenderWindow()->Render();
}

void MainWindow::UpdateNodeName() {
    m_newName = m_treeWidget->selectedItems()[0]->text(0).toStdString();
    if (m_newName == "Template") {
        m_newName += "1";
    }
    if (m_dataBase->CheckMembership(m_newName)) {
        m_treeWidget->blockSignals(1);
        m_newName += "_Duplicate";
        m_treeWidget->selectedItems()[0]->setText(
            0, QString::fromStdString(m_newName));
        m_treeWidget->blockSignals(0);
    }
    m_dataBase->RenameNode(m_oldName, m_newName);
    QList oldItem = mainTable->findItems(QString::fromStdString(m_oldName),
                                         Qt::MatchExactly);
    if (oldItem.count() > 0) {
        int row = oldItem.at(0)->row();
        mainTable->setItem(
            row, 0, new QTableWidgetItem(QString::fromStdString(m_newName)));
    }
    oldItem = supImposedTable->findItems(QString::fromStdString(m_oldName),
                                         Qt::MatchExactly);
    if (oldItem.count() > 0) {
        int row = oldItem.at(0)->row();
        supImposedTable->setItem(
            row, 0, new QTableWidgetItem(QString::fromStdString(m_newName)));
    }
    oldItem = procResTable->findItems(QString::fromStdString(m_oldName),
                                      Qt::MatchExactly);
    if (oldItem.count() > 0) {
        int row = oldItem.at(0)->row();
        procResTable->setItem(
            row, 0, new QTableWidgetItem(QString::fromStdString(m_newName)));
    }
}

void MainWindow::UpdateDataBase() {
    if (m_status == STATUS::outdated) {
        if (m_treeWidget->selectedItems()[0]->parent()) {
            m_treeWidget->blockSignals(true);
            m_treeWidget->selectedItems()[0]->parent()->setIcon(0, QIcon());
            m_treeWidget->blockSignals(false);
        } else {
            m_treeWidget->blockSignals(true);
            m_treeWidget->selectedItems()[0]->setIcon(0, QIcon());
            m_treeWidget->blockSignals(false);
        }
        m_status = STATUS::neutral;
        UnlockTheWindow();
        statusLabel->setText("Status: Idle");
        progressLabel->setPixmap(QPixmap(":/icons/graphics/icons/idle.svg"));
        std::string dataName =
            m_treeWidget->selectedItems()[0]->text(0).toStdString();
        if (m_treeWidget->selectedItems()[0]->parent()) {
            dataName = m_treeWidget->selectedItems()[0]
                           ->parent()
                           ->text(0)
                           .toStdString();
        }
        vtkPoints* fixedLM = m_dataBase->GetTypeI(dataName);
        vtkPoints* curveLM = m_dataBase->GetCurveSliders(dataName);
        vtkPoints* surfaceLM = m_dataBase->GetSurfaceSliders(dataName);
        vtkPolyData* tempMesh = m_dataBase->GetPolyNode(dataName);
        PaintMesh(dataName);
        m_mainMeshActor->SetMapper(m_mainMeshMapper);
        m_mainMeshActor->Modified();
        PlotLandmarks(fixedLM, curveLM, surfaceLM, tempMesh);
        m_mainRenderer->GetRenderWindow()->Render();
        /* delete m_meshPlot;
        m_meshPlot = nullptr; */
    }
    if (m_status == STATUS::neutral) {
        if (m_treeWidget->selectedItems()[0]->parent()) {
            m_treeWidget->blockSignals(true);
            m_treeWidget->selectedItems()[0]->parent()->setIcon(0, QIcon());
            m_treeWidget->blockSignals(false);
        } else {
            m_treeWidget->blockSignals(true);
            m_treeWidget->selectedItems()[0]->setIcon(0, QIcon());
            m_treeWidget->blockSignals(false);
        }
        UnlockTheWindow();
        statusLabel->setText("Status: Idle");
        progressLabel->setPixmap(QPixmap(":/icons/graphics/icons/idle.svg"));
        std::string dataName =
            m_treeWidget->selectedItems()[0]->text(0).toStdString();
        if (m_treeWidget->selectedItems()[0]->parent()) {
            dataName = m_treeWidget->selectedItems()[0]
                           ->parent()
                           ->text(0)
                           .toStdString();
        }
        vtkPoints* fixedLM = m_dataBase->GetTypeI(dataName);
        vtkPoints* curveLM = m_dataBase->GetCurveSliders(dataName);
        vtkPoints* surfaceLM = m_dataBase->GetSurfaceSliders(dataName);
        vtkPolyData* tempMesh = m_dataBase->GetPolyNode(dataName);
        PlotLandmarks(fixedLM, curveLM, surfaceLM, tempMesh);
    }
    if (m_status == STATUS::reset) {
        UnlockTheWindow();
        statusLabel->setText("Status: Idle");
        progressLabel->setPixmap(QPixmap(":/icons/graphics/icons/idle.svg"));
        if (m_treeWidget->topLevelItemCount() > 0) {
            std::string dataName =
                m_treeWidget->selectedItems()[0]->text(0).toStdString();
            if (m_treeWidget->selectedItems()[0]->parent()) {
                dataName = m_treeWidget->selectedItems()[0]
                               ->parent()
                               ->text(0)
                               .toStdString();
                m_treeWidget->blockSignals(true);
                m_treeWidget->selectedItems()[0]->parent()->setIcon(0, QIcon());
                m_treeWidget->blockSignals(false);
            } else {
                m_treeWidget->blockSignals(true);
                m_treeWidget->selectedItems()[0]->setIcon(0, QIcon());
                m_treeWidget->blockSignals(false);
            }
            vtkPoints* fixedLM = m_dataBase->GetTypeI(dataName);
            vtkPoints* curveLM = m_dataBase->GetCurveSliders(dataName);
            vtkPoints* surfaceLM = m_dataBase->GetSurfaceSliders(dataName);
            PlotLandmarks(fixedLM, curveLM, surfaceLM, m_currentMesh);
        }
        m_status = STATUS::neutral;
    }
}

void MainWindow::SetTypeI(vtkPoints* fixedPts) {
    auto name = m_treeWidget->selectedItems()[0]->text(0).toStdString();
    if (m_treeWidget->selectedItems()[0]->parent()) {
        name =
            m_treeWidget->selectedItems()[0]->parent()->text(0).toStdString();
    }
    if (m_dataBase->GetTypeI(name)->GetNumberOfPoints() > 0) {
        m_dataBase->DeleteTypeI(name);
        int targetRow;
        for (int i = 0; i < mainTable->rowCount(); i++) {
            if (mainTable->item(i, 0) &&
                mainTable->item(i, 0)->text() == QString::fromStdString(name)) {
                targetRow = i;
                break;
            }
        }
        mainTable->removeRow(targetRow);
        QTreeWidgetItem* parent = m_treeWidget->selectedItems()[0];
        while (parent->childCount()) {
            QTreeWidgetItem* c = parent->child(0);
            parent->removeChild(c);
            delete c;
        }
    }
    m_dataBase->InsertTypeI(name, fixedPts);
    if (!m_treeWidget->selectedItems()[0]->parent()) {
        if (m_treeWidget->selectedItems()[0]->childCount() == 0) {
            m_typeIBranch = new QTreeWidgetItem();
            m_typeIBranch->setText(0, "Type I-II");
            m_treeWidget->selectedItems()[0]->addChild(m_typeIBranch);
            m_treeWidget->selectedItems()[0]->setExpanded(1);
        }
    }

    int colCount = mainTable->horizontalHeader()->count();
    int diff = colCount - (1 + (fixedPts->GetNumberOfPoints() * 3));
    if (diff < 0) {
        mainTable->setColumnCount(colCount + std::abs(diff));
    }

    int freeRow;
    for (int i = 0; i < mainTable->rowCount(); i++) {
        if (!mainTable->item(i, 0) || mainTable->item(i, 0)->text().isEmpty()) {
            freeRow = i;
            break;
        }
    }
    mainTable->setItem(freeRow, 0,
                       new QTableWidgetItem(QString::fromStdString(name)));
    for (int i = 0; i < fixedPts->GetNumberOfPoints(); i++) {
        for (int j = 0; j < 3; j++) {
            double item = fixedPts->GetPoint(i)[j];
            int index = (3 * i) + j + 1;
            std::stringstream stream;
            stream << std::fixed << std::setprecision(4) << item;
            std::string convString = stream.str();
            mainTable->setItem(
                freeRow, index,
                new QTableWidgetItem(QString::fromStdString(convString)));
        }
    }
}

void MainWindow::SetSliders(vtkPoints* fixedPts, vtkPoints* curveSliderPts,
                            vtkPoints* surfaceSliderPts) {
    auto name = m_treeWidget->selectedItems()[0]->text(0).toStdString();
    if (m_treeWidget->selectedItems()[0]->parent()) {
        name =
            m_treeWidget->selectedItems()[0]->parent()->text(0).toStdString();
    }
    vtkNew<vtkPoints> totalPts;
    if (m_dataBase->GetTotalLandmarks(name)->GetNumberOfPoints() > 0) {
        m_dataBase->DeleteAllLandmarks(name);
        int targetRow;
        for (int i = 0; i < mainTable->rowCount(); i++) {
            if (mainTable->item(i, 0) &&
                mainTable->item(i, 0)->text() == QString::fromStdString(name)) {
                targetRow = i;
                break;
            }
        }
        mainTable->removeRow(targetRow);
        QTreeWidgetItem* parent = m_treeWidget->selectedItems()[0];
        while (parent->childCount()) {
            QTreeWidgetItem* c = parent->child(0);
            parent->removeChild(c);
            delete c;
        }
    }

    if (fixedPts->GetNumberOfPoints() > 0) {
        m_dataBase->InsertTypeI(name, fixedPts);
        for (int i = 0; i < fixedPts->GetNumberOfPoints(); i++) {
            totalPts->InsertNextPoint(fixedPts->GetPoint(i));
        }
        if (!m_treeWidget->selectedItems()[0]->parent()) {
            m_typeIBranch = new QTreeWidgetItem();
            m_typeIBranch->setText(0, "Type I-II");
            m_treeWidget->selectedItems()[0]->addChild(m_typeIBranch);
            m_treeWidget->selectedItems()[0]->setExpanded(1);
        }
    }
    if (curveSliderPts->GetNumberOfPoints() > 0) {
        for (int i = 0; i < curveSliderPts->GetNumberOfPoints(); i++) {
            totalPts->InsertNextPoint(curveSliderPts->GetPoint(i));
        }
        if (!m_treeWidget->selectedItems()[0]->parent()) {
            m_curveSliderBranch = new QTreeWidgetItem();
            m_curveSliderBranch->setText(0, "Curve Slider");
            m_treeWidget->selectedItems()[0]->addChild(m_curveSliderBranch);
            m_treeWidget->selectedItems()[0]->setExpanded(1);
        }

        m_dataBase->InsertCurveSliders(name, curveSliderPts);
    }
    if (surfaceSliderPts->GetNumberOfPoints() > 0) {
        for (int i = 0; i < surfaceSliderPts->GetNumberOfPoints(); i++) {
            totalPts->InsertNextPoint(surfaceSliderPts->GetPoint(i));
        }
        m_dataBase->InsertSurfaceSliders(name, surfaceSliderPts);
        if (!m_treeWidget->selectedItems()[0]->parent()) {
            m_surfaceSliderBranch = new QTreeWidgetItem();
            m_surfaceSliderBranch->setText(0, "Surface Slider");
            m_treeWidget->selectedItems()[0]->addChild(m_surfaceSliderBranch);
            m_treeWidget->selectedItems()[0]->setExpanded(1);
        }
    }
    totalPts->Modified();
    int colCount = mainTable->horizontalHeader()->count();
    int diff = colCount - (1 + (totalPts->GetNumberOfPoints() * 3));
    if (diff < 0) {
        mainTable->setColumnCount(colCount + std::abs(diff));
    }
    int freeRow;
    for (int i = 0; i < mainTable->rowCount(); i++) {
        if (!mainTable->item(i, 0) || mainTable->item(i, 0)->text().isEmpty()) {
            freeRow = i;
            break;
        }
    }

    mainTable->setItem(freeRow, 0,
                       new QTableWidgetItem(QString::fromStdString(name)));
    for (int i = 0; i < totalPts->GetNumberOfPoints(); i++) {
        for (int j = 0; j < 3; j++) {
            double item = totalPts->GetPoint(i)[j];
            int index = (3 * i) + j + 1;
            std::stringstream stream;
            stream << std::fixed << std::setprecision(4) << item;
            std::string convString = stream.str();
            mainTable->setItem(
                freeRow, index,
                new QTableWidgetItem(QString::fromStdString(convString)));
        }
    }
}

void MainWindow::SetSliders(vtkPoints* fixedPts, vtkPoints* curveSliderPts,
                            vtkPoints* surfaceSliderPts, std::string name) {
    vtkNew<vtkPoints> totalPts;
    int itemNum = m_treeWidget->topLevelItemCount();
    if (m_dataBase->GetTotalLandmarks(name)->GetNumberOfPoints() > 0) {
        int targetRow = -1;
        for (int i = 0; i < mainTable->rowCount(); i++) {
            if (mainTable->item(i, 0) &&
                mainTable->item(i, 0)->text() == QString::fromStdString(name)) {
                targetRow = i;
                break;
            }
        }
        if (targetRow >= 0) {
            mainTable->removeRow(targetRow);
        }

        QTreeWidgetItem* parent = m_treeWidget->topLevelItem(itemNum - 1);
        while (parent->childCount()) {
            QTreeWidgetItem* c = parent->child(0);
            parent->removeChild(c);
            delete c;
        }
    }
    if (fixedPts->GetNumberOfPoints() > 0) {
        for (int i = 0; i < fixedPts->GetNumberOfPoints(); i++) {
            totalPts->InsertNextPoint(fixedPts->GetPoint(i));
        }
        m_typeIBranch = new QTreeWidgetItem();
        m_typeIBranch->setText(0, "Type I-II");
        m_treeWidget->topLevelItem(itemNum - 1)->addChild(m_typeIBranch);
        m_treeWidget->topLevelItem(itemNum - 1)->setExpanded(1);
    }
    if (curveSliderPts->GetNumberOfPoints() > 0) {
        for (int i = 0; i < curveSliderPts->GetNumberOfPoints(); i++) {
            totalPts->InsertNextPoint(curveSliderPts->GetPoint(i));
        }
        m_curveSliderBranch = new QTreeWidgetItem();
        m_curveSliderBranch->setText(0, "Curve Slider");
        m_treeWidget->topLevelItem(itemNum - 1)->addChild(m_curveSliderBranch);
        m_treeWidget->topLevelItem(itemNum - 1)->setExpanded(1);
    }
    if (surfaceSliderPts->GetNumberOfPoints() > 0) {
        for (int i = 0; i < surfaceSliderPts->GetNumberOfPoints(); i++) {
            totalPts->InsertNextPoint(surfaceSliderPts->GetPoint(i));
        }
        m_surfaceSliderBranch = new QTreeWidgetItem();
        m_surfaceSliderBranch->setText(0, "Surface Slider");
        m_treeWidget->topLevelItem(itemNum - 1)
            ->addChild(m_surfaceSliderBranch);
        m_treeWidget->topLevelItem(itemNum - 1)->setExpanded(1);
    }
    totalPts->Modified();
    int colCount = mainTable->horizontalHeader()->count();
    int diff = colCount - (1 + (totalPts->GetNumberOfPoints() * 3));
    if (diff < 0) {
        mainTable->setColumnCount(colCount + std::abs(diff));
    }
    int freeRow;
    for (int i = 0; i < mainTable->rowCount(); i++) {
        if (!mainTable->item(i, 0) || mainTable->item(i, 0)->text().isEmpty()) {
            freeRow = i;
            break;
        }
    }
    mainTable->setItem(freeRow, 0,
                       new QTableWidgetItem(QString::fromStdString(name)));
    for (int i = 0; i < totalPts->GetNumberOfPoints(); i++) {
        for (int j = 0; j < 3; j++) {
            double item = totalPts->GetPoint(i)[j];
            int index = (3 * i) + j + 1;
            std::stringstream stream;
            stream << std::fixed << std::setprecision(4) << item;
            std::string convString = stream.str();
            mainTable->setItem(
                freeRow, index,
                new QTableWidgetItem(QString::fromStdString(convString)));
        }
    }
}

void MainWindow::SetStatus(STATUS status) { m_status = status; }

void MainWindow::TemplateStatus(bool status) {
    TemplateIsSet = status;
    if (status == 1) {
        templatePlotToolbarAction->setEnabled(1);
        auto name = "Template";
        m_dataBase->AddNode(name, m_templateMesh, m_templateMeshType);
        vtkNew<vtkDataObjectTreeIterator> iterCurveSlider;
        vtkNew<vtkPoints> curveSliderPts;
        iterCurveSlider->SetDataSet(m_templateCurveSliders);
        iterCurveSlider->SkipEmptyNodesOn();
        iterCurveSlider->VisitOnlyLeavesOn();
        for (iterCurveSlider->InitTraversal();
             !iterCurveSlider->IsDoneWithTraversal();
             iterCurveSlider->GoToNextItem()) {
            vtkDataObject* dso = iterCurveSlider->GetCurrentDataObject();
            vtkPolyData* pd = dynamic_cast<vtkPolyData*>(dso);
            for (int i = 0; i < pd->GetNumberOfPoints(); i++) {
                curveSliderPts->InsertNextPoint(pd->GetPoint(i));
            }
        }

        vtkNew<vtkDataObjectTreeIterator> iterSurfacePatchSlider;
        vtkNew<vtkPoints> surfacePatchSliderPts;
        iterSurfacePatchSlider->SetDataSet(m_templatePatchSurfaceSliders);
        iterSurfacePatchSlider->SkipEmptyNodesOn();
        iterSurfacePatchSlider->VisitOnlyLeavesOn();
        for (iterSurfacePatchSlider->InitTraversal();
             !iterSurfacePatchSlider->IsDoneWithTraversal();
             iterSurfacePatchSlider->GoToNextItem()) {
            vtkDataObject* dso = iterSurfacePatchSlider->GetCurrentDataObject();
            vtkPolyData* pd = dynamic_cast<vtkPolyData*>(dso);
            for (int i = 0; i < pd->GetNumberOfPoints(); i++) {
                surfacePatchSliderPts->InsertNextPoint(pd->GetPoint(i));
            }
        }
        vtkNew<vtkPoints> totalPts;
        if (m_dataBase->GetTotalLandmarks(name) > 0) {
            m_dataBase->DeleteAllLandmarks(name);
            int targetRow;
            for (int i = 0; i < mainTable->rowCount(); i++) {
                if (mainTable->item(i, 0) && mainTable->item(i, 0)->text() ==
                                                 QString::fromStdString(name)) {
                    targetRow = i;
                    break;
                }
            }
            mainTable->removeRow(targetRow);
        }
        if (m_templateTypeI->GetNumberOfPoints() > 0) {
            m_dataBase->InsertTypeI(name, m_templateTypeI);
            for (int i = 0; i < m_templateTypeI->GetNumberOfPoints(); i++) {
                totalPts->InsertNextPoint(m_templateTypeI->GetPoint(i));
            }
        }
        if (curveSliderPts->GetNumberOfPoints() > 0) {
            m_dataBase->InsertCurveSliders(name, curveSliderPts);
            for (int i = 0; i < curveSliderPts->GetNumberOfPoints(); i++) {
                totalPts->InsertNextPoint(curveSliderPts->GetPoint(i));
            }
        }
        if (m_templateSurfaceSliders->GetNumberOfPoints() > 0 &&
            surfacePatchSliderPts->GetNumberOfPoints() == 0) {
            m_dataBase->InsertSurfaceSliders(name, m_templateSurfaceSliders);
            for (int i = 0; i < m_templateSurfaceSliders->GetNumberOfPoints();
                 i++) {
                totalPts->InsertNextPoint(
                    m_templateSurfaceSliders->GetPoint(i));
            }
        }
        if (surfacePatchSliderPts->GetNumberOfPoints() > 0 &&
            m_templateSurfaceSliders->GetNumberOfPoints() == 0) {
            m_dataBase->InsertSurfaceSliders(name, surfacePatchSliderPts);
            for (int i = 0; i < surfacePatchSliderPts->GetNumberOfPoints();
                 i++) {
                totalPts->InsertNextPoint(surfacePatchSliderPts->GetPoint(i));
            }
        }
        totalPts->Modified();
        int colCount = mainTable->horizontalHeader()->count();
        int diff = colCount - (1 + (totalPts->GetNumberOfPoints() * 3));
        if (diff < 0) {
            mainTable->setColumnCount(colCount + std::abs(diff));
        }
        int freeRow;
        for (int i = 0; i < mainTable->rowCount(); i++) {
            if (!mainTable->item(i, 0) ||
                mainTable->item(i, 0)->text().isEmpty()) {
                freeRow = i;
                break;
            }
        }
        mainTable->setItem(freeRow, 0,
                           new QTableWidgetItem(QString::fromStdString(name)));
        for (int i = 0; i < totalPts->GetNumberOfPoints(); i++) {
            for (int j = 0; j < 3; j++) {
                double item = totalPts->GetPoint(i)[j];
                int index = (3 * i) + j + 1;
                std::stringstream stream;
                stream << std::fixed << std::setprecision(4) << item;
                std::string convString = stream.str();
                mainTable->setItem(
                    freeRow, index,
                    new QTableWidgetItem(QString::fromStdString(convString)));
            }
        }
        Eigen::MatrixXd templatePtsMat(totalPts->GetNumberOfPoints(), 3);
        for (int i = 0; i < totalPts->GetNumberOfPoints(); i++) {
            for (int j = 0; j < 3; j++) {
                templatePtsMat.operator()(i, j) = totalPts->GetPoint(i)[j];
            }
        }
        m_templateView->SetPloyData(m_templateMesh);
    }
    if (status == 0) {
        m_templateView->hide();
        m_templateView->Initialize();
        templatePlotToolbarAction->setEnabled(0);
        delete m_meshPlot;
        m_meshPlot = nullptr;
        delete m_SIMP;
        m_SIMP = nullptr;
        SetStatus(STATUS::reset);
        UpdateDataBase();
        m_typeINOL = 0;
        m_surfaceNOS = 0;
        m_surfacePatchNOP = 1;
        m_surfacePatchUNOS = 0;
        m_surfacePatchVNOS = 0;
        m_curveNOS = 0;
        m_curveNOC = 1;
        m_templateMesh->Initialize();
        m_templateTypeI->Initialize();
        m_templateSurfaceSliders->Initialize();
        m_templatePatchSurfaceSliders->Initialize();
        m_templatePatchSurfaceCurve->Initialize();
        m_templateCurveSliders->Initialize();
        m_templateCurvePointsPoly->Initialize();
        ResetLandmarks();
    }
}

void MainWindow::SetTemplateMesh(vtkPolyData* mesh) { m_templateMesh = mesh; }

void MainWindow::SetTemplateMeshType(std::string type){
    if(type == ""){
        m_templateMeshType = "Mesh"; //handling legacy template files
    }
    else{
        m_templateMeshType = type;
    }
}

void MainWindow::SetTemplateTypeI(vtkPoints* fixedPts) {
    m_templateTypeI = fixedPts;
    m_typeINOL = fixedPts->GetNumberOfPoints();
}

void MainWindow::SetTemplateSurfaceSliders(vtkPoints* pts) {
    m_templateSurfaceSliders = pts;
    m_surfaceNOS = m_templateSurfaceSliders->GetNumberOfPoints();
}

void MainWindow::SetTemplateCurveSliders(
    vtkMultiBlockDataSet* sliderPtsPolyBlock,
    vtkMultiBlockDataSet* curvePtsPolyBlock, int numOfSliders) {
    m_templateCurveSliders = sliderPtsPolyBlock;
    m_templateCurvePointsPoly = curvePtsPolyBlock;
    m_curveNOS = numOfSliders;
    m_curveNOC = sliderPtsPolyBlock->GetNumberOfBlocks();
    if (m_curveNOC == 0) {
        m_curveNOC = 1;
    }
}

void MainWindow::SetTemplatePatchSurfaceSliders(
    vtkMultiBlockDataSet* ptsPolyBlock, vtkMultiBlockDataSet* surfaceCurve,
    int Ures, int Vres) {
    m_templatePatchSurfaceSliders = ptsPolyBlock;
    m_surfacePatchNOP = ptsPolyBlock->GetNumberOfBlocks();
    m_templatePatchSurfaceCurve = surfaceCurve;
    if (m_surfacePatchNOP == 0) {
        m_surfacePatchNOP = 1;
    }
    m_surfacePatchUNOS = Ures;
    m_surfacePatchVNOS = Vres;
}

int MainWindow::GetTypeINOL() { return m_typeINOL; }

void MainWindow::ResetLandmarks() {
    if (m_dataBase->CheckMembership("Template")) {
        m_dataBase->DeleteAllLandmarks("Template");
        m_dataBase->DeleteNode("Template");
    }
    QTreeWidgetItemIterator it(m_treeWidget);
    while (*it) {
        if ((*it)->childCount() > 0) {
            auto name = (*it)->text(0).toStdString();
            m_dataBase->DeleteAllLandmarks(name);
            ResetLMData(name);
            while ((*it)->childCount()) {
                QTreeWidgetItem* c = (*it)->child(0);
                (*it)->removeChild(c);
                delete c;
            }
        }
        ++it;
    }
    mainTable->clear();
    mainTable->setColumnCount(TableColNum);
    mainTable->setRowCount(TableRowNum);
    supImposedTable->clear();
    supImposedTable->setColumnCount(TableColNum);
    supImposedTable->setRowCount(TableRowNum);
    procResTable->clear();
    procResTable->setColumnCount(TableColNum);
    procResTable->setRowCount(TableRowNum);

    SetLandmarkHeaders(mainTable);
    SetLandmarkHeaders(supImposedTable);
    SetLandmarkHeaders(procResTable);
    m_mainRenderer->RemoveActor(m_fixedLmActor);
    m_mainRenderer->RemoveActor(m_curveLmActor);
    m_mainRenderer->RemoveActor(m_surfaceLmActor);
    m_mainRenderer->GetRenderWindow()->Render();
}

vtkPolyData* MainWindow::GetTemplateMesh() { return m_templateMesh; }

vtkPoints* MainWindow::GetTemplateTypeI() { return m_templateTypeI; }

vtkPoints* MainWindow::GetTemplateSurfaceSliders() {
    return m_templateSurfaceSliders;
}

vtkMultiBlockDataSet* MainWindow::GetTemplateSurfacePatchSliders() {
    return m_templatePatchSurfaceSliders;
}

int MainWindow::GetURes() { return m_surfacePatchUNOS; }
int MainWindow::GetVRes() { return m_surfacePatchVNOS; }
int MainWindow::GetCurveNOS() { return m_curveNOS; }

vtkMultiBlockDataSet* MainWindow::GetTemplateCurveSliders() {
    return m_templateCurveSliders;
}

vtkMultiBlockDataSet* MainWindow::GetTemplateCurvePoly() {
    return m_templateCurvePointsPoly;
}

vtkMultiBlockDataSet* MainWindow::GetTemplateSurfacePatchCurve() {
    return m_templatePatchSurfaceCurve;
}

vtkPolyData* MainWindow::GetTemplateTotalLandmarks() {
    return m_dataBase->GetTotalLandmarks("Template");
}

void MainWindow::PlotLandmarks(vtkPoints* fixedLandmarks,
                               vtkPoints* curveLandmarks,
                               vtkPoints* surfaceLandmarks, vtkPolyData* mesh) {
    if (fixedLandmarks->GetNumberOfPoints() > 0 ||
        curveLandmarks->GetNumberOfPoints() > 0 ||
        surfaceLandmarks->GetNumberOfPoints() > 0) {
        vtkNew<vtkMassProperties> prop;
        prop->SetInputData(mesh);
        prop->Update();
        const double area = prop->GetSurfaceArea();
        const double diagonal = std::sqrt(area); // Approximate characteristic length
        // Compute size factor based on application-specific parameters
        // Normalized between 0-1 range first, then scaled
        double sizeFactor = (m_typeINOL * 0.03 + 
                    m_surfaceNOS * 0.025 +
                    m_surfacePatchNOP * m_surfacePatchUNOS * m_surfacePatchVNOS * 0.025 +
                    m_curveNOS * m_curveNOC * 0.025);

        // Apply sigmoid function for smooth clamping
        sizeFactor = 1.0 / (1.0 + std::exp(-0.1*(sizeFactor - 50.0))); // Sigmoid normalization

        // Map to reasonable visual range (1%-5% of characteristic length)
        const double minSize = 0.01 * diagonal;
        const double maxSize = 0.05 * diagonal;
        double landmarkSize = minSize + sizeFactor * (maxSize - minSize);
        // Apply to sphere source
        vtkNew<vtkSphereSource> sphereSource;
        sphereSource->SetRadius(landmarkSize);

        if (fixedLandmarks->GetNumberOfPoints() > 0) {
            vtkNew<vtkPolyData> temp;
            temp->SetPoints(fixedLandmarks);
            m_fixedLmVertexFilter->SetInputData(temp);
            m_fixedLmVertexFilter->Update();

            vtkNew<vtkGlyph3DMapper> fixedPointMapper;
            fixedPointMapper->SetInputData(m_fixedLmVertexFilter->GetOutput());
            fixedPointMapper->SetSourceConnection(
                sphereSource->GetOutputPort());
            fixedPointMapper->ScalingOff();
            fixedPointMapper->ScalarVisibilityOff();
            m_fixedLmActor->SetMapper(fixedPointMapper);
            m_fixedLmActor->GetProperty()->SetOpacity(1);
            m_fixedLmActor->GetProperty()->SetDiffuse(0.6);
            m_fixedLmActor->GetProperty()->SetAmbient(0.4);
            m_fixedLmActor->GetProperty()->SetSpecular(0.0);
            m_fixedLmActor->GetProperty()->SetSpecularPower(1);
            m_fixedLmActor->Modified();
            m_mainRenderer->AddActor(m_fixedLmActor);
        }

        if (curveLandmarks->GetNumberOfPoints() > 0) {
            vtkNew<vtkPolyData> temp;
            temp->SetPoints(curveLandmarks);
            m_curveLmVertexFilter->SetInputData(temp);
            m_curveLmVertexFilter->Update();

            vtkNew<vtkGlyph3DMapper> curvePointMapper;
            curvePointMapper->SetInputData(m_curveLmVertexFilter->GetOutput());
            curvePointMapper->SetSourceConnection(
                sphereSource->GetOutputPort());
            curvePointMapper->ScalingOff();
            curvePointMapper->ScalarVisibilityOff();
            m_curveLmActor->SetMapper(curvePointMapper);
            m_curveLmActor->GetProperty()->SetOpacity(1);
            m_curveLmActor->GetProperty()->SetDiffuse(0.6);
            m_curveLmActor->GetProperty()->SetAmbient(0.4);
            m_curveLmActor->GetProperty()->SetSpecular(0.0);
            m_curveLmActor->GetProperty()->SetSpecularPower(1);
            m_curveLmActor->Modified();
            m_mainRenderer->AddActor(m_curveLmActor);
        }

        if (surfaceLandmarks->GetNumberOfPoints() > 0) {
            vtkNew<vtkPolyData> temp;
            temp->SetPoints(surfaceLandmarks);
            m_surfaceLmVertexFilter->SetInputData(temp);
            m_surfaceLmVertexFilter->Update();

            vtkNew<vtkGlyph3DMapper> surfacePointMapper;
            surfacePointMapper->SetInputData(
                m_surfaceLmVertexFilter->GetOutput());
            surfacePointMapper->SetSourceConnection(
                sphereSource->GetOutputPort());
            surfacePointMapper->ScalingOff();
            surfacePointMapper->ScalarVisibilityOff();
            m_surfaceLmActor->SetMapper(surfacePointMapper);
            m_surfaceLmActor->GetProperty()->SetOpacity(1);
            m_surfaceLmActor->GetProperty()->SetDiffuse(0.6);
            m_surfaceLmActor->GetProperty()->SetAmbient(0.4);
            m_surfaceLmActor->GetProperty()->SetSpecular(0.0);
            m_surfaceLmActor->GetProperty()->SetSpecularPower(1);
            m_surfaceLmActor->Modified();
            m_mainRenderer->AddActor(m_surfaceLmActor);
        }

    } else {
        m_mainRenderer->RemoveActor(m_fixedLmActor);
        m_mainRenderer->RemoveActor(m_curveLmActor);
        m_mainRenderer->RemoveActor(m_surfaceLmActor);
    }

    m_mainRenderer->GetRenderWindow()->Render();
}

void MainWindow::PlotLandmarks(vtkPoints* fixedLandmarks,
                               vtkPoints* curveLandmarks,
                               vtkPoints* surfaceLandmarks, vtkPolyData* mesh, std::string landmarkType) {
    if (fixedLandmarks->GetNumberOfPoints() > 0 ||
        curveLandmarks->GetNumberOfPoints() > 0 ||
        surfaceLandmarks->GetNumberOfPoints() > 0) {
        vtkNew<vtkMassProperties> prop;
        prop->SetInputData(mesh);
        prop->Update();
        const double area = prop->GetSurfaceArea();
        const double diagonal = std::sqrt(area); // Approximate characteristic length
        // Compute size factor based on application-specific parameters
        // Normalized between 0-1 range first, then scaled
        double sizeFactor = (m_typeINOL * 0.03 + 
                    m_surfaceNOS * 0.025 +
                    m_surfacePatchNOP * m_surfacePatchUNOS * m_surfacePatchVNOS * 0.025 +
                    m_curveNOS * m_curveNOC * 0.025);

        // Apply sigmoid function for smooth clamping
        sizeFactor = 1.0 / (1.0 + std::exp(-0.1*(sizeFactor - 50.0))); // Sigmoid normalization

        // Map to reasonable visual range (1%-5% of characteristic length)
        const double minSize = 0.01 * diagonal;
        const double maxSize = 0.05 * diagonal;
        double landmarkSize = minSize + sizeFactor * (maxSize - minSize);
        // Apply to sphere source
        vtkNew<vtkSphereSource> sphereSource;
        sphereSource->SetRadius(landmarkSize);

        if (fixedLandmarks->GetNumberOfPoints() > 0) {
            vtkNew<vtkPolyData> temp;
            temp->SetPoints(fixedLandmarks);
            m_fixedLmVertexFilter->SetInputData(temp);
            m_fixedLmVertexFilter->Update();

            vtkNew<vtkGlyph3DMapper> fixedPointMapper;
            fixedPointMapper->SetInputData(m_fixedLmVertexFilter->GetOutput());
            fixedPointMapper->SetSourceConnection(
                sphereSource->GetOutputPort());
            fixedPointMapper->ScalingOff();
            fixedPointMapper->ScalarVisibilityOff();
            m_fixedLmActor->SetMapper(fixedPointMapper);
            m_fixedLmActor->Modified();
            m_mainRenderer->AddActor(m_fixedLmActor);
        }

        if (curveLandmarks->GetNumberOfPoints() > 0) {
            vtkNew<vtkPolyData> temp;
            temp->SetPoints(curveLandmarks);
            m_curveLmVertexFilter->SetInputData(temp);
            m_curveLmVertexFilter->Update();

            vtkNew<vtkGlyph3DMapper> curvePointMapper;
            curvePointMapper->SetInputData(m_curveLmVertexFilter->GetOutput());
            curvePointMapper->SetSourceConnection(
                sphereSource->GetOutputPort());
            curvePointMapper->ScalingOff();
            curvePointMapper->ScalarVisibilityOff();
            m_curveLmActor->SetMapper(curvePointMapper);
            m_curveLmActor->Modified();
            m_mainRenderer->AddActor(m_curveLmActor);
        }

        if (surfaceLandmarks->GetNumberOfPoints() > 0) {
            vtkNew<vtkPolyData> temp;
            temp->SetPoints(surfaceLandmarks);
            m_surfaceLmVertexFilter->SetInputData(temp);
            m_surfaceLmVertexFilter->Update();

            vtkNew<vtkGlyph3DMapper> surfacePointMapper;
            surfacePointMapper->SetInputData(
                m_surfaceLmVertexFilter->GetOutput());
            surfacePointMapper->SetSourceConnection(
                sphereSource->GetOutputPort());
            surfacePointMapper->ScalingOff();
            surfacePointMapper->ScalarVisibilityOff();
            m_surfaceLmActor->SetMapper(surfacePointMapper);
            m_surfaceLmActor->Modified();
            m_mainRenderer->AddActor(m_surfaceLmActor);

        }
        if(landmarkType == "Type I-II"){
            m_fixedLmActor->GetProperty()->SetOpacity(1);
            m_fixedLmActor->GetProperty()->SetDiffuse(0.3);    // Low diffuse glassy
            m_fixedLmActor->GetProperty()->SetSpecular(0.8);
            m_fixedLmActor->GetProperty()->SetSpecularPower(120);
            m_curveLmActor->GetProperty()->SetOpacity(0.3);
            m_surfaceLmActor->GetProperty()->SetOpacity(0.3);
            m_fixedLmActor->Modified();
            m_curveLmActor->Modified();
            m_surfaceLmActor->Modified();
        }
        else if(landmarkType == "Curve Slider"){
            m_fixedLmActor->GetProperty()->SetOpacity(0.3);
            m_curveLmActor->GetProperty()->SetOpacity(1);
            m_curveLmActor->GetProperty()->SetDiffuse(0.3);    
            m_curveLmActor->GetProperty()->SetSpecular(0.8);
            m_curveLmActor->GetProperty()->SetSpecularPower(120);
            m_surfaceLmActor->GetProperty()->SetOpacity(0.3);
            m_fixedLmActor->Modified();
            m_curveLmActor->Modified();
            m_surfaceLmActor->Modified();
                
        }
        else if(landmarkType == "Surface Slider"){
            m_fixedLmActor->GetProperty()->SetOpacity(0.3);
            m_curveLmActor->GetProperty()->SetOpacity(0.3);
            m_surfaceLmActor->GetProperty()->SetOpacity(1);
            m_surfaceLmActor->GetProperty()->SetOpacity(1);
            m_surfaceLmActor->GetProperty()->SetDiffuse(0.3);    
            m_surfaceLmActor->GetProperty()->SetSpecular(0.8);
            m_surfaceLmActor->GetProperty()->SetSpecularPower(120);
            m_fixedLmActor->Modified();
            m_curveLmActor->Modified();
            m_surfaceLmActor->Modified();
        }

    } else {
        m_mainRenderer->RemoveActor(m_fixedLmActor);
        m_mainRenderer->RemoveActor(m_curveLmActor);
        m_mainRenderer->RemoveActor(m_surfaceLmActor);
    }

    m_mainRenderer->GetRenderWindow()->Render();
}

void MainWindow::PCA() {
    if (TemplateIsSet) {
        if (m_dataBase->GetNodeNames().size() >= 3) {
            delete m_pcaWindow;
            m_pcaWindow = new PCAWindow(m_dataBase);
        } else {
            auto errorDialogue = QMessageBox(this);
            errorDialogue.setIcon(QMessageBox::Critical);
            errorDialogue.setWindowTitle("Error");
            errorDialogue.setText(
                "SuperImpose your specimens first (minimum two of them), \n "
                "then "
                "try again!");
            errorDialogue.exec();
        }
    } else {
        auto errorDialogue = QMessageBox(this);
        errorDialogue.setIcon(QMessageBox::Critical);
        errorDialogue.setWindowTitle("Error");
        errorDialogue.setText(
            "Project setting was not found! \n If the setting is already "
            "configured,\n the REGISTER button has not been pushed!");
        errorDialogue.exec();
    }
}

void MainWindow::PaintMesh(std::string name) {
    vtkDoubleArray* tempDataArr = m_dataBase->GetProcDistance(name);
    vtkPolyData* tempMesh = m_dataBase->GetPolyNode(name);

    m_mainMeshMapper->SetInputData(tempMesh);
    m_mainRenderer->RemoveActor(m_scalarBar);
    if (tempDataArr->GetNumberOfTuples() > 0) {
        auto scalarRange = tempMesh->GetPointData()
                               ->GetArray("ProcrustesResidualMagnitude")
                               ->GetRange();
        vtkNew<vtkColorSeries> colorSeries;
        colorSeries->SetColorScheme(11);
        int numColors = colorSeries->GetNumberOfColors();

        vtkNew<vtkColorTransferFunction> ctf;
        ctf->SetColorSpaceToDiverging();
        ctf->AddRGBPoint(0, 0.231373, 0.298039, 0.752941);
        ctf->AddRGBPoint(0.5, 0.865003, 0.865003, 0.865003);
        ctf->AddRGBPoint(1, 0.705882, 0.0156863, 0.14902);
        ctf->SetScaleToLinear();

        vtkNew<vtkLookupTable> lut;
        lut->SetTableRange(scalarRange);
        for (int i = 0; i < lut->GetNumberOfColors(); ++i) {
            std::array<double, 3> rgb;
            ctf->GetColor(double(i) / lut->GetNumberOfColors(), rgb.data());
            std::array<double, 4> rgba{0.0, 0.0, 0.0, 1.0};
            std::copy(std::begin(rgb), std::end(rgb), std::begin(rgba));
            lut->SetTableValue(i, rgba.data());
        }
        lut->Build();

        m_mainMeshMapper->SetScalarModeToUsePointFieldData();
        m_mainMeshMapper->SelectColorArray("ProcrustesResidualMagnitude");
        m_mainMeshMapper->SetScalarRange(scalarRange);
        m_mainMeshMapper->SetLookupTable(lut);

        // Update scalar bar
        m_scalarBar->SetLookupTable(m_mainMeshMapper->GetLookupTable());
        m_scalarBar->Modified();
        m_mainRenderer->AddActor(m_scalarBar);
    } else {
        m_mainRenderer->RemoveActor(m_scalarBar);
    }
    m_mainMeshMapper->Update();
    m_mainMeshMapper->Modified();
}

void MainWindow::ReadLMDataFromFile() {
    if (TemplateIsSet) {
        QString csvFileName =
            QFileDialog::getOpenFileName(this, "CSV Files", QDir::homePath(),
                                         "CSV Files (*.csv);;All Files (*)", nullptr, QFileDialog::DontUseNativeDialog);
        if (csvFileName.isEmpty()) {
            // do nothing
            // return;
        } else {
            if (csvFileName.endsWith(".csv")) {
                std::vector<std::vector<string>> content;
                std::string fname = csvFileName.toStdString();
                std::vector<std::string> row;
                std::string line, word;
                fstream file(fname, ios::in);
                if (file.is_open()) {
                    while (getline(file, line)) {
                        row.clear();
                        std::stringstream str(line);
                        while (getline(str, word, ',')) {
                            row.push_back(word);
                        }
                        content.push_back(row);
                    }
                } else {
                    std::cout << "Could not open the .csv file\n";
                }
                QString geomDir = QFileDialog::getExistingDirectory(
                    this, tr("Open Mesh Directory"), QDir::homePath(),
                    QFileDialog::ShowDirsOnly |
                        QFileDialog::DontResolveSymlinks);
                if (!geomDir.isEmpty()) {
                    LockTheWindow();
                    std::string path = geomDir.toStdString();
                    path = geomDir.toStdString() + "/";
                    qRegisterMetaType<std::string>("std::string");
                    delete m_recoveryThread;
                    m_recoveryThread =
                        new RecoverLMThread(this, content, path, m_mutex);
                    m_recoveryThread->setParent(this);
                    connect(m_recoveryThread,
                            &RecoverLMThread::TreeObjectChanged, this,
                            &MainWindow::OnTreeObjectChanged);
                    connect(m_recoveryThread, &RecoverLMThread::DataBaseChanged,
                            this, &MainWindow::OnDataBaseChanged);
                    connect(m_recoveryThread, &RecoverLMThread::RecoveryIsDone,
                            this, &MainWindow::OnRecoveryIsDone);

                    RunStatThread(m_recoveryThread);
                    m_recoveryThread->start();
                }
            }
        }
    } else {
        auto errorDialogue = QMessageBox(this);
        errorDialogue.setIcon(QMessageBox::Critical);
        errorDialogue.setWindowTitle("Error");
        errorDialogue.setText(
            "Project setting was not found! \n If the setting is already "
            "configured,\n the REGISTER button has not been pushed!");
        errorDialogue.exec();
    }
}

void MainWindow::SaveProject() {
    auto nameList = m_dataBase->GetNodeNames();
    auto filter = "atp(*.atp)";
    QString filename =
        QFileDialog::getSaveFileName(this, "Save file", "", filter, nullptr,
                                     QFileDialog::DontUseNativeDialog);
    QFileInfo fi(filename);
    QString ext = fi.completeSuffix();
    if (filename.isEmpty()) {
        return;
    }
    if (ext != "atp") {
        filename += ".atp";
    }
    QFile f(filename);
    if (f.open(QFile::WriteOnly | QFile::Truncate)) {
        QStringList textItem;
        QTextStream data(&f);

        textItem += QString("Do Not Modify The Content of This File");
        data << textItem.join("") << ENDL;
        textItem.clear();

        textItem += QString("Number of Specimens") + ",";
        textItem += QString::number(nameList.size());
        data << textItem.join("") << ENDL;
        textItem.clear();

        for (std::string name : nameList) {
            vtkPolyData* tempPoly = m_dataBase->GetPolyNode(name);
            vtkPoints* tempFixedLM = m_dataBase->GetTypeI(name);
            vtkPoints* tempCurveLM = m_dataBase->GetCurveSliders(name);
            vtkPoints* tempSurfaceLM = m_dataBase->GetSurfaceSliders(name);
            std::string type = m_dataBase->GetGeometryType(name);

            textItem += QString("Specimen") + ",";
            textItem += QString::fromStdString(name);
            data << textItem.join("") << ENDL;
            textItem.clear();
            
            textItem += QString("Type") + ",";
            textItem += QString::fromStdString(type);
            data << textItem.join("") << ENDL;
            textItem.clear();

            textItem += QString("Number of Fixed Landmarks") + ",";
            textItem += QString::number(tempFixedLM->GetNumberOfPoints());
            data << textItem.join("") << ENDL;
            textItem.clear();
            for (int i = 0; i < tempFixedLM->GetNumberOfPoints(); i++) {
                textItem += QString("Fixed Coords") + ",";
                for (int j = 0; j < 3; j++) {
                    textItem +=
                        QString::number(tempFixedLM->GetPoint(i)[j]) + ",";
                }
                data << textItem.join("") << ENDL;
                textItem.clear();
            }

            textItem += QString("Number of Curve Landmarks") + ",";
            textItem += QString::number(tempCurveLM->GetNumberOfPoints());
            data << textItem.join("") << ENDL;
            textItem.clear();
            for (int i = 0; i < tempCurveLM->GetNumberOfPoints(); i++) {
                textItem += QString("Curve Coords") + ",";
                for (int j = 0; j < 3; j++) {
                    textItem +=
                        QString::number(tempCurveLM->GetPoint(i)[j]) + ",";
                }
                data << textItem.join("") << ENDL;
                textItem.clear();
            }

            textItem += QString("Number of Surface Landmarks") + ",";
            textItem += QString::number(tempSurfaceLM->GetNumberOfPoints());
            data << textItem.join("") << ENDL;
            textItem.clear();
            for (int i = 0; i < tempSurfaceLM->GetNumberOfPoints(); i++) {
                textItem += QString("Surface Coords") + ",";
                for (int j = 0; j < 3; j++) {
                    textItem +=
                        QString::number(tempSurfaceLM->GetPoint(i)[j]) + ",";
                }
                data << textItem.join("") << ENDL;
                textItem.clear();
            }

            textItem += QString("Number of Vertices") + ",";
            textItem += QString::number(tempPoly->GetNumberOfPoints());
            data << textItem.join("") << ENDL;
            textItem.clear();

            for (int i = 0; i < tempPoly->GetNumberOfPoints(); i++) {
                textItem += QString("Vertex Coords") + ",";
                for (int j = 0; j < 3; j++) {
                    textItem += QString::number(tempPoly->GetPoint(i)[j]) + ",";
                }
                data << textItem.join("") << ENDL;
                textItem.clear();
            }
            textItem += QString("Number of Poly") + ",";
            textItem += QString::number(tempPoly->GetNumberOfCells());
            data << textItem.join("") << ENDL;
            textItem.clear();

            for (int i = 0; i < tempPoly->GetNumberOfCells(); i++) {
                int numPts = tempPoly->GetCell(i)->GetNumberOfPoints();
                textItem += QString("Connectivity") + ",";
                textItem += QString::number(numPts) + ",";
                for (int j = 0; j < numPts; j++) {
                    textItem +=
                        QString::number(tempPoly->GetCell(i)->GetPointId(j)) +
                        ",";
                }
                data << textItem.join("") << ENDL;
                textItem.clear();
            }
            textItem += QString("Next Specimen") + ",";
            data << textItem.join("") << ENDL;
            textItem.clear();
        }

        f.close();
    }
}

void MainWindow::ReadProjectFromFile() {
    if (TemplateIsSet) {
        QString atpFileName =
            QFileDialog::getOpenFileName(this, "atp Files", QDir::homePath(),
                                         "atp Files (*.atp);;All Files (*)", nullptr, QFileDialog::DontUseNativeDialog);
        if (atpFileName.isEmpty()) {
            // do nothing
            // return;
        } else {
            if (atpFileName.endsWith(".atp")) {
                LockTheWindow();
                if (m_treeWidget->topLevelItemCount() > 0) {
                    m_treeWidget->currentItem()->setSelected(0);
                }
                qRegisterMetaType<std::string>("std::string");
                delete m_importThread;
                m_importThread = new ImportThread(this, atpFileName, m_mutex);

                connect(m_importThread, &ImportThread::TreeObjectChanged, this,
                        &MainWindow::OnTreeObjectChanged);
                connect(m_importThread, &ImportThread::DataBaseChanged, this,
                        &MainWindow::OnDataBaseChanged);
                connect(m_importThread, &ImportThread::ImportIsDone, this,
                        &MainWindow::OnRecoveryIsDone);

                RunStatThread(m_importThread);

                m_importThread->start();
            }
        }

    } else {
        auto errorDialogue = QMessageBox(this);
        errorDialogue.setIcon(QMessageBox::Critical);
        errorDialogue.setWindowTitle("Error");
        errorDialogue.setText(
            "Project setting was not found! \n If the setting is already "
            "configured,\n the REGISTER button has not been pushed!");
        errorDialogue.exec();
    }
}

void MainWindow::OnRecoveryIsDone() {
    if (m_treeWidget->topLevelItemCount() > 0) {
        auto item = m_treeWidget->topLevelItem(0);
        m_treeWidget->setCurrentItem(item);
        m_treeWidget->topLevelItem(0)->setSelected(1);
    }

    UnlockTheWindow();
}

void MainWindow::OnSuperImpositionIsDone() {
    if (!m_treeWidget->selectedItems().isEmpty()) {
        std::string name =
            m_treeWidget->selectedItems()[0]->text(0).toStdString();
        if (m_treeWidget->selectedItems()[0]->parent()) {
            name = m_treeWidget->selectedItems()[0]
                       ->parent()
                       ->text(0)
                       .toStdString();
        }

        vtkPoints* fixedLandmarks = m_dataBase->GetTypeI(name);
        vtkPoints* curveLandmarks = m_dataBase->GetCurveSliders(name);
        vtkPoints* surfaceLandmarks = m_dataBase->GetSurfaceSliders(name);
        vtkPolyData* tempMesh = m_dataBase->GetPolyNode(name);

        PaintMesh(name);
        PlotLandmarks(fixedLandmarks, curveLandmarks, surfaceLandmarks,
                      tempMesh);
        m_mainRenderer->ResetCamera();
        m_mainRenderer->GetRenderWindow()->Render();
    }
}

void MainWindow::OnTreeObjectChanged(std::string name) {
    m_treeItem = new QTreeWidgetItem();
    m_treeItem->setText(0, QString::fromStdString(name));
    std::string type = m_dataBase->GetGeometryType(name);
    m_treeItem->setText(1, QString::fromStdString(type));
    m_treeWidget->addTopLevelItem(m_treeItem);
    m_treeWidget->setCurrentItem(m_treeItem);
}

void MainWindow::OnDataBaseChanged(std::string name) {
    m_mutex->lock();
    auto fixedPts = m_dataBase->GetTypeI(name);
    auto curveSliderPts = m_dataBase->GetCurveSliders(name);
    auto surfaceSliderPts = m_dataBase->GetSurfaceSliders(name);
    SetSliders(fixedPts, curveSliderPts, surfaceSliderPts, name);
    m_mutex->unlock();
}

bool MainWindow::IsNumber(const std::string& s) {
    char* end = nullptr;
    double val = strtod(s.c_str(), &end);
    return end != s.c_str() && *end == '\0' && val != HUGE_VAL;
}

void MainWindow::LockTheWindow() {
    exportCSVAction->setEnabled(0);
    exportVTKAction->setEnabled(0);
    importMeshAction->setEnabled(0);
    meshPlotToolbarAction->setEnabled(0);
    superImpositionToolbarAction->setEnabled(0);
    pcaToolbarAction->setEnabled(0);
    renameNodeAction->setEnabled(0);
    deleteNodeAction->setEnabled(0);
    exportGeometryAction->setEnabled(0);
    deleteBranchAction->setEnabled(0);
    recoverActionLegacy->setEnabled(0);
    openProjectAction->setEnabled(0);
    saveProjectAction->setEnabled(0);
    m_treeWidget->setSelectionMode(QAbstractItemView::NoSelection);
}

void MainWindow::UnlockTheWindow() {
    exportCSVAction->setEnabled(1);
    exportVTKAction->setEnabled(1);
    importMeshAction->setEnabled(1);
    meshPlotToolbarAction->setEnabled(1);
    superImpositionToolbarAction->setEnabled(1);
    pcaToolbarAction->setEnabled(1);
    renameNodeAction->setEnabled(1);
    deleteNodeAction->setEnabled(1);
    exportGeometryAction->setEnabled(1);
    deleteBranchAction->setEnabled(1);
    recoverActionLegacy->setEnabled(1);
    openProjectAction->setEnabled(1);
    saveProjectAction->setEnabled(1);
    m_treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
}

void MainWindow::OnStatusChanged(int status) {
    if (status != 0) {
        statusLabel->setText("Status: Busy");
        if (status % 2 == 0) {
            progressLabel->setPixmap(
                QPixmap(":/icons/graphics/icons/vBusy.svg"));
        } else {
            progressLabel->setPixmap(
                QPixmap(":/icons/graphics/icons/busy.svg"));
        }
    } else {
        statusLabel->setText("Status: Idle");
        progressLabel->setPixmap(QPixmap(":/icons/graphics/icons/idle.svg"));
    }
}

void MainWindow::RecoverDigitisedLM() { ReadLMDataFromFile(); }

void MainWindow::RecoverProject() { ReadProjectFromFile(); }

void MainWindow::RunStatThread(QThread* thread) {
    delete m_statThread;
    m_statThread = new StatusReporterThread(thread);
    connect(m_statThread, &StatusReporterThread::StatusChanged, this,
            &MainWindow::OnStatusChanged);
    m_statThread->setParent(this);
    m_statThread->start();
}

void MainWindow::SetToExport(bool lm, bool si, bool pv, bool pm) {
    m_lm = lm;
    m_si = si;
    m_pv = pv;
    m_pm = pm;
}

DataBase* MainWindow::GetDataBase() { return m_dataBase; }

bool MainWindow::GetTemplateStatus() { return TemplateIsSet; }

int MainWindow::GetNumberOfCurves() { return m_curveNOC; }

int MainWindow::GetSurfaceNOS() { return m_surfaceNOS; }

int MainWindow::GetNumberOfPatches() { return m_surfacePatchNOP; }

bool MainWindow::GetIgnorSetting() {
    return projectSettings->GetIgnorInternals();
}

QMutex* MainWindow::GetMutex() { return m_mutex; }

void MainWindow::PrintHelp() { helpWindow->show(); }

void MainWindow::SetLandmarkHeaders(QTableWidget* table) {
    QStringList headers;
    headers << "Specimen";  // First column
    for (int i = 1; i <= TableColNum - 1; ++i) {
        headers << QString("LM%1X").arg(i);
        headers << QString("LM%1Y").arg(i);
        headers << QString("LM%1Z").arg(i);
    }

    table->setColumnCount(headers.size());  // Ensure table has enough columns
    table->setHorizontalHeaderLabels(headers);
}

MainWindow::~MainWindow() {
    delete mainRenderWindowWidget;
    delete m_dataBase;
    delete m_meshPlot;
    delete projectSettings;
    delete m_templateView;
    delete m_SIMP;
    delete m_pcaWindow;
    delete m_treeWidget;
    delete m_recoveryThread;
    delete m_importThread;
    delete m_exportDial;
    delete helpTab;
    delete helpEngine;
    delete textViewer;
    delete horizSplitter;
    delete helpWindow;
    delete m_mutex;
}
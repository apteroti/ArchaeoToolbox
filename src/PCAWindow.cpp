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

#include "include/PCAWindow.h"

PCAWindow::PCAWindow(DataBase *parentDB) : m_parentDataBase(parentDB) {
    std::vector<std::string> tempNameList = m_parentDataBase->GetNodeNames();

    m_eigenScores.resize(0, 0);
    for (std::string names : tempNameList) {
        vtkDoubleArray *magArray = m_parentDataBase->GetProcDistance(names);
        if (magArray->GetNumberOfTuples() > 0) {
            m_nameList.push_back(names);
        }
    }
    int numSpecimen = m_nameList.size();
    if (numSpecimen >= 2) {
        m_meshRenderer = vtkSmartPointer<vtkRenderer>::New();
        m_style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
        m_meshIren = vtkSmartPointer<vtkGenericRenderWindowInteractor>::New();
        m_meshActor = vtkSmartPointer<vtkActor>::New();
        m_meshMapper = vtkSmartPointer<vtkDataSetMapper>::New();

        m_graphLabelArray = vtkSmartPointer<vtkStringArray>::New();
        m_lmLabelActor = vtkSmartPointer<vtkActor2D>::New();
        m_scatterChartView = vtkSmartPointer<vtkContextView>::New();
        m_scatterChart = vtkSmartPointer<vtkChartXY>::New();

        m_meshData = m_parentDataBase->GetPolyNode("Template");
        m_landmarksPoly = m_parentDataBase->GetTotalLandmarks("Template");
        int numLM = m_landmarksPoly->GetNumberOfPoints();
        m_dataMatrix = Eigen::MatrixXd::Zero(numSpecimen, numLM);
        int counter = 0;
        for (std::string names : m_nameList) {
            vtkDoubleArray *magArray = m_parentDataBase->GetProcDistance(names);
            for (int j = 0; j < numLM; j++) {
                m_dataMatrix.operator()(counter, j) =
                    magArray->GetComponent(0, j);
            }
            counter += 1;
        }
        Calculate(m_dataMatrix);

        // Now GUI stuff
        this->setWindowTitle("Principal Component Results");
        this->resize(1400, 600);
        m_layout = new QGridLayout;
        m_meshRenderWidget = new QVTKOpenGLWidget();
        m_meshRenderWidget->hide();
        m_graphRenderWidget = new QVTKOpenGLWidget();
        m_graphRenderWidget->hide();
        m_dockedToolbar = new QDockWidget;
        m_dockedToolbar->setFeatures(QDockWidget::DockWidgetFloatable |
                                     QDockWidget::DockWidgetMovable);
        this->addDockWidget(Qt::LeftDockWidgetArea, m_dockedToolbar);
        specimenTreeWidget = new QTreeWidget();
        specimenTreeWidget->setHeaderLabels(QStringList{"Specimen"});
        specimenTreeWidget->setSelectionMode(
            QAbstractItemView::SingleSelection);
        for (std::string names : m_nameList) {
            QTreeWidgetItem *tempItem = new QTreeWidgetItem();
            tempItem->setText(0, QString::fromStdString(names));
            specimenTreeWidget->addTopLevelItem(tempItem);
        }

        m_dockedToolbar->setWidget(specimenTreeWidget);

        std::string renLabelTitle =
            "Contribution Plot of PC " + std::to_string(m_x);
        m_meshRenLabel = new QLabel(QString::fromStdString(renLabelTitle));
        QLabel *graphRenLabel = new QLabel(tr("PCA Plot"));

        m_meshPlaceholder = new QFrame();
        m_meshPlaceholder->setStyleSheet(
            "background-color: rgb(112, 128, 144);");

        m_plotPlaceholder = new QFrame();
        m_plotPlaceholder->setStyleSheet(
            "background-color: rgb(112, 128, 144);");

        m_layout->addWidget(m_meshRenLabel, 0, 2, 1, 3);
        m_layout->addWidget(graphRenLabel, 0, 8, 1, 9);
        m_layout->addWidget(m_meshPlaceholder, 1, 2, 10, 7);
        m_layout->addWidget(m_plotPlaceholder, 1, 9, 10, 10);

        // Set layout in QWidget
        QWidget *window = new QWidget();
        window->setLayout(m_layout);
        // Set QWidget as the central layout of the main window
        this->setCentralWidget(window);

        // Setting up Toolbars
        mainToolbar = this->addToolBar("Toolbar");
        mainToolbar->setMovable(false);
        QLabel *selectPCsLabel1 = new QLabel(tr("Select The First PC:"));
        QLabel *selectPCsLabel2 = new QLabel(tr("Select The Next PC:"));
        select1stPCsSpinBox = new QSpinBox();
        select1stPCsSpinBox->setRange(1, m_eigenVectors.cols());
        select1stPCsSpinBox->setValue(m_x);

        select2ndPCsSpinBox = new QSpinBox();
        select2ndPCsSpinBox->setRange(1, m_eigenVectors.cols());
        select2ndPCsSpinBox->setValue(m_y);

        QLabel *showMIL = new QLabel();
        showMIL->setText(tr(" MIL"));
        showMIL->setParent(this);

        milLineEdit = new QLineEdit;
        milLineEdit->setPlaceholderText("0");
        milLineEdit->setReadOnly(1);
        milLineEdit->setMaximumWidth(50);

        QLabel *showPtsIdsLabel = new QLabel();
        showPtsIdsLabel->setText(tr("  Show Landmark ids"));
        showPtsIdsLabel->setParent(this);

        exportButton = new QPushButton();
        exportButton->setToolTip("Export to CSV");
        exportButton->setText("Export PCs");

        showPtsIdsBox = new QCheckBox();
        showPtsIdsBox->setChecked(false);

        mainToolbar->addWidget(selectPCsLabel1);
        mainToolbar->addWidget(select1stPCsSpinBox);

        mainToolbar->addWidget(selectPCsLabel2);
        mainToolbar->addWidget(select2ndPCsSpinBox);
        mainToolbar->addWidget(showMIL);
        mainToolbar->addWidget(milLineEdit);

        mainToolbar->addWidget(showPtsIdsLabel);
        mainToolbar->addWidget(showPtsIdsBox);
        this->statusBar()->addPermanentWidget(exportButton, 0);

        connect(exportButton, &QPushButton::clicked, this,
                &PCAWindow::Export2Csv);

        void (QSpinBox ::*change1stPCFp)(int) = &QSpinBox ::valueChanged;
        connect(select1stPCsSpinBox, change1stPCFp, this,
                &PCAWindow::Update1stPC);

        void (QSpinBox ::*change2ndPCFp)(int) = &QSpinBox ::valueChanged;
        connect(select2ndPCsSpinBox, change2ndPCFp, this,
                &PCAWindow::Update2ndPC);

        connect(showPtsIdsBox, &QCheckBox::stateChanged, this,
                &PCAWindow::ShowPtsIds);
        //--------------------------

        this->show();
        this->DelayedPlotter();
    } else {
        auto errorDialogue = QMessageBox(this);
        errorDialogue.setIcon(QMessageBox::Critical);
        errorDialogue.setWindowTitle("Error");
        errorDialogue.setText(
            "SuperImpose your specimens first (minimum two of them), \n then "
            "try again!");
        errorDialogue.exec();
    }
}

void PCAWindow::Plot() {
    int numLM = m_landmarksPoly->GetNumberOfPoints();
    vtkNew<vtkNamedColors> colors;
    m_ctf = vtkSmartPointer<vtkColorTransferFunction>::New();
    m_ctf->SetColorSpaceToDiverging();
    m_ctf->AddRGBPoint(0, 0.231373, 0.298039, 0.752941);
    m_ctf->AddRGBPoint(0.5, 0.865003, 0.865003, 0.865003);
    m_ctf->AddRGBPoint(1, 0.705882, 0.0156863, 0.14902);
    auto meshRenWin = m_meshRenderWidget->GetRenderWindow();
    meshRenWin->AddRenderer(m_meshRenderer);
    m_style->SetCurrentRenderer(m_meshRenderer);
    m_meshIren->SetInteractorStyle(m_style);
    m_meshIren->SetRenderWindow(meshRenWin);

    // Mesh properties and color etc
    m_meshMapper->SetInputData(m_meshData);
    m_meshMapper->SetResolveCoincidentTopologyToOff();
    m_meshActor->SetMapper(m_meshMapper);
    m_meshActor->GetProperty()->SetColor(1, 0.992, 0.815);
    m_meshActor->GetProperty()->SetOpacity(0.5);
    m_meshRenderer->AddActor(m_meshActor);

    // LM properties
    // Point properties and color etc
    vtkNew<vtkMassProperties> prop;
    prop->SetInputData(m_meshData);
    prop->Update();
    const double area = prop->GetSurfaceArea();
    const double diagonal =
        std::sqrt(area);  // Approximate characteristic length
    // Compute size factor based on application-specific parameters
    // Normalized between 0-1 range first, then scaled
    double sizeFactor = (numLM * 0.025);

    // Apply sigmoid function for smooth clamping
    sizeFactor =
        1.0 /
        (1.0 + std::exp(-0.1 * (sizeFactor - 50.0)));  // Sigmoid normalization

    // Map to reasonable visual range (1%-5% of characteristic length)
    const double minSize = 0.01 * diagonal;
    const double maxSize = 0.05 * diagonal;
    double landmarkSize = minSize + sizeFactor * (maxSize - minSize);
    // Apply to sphere source
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetRadius(landmarkSize);
    m_lmVertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    m_lmActor = vtkSmartPointer<vtkActor>::New();
    m_lmMapper = vtkSmartPointer<vtkGlyph3DMapper>::New();
    double windowWidth = 800;
    double windowHeight = 800;
    m_scalarBar = vtkSmartPointer<vtkScalarBarActor>::New();
    m_scalarBar->SetTitle("Contribution");
    m_scalarBar->UnconstrainedFontSizeOn();
    m_scalarBar->SetNumberOfLabels(5);
    m_scalarBar->SetMaximumWidthInPixels(windowWidth / 10);
    m_scalarBar->SetMaximumHeightInPixels(windowHeight / 3);
    UpdateContribution(m_x);
    m_lmMapper->SetInputData(m_lmVertexFilter->GetOutput());
    m_lmMapper->SetSourceConnection(sphereSource->GetOutputPort());
    m_lmMapper->ScalingOff();
    m_lmMapper->ScalarVisibilityOn();
    m_lmMapper->SetColorModeToMapScalars();
    // m_lmActor->GetProperty()->SetColor(1.0, 0.0, 0.0);
    m_lmActor->GetProperty()->SetDiffuse(0.8);
    m_lmActor->GetProperty()->SetSpecular(0.5);
    m_lmActor->GetProperty()->SetSpecularPower(30);
    m_lmActor->SetMapper(m_lmMapper);
    m_meshRenderer->AddActor(m_lmActor);
    m_meshRenderer->AddActor(m_scalarBar);

    // Labels
    vtkNew<vtkLabeledDataMapper> lmLabelMapper;
    vtkNew<vtkVertexGlyphFilter> lmLabelVertexFilter;
    lmLabelVertexFilter->SetInputData(m_landmarksPoly);
    lmLabelVertexFilter->Update();
    lmLabelMapper->SetInputData(lmLabelVertexFilter->GetOutput());
    lmLabelMapper->GetLabelTextProperty()->SetFontSize(15);
    m_lmLabelActor->SetMapper(lmLabelMapper);
    m_lmLabelActor->GetProperty()->SetColor(
        colors->GetColor3d("black").GetData());
    m_lmLabelActor->SetPickable(0);

    m_meshRenderer->AddActor(m_MILLabelActor[0]);

    //----------scatter chart
    m_graphLabelArray->SetName("Names");
    for (int i = 0; i < m_nameList.size(); i++) {
        m_graphLabelArray->InsertNextValue(m_nameList[i]);
    }
    m_graphRenWin = m_graphRenderWidget->GetRenderWindow();
    m_scatterChartView->SetRenderWindow(m_graphRenWin);
    m_scatterChartView->GetScene()->AddItem(m_scatterChart);
    m_graphRenderWidget->SetRenderWindow(m_scatterChartView->GetRenderWindow());
    UpdateScatter(m_x, m_y);
    // bg color
    m_meshRenderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

    meshRenWin->Render();
    m_meshIren->Initialize();

    m_scatterChartView->Render();
}

void PCAWindow::DelayedPlotter() {
    QTimer::singleShot(1000, this, [this]() {
        this->Plot();
        m_meshRenderWidget->show();
        m_graphRenderWidget->show();

        m_layout->removeWidget(m_plotPlaceholder);
        m_plotPlaceholder->deleteLater();

        m_layout->removeWidget(m_meshPlaceholder);
        m_meshPlaceholder->deleteLater();

        m_layout->addWidget(m_meshRenderWidget, 1, 2, 10, 7);
        m_layout->addWidget(m_graphRenderWidget, 1, 9, 10, 10);
    });
}

void PCAWindow::UpdateScatter(int x, int y) {
    auto firstPc = m_eigenScores.col(x - 1);
    auto secondtPc = m_eigenScores.col(y - 1);
    /* DebugPrintMatrix(firstPc);
    DebugPrintMatrix(secondtPc); */
    int numSpecimen = m_eigenScores.rows();
    vtkNew<vtkTable> table;
    vtkNew<vtkFloatArray> arrX;
    std::string xAxLabel = "PC " + std::to_string(x);
    std::string yAxLabel = "PC " + std::to_string(y);
    arrX->SetName("X");
    table->AddColumn(arrX);

    vtkNew<vtkFloatArray> arrY;
    arrY->SetName("Y");
    table->AddColumn(arrY);
    table->SetNumberOfRows(numSpecimen);

    for (int i = 0; i < numSpecimen; i++) {
        table->SetValue(i, 0, firstPc[i]);
        table->SetValue(i, 1, secondtPc[i]);
    }
    table->Modified();

    m_scatterChart->ClearPlots();
    vtkPlot *scatterPoints = m_scatterChart->AddPlot(vtkChart::POINTS);
    scatterPoints->SetInputData(table, 0, 1);
    scatterPoints->SetColor(0, 0, 0, 255);
    scatterPoints->SetWidth(1.0);
    scatterPoints->SetIndexedLabels(m_graphLabelArray);
    scatterPoints->SetTooltipLabelFormat("%i");
    dynamic_cast<vtkPlotPoints *>(scatterPoints)
        ->SetMarkerStyle(vtkPlotPoints::CROSS);
    vtkAxis *xAxis = m_scatterChart->GetAxis(vtkAxis::BOTTOM);
    xAxis->SetTitle(xAxLabel);
    vtkAxis *yAxis = m_scatterChart->GetAxis(vtkAxis::LEFT);
    yAxis->SetTitle(yAxLabel);
    m_scatterChart->Update();
    m_scatterChart->Modified();
    m_scatterChartView->Update();
    m_scatterChartView->Modified();
    m_scatterChartView->Render();
}

void PCAWindow::UpdateContribution(int pc) {
    auto targetVect = m_eigenVectors.col(pc - 1);
    // Create an array to hold the scalar point data
    vtkNew<vtkDoubleArray> scalars;
    scalars->SetName("Contribution");
    scalars->SetNumberOfComponents(1);
    scalars->SetNumberOfTuples(m_landmarksPoly->GetNumberOfPoints());

    for (int i = 0; i < targetVect.rows(); i++) {
        scalars->InsertTuple1(i, std::abs(targetVect[i]));
    }
    scalars->Modified();
    m_landmarksPoly->GetPointData()->SetScalars(scalars);
    m_landmarksPoly->Modified();
    m_lmVertexFilter->SetInputData(m_landmarksPoly);
    m_lmVertexFilter->Update();

    double maxContr = scalars->GetRange()[1];
    vtkIdType maxIndex = 0;
    double *scalarArray = static_cast<double *>(scalars->GetVoidPointer(0));
    for (vtkIdType i = 1; i < scalars->GetNumberOfTuples(); ++i) {
        if (scalarArray[i] == maxContr) {
            maxIndex = i;
            break;
        }
    }

    milLineEdit->setText(QString::number(maxIndex));
    QString milText = QString("MIL (%1)").arg(maxContr, 0, 'g', 3);

    vtkNew<vtkNamedColors> colors;
    m_meshRenderer->RemoveActor(m_MILLabelActor[0]);
    vtkNew<vtkBillboardTextActor3D> tempActor;
    tempActor->SetPosition(m_landmarksPoly->GetPoint(maxIndex));
    tempActor->SetInput(milText.toStdString().c_str());
    tempActor->GetTextProperty()->SetFontSize(20);
    tempActor->GetTextProperty()->SetBold(1);
    tempActor->GetTextProperty()->SetColor(
        colors->GetColor3d("Black").GetData());
    m_MILLabelActor[0] = tempActor;
    m_meshRenderer->AddActor(m_MILLabelActor[0]);

    vtkNew<vtkLookupTable> lut;
    lut->SetNumberOfTableValues(512);
    lut->SetTableRange(scalars->GetRange());
    for (int i = 0; i < lut->GetNumberOfColors(); ++i) {
        std::array<double, 3> rgb;
        m_ctf->GetColor(double(i) / lut->GetNumberOfColors(), rgb.data());
        std::array<double, 4> rgba{0.0, 0.0, 0.0, 1.0};
        std::copy(std::begin(rgb), std::end(rgb), std::begin(rgba));
        lut->SetTableValue(i, rgba.data());
    }
    lut->Build();

    m_lmMapper->SetInputData(m_lmVertexFilter->GetOutput());
    m_lmMapper->SetScalarModeToUsePointFieldData();
    m_lmMapper->SelectColorArray("Contribution");
    m_lmMapper->SetScalarRange(scalars->GetRange());
    m_lmMapper->SetLookupTable(lut);
    m_lmMapper->Update();

    InterpolateTPSContributionToMesh(scalars);
}

void PCAWindow::InterpolateTPSContributionToMesh(vtkDoubleArray *scalars) {
    vtkNew<vtkDoubleArray> interpolatedScalars;
    // Modal dialog
    WaitDialog waitDialog(this);
    waitDialog.setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    waitDialog.setModal(true);
    waitDialog.setFixedSize(200, 150);

    QVBoxLayout layout(&waitDialog);

    QLabel msg("Updating...");
    msg.setAlignment(Qt::AlignCenter);
    SpinnerWidget spinner;
    layout.addWidget(&msg);
    layout.addWidget(&spinner, 0, Qt::AlignCenter);

    // Spinner thread
    SpinnerThread spinThread(&spinner);
    QObject::connect(&spinThread, &SpinnerThread::updateAngle, &spinner,
                     &SpinnerWidget::setAngle);

    // Decimation thread
    QThread updateThread;
    UpdateContributionThread *worker = new UpdateContributionThread(
        m_landmarksPoly, m_meshData, scalars, interpolatedScalars);

    worker->moveToThread(&updateThread);
    // Cleanup for *success*
    QObject::connect(&updateThread, &QThread::started, worker,
                     &UpdateContributionThread::run);
    QObject::connect(worker, &UpdateContributionThread::finished, &updateThread,
                     &QThread::quit);
    QObject::connect(worker, &UpdateContributionThread::finished, &spinThread,
                     &QThread::quit);
    QObject::connect(worker, &UpdateContributionThread::finished, &waitDialog,
                     &QDialog::accept);
    // Cleanup for *failure*
    QObject::connect(worker, &UpdateContributionThread::failed, &updateThread,
                     &QThread::quit);
    QObject::connect(worker, &UpdateContributionThread::failed, &spinThread,
                     &QThread::quit);
    QObject::connect(worker, &UpdateContributionThread::failed, &waitDialog,
                     &QDialog::accept);

    QObject::connect(&updateThread, &QThread::finished, worker,
                     &QObject::deleteLater);

    connect(worker, &UpdateContributionThread::failed, this,
            [this](const QString &reason) {
                QMessageBox::warning(this, "Updating Contribution Failed!",
                                     reason);
            });

    // Start both threads
    spinThread.start();
    updateThread.start();

    waitDialog.exec();  // Blocks UI

    // Cleanup
    spinThread.wait();
    updateThread.wait();

    if (interpolatedScalars->GetNumberOfTuples() > 0) {
        // Assign to mesh and render
        m_meshData->GetPointData()->SetScalars(interpolatedScalars);
        m_meshData->Modified();

        vtkNew<vtkLookupTable> lut;
        lut->SetNumberOfTableValues(512);
        double range[2];
        interpolatedScalars->GetRange(range);
        lut->SetTableRange(range);
        for (int i = 0; i < lut->GetNumberOfTableValues(); ++i) {
            double val =
                static_cast<double>(i) / (lut->GetNumberOfTableValues() - 1);
            double rgb[3];
            m_ctf->GetColor(val, rgb);
            lut->SetTableValue(i, rgb[0], rgb[1], rgb[2], 1.0);
        }
        lut->Build();

        m_meshMapper->SetInputData(m_meshData);
        m_meshMapper->SetScalarModeToUsePointData();
        m_meshMapper->SelectColorArray("Contribution");
        m_meshMapper->SetScalarRange(range);
        m_meshMapper->SetLookupTable(lut);
        m_meshMapper->Update();

        m_scalarBar->SetLookupTable(lut);
        m_scalarBar->Modified();

        m_meshRenderer->GetRenderWindow()->Render();
    } else {
        std::cerr << "TPS interpolation failed to update contributions"
                  << std::endl;
    }
}

void PCAWindow::Update1stPC(int PC1) {
    select1stPCsSpinBox->setEnabled(false);
    m_x = PC1;
    UpdateScatter(m_x, m_y);
    std::string renLabelTitle =
        "Contribution Plot of PC " + std::to_string(m_x);
    m_meshRenLabel->setText(QString::fromStdString(renLabelTitle));
    m_meshRenLabel->update();
    UpdateContribution(m_x);
    select1stPCsSpinBox->setEnabled(true);
}
void PCAWindow::Update2ndPC(int PC2) {
    m_y = PC2;
    UpdateScatter(m_x, m_y);
}

template <typename M>
M PCAWindow::LoadCSV(const std::string &path) {
    std::ifstream inData;
    inData.open(path);
    std::string line;
    std::vector<double> dValues;
    uint rows = 0;
    while (std::getline(inData, line, '\n')) {
        std::stringstream lineStream(line);
        std::string cell;
        while (std::getline(lineStream, cell, ',')) {
            dValues.push_back(std::stod(cell));
        }
        ++rows;
    }
    if (dValues.size() > 0) {
        return Map<const Matrix<typename M::Scalar, M::RowsAtCompileTime,
                                M::ColsAtCompileTime, RowMajor>>(
            dValues.data(), rows, dValues.size() / rows);
    } else {
        Eigen::MatrixXd matrix;
        return matrix;
    }
}

void PCAWindow::Calculate(Eigen::MatrixXd &data, bool standardise) {
    Eigen::MatrixXd centeredMatrix;
    Eigen::MatrixXd covarianceMatrix;
    // Calculate mean of each column (feature)
    // Formula: μ = (Σx)/n for each column
    Eigen::Matrix<double, Eigen::Dynamic, 1, Eigen::ColMajor> mean =
        data.colwise().sum() / (data.rows());
    // Calculate variance of each column (feature)
    // Formula: σ² = Σ(x-μ)²/(n-1) for each column
    Eigen::Matrix<double, Eigen::Dynamic, 1, Eigen::ColMajor> variance =
        (data.rowwise() - mean.transpose()).array().pow(2).colwise().sum() /
        (data.rows() - 1);
    // Calculate standard deviation (square root of variance)
    Eigen::Matrix<double, Eigen::Dynamic, 1, Eigen::ColMajor> std =
        variance.array().sqrt();
    // Center the data by subtracting the mean from each feature
    centeredMatrix = data.rowwise() - mean.transpose();
    // If standardization requested, scale each feature by its standard
    // deviation
    if (standardise) {
        centeredMatrix.array().rowwise() /=
            std.transpose()
                .array();  // Standardize data (z-score normalization)
    }

    // Compute covariance matrix of the centered (and possibly standardized)
    // data Formula: cov(X) = (Xᵀ * X)/(n-1)
    covarianceMatrix = (centeredMatrix.adjoint().operator*(centeredMatrix)) /
                       (data.rows() - 1);
    // Perform eigendecomposition of the covariance matrix
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> es;
    es.compute(covarianceMatrix);
    // Get eigenvalues and eigenvectors from the decomposition
    auto eigen_values = es.eigenvalues();
    auto eigen_vectors = es.eigenvectors();
    // DebugPrintMatrix(eigen_vectors);
    //  Pair eigenvalues with their original indices for sorting
    typedef std::pair<double, int> eigen_pair;
    std::vector<eigen_pair> ep;
    for (int i = 0; i < data.cols(); ++i) {
        ep.push_back(std::make_pair(eigen_values(i), i));
    }
    // Sort eigenvalues in descending order (default behavior)
    sort(ep.begin(), ep.end());  // Ascending order by default
    // Initialize member matrices for results
    m_eigenVectors =
        Eigen::MatrixXd::Zero(eigen_vectors.rows(), eigen_vectors.cols());
    m_eigenValues = Eigen::VectorXd::Zero(data.cols());
    m_expVariance = Eigen::VectorXd::Zero(data.cols());
    // Store eigenvalues and vectors in descending order (most significant
    // first)
    int colnum = 0;
    for (int i = ep.size() - 1; i > -1; i--) {
        m_eigenValues(colnum) = ep[i].first;  // Store sorted eigenvalues
        m_eigenVectors.col(colnum) += eigen_vectors.col(
            ep[i].second);  // Store corresponding eigenvectors
        colnum++;
    }
    // Calculate principal component scores (projected data)
    // Formula: scores = centered_data * eigenvectors
    m_eigenScores = centeredMatrix.operator*(m_eigenVectors);

    // each column of eigenvector correlates with PCs, and each row with
    // variables e.g. for pc1, we separate column 0, and check the loading
    // DebugPrintMatrix(m_eigenVectors);

    // Calculate explained variance for each principal component
    // Formula: λ_i / Σλ (eigenvalue divided by sum of all eigenvalues)
    double sumTotVar = m_eigenValues.sum();
    for (int i = 0; i < data.cols(); i++) {
        m_expVariance(i) = m_eigenValues(i) / sumTotVar;
    }
    // Free memory by resizing working matrices to 0
    centeredMatrix.resize(0, 0);
    covarianceMatrix.resize(0, 0);
}

void PCAWindow::DebugPrintMatrix(Eigen::MatrixXd matrix) {
    Eigen::IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");
    std::string sep = "\n----------------------------------------\n";
    std::cout << matrix.format(CleanFmt) << sep;
}

void PCAWindow::ShowPtsIds() {
    if (showPtsIdsBox->isChecked()) {
        m_meshRenderer->AddActor(m_lmLabelActor);
        m_meshRenderer->GetRenderWindow()->Render();
    } else {
        m_meshRenderer->RemoveActor2D(m_lmLabelActor);
        m_meshRenderer->GetRenderWindow()->Render();
    }
}

// void PCAWindow::ShowSpecimenIds(){}

void PCAWindow::Export2Csv() {
    // const static Eigen::IOFormat CSVFormat(Eigen::StreamPrecision,
    // Eigen::DontAlignCols, ", ", "\n");
    const static Eigen::IOFormat CSVFormat(Eigen::StreamPrecision,
                                           Eigen::DontAlignCols, ", ");
    auto filter = "csv(*.csv)";
    std::string name;
    QString filename =
        QFileDialog::getSaveFileName(this, "Save file", "", filter, nullptr,
                                     QFileDialog::DontUseNativeDialog);
    QFileInfo fi(filename);
    QString ext = fi.completeSuffix();
    if (filename.isEmpty()) {
        return;
    }
    if (ext != "csv") {
        filename += ".csv";
    }
#ifdef _WIN32
    name = filename.toLocal8Bit().constData();
#else
    name = filename.toUtf8().constData();
#endif

    std::ofstream file(name.c_str());
    file << "Specimen"
         << ", ";
    for (int i = 1; i < m_eigenScores.cols() + 1; i++) {
        std::string colLabel = "PC" + std::to_string(i);
        file << colLabel << ", ";
    }
    file << "\n";
    for (int i = 0; i < m_nameList.size(); i++) {
        auto tempRow = m_eigenScores.row(i);
        file << m_nameList[i] << "," << tempRow.format(CSVFormat) << "\n";
    }

    /* file << "\n";
    file << m_eigenScores.format(CSVFormat); */
    file.close();
}

PCAWindow::~PCAWindow() {}
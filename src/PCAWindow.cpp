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

#include "../include/PCAWindow.h"

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
        /* Eigen::MatrixXd A = LoadCSV<Eigen::MatrixXd>("../iris.csv");
        Calculate(A); */

        // Now GUI stuff
        this->setWindowTitle("Principal Component Results");
        this->resize(1400, 600);
        QGridLayout *layout = new QGridLayout;
        m_meshRenderWidget = new QVTKOpenGLWidget();
        m_graphRenderWidget = new QVTKOpenGLWidget();
        /* m_graphRenderSeries = new QtCharts::QScatterSeries();
        m_graphRenderSeries->setMarkerShape(QtCharts::QScatterSeries::MarkerShapeCircle);
        m_graphRenderSeries->setPointLabelsVisible(0); */
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

        /* m_graphChart = new QtCharts::QChart();
        m_graphChart->setBackgroundVisible(false);
        m_graphChart->legend()->hide();
        m_axisX = new QtCharts::QValueAxis();
        m_axisY = new QtCharts::QValueAxis();
        m_graphChartView = new QtCharts::QChartView();
        //Handle series
        UpdateAxis(m_x, m_y);
        m_graphChart->addAxis(m_axisX, Qt::AlignBottom);
        m_graphChart->addAxis(m_axisY, Qt::AlignLeft);
        m_graphChart->addSeries(m_graphRenderSeries);
        m_graphRenderSeries->attachAxis(m_axisX);
        m_graphRenderSeries->attachAxis(m_axisY);
        m_graphChartView->setChart(m_graphChart); */

        std::string renLabelTitle =
            "Contribution Plot of PC " + std::to_string(m_x);
        m_meshRenLabel = new QLabel(QString::fromStdString(renLabelTitle));
        QLabel *graphRenLabel = new QLabel(tr("PCA Plot"));

        layout->addWidget(m_meshRenLabel, 0, 2, 1, 3);
        layout->addWidget(graphRenLabel, 0, 8, 1, 9);
        layout->addWidget(m_meshRenderWidget, 1, 2, 10, 7);
        // layout->addWidget(m_graphChartView, 1, 9, 10, 10);
        layout->addWidget(m_graphRenderWidget, 1, 9, 10, 10);

        // Set layout in QWidget
        QWidget *window = new QWidget();
        window->setLayout(layout);
        // Set QWidget as the central layout of the main window
        this->setCentralWidget(window);

        // Setting up Toolbars
        mainToolbar = this->addToolBar("Toolbar");
        mainToolbar->setMovable(false);
        QLabel *selectPCsLabel1 = new QLabel(tr("Select The First PC:"));
        QLabel *selectPCsLabel2 = new QLabel(tr("Select The Next PC:"));
        select1stPCsComboBox = new QSpinBox();
        select1stPCsComboBox->setRange(1, m_eigenVectors.cols());
        select1stPCsComboBox->setValue(m_x);

        select2ndPCsComboBox = new QSpinBox();
        select2ndPCsComboBox->setRange(1, m_eigenVectors.cols());
        select2ndPCsComboBox->setValue(m_y);

        QLabel *showMIL = new QLabel();
        showMIL->setText(tr(" MIL"));
        showMIL->setParent(this);

        milLineEdit = new QLineEdit;
        milLineEdit->setPlaceholderText("0");
        milLineEdit->setReadOnly(1);
        milLineEdit->setMaximumWidth(50);
        //milLineEdit->setValidator(new QIntValidator(0, 10000, this));

        QLabel *showPtsIdsLabel = new QLabel();
        showPtsIdsLabel->setText(tr("  Show Landmark ids"));
        showPtsIdsLabel->setParent(this);

        exportButton = new QPushButton();
        exportButton->setToolTip("Export to CSV");
        exportButton->setText("Export PCs");

        showPtsIdsBox = new QCheckBox();
        showPtsIdsBox->setChecked(false);

        mainToolbar->addWidget(selectPCsLabel1);
        mainToolbar->addWidget(select1stPCsComboBox);

        mainToolbar->addWidget(selectPCsLabel2);
        mainToolbar->addWidget(select2ndPCsComboBox);
        // mainToolbar->addWidget(exportButton);
        mainToolbar->addWidget(showMIL);
        mainToolbar->addWidget(milLineEdit);

        mainToolbar->addWidget(showPtsIdsLabel);
        mainToolbar->addWidget(showPtsIdsBox);
        this->statusBar()->addPermanentWidget(exportButton, 0);

        connect(exportButton, &QPushButton::clicked, this,
                &PCAWindow::Export2Csv);

        void (QSpinBox ::*change1stPCFp)(int) = &QSpinBox ::valueChanged;
        connect(select1stPCsComboBox, change1stPCFp, this,
                &PCAWindow::Update1stPC);

        void (QSpinBox ::*change2ndPCFp)(int) = &QSpinBox ::valueChanged;
        connect(select2ndPCsComboBox, change2ndPCFp, this,
                &PCAWindow::Update2ndPC);

        connect(showPtsIdsBox, &QCheckBox::stateChanged, this,
                &PCAWindow::ShowPtsIds);
        // connect(m_graphRenderSeries, &QtCharts::QScatterSeries::pressed,
        // this, &PCAWindow::ShowSpecimenIds);

        //--------------------------
        vtkNew<vtkNamedColors> colors;
        m_meshRenderer = vtkSmartPointer<vtkRenderer>::New();
        m_meshRenWin = m_meshRenderWidget->GetRenderWindow();
        m_style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
        m_meshIren = vtkSmartPointer<vtkGenericRenderWindowInteractor>::New();
        m_meshActor = vtkSmartPointer<vtkActor>::New();

        m_meshRenWin->AddRenderer(m_meshRenderer);
        m_style->SetCurrentRenderer(m_meshRenderer);
        m_meshIren->SetInteractorStyle(m_style);
        m_meshIren->SetRenderWindow(m_meshRenWin);

        // scatter plot

        /*   #include "matplotlibcpp.h"
          namespace plt = matplotlibcpp;
          int main() {
          plt::plot({1,3,2,4});
          plt::show();
          } */

        // Mesh properties and color etc
        vtkNew<vtkDataSetMapper> meshmMapper;
        meshmMapper->SetInputData(m_meshData);
        meshmMapper->SetResolveCoincidentTopologyToOff();
        m_meshActor->SetMapper(meshmMapper);
        m_meshActor->GetProperty()->SetColor(1, 0.992, 0.815);
        m_meshActor->GetProperty()->SetOpacity(0.5);
        m_meshRenderer->AddActor(m_meshActor);

        // LM properties
        // Point properties and color etc
        vtkNew<vtkMassProperties> prop;
        prop->SetInputData(m_meshData);
        prop->Update();
        double area = prop->GetSurfaceArea();
        area = std::sqrt(area);
        double sizeConstant = numLM;
        if (sizeConstant < 100) {
            sizeConstant = 100;
        }
        if (sizeConstant > 300) {
            sizeConstant = 300;
        }
        vtkNew<vtkSphereSource> sphereSource;
        sphereSource->SetRadius(area / (sizeConstant));
        /*  vtkNew<vtkSphereSource> sphereSource2;
         sphereSource2->SetRadius((area/sizeConstant) * 1.5);
         vtkNew<vtkSphereSource> sphereSource3;
         sphereSource3->SetRadius((area/sizeConstant) * 0.5 ); */

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
        m_lmLabelActor = vtkSmartPointer<vtkActor2D>::New();
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
        m_graphLabelArray = vtkSmartPointer<vtkStringArray>::New();
        m_graphLabelArray->SetName("Names");
        for (int i = 0; i < m_nameList.size(); i++) {
            m_graphLabelArray->InsertNextValue(m_nameList[i]);
        }
        /* m_graphLabelPoly = vtkSmartPointer<vtkPolyData>::New();
        m_graphLabelActor = vtkSmartPointer<vtkActor2D>::New(); */
        m_scatterChartView = vtkSmartPointer<vtkContextView>::New();
        m_scatterChart = vtkSmartPointer<vtkChartXY>::New();

        m_graphRenWin = m_graphRenderWidget->GetRenderWindow();
        m_scatterChartView->SetRenderWindow(m_graphRenWin);
        m_scatterChartView->GetScene()->AddItem(m_scatterChart);
        // m_scatterChartView->SetInteractor(m_graphRenderWidget->GetInteractor());
        m_graphRenderWidget->SetRenderWindow(
            m_scatterChartView->GetRenderWindow());
        UpdateScatter(m_x, m_y);

        // m_scatterChart->GetTooltip()->SetVisible(1);
        // m_scatterChart->SetActionToButton(vtkChart::ZOOM_AXIS,
        // vtkContextMouseEvent::LEFT_BUTTON);
        // m_scatterChart->SetSelectionMethod(vtkChart::SELECTION_PLOTS);
        // m_scatterChart->SetDrawAxesAtOrigin(1);
        // m_scatterChart->SetForceAxesToBounds(1);

        /* vtkNew<vtkVertexGlyphFilter> graphLabelVertexFilter;
        graphLabelVertexFilter->SetInputData(m_graphLabelPoly);
        graphLabelVertexFilter->Update();
        vtkNew<vtkLabeledDataMapper> labelMapper;
        labelMapper->SetInputData(graphLabelVertexFilter->GetOutput());
        labelMapper->GetLabelTextProperty()->SetFontSize(15);
        m_graphLabelActor->SetMapper(labelMapper);
        m_graphLabelActor->SetPickable(0);
        m_scatterChartView->GetRenderer()->AddActor(m_graphLabelActor); */

        // bg color
        m_meshRenderer->SetBackground(
            colors->GetColor3d("SlateGray").GetData());

        m_meshRenWin->Render();
        m_meshIren->Start();

        m_graphRenderWidget->show();
        m_scatterChartView->Render();

        this->show();
    } else {
        auto errorDialogue = QMessageBox();
        errorDialogue.setIcon(QMessageBox::Critical);
        errorDialogue.setWindowTitle("Error");
        errorDialogue.setText(
            "SuperImpose your specimens first (minimum two of them), \n then "
            "try again!");
        errorDialogue.exec();
    }
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
    // arrY->SetName(yAxLabel.c_str());
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

    vtkNew<vtkColorTransferFunction> ctf;
    ctf->SetColorSpaceToDiverging();
    ctf->AddRGBPoint(0, 0.231373, 0.298039, 0.752941);
    ctf->AddRGBPoint(0.5, 0.865003, 0.865003, 0.865003);
    ctf->AddRGBPoint(1, 0.705882, 0.0156863, 0.14902);
    ctf->SetScaleToLinear();

    double maxContr = scalars->GetRange()[1];
    int maxIndex = scalars->LookupValue(maxContr);
    /* std::cout<< "Index of the most influential Landmark is:"<<std::endl;
    std::cout<< maxIndex<<std::endl; */
    milLineEdit->setText(QString::number(maxIndex));

    vtkNew<vtkNamedColors> colors;
    m_meshRenderer->RemoveActor(m_MILLabelActor[0]);
    vtkNew<vtkBillboardTextActor3D> tempActor;
    tempActor->SetPosition(m_landmarksPoly->GetPoint(maxIndex));
    tempActor->SetInput("MIL");
    tempActor->GetTextProperty()->SetFontSize(20);
    tempActor->GetTextProperty()->SetBold(1);
    tempActor->GetTextProperty()->SetColor(
            colors->GetColor3d("Green").GetData()); 
    m_MILLabelActor[0] = tempActor;
    m_meshRenderer->AddActor(m_MILLabelActor[0]);
    
    
    vtkNew<vtkLookupTable> lut;
    lut->SetTableRange(scalars->GetRange());
    for (int i = 0; i < lut->GetNumberOfColors(); ++i) {
        std::array<double, 3> rgb;
        ctf->GetColor(double(i) / lut->GetNumberOfColors(), rgb.data());
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

    // Update scalar bar
    m_scalarBar->SetLookupTable(m_lmMapper->GetLookupTable());
    m_scalarBar->Modified();

    m_meshRenderer->GetRenderWindow()->Render();
}

void PCAWindow::Update1stPC(int PC1) {
    m_x = PC1;
    UpdateScatter(m_x, m_y);

    std::string renLabelTitle =
        "Contribution Plot of PC " + std::to_string(m_x);
    m_meshRenLabel->setText(QString::fromStdString(renLabelTitle));
    m_meshRenLabel->update();
    UpdateContribution(m_x);
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
    Eigen::Matrix<double, Eigen::Dynamic, 1, Eigen::ColMajor> mean =
        data.colwise().sum() / (data.rows());
    Eigen::Matrix<double, Eigen::Dynamic, 1, Eigen::ColMajor> variance =
        (data.rowwise() - mean.transpose()).array().pow(2).colwise().sum() /
        (data.rows() - 1);
    Eigen::Matrix<double, Eigen::Dynamic, 1, Eigen::ColMajor> std =
        variance.array().sqrt();

    centeredMatrix = data.rowwise() - mean.transpose();
    if (standardise) {
        centeredMatrix.array().rowwise() /=
            std.transpose().array();  // Standardize data.
    }

    // Compute covariance matrix
    covarianceMatrix = (centeredMatrix.adjoint().operator*(centeredMatrix)) /
                       (data.rows() - 1);
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> es;
    es.compute(covarianceMatrix);

    auto eigen_values = es.eigenvalues();
    auto eigen_vectors = es.eigenvectors();
    //DebugPrintMatrix(eigen_vectors);

    typedef std::pair<double, int> eigen_pair;
    std::vector<eigen_pair> ep;
    for (int i = 0; i < data.cols(); ++i) {
        ep.push_back(std::make_pair(eigen_values(i), i));
    }
    sort(ep.begin(), ep.end());  // Ascending order by default
    // Sort them all in descending order
    m_eigenVectors =
        Eigen::MatrixXd::Zero(eigen_vectors.rows(), eigen_vectors.cols());
    m_eigenValues = Eigen::VectorXd::Zero(data.cols());
    m_expVariance = Eigen::VectorXd::Zero(data.cols());
    int colnum = 0;
    for (int i = ep.size() - 1; i > -1; i--) {
        m_eigenValues(colnum) = ep[i].first;
        m_eigenVectors.col(colnum) += eigen_vectors.col(ep[i].second);
        colnum++;
    }
    m_eigenScores = centeredMatrix.operator*(m_eigenVectors);
    
    // DebugPrintMatrix(m_eigenScores);

    // each column of eigenvector correlates with PCs, and each row with
    // variables e.g. for pc1, we separate column 0, and check the loading
    // DebugPrintMatrix(m_eigenVectors);

    // Explained Variance
    double sumTotVar = m_eigenValues.sum();
    for (int i = 0; i < data.cols(); i++) {
        m_expVariance(i) = m_eigenValues(i) / sumTotVar;
    }
    //DebugPrintMatrix(m_expVariance);

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
        QFileDialog::getSaveFileName(this, "Save file", "", filter);
    QFileInfo fi(filename);
    QString ext = fi.completeSuffix();
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
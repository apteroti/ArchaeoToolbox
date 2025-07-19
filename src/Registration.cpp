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

#include "../include/Registration.h"

#include "../include/SpecimenDigitiser.h"

/* Registration::Registration(){
    m_templateRenderWidget = new QVTKOpenGLWidget();
    m_overlayRenderWidget = new QVTKOpenGLWidget();
    m_targetRenderWidget = new QVTKOpenGLWidget();
} */

Registration::Registration(vtkPolyData* data, vtkPolyData* templateMesh,
                           vtkPoints* sliders, SpecimenDigitiser* parent)
    : m_parent(parent),
      m_meshData(data),
      m_templateMesh(templateMesh),
      m_templateSliders(sliders) {
        
    /* m_templateTypeII = m_parent->GetTemplateTypeI();
    m_templateCurveSliders = vtkSmartPointer<vtkPoints>::New();
    m_parent->GetTemplateCurveSliders(m_templateCurveSliders);

    m_typeII = m_parent->GetTypeI();
    m_curveSliders = vtkSmartPointer<vtkPoints>::New();
    m_parent->GetCurveSliders(m_curveSliders); */
    m_mutex = new QMutex();
    vtkNew<vtkIntArray> OrgIds;
    OrgIds->SetName("OriginalID");
    OrgIds->SetNumberOfComponents(1);
    OrgIds->SetNumberOfTuples(m_meshData->GetNumberOfPoints());
    for (int i = 0; i < OrgIds->GetNumberOfTuples(); i++) {
        OrgIds->SetValue(i, i);
    }
    m_meshData->GetPointData()->AddArray(OrgIds);
    m_meshData->Modified();

    m_ignoreInside = m_parent->GetIgnorSetting();
    //-----------------------------------------
    // For automatic registration

    m_templateAnchor = vtkSmartPointer<vtkPoints>::New();
    m_sourceAnchor = vtkSmartPointer<vtkPoints>::New();
    //------------------------------------------
    m_templateRenWin = vtkSmartPointer<vtkRenderWindow>::New();
    m_templateRenderer = vtkSmartPointer<vtkRenderer>::New();
    m_templateIren = vtkSmartPointer<vtkGenericRenderWindowInteractor>::New();
    m_templateStyle = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    m_templatePointPickerStyle =
        vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    m_templatePointMoverStyle =
        vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    m_template2Dstyle = vtkSmartPointer<vtkInteractorStyleImage>::New();
    m_templateMeshActor = vtkSmartPointer<vtkActor>::New();
    m_templatePointActor = vtkSmartPointer<vtkActor>::New();
    m_templatePointsPoly = vtkSmartPointer<vtkPolyData>::New();
    m_templateVertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    m_templateHighlightPoints = vtkSmartPointer<vtkPoints>::New();
    m_templateOldIds = vtkSmartPointer<vtkIntArray>::New();
    m_templateGrabPicker = vtkSmartPointer<vtkPointPicker>::New();
    //-----------------------------------------
    m_targetRenWin = vtkSmartPointer<vtkRenderWindow>::New();
    m_targetRenderer = vtkSmartPointer<vtkRenderer>::New();
    m_targetIren = vtkSmartPointer<vtkGenericRenderWindowInteractor>::New();
    m_targetStyle = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    m_targetPointPickerStyle =
        vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    m_targetPointMoverStyle =
        vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    m_target2Dstyle = vtkSmartPointer<vtkInteractorStyleImage>::New();
    m_targetMeshActor = vtkSmartPointer<vtkActor>::New();
    m_targetPointActor = vtkSmartPointer<vtkActor>::New();
    m_targetPointsPoly = vtkSmartPointer<vtkPolyData>::New();
    m_targetVertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    m_targetHighlightPoints = vtkSmartPointer<vtkPoints>::New();
    m_targetOldIds = vtkSmartPointer<vtkIntArray>::New();
    m_targetGrabPicker = vtkSmartPointer<vtkPointPicker>::New();
    m_targetMapper = vtkSmartPointer<vtkDataSetMapper>::New();
    //-----------------------------------------
    m_overlayRenWin = vtkSmartPointer<vtkRenderWindow>::New();
    m_overlayRenderer = vtkSmartPointer<vtkRenderer>::New();
    m_overlayMapper = vtkSmartPointer<vtkDataSetMapper>::New();
    m_overlayMeshActor = vtkSmartPointer<vtkActor>::New();
    m_overlayTemplateActor = vtkSmartPointer<vtkActor>::New();
    m_sliderPointActor = vtkSmartPointer<vtkActor>::New();
    m_sliderPointActor->GetProperty()->SetOpacity(1);
    // m_sliderPointsPoly = vtkSmartPointer<vtkPolyData>::New();
    m_sliderVertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    m_preSliderHighlightPoints = vtkSmartPointer<vtkPoints>::New();
    m_overlaidMesh = vtkSmartPointer<vtkPolyData>::New();
    m_sliderTempPointActor = vtkSmartPointer<vtkActor>::New();
    m_sliderTempVertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    m_morphedMesh = vtkSmartPointer<vtkPolyData>::New();
    // Set layout
    this->setWindowTitle("Registration");
    this->resize(800, 300);
    QGridLayout* layout = new QGridLayout;
    m_templateRenderWidget = new QVTKOpenGLWidget();
    m_overlayRenderWidget = new QVTKOpenGLWidget();
    m_targetRenderWidget = new QVTKOpenGLWidget();
    QLabel* templateLabel = new QLabel(tr("Template"));
    overlayLabel = new QLabel(tr("Registration Overlay"));
    QLabel* targetLabel = new QLabel(tr("Specimen"));
    layout->addWidget(templateLabel, 0, 0, 1, 1);
    layout->addWidget(overlayLabel, 0, 1, 1, 1);
    layout->addWidget(targetLabel, 0, 2, 1, 1);
    layout->addWidget(m_templateRenderWidget, 1, 0, 10, 1);
    layout->addWidget(m_overlayRenderWidget, 1, 1, 10, 1);
    layout->addWidget(m_targetRenderWidget, 1, 2, 10, 1);
    // Set layout in QWidget
    QWidget* window = new QWidget();
    window->setLayout(layout);
    // Set QWidget as the central layout of the main window
    this->setCentralWidget(window);
    // Setting up Toolbars
    mainToolbar = this->addToolBar("Toolbar");
    mainToolbar->setMovable(false);
    QLabel* registerTypeLabel = new QLabel(tr("Register Type:"));
    registerTypeComboBox = new QComboBox();
    registerTypeComboBox->addItem(tr("Manual"));
    registerTypeComboBox->addItem(tr("Semi-Automatic"));
    registerTypeComboBox->addItem(tr("Automatic"));
    mainToolbar->addWidget(registerTypeLabel);
    mainToolbar->addWidget(registerTypeComboBox);
    // set buttons
    morphButton = new QPushButton("Morph");
    morphButton->setToolTip("Morph target to Template");
    morphButton->setCheckable(false);
    morphButton->setEnabled(0);
    mainToolbar->addWidget(morphButton);

    sliderButton = new QPushButton("Set Sliders");
    sliderButton->setToolTip("Set Primitive Sliders");
    sliderButton->setCheckable(false);
    mainToolbar->addWidget(sliderButton);

    resetButton = new QPushButton("Reset");
    resetButton->setToolTip("Reset the progress");
    resetButton->setCheckable(false);
    this->statusBar()->addWidget(resetButton);

    connect(morphButton, &QPushButton::clicked, this, &Registration::MorphTool);
    connect(sliderButton, &QPushButton::clicked, this,
            &Registration::SliderTool);
    connect(resetButton, &QPushButton::clicked, this, &Registration::ResetTool);
    // connect(refineButton, &QPushButton::clicked, this,
    // &Registration::Refine);
    void (QComboBox ::*fp)(int) = &QComboBox ::currentIndexChanged;
    connect(registerTypeComboBox, fp, this, &Registration::ChangeRegisterMode);

    progressLineEdit = new QLineEdit();
    progressLineEdit->setReadOnly(1);
    progressLineEdit->hide();
    this->statusBar()->addPermanentWidget(progressLineEdit, 1);

    statusLabel = new QLabel(this);
    statusLabel->setText("Status: Idle");
    progressLabel = new QLabel(this);
    progressLabel->setPixmap(QPixmap(":/icons/graphics/icons/idle.svg"));

    this->statusBar()->addPermanentWidget(statusLabel, 0);
    this->statusBar()->addPermanentWidget(progressLabel, 0);

    SetTemplateScene();
    SetTargetScene();
    Register();

    this->show();
}

void Registration::Register() {
    if (m_meshData->GetNumberOfPoints() < m_resampledRes) {
        m_resampledRes = m_meshData->GetNumberOfPoints();
    }
    if (m_regMethod == METHOD::Auto) {
        m_status = ALIGNEDSTATUS::Aligned;
        ResetOverlay();
        m_templateIren->SetInteractorStyle(m_templateStyle);
        m_targetIren->SetInteractorStyle(m_targetStyle);

        sliderButton->setEnabled(0);
        resetButton->setEnabled(0);
        registerTypeComboBox->setEnabled(0);
        if(m_anchorStat){
            vtkNew<vtkLandmarkTransform> landmarkTransform;
            landmarkTransform->SetTargetLandmarks(m_templateAnchor);
            landmarkTransform->SetSourceLandmarks(m_sourceAnchor);
            landmarkTransform->SetModeToSimilarity();
            landmarkTransform->Update();

            vtkNew<vtkTransformPolyDataFilter> transformFilter;
            transformFilter->SetInputData(m_meshData);
            transformFilter->SetTransform(landmarkTransform);
            transformFilter->Update();

            vtkPolyData* tempMeshData = transformFilter->GetOutput();

            if(!m_accuracy){
                QMessageBox::warning(
                this,
                tr("Potential Inaccuracy"),
                tr("Automated Registration needs to have other types of landmarks as anchors to get acceptable results.\n"
                "There will be a possibility of getting inaccurate landmark distribution after the sliding process!"),
                QMessageBox::Ok);
            }

            delete m_regThread;
            m_regThread =
                new RegistrationThread(m_templateMesh, tempMeshData, m_morphedMesh,
                                   m_ignoreInside, m_resampledRes, m_mutex, true);
            m_regThread->setParent(this);
            connect(m_regThread, &RegistrationThread::MeshMorphed, this,
                &Registration::OnMeshMorphed);
            m_regThread->start();
            RegistrationStatus();
        }
        else{
            QMessageBox::warning(
                this,
                tr("Landmark Digitisation Required"),
                tr("You need to digitise other types of landmarks (Type I-II or curve sliders) first.\n"
                "You will possibly get inaccurate landmark distribution after the sliding process!"),
                QMessageBox::Ok);
            delete m_regThread;
            m_regThread =
                new RegistrationThread(m_templateMesh, m_meshData, m_morphedMesh,
                                   m_ignoreInside, m_resampledRes, m_mutex, false);
            m_regThread->setParent(this);
            connect(m_regThread, &RegistrationThread::MeshMorphed, this,
                &Registration::OnMeshMorphed);
            m_regThread->start();
            RegistrationStatus();
        }
    }
    if (m_regMethod == METHOD::Manual) {
        ResetOverlay();
        m_overlaidMesh->DeepCopy(m_meshData);
        m_templateIren->SetInteractorStyle(m_templatePointPickerStyle);
        m_targetIren->SetInteractorStyle(m_targetPointPickerStyle);
    }
    if (m_regMethod == METHOD::Semi) {
        ResetOverlay();
        m_overlaidMesh->DeepCopy(m_meshData);
        m_templateIren->SetInteractorStyle(m_templatePointPickerStyle);
        m_targetIren->SetInteractorStyle(m_targetPointPickerStyle);
    }
}

void Registration::RegistrationStatus() {
    if (m_regThread) {
        delete m_morphingStatThread;
        m_morphingStatThread = new StatusReporterThread(m_regThread);
        m_morphingStatThread->setParent(this);
        connect(m_morphingStatThread, &StatusReporterThread::StatusChanged, this,
                &Registration::OnRegisterStatusChanged);
        m_morphingStatThread->start();
    }
}

void Registration::OnMeshMorphed() {
    m_mutex->lock();
    m_overlaidMesh->DeepCopy(m_morphedMesh);
    m_overlayMapper->SetInputData(m_overlaidMesh);
    m_overlayMapper->Modified();
    m_overlayMeshActor->Modified();
    m_overlayRenderer->GetRenderWindow()->Render();

    sliderButton->setEnabled(1);
    sliderButton->setEnabled(1);
    resetButton->setEnabled(1);
    registerTypeComboBox->setEnabled(1);
    morphButton->setEnabled(1);
    m_mutex->unlock();
}

void Registration::PCA(Eigen::MatrixXd& data, Eigen::MatrixXd& out) {
    out.resize(0, 0);
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
    covarianceMatrix = (centeredMatrix.adjoint().operator*(centeredMatrix)) /
                       (data.rows() - 1);
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> es;
    es.compute(covarianceMatrix);

    auto eigen_values = es.eigenvalues();
    auto eigen_vectors = es.eigenvectors();
    typedef std::pair<double, int> eigen_pair;
    std::vector<eigen_pair> ep;
    for (int i = 0; i < data.cols(); ++i) {
        ep.push_back(std::make_pair(eigen_values(i), i));
    }
    sort(ep.begin(), ep.end());  // Ascending order by default
    // Sort them all in descending order
    Eigen::MatrixXd eigenVectors =
        Eigen::MatrixXd::Zero(eigen_vectors.rows(), eigen_vectors.cols());
    Eigen::VectorXd eigenValues = Eigen::VectorXd::Zero(data.cols());
    int colnum = 0;
    for (int i = ep.size() - 1; i > -1; i--) {
        eigenValues(colnum) = ep[i].first;
        eigenVectors.col(colnum) += eigen_vectors.col(ep[i].second);
        colnum++;
    }
    out = centeredMatrix.operator*(eigenVectors);
    centeredMatrix.resize(0, 0);
    covarianceMatrix.resize(0, 0);
}

void Registration::OnCoordinateChanged(Eigen::MatrixXd sendOffData) {
    FinalizeDigitization(sendOffData, 1);
}
void Registration::OnCoordinateNotChanged(Eigen::MatrixXd sendOffData) {
    FinalizeDigitization(sendOffData, 0);
}

void Registration::FinalizeDigitization(Eigen::MatrixXd& Lndmrks,
                                        bool sendOffData) {
    m_sliderPointActor->GetProperty()->SetOpacity(0.5);
    m_sliderPointActor->Modified();
    int nmFixed = m_templateHighlightPoints->GetNumberOfPoints();
    int numSurfaceSliders = m_templateSliders->GetNumberOfPoints();
    vtkNew<vtkPoints> surfacePts;

    if (numSurfaceSliders != 0) {
        int start = nmFixed;
        auto surfaceCoordsBlock = Lndmrks.block(start, 0, numSurfaceSliders, 3);
        for (int i = 0; i < numSurfaceSliders; i++) {
            surfacePts->InsertNextPoint(surfaceCoordsBlock(i, 0),
                                        surfaceCoordsBlock(i, 1),
                                        surfaceCoordsBlock(i, 2));
        }
        surfacePts->Modified();
        vtkNew<vtkPolyData> tempSurfacePtsPoly;
        tempSurfacePtsPoly->SetPoints(surfacePts);
        if (!sendOffData) {
            m_sliderTempVertexFilter->SetInputData(tempSurfacePtsPoly);
            m_sliderTempVertexFilter->Update();
            m_sliderTempVertexFilter->Modified();
            m_sliderTempPointActor->Modified();
            m_overlayRenderer->GetRenderWindow()->Render();
        }
        if (sendOffData) {
            m_sliderPointActor->GetProperty()->SetOpacity(1.0);
            m_sliderPointActor->Modified();
            m_parent->SetSurfaceSlider(surfacePts);
            tempSurfacePtsPoly->SetPoints(surfacePts);
            m_sliderVertexFilter->SetInputData(tempSurfacePtsPoly);
            m_sliderVertexFilter->Update();
            m_sliderVertexFilter->Modified();
            vtkNew<vtkPolyData> tempPoly;
            m_sliderTempVertexFilter->SetInputData(tempPoly);
            m_sliderTempVertexFilter->Update();
            m_sliderTempVertexFilter->Modified();
            m_overlayRenderer->RemoveActor(m_sliderTempPointActor);
            // m_overlayRenderer->ResetCamera();
            m_overlayRenderer->GetRenderWindow()->Render();
            progressLineEdit->hide();
            resetButton->setEnabled(1);
        }
    }
}

void Registration::OnRegisterStatusChanged(int status) {
    if (status > 0) {
        statusLabel->setText("Status: Morphing");
        if (status % 2 == 0) {
            progressLabel->setPixmap(
                QPixmap(":/icons/graphics/icons/vBusy.svg"));
        } else {
            progressLabel->setPixmap(
                QPixmap(":/icons/graphics/icons/busy.svg"));
        }
    }
    if (status == -1) {
        statusLabel->setText("Status: Aborting");
        progressLabel->setPixmap(QPixmap(":/icons/graphics/icons/busy.svg"));
    }
    if (status == 0) {
        statusLabel->setText("Status: Idle");
        progressLabel->setPixmap(QPixmap(":/icons/graphics/icons/idle.svg"));
    }
}


void Registration::DebugPrintMatrix(Eigen::MatrixXd matrix) {
    Eigen::IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");
    std::string sep = "\n----------------------------------------\n";
    std::cout << matrix.format(CleanFmt) << sep;
}

void Registration::ChangeRegisterMode(int index) {
    switch (index) {
        case 0:
            m_regMethod = METHOD::Manual;
            Register();
            break;

        case 1:
            m_regMethod = METHOD::Semi;
            Register();
            break;

        case 2:
            m_regMethod = METHOD::Auto;
            Register();
            break;
    }
}

void Registration::SetTemplateScene() {
    vtkNew<vtkNamedColors> colors;
    m_templateRenWin = m_templateRenderWidget->GetRenderWindow();
    m_templateRenWin->AddRenderer(m_templateRenderer);
    m_templatePointPickerStyle->SetCurrentRenderer(m_templateRenderer);
    m_template2Dstyle->SetCurrentRenderer(m_templateRenderer);
    m_templatePointMoverStyle->SetCurrentRenderer(m_templateRenderer);
    m_templateIren->SetInteractorStyle(m_templateStyle);
    m_templateIren->SetRenderWindow(m_templateRenWin);
    // Mesh properties and color etc
    vtkNew<vtkNamedColors> nc;
    nc->SetColor("Bone", 1.0, 0.3882, 0.2784);
    nc->Modified();
    vtkNew<vtkColorSeries> maskColorsSeries;
    maskColorsSeries->SetColorSchemeByName("myMaskColors");
    maskColorsSeries->AddColor(nc->GetColor3ub("Bone"));
    maskColorsSeries->AddColor(nc->GetColor3ub("Gray"));
    maskColorsSeries->Modified();
    vtkNew<vtkLookupTable> lut;
    maskColorsSeries->BuildLookupTable(lut, maskColorsSeries->ORDINAL);
    lut->Modified();

    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetInputData(m_templateMesh);
    mapper->SetResolveCoincidentTopologyToOff();
    mapper->SetScalarModeToUseCellFieldData();
    mapper->SelectColorArray("Masked");
    mapper->SetLookupTable(lut);

    m_templateMeshActor->SetMapper(mapper);
    m_templateRenderer->AddActor(m_templateMeshActor);
    // Point properties and color etc
    vtkNew<vtkMassProperties> prop;
    prop->SetInputData(m_templateMesh);
    prop->Update();
    double area = prop->GetSurfaceArea();
    double sizeConstant = m_parent->GetTemplateNOL();
    if (sizeConstant < 800) {
        sizeConstant = 5000;
    }
    if (sizeConstant >= 800) {
        sizeConstant = 10000;
    }

    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetRadius(area / (sizeConstant));
    vtkNew<vtkGlyph3DMapper> pointMapper;
    pointMapper->SetInputData(m_templateVertexFilter->GetOutput());
    pointMapper->SetSourceConnection(sphereSource->GetOutputPort());
    pointMapper->ScalingOff();
    pointMapper->ScalarVisibilityOff();
    // m_pointActor->GetProperty()->SetPointSize(10);
    m_templatePointActor->GetProperty()->SetColor(0.0, 0.5, 1.0);
    m_templatePointActor->GetProperty()->SetDiffuse(.8);
    m_templatePointActor->GetProperty()->SetSpecular(.5);
    m_templatePointActor->GetProperty()->SetSpecularPower(30);
    m_templatePointActor->SetMapper(pointMapper);
    m_templateRenderer->AddActor(m_templatePointActor);
    // Label properties
    vtkNew<vtkLabeledDataMapper> labelMapper;
    labelMapper->SetInputData(m_templateVertexFilter->GetOutput());
    labelMapper->GetLabelTextProperty()->SetFontSize(15);
    vtkNew<vtkActor2D> labelActor;
    labelActor->SetMapper(labelMapper);
    labelActor->GetProperty()->SetColor(colors->GetColor3d("Orange").GetData());
    m_templateRenderer->AddActor(labelActor);
    // Ambient properties
    m_templateRenderer->SetBackground(
        colors->GetColor3d("SlateGray").GetData());
    // Observers
    m_templatePointPickerStyle->AddObserver(vtkCommand::LeftButtonPressEvent,
                                            this,
                                            &Registration::TemplatePickFunc, 1);
    m_templatePointPickerStyle->AddObserver(
        vtkCommand::LeftButtonReleaseEvent, this,
        &Registration::TemplateResetLeftClck, 1);
    m_templatePointPickerStyle->AddObserver(vtkCommand::MiddleButtonPressEvent,
                                            this,
                                            &Registration::TemplateMoveFunc, 1);
    m_templatePointMoverStyle->AddObserver(
        vtkCommand::MiddleButtonReleaseEvent, this,
        &Registration::TemplateResetMouseMove, 1);
    m_templatePointMoverStyle->AddObserver(
        vtkCommand::MouseMoveEvent, this, &Registration::TemplateCoordinateFunc,
        1);
    m_templateRenWin->Render();
    m_templateIren->Start();
}

void Registration::TemplatePickFunc(vtkObject* caller,
                                    long unsigned int eventId, void* callData) {
    if (m_templateIren->GetControlKey()) {
        m_templateIren->SetInteractorStyle(m_template2Dstyle);
        m_templateIren->Modified();
        m_templateMeshActor->SetPickable(1);
        m_templateMeshActor->Modified();
        m_templatePointActor->SetPickable(0);
        m_templatePointActor->Modified();
        auto clickPos = m_templateIren->GetEventPosition();
        vtkNew<vtkCellPicker> picker;
        picker->Pick(clickPos[0], clickPos[1], 0, m_templateRenderer);
        if (picker->GetPointId() != -1) {
            m_templateOldIds->InsertNextValue(picker->GetPointId());
            m_templateHighlightPoints->InsertNextPoint(
                m_templateMesh->GetPoint(picker->GetPointId()));
            if (m_regMethod == METHOD::Manual) {
                Warp();
            }
            if (m_regMethod == METHOD::Semi) {
                SI();
            }
            m_templatePointActor->Modified();
            m_templatePointsPoly->SetPoints(m_templateHighlightPoints);
            m_templatePointsPoly->GetPointData()->SetScalars(m_templateOldIds);
            m_templateVertexFilter->SetInputData(m_templatePointsPoly);
            m_templateVertexFilter->Update();
            m_templateVertexFilter->Modified();
            m_templateRenderer->GetRenderWindow()->Render();
        }
    }

    if (m_templateIren->GetInteractorStyle()->GetClassName() ==
        m_templatePointPickerStyle->GetClassName()) {
        m_templatePointPickerStyle->OnLeftButtonDown();
    } else if (m_templateIren->GetInteractorStyle()->GetClassName() ==
               m_template2Dstyle->GetClassName()) {
        m_templateIren->SetInteractorStyle(m_templatePointPickerStyle);
    }
}

void Registration::TemplateResetLeftClck(vtkObject* caller,
                                         long unsigned int eventId,
                                         void* callData) {
    m_templatePointPickerStyle->OnLeftButtonUp();
}

void Registration::TemplateMoveFunc(vtkObject* caller,
                                    long unsigned int eventId, void* callData) {
    if (m_templateIren->GetControlKey()) {
        m_templateMeshActor->SetPickable(0);
        m_templateMeshActor->Modified();
        m_templatePointActor->SetPickable(1);
        m_templatePointActor->Modified();
        auto clickPos = m_templateIren->GetEventPosition();
        m_templateGrabPicker->Pick(clickPos[0], clickPos[1], 0,
                                   m_templateRenderer);
        if (m_templateGrabPicker->GetPointId() != -1) {
            m_templateIren->SetInteractorStyle(m_templatePointMoverStyle);
        }
    } else {
        m_templatePointPickerStyle->OnMiddleButtonDown();
    }
}

void Registration::TemplateResetMouseMove(vtkObject* caller,
                                          long unsigned int eventId,
                                          void* callData) {
    m_templatePointMoverStyle->OnMiddleButtonUp();
    m_templateIren->SetInteractorStyle(m_templatePointPickerStyle);
}

void Registration::TemplateCoordinateFunc(vtkObject* caller,
                                          long unsigned int eventId,
                                          void* callData) {
    m_templateMeshActor->SetPickable(1);
    m_templateMeshActor->Modified();
    auto clickPos = m_templateIren->GetEventPosition();
    vtkNew<vtkCellPicker> meshPicker;
    meshPicker->Pick(clickPos[0], clickPos[1], 0, m_templateRenderer);
    double* newLocation = meshPicker->GetPickPosition();
    if (meshPicker->GetPointId() != -1) {
        auto id = m_templateGrabPicker->GetPointId();
        m_templateHighlightPoints->SetPoint(id, newLocation);
        m_templateHighlightPoints->Modified();
        m_templatePointsPoly->Modified();
        m_templateVertexFilter->Update();
        m_templateVertexFilter->Modified();
        if (m_regMethod == METHOD::Manual) {
            Warp();
        }
        if (m_regMethod == METHOD::Semi) {
            SI();
        }
        m_templateRenderer->GetRenderWindow()->Render();
    }
}

void Registration::SetTargetScene() {
    vtkNew<vtkNamedColors> colors;
    m_targetRenWin = m_targetRenderWidget->GetRenderWindow();
    m_targetRenWin->AddRenderer(m_targetRenderer);
    m_targetPointPickerStyle->SetCurrentRenderer(m_targetRenderer);
    m_target2Dstyle->SetCurrentRenderer(m_targetRenderer);
    m_targetPointMoverStyle->SetCurrentRenderer(m_targetRenderer);
    m_targetIren->SetInteractorStyle(m_targetStyle);
    m_targetIren->SetRenderWindow(m_targetRenWin);
    // Mesh properties and color etc
    vtkNew<vtkNamedColors> nc;
    nc->SetColor("Bone", 1, 0.992, 0.815);
    nc->Modified();
    vtkNew<vtkColorSeries> maskColorsSeries;
    maskColorsSeries->SetColorSchemeByName("myMaskColors");
    maskColorsSeries->AddColor(nc->GetColor3ub("Bone"));
    maskColorsSeries->AddColor(nc->GetColor3ub("Gray"));
    maskColorsSeries->Modified();
    vtkNew<vtkLookupTable> lut;
    maskColorsSeries->BuildLookupTable(lut, maskColorsSeries->ORDINAL);
    lut->Modified();

    vtkDataArray* maskedArray = m_meshData->GetCellData()->GetArray("Masked");
    if (maskedArray && vtkIntArray::SafeDownCast(maskedArray)) {
        m_targetMapper->SetInputData(m_meshData);
        m_targetMapper->SetResolveCoincidentTopologyToOff();
        m_targetMapper->SetScalarModeToUseCellFieldData();
        m_targetMapper->SelectColorArray("Masked");
        m_targetMapper->SetLookupTable(lut);

        m_targetMeshActor->SetMapper(m_targetMapper);
        m_targetMeshActor->GetProperty()->SetOpacity(1);
    } 
    else {
        m_targetMapper->SetInputData(m_meshData);
        m_targetMeshActor->SetMapper(m_targetMapper);
        m_targetMeshActor->GetProperty()->SetColor(1, 0.992, 0.815);
    }    
    m_targetRenderer->AddActor(m_targetMeshActor);
    // Point properties and color etc
    vtkNew<vtkMassProperties> prop;
    prop->SetInputData(m_meshData);
    prop->Update();
    double area = prop->GetSurfaceArea();
    double sizeConstant = m_parent->GetTemplateNOL();
    if (sizeConstant < 800) {
        sizeConstant = 5000;
    }
    if (sizeConstant >= 800) {
        sizeConstant = 10000;
    }
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetRadius(area / (sizeConstant));
    vtkNew<vtkGlyph3DMapper> pointMapper;
    pointMapper->SetInputData(m_targetVertexFilter->GetOutput());
    pointMapper->SetSourceConnection(sphereSource->GetOutputPort());
    pointMapper->ScalingOff();
    pointMapper->ScalarVisibilityOff();
    m_targetPointActor->GetProperty()->SetColor(0.0, 0.5, 1.0);
    m_targetPointActor->GetProperty()->SetDiffuse(.8);
    m_targetPointActor->GetProperty()->SetSpecular(.5);
    m_targetPointActor->GetProperty()->SetSpecularPower(30);
    m_targetPointActor->SetMapper(pointMapper);
    m_targetRenderer->AddActor(m_targetPointActor);
    // Label properties
    vtkNew<vtkLabeledDataMapper> labelMapper;
    labelMapper->SetInputData(m_targetVertexFilter->GetOutput());
    labelMapper->GetLabelTextProperty()->SetFontSize(15);
    vtkNew<vtkActor2D> labelActor;
    labelActor->SetMapper(labelMapper);
    labelActor->GetProperty()->SetColor(colors->GetColor3d("Orange").GetData());
    m_targetRenderer->AddActor(labelActor);
    // Ambient properties
    m_targetRenderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
    // Observers
    m_targetPointPickerStyle->AddObserver(vtkCommand::LeftButtonPressEvent,
                                          this, &Registration::TargetPickFunc,
                                          1);
    m_targetPointPickerStyle->AddObserver(
        vtkCommand::LeftButtonReleaseEvent, this,
        &Registration::TargetResetLeftClck, 1);
    m_targetPointPickerStyle->AddObserver(vtkCommand::MiddleButtonPressEvent,
                                          this, &Registration::TargetMoveFunc,
                                          1);
    m_targetPointMoverStyle->AddObserver(
        vtkCommand::MiddleButtonReleaseEvent, this,
        &Registration::TargetResetMouseMove, 1);
    m_targetPointMoverStyle->AddObserver(vtkCommand::MouseMoveEvent, this,
                                         &Registration::TargetCoordinateFunc,
                                         1);
    m_targetRenWin->Render();
    m_targetIren->Start();
}

void Registration::TargetPickFunc(vtkObject* caller, long unsigned int eventId,
                                  void* callData) {
    if (m_targetIren->GetControlKey()) {
        m_targetIren->SetInteractorStyle(m_target2Dstyle);
        m_targetIren->Modified();
        m_targetMeshActor->SetPickable(1);
        m_targetMeshActor->Modified();
        m_targetPointActor->SetPickable(0);
        m_targetPointActor->Modified();
        auto clickPos = m_targetIren->GetEventPosition();
        vtkNew<vtkCellPicker> picker;
        picker->Pick(clickPos[0], clickPos[1], 0, m_targetRenderer);
        if (picker->GetPointId() != -1) {
            m_targetOldIds->InsertNextValue(picker->GetPointId());
            m_targetHighlightPoints->InsertNextPoint(
                m_meshData->GetPoint(picker->GetPointId()));
            if (m_regMethod == METHOD::Manual) {
                Warp();
            }
            if (m_regMethod == METHOD::Semi) {
                SI();
            }
            m_targetPointActor->Modified();
            m_targetPointsPoly->SetPoints(m_targetHighlightPoints);
            m_targetPointsPoly->GetPointData()->SetScalars(m_targetOldIds);
            m_targetVertexFilter->SetInputData(m_targetPointsPoly);
            m_targetVertexFilter->Update();
            m_targetVertexFilter->Modified();
            m_targetRenderer->GetRenderWindow()->Render();
        }
    }

    if (m_targetIren->GetInteractorStyle()->GetClassName() ==
        m_targetPointPickerStyle->GetClassName()) {
        m_targetPointPickerStyle->OnLeftButtonDown();
    } else if (m_targetIren->GetInteractorStyle()->GetClassName() ==
               m_target2Dstyle->GetClassName()) {
        m_targetIren->SetInteractorStyle(m_targetPointPickerStyle);
    }
}

void Registration::TargetResetLeftClck(vtkObject* caller,
                                       long unsigned int eventId,
                                       void* callData) {
    m_targetPointPickerStyle->OnLeftButtonUp();
}

void Registration::TargetMoveFunc(vtkObject* caller, long unsigned int eventId,
                                  void* callData) {
    if (m_targetIren->GetControlKey()) {
        m_targetMeshActor->SetPickable(0);
        m_targetMeshActor->Modified();
        m_targetPointActor->SetPickable(1);
        m_targetPointActor->Modified();
        auto clickPos = m_targetIren->GetEventPosition();
        m_targetGrabPicker->Pick(clickPos[0], clickPos[1], 0, m_targetRenderer);
        if (m_targetGrabPicker->GetPointId() != -1) {
            m_targetIren->SetInteractorStyle(m_targetPointMoverStyle);
        }
    } else {
        m_targetPointPickerStyle->OnMiddleButtonDown();
    }
}

void Registration::TargetResetMouseMove(vtkObject* caller,
                                        long unsigned int eventId,
                                        void* callData) {
    m_targetPointMoverStyle->OnMiddleButtonUp();
    m_targetIren->SetInteractorStyle(m_targetPointPickerStyle);
}

void Registration::TargetCoordinateFunc(vtkObject* caller,
                                        long unsigned int eventId,
                                        void* callData) {
    m_targetMeshActor->SetPickable(1);
    m_targetMeshActor->Modified();
    auto clickPos = m_targetIren->GetEventPosition();
    vtkNew<vtkCellPicker> meshPicker;
    meshPicker->Pick(clickPos[0], clickPos[1], 0, m_targetRenderer);
    double* newLocation = meshPicker->GetPickPosition();
    if (meshPicker->GetPointId() != -1) {
        auto id = m_targetGrabPicker->GetPointId();
        m_targetHighlightPoints->SetPoint(id, newLocation);
        m_targetHighlightPoints->Modified();
        m_targetPointsPoly->Modified();
        m_targetVertexFilter->Update();
        m_targetVertexFilter->Modified();
        if (m_regMethod == METHOD::Manual) {
            Warp();
        }
        if (m_regMethod == METHOD::Semi) {
            SI();
        }
        m_targetRenderer->GetRenderWindow()->Render();
    }
}

void Registration::SetOverlayScene() {
    vtkNew<vtkNamedColors> colors;
    vtkSmartPointer<vtkGenericRenderWindowInteractor> iren =
        vtkSmartPointer<vtkGenericRenderWindowInteractor>::New();
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
        vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();

    m_overlayRenWin = m_overlayRenderWidget->GetRenderWindow();
    m_overlayRenWin->AddRenderer(m_overlayRenderer);
    iren->SetInteractorStyle(style);
    iren->SetRenderWindow(m_overlayRenWin);
    // Mesh properties and color etc
    // overlay mesh
    m_overlayMapper->SetInputData(m_meshData);
    m_overlayMapper->ScalarVisibilityOff();
    m_overlayMeshActor->SetMapper(m_overlayMapper);
    m_overlayMeshActor->GetProperty()->SetOpacity(.6);
    /* m_overlayMeshActor->GetProperty()->SetDiffuseColor(
        colors->GetColor3d("Aqua").GetData()); */
    m_overlayMeshActor->GetProperty()->SetColor(1, 0.992, 0.815);

    m_overlayRenderer->AddActor(m_overlayMeshActor);
    // template mesh
    vtkNew<vtkDataSetMapper> templateMapper;
    templateMapper->SetInputData(m_templateMesh);
    templateMapper->ScalarVisibilityOff();  // <- disables scalar-based coloring
    m_overlayTemplateActor->SetMapper(templateMapper);
    m_overlayTemplateActor->GetProperty()->SetDiffuseColor(
        colors->GetColor3d("Tomato").GetData());
    m_overlayRenderer->AddActor(m_overlayTemplateActor);

    // Point properties and color etc
    double sphereScaleRatio = 0.01; // ~1% of geometry diagonal

    double bounds[6];
    m_meshData->GetBounds(bounds);

    double dx = bounds[1] - bounds[0];
    double dy = bounds[3] - bounds[2];
    double dz = bounds[5] - bounds[4];
    double diagonal = std::sqrt(dx * dx + dy * dy + dz * dz);

    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetRadius(sphereScaleRatio * diagonal);

    vtkNew<vtkGlyph3DMapper> pointMapper;
    vtkNew<vtkPolyData> tempSurfacePtsPoly;
    tempSurfacePtsPoly->SetPoints(m_preSliderHighlightPoints);
    m_sliderVertexFilter->SetInputData(tempSurfacePtsPoly);
    m_sliderVertexFilter->Update();
    m_sliderVertexFilter->Modified();
    pointMapper->SetInputData(m_sliderVertexFilter->GetOutput());
    pointMapper->SetSourceConnection(sphereSource->GetOutputPort());
    pointMapper->ScalingOff();
    pointMapper->ScalarVisibilityOff();
    m_sliderPointActor->GetProperty()->SetColor(0.0, 0.5, 1.0);
    m_sliderPointActor->GetProperty()->SetDiffuse(.8);
    m_sliderPointActor->GetProperty()->SetSpecular(.5);
    m_sliderPointActor->GetProperty()->SetSpecularPower(30);
    m_sliderPointActor->SetMapper(pointMapper);
    m_overlayRenderer->AddActor(m_sliderPointActor);

    vtkNew<vtkGlyph3DMapper> pointTempMapper;
    pointTempMapper->SetInputData(m_sliderTempVertexFilter->GetOutput());
    pointTempMapper->SetSourceConnection(sphereSource->GetOutputPort());
    pointTempMapper->ScalingOff();
    pointTempMapper->ScalarVisibilityOff();

    m_sliderTempPointActor->GetProperty()->SetColor(0.0, 0.5, 1.0);
    m_sliderTempPointActor->GetProperty()->SetDiffuse(.8);
    m_sliderTempPointActor->GetProperty()->SetSpecular(.5);
    m_sliderTempPointActor->GetProperty()->SetSpecularPower(30);
    m_sliderTempPointActor->SetMapper(pointTempMapper);
    m_overlayRenderer->AddActor(m_sliderTempPointActor);
    // Ambient properties
    m_overlayRenderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

    m_overlayRenderer->ResetCamera();
    m_overlayRenWin->Render();
    iren->Start();
}

void Registration::MorphTool() {
    if (m_overlaidMesh->GetNumberOfPoints() > 0) {
        sliderButton->setEnabled(0);
        resetButton->setEnabled(0);
        registerTypeComboBox->setEnabled(0);
        morphButton->setEnabled(0);

        delete m_regThread;
        m_regThread = new RegistrationThread(m_templateMesh, m_overlaidMesh,
                                             m_morphedMesh, m_ignoreInside,
                                             m_resampledRes, m_mutex, true);
        m_regThread->setParent(this);
        connect(m_regThread, &RegistrationThread::MeshMorphed, this,
                &Registration::OnMeshMorphed);
        m_regThread->start();
        RegistrationStatus();
    } else {
        std::cout << "Problem, Debug MorphTool" << std::endl;
    }
}

void Registration::SliderTool() {
    m_preSliderHighlightPoints->Initialize();
    sliderButton->setEnabled(0);
    morphButton->setEnabled(0);
    // refineButton->setEnabled(1);
    overlayLabel->setText(tr("Specimen & Sliders"));
    vtkNew<vtkNamedColors> colors;
    registerTypeComboBox->setEnabled(0);
    m_overlayRenderer->RemoveActor(m_overlayMeshActor);
    m_overlayRenderer->RemoveActor(m_overlayTemplateActor);
    //----------------------------
    m_overlayMapper->SetInputData(m_meshData);
    m_overlayMapper->Modified();
    m_overlayMeshActor->GetProperty()->SetOpacity(1.0);
    m_overlayMeshActor->GetProperty()->SetColor(1, 0.992, 0.815);
    m_overlayMeshActor->Modified();
    m_overlayRenderer->AddActor(m_overlayMeshActor);

    vtkNew<vtkThreshold> threshold;
    threshold->SetInputData(m_overlaidMesh);
    threshold->ThresholdBetween(0, 0);
    threshold->SetInputArrayToProcess(
        0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_CELLS, "Masked");
    threshold->Update();

    vtkSmartPointer<vtkPolyData> pointLocatorData;
    if (threshold->GetOutput()->GetNumberOfPoints() > 0) {
        vtkNew<vtkGeometryFilter> geometry;
        geometry->SetInputData(threshold->GetOutput());
        geometry->Update();

        vtkNew<vtkCleanPolyData> cleanFilter;
        cleanFilter->SetInputData(geometry->GetOutput());
        cleanFilter->Update();

        pointLocatorData = cleanFilter->GetOutput();
    } else {
        pointLocatorData = m_overlaidMesh;
    }
    vtkNew<vtkPointLocator> pointTree;
    pointTree->SetDataSet(pointLocatorData);
    pointTree->BuildLocator();
    pointTree->Update();
    vtkIntArray* originalIdArray = vtkIntArray::SafeDownCast(
        pointLocatorData->GetPointData()->GetArray("OriginalID"));

    for (vtkIdType i = 0; i < m_templateSliders->GetNumberOfPoints(); ++i) {
        double* point = m_templateSliders->GetPoint(i);
        vtkIdType localPtId = pointTree->FindClosestPoint(point);

        int originalPtId = originalIdArray->GetValue(localPtId);  // <- Correct
        double* originalPoint = m_meshData->GetPoint(originalPtId);

        m_preSliderHighlightPoints->InsertNextPoint(originalPoint);
    }

    m_preSliderHighlightPoints->Modified();
    m_parent->SetSurfaceSlider(m_preSliderHighlightPoints);
    m_sliderPointActor->Modified();
    vtkNew<vtkPolyData> tempSurfacePtsPoly;
    tempSurfacePtsPoly->SetPoints(m_preSliderHighlightPoints);
    m_sliderVertexFilter->SetInputData(tempSurfacePtsPoly);
    m_sliderVertexFilter->Update();
    m_sliderVertexFilter->Modified();
    m_overlayRenderer->ResetCamera();
    m_overlayRenderer->GetRenderWindow()->Render();
}

void Registration::Warp() {
    int tarNum = m_targetHighlightPoints->GetNumberOfPoints();
    int tempNum = m_templateHighlightPoints->GetNumberOfPoints();
    if (tarNum == tempNum) {
        vtkNew<vtkThinPlateSplineTransform> tpsTrans;
        tpsTrans->SetTargetLandmarks(m_templateHighlightPoints);
        tpsTrans->SetSourceLandmarks(m_targetHighlightPoints);
        tpsTrans->SetBasisToR();
        tpsTrans->Update();
        vtkNew<vtkTransformPolyDataFilter> transform;
        transform->SetInputData(m_meshData);
        transform->SetTransform(tpsTrans);
        transform->Update();
        m_overlaidMesh->DeepCopy(transform->GetOutput());
        m_overlayMapper->SetInputData(m_overlaidMesh);
        m_overlayMapper->Modified();
        m_overlayMeshActor->Modified();
        m_overlayRenderer->GetRenderWindow()->Render();
        if (tarNum > 2) {
            m_status = ALIGNEDSTATUS::Aligned;
        }
    }
}

void Registration::SI() {
    int tarNum = m_targetHighlightPoints->GetNumberOfPoints();
    int tempNum = m_templateHighlightPoints->GetNumberOfPoints();
    if (tarNum == tempNum) {
        vtkNew<vtkTransform> transMatrix;
        Eigen::MatrixXd templateLM;
        templateLM.resize(m_templateHighlightPoints->GetNumberOfPoints(), 3);
        for (int i = 0; i < m_templateHighlightPoints->GetNumberOfPoints();
             i++) {
            templateLM.operator()(i, 0) =
                m_templateHighlightPoints->GetPoint(i)[0];
            templateLM.operator()(i, 1) =
                m_templateHighlightPoints->GetPoint(i)[1];
            templateLM.operator()(i, 2) =
                m_templateHighlightPoints->GetPoint(i)[2];
        }

        Eigen::MatrixXd specimenLM;
        specimenLM.resize(m_targetHighlightPoints->GetNumberOfPoints(), 3);
        for (int i = 0; i < m_targetHighlightPoints->GetNumberOfPoints(); i++) {
            specimenLM.operator()(i, 0) =
                m_targetHighlightPoints->GetPoint(i)[0];
            specimenLM.operator()(i, 1) =
                m_targetHighlightPoints->GetPoint(i)[1];
            specimenLM.operator()(i, 2) =
                m_targetHighlightPoints->GetPoint(i)[2];
        }

        try {
            GetKabschTransformation(templateLM, specimenLM, transMatrix, 1);
            transMatrix->Modified();
        } catch (std::bad_alloc e) {
            std::cout << "Sample is throwing exception" << std::endl;
        }
        vtkNew<vtkTransformPolyDataFilter> transform;
        transform->SetInputData(m_meshData);
        transform->SetTransform(transMatrix);
        transform->Update();
        m_overlaidMesh->DeepCopy(transform->GetOutput());
        m_overlayMapper->SetInputData(m_overlaidMesh);
        m_overlayMapper->Modified();
        m_overlayMeshActor->Modified();
        m_overlayRenderer->GetRenderWindow()->Render();
    }
    if (tarNum == tempNum && tarNum > 2) {
        morphButton->setEnabled(1);
        m_status = ALIGNEDSTATUS::Aligned;
    } else {
        morphButton->setEnabled(0);
        // m_status = ALIGNEDSTATUS::NotAligned;
    }
}

void Registration::GetKabschTransformation(const Eigen::MatrixXd& tmplate,
                                           Eigen::MatrixXd& specimen,
                                           vtkTransform* outTrans, bool scale) {
    // It's important to transpose coordinates for SVD operations
    Eigen::Matrix4d rt =
        Eigen::umeyama(specimen.transpose(), tmplate.transpose(), scale);

    vtkNew<vtkMatrix4x4> tempTrans;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            tempTrans->SetElement(i, j, rt.coeff(i, j));
        }
    }

    vtkNew<vtkTransform> trans;
    trans->SetMatrix(tempTrans);
    trans->Update();

    outTrans->DeepCopy(trans);
}

void Registration::ResetOverlay() {
    m_morphedMesh->Initialize();
    m_preSliderHighlightPoints->Initialize();
    m_overlaidMesh->Initialize();

    m_templateOldIds->Initialize();
    m_templateHighlightPoints->Initialize();
    m_targetOldIds->Initialize();
    m_targetHighlightPoints->Initialize();

    m_templatePointsPoly->Initialize();
    vtkNew<vtkPolyData> tempPoly;
    m_templateVertexFilter->SetInputData(tempPoly);
    m_templateVertexFilter->Update();
    m_templateVertexFilter->Modified();

    m_targetPointsPoly->Initialize();
    m_targetVertexFilter->SetInputData(tempPoly);
    m_targetVertexFilter->Update();
    m_targetVertexFilter->Modified();

    m_targetRenderer->GetRenderWindow()->Render();
    m_templateRenderer->GetRenderWindow()->Render();
    SetOverlayScene();
    m_preSliderHighlightPoints->Modified();
    m_parent->SetSurfaceSlider(m_preSliderHighlightPoints);
}

void Registration::ResetTool() {
    sliderButton->setEnabled(1);
    registerTypeComboBox->setEnabled(1);
    registerTypeComboBox->setCurrentIndex(0);
    morphButton->setEnabled(0);
    Register();
    // refineButton->setEnabled(0);
}

void Registration::closeEvent(QCloseEvent* event) {
    if (m_regThread) {
        if (m_regThread->isRunning()) {
            QMessageBox warning;
            warning.setText("Morphing is in the process, be patient please!");
            warning.exec();
            // m_regThread->wait();
            event->ignore();
        }
        if (!m_regThread->isRunning()) {
            if (m_status == ALIGNEDSTATUS::Aligned &&
                m_preSliderHighlightPoints->GetNumberOfPoints() == 0) {
                auto choice =
                    QMessageBox::question(this, "Warning",
                                          "You have not set the sliders yet, "
                                          "nothing will be registered!"
                                          "\n Do you want to close the window?",
                                          QMessageBox::Yes | QMessageBox::No);
                if (choice == QMessageBox::Yes) {
                    event->ignore();
                    m_meshData->GetPointData()->RemoveArray("OriginalID");
                    m_meshData->Modified();
                    m_meshData->GetCellData()->RemoveArray("Masked");
                    m_meshData->Modified();
                    m_parent->FlipSurfaceButton();
                    event->accept();
                } else {
                    event->ignore();
                }
            } else {
                event->ignore();
                m_meshData->GetPointData()->RemoveArray("OriginalID");
                m_meshData->Modified();
                m_meshData->GetCellData()->RemoveArray("Masked");
                m_meshData->Modified();
                m_parent->FlipSurfaceButton();
                event->accept();
            }
        }
    } else if (m_status == ALIGNEDSTATUS::Aligned &&
               m_preSliderHighlightPoints->GetNumberOfPoints() == 0) {
        auto choice = QMessageBox::question(
            this, "Warning",
            "You have not set the sliders yet, nothing will be registered!"
            "\n Do you want to close the window?",
            QMessageBox::Yes | QMessageBox::No);
        if (choice == QMessageBox::Yes) {
            event->ignore();
            m_meshData->GetPointData()->RemoveArray("OriginalID");
            m_meshData->Modified();
            m_meshData->GetCellData()->RemoveArray("Masked");
            m_meshData->Modified();
            m_parent->FlipSurfaceButton();
            event->accept();
        } else {
            event->ignore();
        }
    } else {
        event->ignore();
        m_meshData->GetPointData()->RemoveArray("OriginalID");
        m_meshData->Modified();
        m_meshData->GetCellData()->RemoveArray("Masked");
        m_meshData->Modified();
        m_parent->FlipSurfaceButton();
        event->accept();
    }
}

bool Registration::IsRunning() {
    bool morphStatus = false;
    if (m_regThread) {
        if (m_regThread->isRunning()) {
            morphStatus = true;
        } else {
            morphStatus = false;
        }
    }
    bool status = false;
    if (morphStatus) {
        status = true;
    }
    return status;
}

void Registration::SetAnchors(vtkPoints* sourceAnchor, vtkPoints* templateAnchor){
    m_templateAnchor->DeepCopy(templateAnchor);
    m_sourceAnchor->DeepCopy(sourceAnchor);
    m_anchorStat = 1;
}

void Registration::SetAccuracy(bool accuracy){
    m_accuracy = accuracy;
}

Registration::~Registration() {
    delete m_templateRenderWidget;
    delete m_overlayRenderWidget;
    delete m_targetRenderWidget;
    delete m_mutex;
}
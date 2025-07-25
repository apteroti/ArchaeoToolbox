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

#include "../include/SuperImposition.h"

#include "../include/MainWindow.h"

SuperImposition::SuperImposition(DataBase *dataBase, MainWindow *parent,
                                 QMutex *mutex)
    : m_dataBase(dataBase), m_parent(parent), m_mutex(mutex) {
    m_numLm = m_dataBase->GetTotalLandmarks("Template")->GetNumberOfPoints();
    this->setWindowTitle("SuperImposition");
    this->resize(800, 500);
    m_vtkRenderWidget = new QVTKOpenGLWidget();
    m_renderer = vtkSmartPointer<vtkRenderer>::New();
    m_renWin = vtkSmartPointer<vtkRenderWindow>::New();
    m_iren = vtkSmartPointer<vtkGenericRenderWindowInteractor>::New();
    m_style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    m_meshActor = vtkSmartPointer<vtkActor>::New();
    m_labelActor = vtkSmartPointer<vtkActor2D>::New();
    m_meshData = vtkSmartPointer<vtkPolyData>::New();
    m_fixLmData = vtkSmartPointer<vtkPoints>::New();
    m_curveLmData = vtkSmartPointer<vtkPoints>::New();
    m_surfaceLmData = vtkSmartPointer<vtkPoints>::New();
    m_totalLmData = vtkSmartPointer<vtkPolyData>::New();
    m_lmIdList = new std::vector<int>;

    m_renWin = m_vtkRenderWidget->GetRenderWindow();
    m_renWin->AddRenderer(m_renderer);
    m_style->SetCurrentRenderer(m_renderer);
    m_iren->SetInteractorStyle(m_style);
    m_iren->SetRenderWindow(m_renWin);
    // Setting up render scene
    vtkNew<vtkNamedColors> colors;
    m_renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
    m_renWin->Render();
    m_iren->Start();

    this->setCentralWidget(m_vtkRenderWidget);

    auto landmarkListDockedWidg = new QDockWidget(tr("Landmarks"));
    addDockWidget(Qt::LeftDockWidgetArea, landmarkListDockedWidg);

    auto nameListDockedWidg = new QDockWidget(tr("Specimens"));
    addDockWidget(Qt::LeftDockWidgetArea, nameListDockedWidg);

    //---------------
    lmList = new QListWidget;
    lmAnchorList = new QListWidget;
    QListWidgetItem *item1 = new QListWidgetItem("Available To Use");
    item1->setFlags(item1->flags() & ~Qt::ItemIsSelectable);
    lmList->addItem(item1);
    QListWidgetItem *item2 = new QListWidgetItem("Already Used");
    item2->setFlags(item2->flags() & ~Qt::ItemIsSelectable);
    lmAnchorList->addItem(item2);

    nameQList = new QListWidget;

    btnAllToSelected = new QToolButton();
    btnAllToSelected->setText(">>");
    btnMoveToSelected = new QToolButton();
    btnMoveToSelected->setText(">");
    btnMoveToAvailable = new QToolButton();
    btnMoveToAvailable->setText("<");
    btnAllToAvailable = new QToolButton();
    btnAllToAvailable->setText("<<");

    QGroupBox *btnGroup = new QGroupBox();
    QVBoxLayout *btnLayout = new QVBoxLayout;
    btnLayout->addItem(
        new QSpacerItem(10, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
    btnLayout->addWidget(btnAllToSelected);
    btnLayout->addWidget(btnMoveToSelected);
    btnLayout->addWidget(btnMoveToAvailable);
    btnLayout->addWidget(btnAllToAvailable);
    btnLayout->addItem(
        new QSpacerItem(10, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
    btnGroup->setLayout(btnLayout);

    QGroupBox *landmarkListGroup = new QGroupBox();
    QHBoxLayout *listLayout = new QHBoxLayout;
    listLayout->addWidget(lmList);
    listLayout->addWidget(btnGroup);
    listLayout->addWidget(lmAnchorList);

    landmarkListGroup->setLayout(listLayout);
    landmarkListDockedWidg->setWidget(landmarkListGroup);
    landmarkListDockedWidg->setMaximumWidth(
        (lmList->sizeHintForColumn(0) + lmList->frameWidth() * 2) * 4);
    nameListDockedWidg->setWidget(nameQList);
    tabifyDockWidget(nameListDockedWidg, landmarkListDockedWidg);
    //---------------
    connect(btnAllToSelected, &QToolButton::clicked, this,
            &SuperImposition::SelectAll);
    connect(btnAllToAvailable, &QToolButton::clicked, this,
            &SuperImposition::DeSelectAll);
    connect(btnMoveToSelected, &QToolButton::clicked, this,
            &SuperImposition::MoveToAnchor);
    connect(btnMoveToAvailable, &QToolButton::clicked, this,
            &SuperImposition::MoveToSource);
    connect(lmAnchorList, &QListWidget::itemClicked, this,
            &SuperImposition::OnAnchorSelected);
    connect(lmList, &QListWidget::itemClicked, this,
            &SuperImposition::OnSourceSelected);

    QToolBar *mainToolbar = this->addToolBar("Main Toolbar");
    mainToolbar->setMovable(false);
    this->addToolBarBreak();
    QLabel *showPtsIdsLabel = new QLabel();
    showPtsIdsLabel->setText(tr("Show Landmark ids"));
    mainToolbar->addWidget(showPtsIdsLabel);
    showPtsIdsBox = new QCheckBox();
    showPtsIdsBox->setChecked(true);
    mainToolbar->addWidget(showPtsIdsBox);
    mainToolbar->addSeparator();
    connect(showPtsIdsBox, &QCheckBox::stateChanged, this,
            &SuperImposition::ShowIds);

    btnSuperImposed = new QPushButton();
    btnSuperImposed->setText("SuperImpose");
    btnSuperImposed->setEnabled(0);
    mainToolbar->addWidget(btnSuperImposed);
    mainToolbar->addSeparator();
    connect(btnSuperImposed, &QPushButton::clicked, this,
            &SuperImposition::MakeImposed);

    QLabel *methodSelectLabel = new QLabel();
    methodSelectLabel->setText(tr("Method"));
    methodSelectComboBox = new QComboBox();
    methodSelectComboBox->addItem("Full");
    methodSelectComboBox->addItem("Partial");
    mainToolbar->addWidget(methodSelectLabel);
    mainToolbar->addWidget(methodSelectComboBox);
    void (QComboBox ::*methodSelectFp)(int) = &QComboBox ::currentIndexChanged;
    connect(methodSelectComboBox, methodSelectFp, this,
            &SuperImposition::ChangeImposeMethod);

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

    Populate();
    Plot();

    this->show();
    if (m_nameList.size() == 0) {
        auto errorDialogue = QMessageBox();
        errorDialogue.setIcon(QMessageBox::Critical);
        errorDialogue.setWindowTitle("Error");
        errorDialogue.setText(
            "No Landmark to SuperImpose: digitize your specimen!");
        errorDialogue.exec();
    }
}

void SuperImposition::ChangeImposeMethod(int index) {
    if (index == 0) {
        m_method = Method::full;
    }
    if (index == 1) {
        m_method = Method::partial;
    }
}

void SuperImposition::SelectAll() {
    lmList->clear();
    QListWidgetItem *item1 = new QListWidgetItem("Available");
    item1->setFlags(item1->flags() & ~Qt::ItemIsSelectable);
    lmList->addItem(item1);
    lmAnchorList->clear();
    QListWidgetItem *item2 = new QListWidgetItem("   Use   ");
    item2->setFlags(item2->flags() & ~Qt::ItemIsSelectable);
    lmAnchorList->addItem(item2);

    for (int i = 0; i < m_numLm; i++) {
        lmAnchorList->addItem(QString::number(i));
    }
    if (m_nameList.size() > 0) {
        btnSuperImposed->setEnabled(1);
    }
}

void SuperImposition::DeSelectAll() {
    lmList->clear();
    QListWidgetItem *item1 = new QListWidgetItem("Available");
    item1->setFlags(item1->flags() & ~Qt::ItemIsSelectable);
    lmList->addItem(item1);
    lmAnchorList->clear();
    QListWidgetItem *item2 = new QListWidgetItem("   Use   ");
    item2->setFlags(item2->flags() & ~Qt::ItemIsSelectable);
    lmAnchorList->addItem(item2);

    for (int i = 0; i < m_numLm; i++) {
        lmList->addItem(QString::number(i));
    }
    btnSuperImposed->setEnabled(0);
}

void SuperImposition::MoveToAnchor() {
    auto items = lmList->selectedItems();
    foreach (QListWidgetItem *item, items) {
        auto temp = lmList->takeItem(lmList->row(item));
        // int indx  = (temp->text().toInt()) + 1;
        // lmAnchorList->insertItem(indx, temp);
        lmAnchorList->addItem(temp);
        // lmAnchorList->sortItems();
    }
    std::vector<int> tempList;
    for (int i = 1; i < lmAnchorList->count(); i++) {
        tempList.push_back(lmAnchorList->item(i)->text().toInt());
    }
    std::sort(tempList.begin(), tempList.end());
    lmAnchorList->clear();
    QListWidgetItem *item2 = new QListWidgetItem("   Use   ");
    item2->setFlags(item2->flags() & ~Qt::ItemIsSelectable);
    lmAnchorList->addItem(item2);
    for (int i = 0; i < tempList.size(); i++) {
        lmAnchorList->addItem(QString::number(tempList[i]));
    }
    if (lmAnchorList->count() > 3 && m_nameList.size() > 0) {
        btnSuperImposed->setEnabled(1);
    }
}

void SuperImposition::MoveToSource() {
    auto items = lmAnchorList->selectedItems();
    foreach (QListWidgetItem *item, items) {
        auto temp = lmAnchorList->takeItem(lmAnchorList->row(item));
        // int indx  = (temp->text().toInt()) + 1;
        // lmList->insertItem(indx, temp);
        lmList->addItem(temp);
        // lmList->sortItems();
    }
    std::vector<int> tempList;
    for (int i = 1; i < lmList->count(); i++) {
        tempList.push_back(lmList->item(i)->text().toInt());
    }
    std::sort(tempList.begin(), tempList.end());
    lmList->clear();
    QListWidgetItem *item1 = new QListWidgetItem("Available");
    item1->setFlags(item1->flags() & ~Qt::ItemIsSelectable);
    lmList->addItem(item1);
    for (int i = 0; i < tempList.size(); i++) {
        lmList->addItem(QString::number(tempList[i]));
    }
    if (lmAnchorList->count() <= 3 || m_nameList.size() == 0) {
        btnSuperImposed->setEnabled(0);
    }
}

void SuperImposition::OnSourceSelected() { lmAnchorList->clearSelection(); }

void SuperImposition::OnAnchorSelected() { lmList->clearSelection(); }

void SuperImposition::Populate() {
    auto tempList = m_dataBase->GetNodeNames();
    for (int i = 0; i < tempList.size(); i++) {
        if (m_dataBase->GetTotalLandmarks(tempList[i])->GetNumberOfPoints() ==
            m_numLm) {
            if (tempList[i] != "Template") {
                m_nameList.push_back(tempList[i]);
                nameQList->addItem(QString::fromUtf8(tempList[i].c_str()));
            }
        }
    }
    for (int i = 0; i < m_numLm; i++) {
        lmAnchorList->addItem(QString::number(i));
    }
    if (m_nameList.size() > 0) {
        btnSuperImposed->setEnabled(1);
    }
    m_mutex->lock();
    m_meshData->DeepCopy(m_dataBase->GetPolyNode("Template"));
    m_fixLmData->DeepCopy(m_dataBase->GetTypeI("Template"));
    m_curveLmData->DeepCopy(m_dataBase->GetCurveSliders("Template"));
    m_surfaceLmData->DeepCopy(m_dataBase->GetSurfaceSliders("Template"));
    m_totalLmData->DeepCopy(m_dataBase->GetTotalLandmarks("Template"));
    m_mutex->unlock();
}

void SuperImposition::Plot() {
    vtkNew<vtkNamedColors> colors;

    // Mesh properties and color etc
    vtkNew<vtkDataSetMapper> mapper;
    mapper->ScalarVisibilityOff();  // <- disables scalar-based coloring
    mapper->SetInputData(m_meshData);
    m_meshActor->SetMapper(mapper);
    m_meshActor->GetProperty()->SetColor(1, 0.992, 0.815);
    m_renderer->AddActor(m_meshActor);
    // Point properties and color etc
    vtkNew<vtkMassProperties> prop;
    prop->SetInputData(m_meshData);
    prop->Update();
    const double area = prop->GetSurfaceArea();
    const double diagonal =
        std::sqrt(area);  // Approximate characteristic length
    // Compute size factor based on application-specific parameters
    // Normalized between 0-1 range first, then scaled

    int typeINOL = m_dataBase->GetNumberOfTypeI("Template");
    int curveNOS = m_dataBase->GetNumberOfCurveSliders("Template");
    int surfaceNOS = m_dataBase->GetNumberOfSurfaceSliders("Template");
    double sizeFactor =
        (typeINOL * 0.03 + curveNOS * 0.025 + surfaceNOS * 0.025);
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

    vtkNew<vtkPolyData> fixPoly;
    fixPoly->SetPoints(m_fixLmData);
    vtkNew<vtkPolyData> curvePoly;
    curvePoly->SetPoints(m_curveLmData);
    vtkNew<vtkPolyData> surfacePoly;
    surfacePoly->SetPoints(m_surfaceLmData);

    vtkNew<vtkGlyph3DMapper> fixLmPointMapper;
    vtkNew<vtkVertexGlyphFilter> fixLmVertexFilter;
    vtkNew<vtkActor> fixPtActor;

    vtkNew<vtkGlyph3DMapper> curveLmPointMapper;
    vtkNew<vtkVertexGlyphFilter> curveLmVertexFilter;
    vtkNew<vtkActor> curvePtActor;

    vtkNew<vtkGlyph3DMapper> surfaceLmPointMapper;
    vtkNew<vtkVertexGlyphFilter> surfaceLmVertexFilter;
    vtkNew<vtkActor> surfacePtActor;

    fixLmVertexFilter->SetInputData(fixPoly);
    fixLmVertexFilter->Update();
    fixLmPointMapper->SetInputData(fixLmVertexFilter->GetOutput());
    fixLmPointMapper->SetSourceConnection(sphereSource->GetOutputPort());
    fixLmPointMapper->ScalingOff();
    fixLmPointMapper->ScalarVisibilityOff();
    fixPtActor->GetProperty()->SetColor(1.0, 0.0, 0.0);
    fixPtActor->GetProperty()->SetDiffuse(0.8);
    fixPtActor->GetProperty()->SetSpecular(0.5);
    fixPtActor->GetProperty()->SetSpecularPower(30);
    fixPtActor->SetMapper(fixLmPointMapper);
    m_renderer->AddActor(fixPtActor);

    curveLmVertexFilter->SetInputData(curvePoly);
    curveLmVertexFilter->Update();
    curveLmPointMapper->SetInputData(curveLmVertexFilter->GetOutput());
    curveLmPointMapper->SetSourceConnection(sphereSource->GetOutputPort());
    curveLmPointMapper->ScalingOff();
    curveLmPointMapper->ScalarVisibilityOff();
    curvePtActor->GetProperty()->SetColor(0.0, 1.0, 0.0);
    curvePtActor->GetProperty()->SetDiffuse(0.8);
    curvePtActor->GetProperty()->SetSpecular(0.5);
    curvePtActor->GetProperty()->SetSpecularPower(30);
    curvePtActor->SetMapper(curveLmPointMapper);
    m_renderer->AddActor(curvePtActor);

    surfaceLmVertexFilter->SetInputData(surfacePoly);
    surfaceLmVertexFilter->Update();
    surfaceLmPointMapper->SetInputData(surfaceLmVertexFilter->GetOutput());
    surfaceLmPointMapper->SetSourceConnection(sphereSource->GetOutputPort());
    surfaceLmPointMapper->ScalingOff();
    surfaceLmPointMapper->ScalarVisibilityOff();
    surfacePtActor->GetProperty()->SetColor(0.0, 0.0, 1.0);
    surfacePtActor->GetProperty()->SetDiffuse(0.8);
    surfacePtActor->GetProperty()->SetSpecular(0.5);
    surfacePtActor->GetProperty()->SetSpecularPower(30);
    surfacePtActor->SetMapper(surfaceLmPointMapper);
    m_renderer->AddActor(surfacePtActor);

    // Label properties
    vtkNew<vtkVertexGlyphFilter> lmVertexFilter;
    lmVertexFilter->SetInputData(m_totalLmData);
    lmVertexFilter->Update();
    vtkNew<vtkLabeledDataMapper> labelMapper;
    labelMapper->SetInputData(lmVertexFilter->GetOutput());
    labelMapper->GetLabelTextProperty()->SetFontSize(10);
    m_labelActor->SetMapper(labelMapper);
    m_labelActor->GetProperty()->SetColor(
        colors->GetColor3d("Black").GetData());
    m_labelActor->SetPickable(0);
    m_renderer->AddActor(m_labelActor);

    m_renderer->ResetCamera();
    m_renderer->GetRenderWindow()->Render();
}

void SuperImposition::ShowIds() {
    if (!showPtsIdsBox->isChecked()) {
        m_renderer->RemoveActor2D(m_labelActor);
        m_renderer->GetRenderWindow()->Render();
    } else {
        m_renderer->AddActor(m_labelActor);
        m_renderer->GetRenderWindow()->Render();
    }
}

void SuperImposition::MakeImposed() {
    auto choice = QMessageBox::question(
        this, "Reminder",
        "It is reccomended to save the project before superimposition.\n Do "
        "you want to proceed with the superimposition?",
        QMessageBox::Yes | QMessageBox::No);
    if (choice == QMessageBox::Yes) {
        if (m_nameList.size() > 0 && lmAnchorList->count() > 3) {
            btnAllToSelected->setEnabled(0);
            btnAllToAvailable->setEnabled(0);
            btnMoveToSelected->setEnabled(0);
            btnMoveToAvailable->setEnabled(0);
            btnSuperImposed->setEnabled(0);
            methodSelectComboBox->setEnabled(0);
            showPtsIdsBox->setChecked(0);
            ShowIds();
            showPtsIdsBox->setEnabled(0);

            progressLineEdit->show();
            progressLineEdit->setText("Prepare SuperImposition...");

            for (int i = 1; i < lmAnchorList->count(); i++) {
                m_lmIdList->push_back(lmAnchorList->item(i)->text().toInt());
            }
            progressLineEdit->setText("Superimposition in Progress...");
            qRegisterMetaType<std::string>("std::string");
            if (m_method == Method::full) {
                delete m_impositionThread;
                m_impositionThread = new SuperImpositionThread(
                    m_dataBase, m_nameList, m_lmIdList, 0, m_mutex);
            } else if (m_method == Method::partial) {
                delete m_impositionThread;
                m_impositionThread = new SuperImpositionThread(
                    m_dataBase, m_nameList, m_lmIdList, 1, m_mutex);
            }

            connect(m_impositionThread,
                    &SuperImpositionThread::CoordinateChanged, this,
                    &SuperImposition::OnCoordinateChanged);
            connect(m_impositionThread,
                    &SuperImpositionThread::CoordinateNotChanged, this,
                    &SuperImposition::OnCoordinateNotChanged);
            /* connect(m_impositionThread,
                    &SuperImpositionThread::Done, this,
                    &SuperImposition::OnProcessIsDone); */

            m_parent->RunStatThread(m_impositionThread);
            m_parent->ResetImposition();
            m_impositionThread->setParent(this);
            ImposingStatus();
            m_impositionThread->start();

        } else {
            std::cout << "SuperImposing Error" << std::endl;
        }
    }
}

void SuperImposition::ImposingStatus() {
    if (m_impositionThread) {
        delete m_impositionStatThread;
        m_impositionStatThread = new StatusReporterThread(m_impositionThread);
        connect(m_impositionStatThread, &StatusReporterThread::StatusChanged,
                this, &SuperImposition::OnStatusChanged);
        m_impositionStatThread->setParent(this);
        m_impositionStatThread->start();
    }
}

void SuperImposition::OnCoordinateChanged(std::string name) {
    m_parent->OnSuperImposed(name);
    std::string msg = name + " was SuperImposed...";
    progressLineEdit->setText(QString::fromUtf8(msg.c_str()));
}
void SuperImposition::OnCoordinateNotChanged(std::string name) {
    std::cout << name << std::endl;
    std::string errMsg =
        "Specimen " + name +
        " is throwing an exception. \n the superimposition process is Aborted";
    QMessageBox warning;
    warning.setText(QString::fromUtf8(errMsg.c_str()));
    warning.exec();
}

/* void SuperImposition::OnProcessIsDone(){
    m_parent->OnSuperImpositionIsDone();
} */

void SuperImposition::OnStatusChanged(int status) {
    if (status != 0) {
        statusLabel->setText("Status: superpositioning");
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
        progressLineEdit->hide();
    }
}

void SuperImposition::closeEvent(QCloseEvent *event) {
    event->ignore();
    if (m_impositionStatThread) {
        if (m_impositionStatThread->isRunning()) {
            QMessageBox warning;
            warning.setText(
                "SuperImposition is in the process, be patient please!");
            warning.exec();
            // m_regThread->wait();
            event->ignore();
        } else {
            m_parent->FinaliseImposition();
            m_parent->OnSuperImpositionIsDone();
            event->accept();
        }
    } else {
        m_parent->FinaliseImposition();
        m_parent->OnSuperImpositionIsDone();
        event->accept();
    }
}

SuperImposition::~SuperImposition() { delete m_lmIdList; }
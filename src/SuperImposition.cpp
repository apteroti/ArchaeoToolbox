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

SuperImposition::SuperImposition(DataBase *dataBase, MainWindow *parent, QMutex* mutex)
    : m_dataBase(dataBase), m_parent(parent),m_mutex(mutex) {
    m_numLm = m_dataBase->GetTotalLandmarks("Template")->GetNumberOfPoints();
    this->setWindowTitle("SuperImposition");
    this->resize(800, 500);
    m_vtkRenderWidget = new QVTKOpenGLWidget();
    m_renderer = vtkSmartPointer<vtkRenderer>::New();
    m_renWin = vtkSmartPointer<vtkRenderWindow>::New();
    m_iren = vtkSmartPointer<vtkGenericRenderWindowInteractor>::New();
    m_style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    m_meshActor = vtkSmartPointer<vtkActor>::New();
    m_lmPointActor = vtkSmartPointer<vtkActor>::New();
    m_anchorActor = vtkSmartPointer<vtkActor>::New();
    m_labelActor = vtkSmartPointer<vtkActor2D>::New();
    m_meshData = vtkSmartPointer<vtkPolyData>::New();
    m_anchorData = vtkSmartPointer<vtkPoints>::New();
    m_lmData = vtkSmartPointer<vtkPolyData>::New();
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
    QListWidgetItem *item1 = new QListWidgetItem("Available");
    item1->setFlags(item1->flags() & ~Qt::ItemIsSelectable);
    lmList->addItem(item1);
    QListWidgetItem *item2 = new QListWidgetItem("   Use   ");
    item2->setFlags(item2->flags() & ~Qt::ItemIsSelectable);
    lmAnchorList->addItem(item2);

    nameQList = new QListWidget;

    // lmInputList->setMaximumWidth(lmInputList->sizeHintForColumn(0) +
    // lmInputList->frameWidth() * 2);
    // lmOutputList->setMaximumWidth(lmOutputList->sizeHintForColumn(0) +
    // lmOutputList->frameWidth() * 2);

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
        errorDialogue.setIcon(QMessageBox::Information);
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
    m_lmData->DeepCopy(m_dataBase->GetTotalLandmarks("Template"));
    m_mutex->unlock();
}

void SuperImposition::Plot() {
    vtkNew<vtkNamedColors> colors;

    // Mesh properties and color etc
    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetInputData(m_meshData);
    m_meshActor->SetMapper(mapper);
    m_meshActor->GetProperty()->SetColor(1, 0.992, 0.815);
    m_renderer->AddActor(m_meshActor);
    // Point properties and color etc
    vtkNew<vtkMassProperties> prop;
    prop->SetInputData(m_meshData);
    prop->Update();
    double area = prop->GetSurfaceArea();
    area = std::sqrt(area);

    double sizeConstant = m_lmData->GetNumberOfPoints();
    if (sizeConstant < 100) {
        sizeConstant = 100.0;
    }
    if (sizeConstant > 300) {
        sizeConstant = 300.0;
    }
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetRadius(area / (sizeConstant));
    vtkNew<vtkSphereSource> sphereSource2;
    sphereSource2->SetRadius((area / sizeConstant) * 1.5);

    vtkNew<vtkGlyph3DMapper> lmPointMapper;
    vtkNew<vtkVertexGlyphFilter> lmVertexFilter;
    lmVertexFilter->SetInputData(m_lmData);
    lmVertexFilter->Update();
    lmPointMapper->SetInputData(lmVertexFilter->GetOutput());
    lmPointMapper->SetSourceConnection(sphereSource2->GetOutputPort());
    lmPointMapper->ScalingOff();
    lmPointMapper->ScalarVisibilityOff();
    m_lmPointActor->GetProperty()->SetColor(0.0, 0.0, 1.0);
    m_lmPointActor->GetProperty()->SetDiffuse(0.8);
    m_lmPointActor->GetProperty()->SetSpecular(0.5);
    m_lmPointActor->GetProperty()->SetSpecularPower(30);
    m_lmPointActor->SetMapper(lmPointMapper);
    m_renderer->AddActor(m_lmPointActor);

    // Label properties
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

SuperImposition::~SuperImposition() {
    delete m_lmIdList;
}
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

#include "include/ExclusionPaint.h"

ExclusionPaint::ExclusionPaint(vtkPolyData *data) : m_meshData(data) {
    this->setWindowTitle("Exclusion Paint");
    this->resize(600, 500);
    m_maskArray = vtkSmartPointer<vtkIntArray>::New();
    m_maskArray->SetName("Masked");
    m_maskArray->SetNumberOfValues(m_meshData->GetNumberOfCells());
    for (int i = 0; i < m_meshData->GetNumberOfCells(); i++) {
        m_maskArray->SetValue(i, 0);
    }
    m_maskArray->Modified();
    m_meshData->GetCellData()->RemoveArray("Masked");
    m_meshData->Modified();
    m_meshData->GetCellData()->AddArray(m_maskArray);

    //-----------------------------------------------------
    m_vtkRenderWidget = new QVTKOpenGLWidget();
    m_renWin = vtkSmartPointer<vtkRenderWindow>::New();
    m_renderer = vtkSmartPointer<vtkRenderer>::New();
    m_iren = vtkSmartPointer<vtkGenericRenderWindowInteractor>::New();
    m_PointPainterStyle =
        vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();

    m_2Dstyle = vtkSmartPointer<vtkInteractorStyleImage>::New();
    m_meshActor = vtkSmartPointer<vtkActor>::New();

    this->setCentralWidget(m_vtkRenderWidget);
    mainToolbar = this->addToolBar("Main Toolbar");
    mainToolbar->setMovable(false);
    this->addToolBarBreak();

    surfacePaintButton = new QPushButton();
    surfacePaintButton->setToolTip("Paint exclusion masks");
    surfacePaintButton->setIcon(QIcon(":/icons/graphics/icons/brush.svg"));
    surfacePaintButton->setCheckable(true);
    surfacePaintButton->setEnabled(1);
    mainToolbar->addWidget(surfacePaintButton);
    mainToolbar->addSeparator();
    connect(surfacePaintButton, &QPushButton::clicked, this,
            &ExclusionPaint::BrushTool);
    // Create opacity effect
    QGraphicsOpacityEffect *effectForPainting =
        new QGraphicsOpacityEffect(surfacePaintButton);
    surfacePaintButton->setGraphicsEffect(effectForPainting);
    // Create animation
    m_painterAnimation = new QPropertyAnimation(effectForPainting, "opacity");
    m_painterAnimation->setDuration(1000);   // 1 second cycle
    m_painterAnimation->setStartValue(1.0);  // Fully visible
    m_painterAnimation->setEndValue(0.2);    // Almost transparent
    m_painterAnimation->setEasingCurve(QEasingCurve::InOutQuad);
    m_painterAnimation->setLoopCount(-1);  // Infinite loop
    m_painterAnimation->start();

    QLabel *brushSizeLabel = new QLabel();
    brushSizeLabel->setText(tr("Brush Size"));
    mainToolbar->addWidget(brushSizeLabel);
    brushSizeSpinBox = new QSpinBox();
    brushSizeSpinBox->setRange(0, 10);
    brushSizeSpinBox->setValue(1);
    mainToolbar->addWidget(brushSizeSpinBox);
    mainToolbar->addSeparator();
    void (QSpinBox ::*changeBrushSizeFp)(int) = &QSpinBox ::valueChanged;
    connect(brushSizeSpinBox, changeBrushSizeFp, this,
            &ExclusionPaint::ChangeBrushSize);

    closeButton = new QPushButton("Done");
    closeButton->setToolTip("Close and Proceed With Digitisation");
    closeButton->setCheckable(false);
    mainToolbar->addWidget(closeButton);

    connect(closeButton, &QPushButton::clicked, this,
            &ExclusionPaint::HandleCloseButton);

    Plot();
    this->hide();
}

void ExclusionPaint::Plot() {
    // Setting up render scene
    vtkNew<vtkNamedColors> colors;
    m_renWin = m_vtkRenderWidget->GetRenderWindow();
    m_renWin->AddRenderer(m_renderer);
    // m_style->SetCurrentRenderer(m_renderer);
    m_PointPainterStyle->SetCurrentRenderer(m_renderer);
    m_2Dstyle->SetCurrentRenderer(m_renderer);
    // m_iren->SetInteractorStyle(m_style);
    m_iren->SetInteractorStyle(m_PointPainterStyle);
    m_iren->SetRenderWindow(m_renWin);

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

    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetInputData(m_meshData);
    mapper->SetResolveCoincidentTopologyToOff();
    mapper->SetScalarModeToUseCellFieldData();
    mapper->SelectColorArray("Masked");
    mapper->SetLookupTable(lut);
    m_meshActor->SetMapper(mapper);
    // m_meshActor->GetProperty()->SetColor(1, 0.992, 0.815);
    m_meshActor->GetProperty()->SetOpacity(1);
    m_renderer->AddActor(m_meshActor);

    // Ambient properties
    m_renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
    m_renderer->ResetCamera();

    // Observers
    m_PointPainterStyle->AddObserver(vtkCommand::MouseMoveEvent, this,
                                     &ExclusionPaint::PaintFunc, 1);
    m_PointPainterStyle->AddObserver(vtkCommand::LeftButtonPressEvent, this,
                                     &ExclusionPaint::PaintMouseClicked, 1);
    m_PointPainterStyle->AddObserver(vtkCommand::LeftButtonReleaseEvent, this,
                                     &ExclusionPaint::PaintMouseReleased, 1);

    QShortcut *selectAllShortcut =
        new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_A), this);
    QObject::connect(selectAllShortcut, &QShortcut::activated, this,
                     &ExclusionPaint::SelectAll);
    QShortcut *deSelectAllShortcut =
        new QShortcut(QKeySequence(Qt::SHIFT + Qt::Key_A), this);
    QObject::connect(deSelectAllShortcut, &QShortcut::activated, this,
                     &ExclusionPaint::DeSelectAll);

    m_renWin->Render();
    m_iren->Start();
}

void ExclusionPaint::BrushTool() {
    if (surfacePaintButton->isChecked()) {
        QGraphicsOpacityEffect *effect = qobject_cast<QGraphicsOpacityEffect *>(
            surfacePaintButton->graphicsEffect());
        if (effect) {
            effect->setOpacity(1.0);
        }
        m_painterAnimation->stop();
    } else {
        m_painterAnimation->start();
    }
}

void ExclusionPaint::ChangeBrushSize(int index) {
    if (m_meshData->GetNumberOfCells() > index) {
        m_brushSize = index;
    } else {
        brushSizeSpinBox->setValue(1);
        m_brushSize = 1;
    }
}

void ExclusionPaint::PaintMouseClicked() {
    m_mouseIsClicked = 1;
    m_PointPainterStyle->OnLeftButtonDown();
}

void ExclusionPaint::PaintMouseReleased() {
    m_mouseIsClicked = 0;
    m_PointPainterStyle->OnLeftButtonUp();
}

void ExclusionPaint::DeSelectAll() {
    if (surfacePaintButton->isChecked()) {
        for (int i = 0; i < m_meshData->GetNumberOfCells(); i++) {
            m_maskArray->SetValue(i, 0);
        }
        m_maskArray->Modified();
        m_meshData->GetCellData()->RemoveArray("Masked");
        m_meshData->Modified();
        m_meshData->GetCellData()->AddArray(m_maskArray);
        m_meshData->Modified();
        m_meshActor->Modified();
        m_renderer->GetRenderWindow()->Render();
    }
}

void ExclusionPaint::SelectAll() {
    if (surfacePaintButton->isChecked()) {
        for (int i = 0; i < m_meshData->GetNumberOfCells(); i++) {
            m_maskArray->SetValue(i, 1);
        }
        m_maskArray->Modified();
        m_meshData->GetCellData()->RemoveArray("Masked");
        m_meshData->Modified();
        m_meshData->GetCellData()->AddArray(m_maskArray);
        m_meshData->Modified();
        m_meshActor->Modified();
        m_renderer->GetRenderWindow()->Render();
    }
}

void ExclusionPaint::PaintFunc(vtkObject *caller, long unsigned int eventId,
                               void *callData) {
    if (m_iren->GetControlKey() && m_mouseIsClicked) {
        if (surfacePaintButton->isChecked()) {
            m_iren->SetInteractorStyle(m_2Dstyle);
            m_iren->Modified();
            m_meshActor->SetPickable(1);
            m_meshActor->Modified();

            auto clickPos = m_iren->GetEventPosition();
            vtkNew<vtkCellPicker> picker;
            picker->Pick(clickPos[0], clickPos[1], 0, m_renderer);
            if (picker->GetCellId() != -1) {
                std::vector<int> Outlist;
                NeighborFinder(picker->GetCellId(), m_brushSize, Outlist);
                for (int ids : Outlist) {
                    m_maskArray->SetValue(ids, 1);
                }
                m_maskArray->Modified();
                m_meshData->GetCellData()->RemoveArray("Masked");
                m_meshData->Modified();
                m_meshData->GetCellData()->AddArray(m_maskArray);
                m_meshData->Modified();
                m_meshActor->Modified();
                m_renderer->GetRenderWindow()->Render();
            }
        }
    }
    if (m_iren->GetShiftKey() && m_mouseIsClicked) {
        if (surfacePaintButton->isChecked()) {
            m_iren->SetInteractorStyle(m_2Dstyle);
            m_iren->Modified();
            m_meshActor->SetPickable(1);
            m_meshActor->Modified();

            auto clickPos = m_iren->GetEventPosition();
            vtkNew<vtkCellPicker> picker;
            picker->Pick(clickPos[0], clickPos[1], 0, m_renderer);
            if (picker->GetCellId() != -1) {
                std::vector<int> Outlist;
                NeighborFinder(picker->GetCellId(), m_brushSize, Outlist);
                for (int ids : Outlist) {
                    m_maskArray->SetValue(ids, 0);
                }
                m_maskArray->Modified();
                m_meshData->GetCellData()->RemoveArray("Masked");
                m_meshData->Modified();
                m_meshData->GetCellData()->AddArray(m_maskArray);
                m_meshData->Modified();
                m_meshActor->Modified();
                m_renderer->GetRenderWindow()->Render();
            }
        }
    }

    if (m_iren->GetInteractorStyle()->GetClassName() ==
        m_PointPainterStyle->GetClassName()) {
        m_PointPainterStyle->OnMouseMove();
    } else if (m_iren->GetInteractorStyle()->GetClassName() ==
               m_2Dstyle->GetClassName()) {
        m_iren->SetInteractorStyle(m_PointPainterStyle);
    }
}

void ExclusionPaint::NeighborFinder(int initId, int brushSize,
                                    std::vector<int> &outList) {
    outList.clear();
    vtkNew<vtkIdList> list;
    vtkNew<vtkIdList> tempList;
    list->InsertNextId(initId);
    while (brushSize > 0) {
        for (int i = 0; i < list->GetNumberOfIds(); i++) {
            if (list->GetId(i) < m_meshData->GetNumberOfCells()) {
                vtkNew<vtkIdList> cellPointIds;
                m_meshData->GetCellPoints(list->GetId(i), cellPointIds);
                for (int j = 0; j < cellPointIds->GetNumberOfIds(); j++) {
                    vtkNew<vtkIdList> tempPtsList;
                    tempPtsList->InsertNextId(cellPointIds->GetId(j));
                    vtkNew<vtkIdList> neighborCellIds;
                    m_meshData->GetCellNeighbors(list->GetId(i), tempPtsList,
                                                 neighborCellIds);
                    for (int k = 0; k < neighborCellIds->GetNumberOfIds();
                         k++) {
                        tempList->InsertUniqueId(neighborCellIds->GetId(k));
                    }
                }
            } else {
                std::cerr << "Cell Id is out of bound, Debug!" << std::endl;
            }
        }
        for (int i = 0; i < tempList->GetNumberOfIds(); i++) {
            list->InsertUniqueId(tempList->GetId(i));
        }
        brushSize -= 1;
    }
    for (int i = 0; i < list->GetNumberOfIds(); i++) {
        if (list->GetId(i) < m_meshData->GetNumberOfCells()) {
            outList.push_back(list->GetId(i));
        }
    }
}

void ExclusionPaint::HandleCloseButton() {
    m_forceClose = true;
    emit windowClosed();
    this->close();
}

void ExclusionPaint::closeEvent(QCloseEvent *event) {
    if (!m_forceClose) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, tr("Confirm Close"),
            tr("Close the window and proceed with digitalisation?"),
            QMessageBox::Yes | QMessageBox::No);

        if (reply != QMessageBox::Yes) {
            event->ignore();
            return;
        }

        emit windowClosed();  // emit only on user confirmation
    }

    QMainWindow::closeEvent(event);
}

ExclusionPaint::~ExclusionPaint() {}
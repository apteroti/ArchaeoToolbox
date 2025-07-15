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

#include "../include/TemplateDigitiser.h"

#include "../include/ProSetMenu.h"

TemplateDigitiser::TemplateDigitiser(ProSetMenu* parent) : m_parent(parent) {
    this->setWindowTitle("Template");
    this->resize(600, 500);
    m_mutex = new QMutex();
    m_maskArray = vtkSmartPointer<vtkIntArray>::New();
    m_cutMeshData = vtkSmartPointer<vtkPolyData>::New();
    m_curveType = new std::vector<int>;
    m_vtkRenderWidget = new QVTKOpenGLWidget();
    m_renWin = vtkSmartPointer<vtkRenderWindow>::New();
    m_renderer = vtkSmartPointer<vtkRenderer>::New();
    m_iren = vtkSmartPointer<vtkGenericRenderWindowInteractor>::New();
    m_style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    m_PointPickerStyle =
        vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    m_PointPainterStyle =
        vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    m_PointMoverStyle =
        vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    m_2Dstyle = vtkSmartPointer<vtkInteractorStyleImage>::New();
    m_meshActor = vtkSmartPointer<vtkActor>::New();
    m_fixedPointActor = vtkSmartPointer<vtkActor>::New();
    m_fixedPointsPoly = vtkSmartPointer<vtkPolyData>::New();
    m_fixedVertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    m_fixedHighlightPoints = vtkSmartPointer<vtkPoints>::New();
    m_fixedPtsIds = vtkSmartPointer<vtkIntArray>::New();
    m_surfacePtsIds = new std::vector<std::vector<int>*>;
    m_surfacePtsIds->push_back(new std::vector<int>);
    m_grabPicker = vtkSmartPointer<vtkPointPicker>::New();
    m_cosmeticCurvePoly = vtkSmartPointer<vtkPolyData>::New();
    m_largestDiamTubeActor = vtkSmartPointer<vtkActor>::New();
    m_largestDiamTubeActor->SetPickable(0);
    //-----------------------------------------------------------------------
    m_cutMeshActor = vtkSmartPointer<vtkActor>::New();
    m_surfaceCurveCtrlPointActor = vtkSmartPointer<vtkActor>::New();
    m_surfaceCurveCtrlPointsPoly = vtkSmartPointer<vtkPolyData>::New();
    m_surfaceCtrlEdgesPoly = vtkSmartPointer<vtkPolyData>::New();
    m_surfaceCurveCtrlVertexFilter =
        vtkSmartPointer<vtkVertexGlyphFilter>::New();
    m_surfaceCurveHighlightCtrlPoints = vtkSmartPointer<vtkPoints>::New();
    m_surfaceCtrlPointActor = vtkSmartPointer<vtkActor>::New();
    m_surfaceCtrlPointsPoly = vtkSmartPointer<vtkPolyData>::New();
    m_surfaceCtrlVertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    m_surfaceTubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
    m_surfacePointActor = vtkSmartPointer<vtkActor>::New();
    m_surfaceEdgeActor = vtkSmartPointer<vtkActor>::New();
    m_surfacePointsPoly = vtkSmartPointer<vtkPolyData>::New();
    m_surfaceVertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    m_surfaceHighlightPoints = vtkSmartPointer<vtkPoints>::New();
    m_surfaceCurveActor = vtkSmartPointer<vtkActor>::New();
    m_surfaceCurvePoly = vtkSmartPointer<vtkPolyData>::New();
    m_surfaceCurveTubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
    m_surfaceMask = vtkSmartPointer<vtkPolyData>::New();

    m_surfacePatchHighlightPoints = vtkSmartPointer<vtkPoints>::New();
    m_surfacePatchVertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    m_surfacePatchPointActor = vtkSmartPointer<vtkActor>::New();

    m_surfacePatchVertexDeactiveFilter =
        vtkSmartPointer<vtkVertexGlyphFilter>::New();
    m_surfacePatchPointDeactiveActor = vtkSmartPointer<vtkActor>::New();
    m_surfaceCurveVertexDeactiveFilter =
        vtkSmartPointer<vtkVertexGlyphFilter>::New();
    m_surfaceCurveCtrlPtsDeactiveActor = vtkSmartPointer<vtkActor>::New();
    m_surfaceSliderPtsTotal = vtkSmartPointer<vtkPoints>::New();
    m_surfaceLabelPoints = vtkSmartPointer<vtkPoints>::New();
    m_surfaceLabelVertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    m_surfaceCurveTubeFilterDeactive = vtkSmartPointer<vtkTubeFilter>::New();
    m_surfaceCurveActorDeactive = vtkSmartPointer<vtkActor>::New();

    m_surfaceArrowActor = vtkSmartPointer<vtkActor>::New();
    m_glyphSurfaceArrow = vtkSmartPointer<vtkGlyph3D>::New();

    m_surfaceBlock = vtkSmartPointer<vtkMultiBlockDataSet>::New();
    m_surfaceCtrlPtsBlock = vtkSmartPointer<vtkMultiBlockDataSet>::New();
    m_surfaceCurveCtrlBlock = vtkSmartPointer<vtkMultiBlockDataSet>::New();
    m_surfaceCurveBlock = vtkSmartPointer<vtkMultiBlockDataSet>::New();
    m_surfaceMaskBlock = vtkSmartPointer<vtkMultiBlockDataSet>::New();
    //-------------------------------------------------------------------------
    m_curveVertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    m_curvectrlPointActor = vtkSmartPointer<vtkActor>::New();
    m_curvectrlPointsPoly = vtkSmartPointer<vtkPolyData>::New();
    m_curvectrlVertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    m_curveHighlightCtrlPoints = vtkSmartPointer<vtkPoints>::New();
    m_curveHighlightPoints = vtkSmartPointer<vtkPoints>::New();
    m_curveHighlightPointsTotal = vtkSmartPointer<vtkPoints>::New();
    m_curvePoly = vtkSmartPointer<vtkPolyData>::New();
    m_curveTubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
    m_curveActor = vtkSmartPointer<vtkActor>::New();
    m_curvePointActor = vtkSmartPointer<vtkActor>::New();
    m_curvePointsPoly = vtkSmartPointer<vtkPolyData>::New();
    m_curveLabelVertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    m_curveLabelPoints = vtkSmartPointer<vtkPoints>::New();
    m_curvePointActorDeactive = vtkSmartPointer<vtkActor>::New();
    m_curveVertexFilterDeactive = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    m_curveTubeFilterDeactive = vtkSmartPointer<vtkTubeFilter>::New();
    m_curveActorDeactive = vtkSmartPointer<vtkActor>::New();
    // m_curveLMColorsSeries = vtkSmartPointer<vtkColorSeries>::New();
    m_curveArrowActor = vtkSmartPointer<vtkActor>::New();
    m_glyphCurveArrow = vtkSmartPointer<vtkGlyph3D>::New();

    m_curveCtrlBlock = vtkSmartPointer<vtkMultiBlockDataSet>::New();
    m_curveBlock = vtkSmartPointer<vtkMultiBlockDataSet>::New();
    m_curvePolyLineBlock = vtkSmartPointer<vtkMultiBlockDataSet>::New();
    // m_curveAnchorIdList = new std::vector<int>;
    m_meshBoundActor = vtkSmartPointer<vtkActor>::New();
    m_meshBoundActor->SetPickable(0);
    m_meshBoundaries = vtkSmartPointer<vtkPolyData>::New();
    //-------------------------------------------------------------------------

    QString style =
        "QGroupBox {"
        "font: bold;"
        "border: 1px solid silver;"
        "border-radius: 5px;"
        "margin-top: 1ex;"
        "}"
        "QGroupBox::title {"
        "subcontrol-origin: margin;"
        "left: 7px;"
        "padding: 0px 5px 0px 5px;"
        "}"
        "QProgressBar::chunk{"
        "background-color: #2196F3;"
        "width: 10px;"
        "margin: 0.5px;"
        "}"
        "QProgressBar{"
        "border: 2px solid #2196F3;"
        "border-radius: 5px;"
        "background-color: #E0E0E0;"
        "}";

    this->setCentralWidget(m_vtkRenderWidget);
    mainToolbar = this->addToolBar("Main Toolbar");
    mainToolbar->setMovable(false);
    this->addToolBarBreak();

    landmarkToolbar = this->addToolBar("Landmark Toolbar");
    landmarkToolbar->setVisible(false);

    surfacePatchToolbar = this->addToolBar("Surface Toolbar");
    surfacePatchToolbar->setVisible(false);

    surfaceToolbar = this->addToolBar("Surface Toolbar");
    surfaceToolbar->setVisible(false);

    curveToolbar = this->addToolBar("Curve Toolbar");
    curveToolbar->setVisible(false);

    counterLayout = new QHBoxLayout();
    counterGroup = new QGroupBox(tr("Counter"));
    this->statusBar()->addWidget(counterGroup, 0);

    statusLabel = new QLabel(this);
    statusLabel->setText("Status: Idle");

    progressLabel = new QLabel(this);
    progressLabel->setPixmap(QPixmap(":/icons/graphics/icons/idle.svg"));
    /* statusProgressBar = new QProgressBar(this);
    statusProgressBar->setTextVisible(true);
    statusProgressBar->setFormat("Idle...");
    statusProgressBar->setAlignment(Qt::AlignCenter);
    statusProgressBar->setStyleSheet(style); */

    this->statusBar()->addPermanentWidget(statusLabel, 0);
    this->statusBar()->addPermanentWidget(progressLabel, 0);
    // this->statusBar()->addPermanentWidget(statusProgressBar);
    //-------------------------------------------------------------------------
    landmarkButton = new QPushButton();
    landmarkButton->setToolTip("Digitize Fixed Landmarks");
    landmarkButton->setIcon(QIcon(":/icons/graphics/icons/landmarks.svg"));
    landmarkButton->setCheckable(true);
    mainToolbar->addWidget(landmarkButton);
    connect(landmarkButton, &QPushButton::clicked, this,
            &TemplateDigitiser::TypeITool);

    QLabel* showLargestDiamLabel = new QLabel();
    showLargestDiamLabel->setText(tr("Show Largest Diameter"));
    landmarkToolbar->addWidget(showLargestDiamLabel);
    showDiameterBox = new QCheckBox();
    showDiameterBox->setChecked(false);
    landmarkToolbar->addWidget(showDiameterBox);
    landmarkToolbar->addSeparator();
    connect(showDiameterBox, &QCheckBox::stateChanged, this,
            &TemplateDigitiser::ShowLargestDiameter);
    //--------------------------------------------------------------------------
    curveSliderButton = new QPushButton();
    curveSliderButton->setToolTip("Digitize Curve Sliders");
    curveSliderButton->setIcon(
        QIcon(":/icons/graphics/icons/curveLandmarks.svg"));
    curveSliderButton->setCheckable(true);
    mainToolbar->addWidget(curveSliderButton);
    connect(curveSliderButton, &QPushButton::clicked, this,
            &TemplateDigitiser::CurveTool);

    curveAddButton = new QPushButton();
    curveAddButton->setToolTip("Add a Curve");
    curveAddButton->setIcon(QIcon(":/icons/graphics/icons/plus.png"));
    curveAddButton->setCheckable(false);
    curveAddButton->setEnabled(0);
    curveToolbar->addWidget(curveAddButton);
    curveToolbar->addSeparator();
    connect(curveAddButton, &QPushButton::clicked, this,
            &TemplateDigitiser::AddCurve);
    curveTypeButton = new QPushButton();
    curveTypeButton->setToolTip("Change Curve Type");
    curveTypeButton->setIcon(QIcon(":/icons/graphics/icons/closed_curve.svg"));
    curveTypeButton->setCheckable(true);
    curveToolbar->addWidget(curveTypeButton);
    curveToolbar->addSeparator();
    connect(curveTypeButton, &QPushButton::clicked, this,
            &TemplateDigitiser::ChangeCurveType);

    QLabel* curveSelectLabel = new QLabel();
    // curveSelectLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    curveSelectLabel->setText(tr("Curve Index"));
    curveToolbar->addWidget(curveSelectLabel);
    curveSelectComboBox = new QComboBox();
    curveToolbar->addWidget(curveSelectComboBox);
    curveToolbar->addSeparator();
    void (QComboBox ::*curveSelectFp)(int) = &QComboBox ::currentIndexChanged;
    connect(curveSelectComboBox, curveSelectFp, this,
            &TemplateDigitiser::ChangeCurve);

    QLabel* curvePickLabel = new QLabel();
    curvePickLabel->setText(tr("From Surface"));
    curveToolbar->addWidget(curvePickLabel);
    curvePickSourceComboBox = new QComboBox();
    curvePickSourceComboBox->addItem(QString::fromStdString("Index"));
    curveToolbar->addWidget(curvePickSourceComboBox);
    curveToolbar->addSeparator();
    void (QComboBox ::*curveSourceFp)(int) = &QComboBox ::currentIndexChanged;
    connect(curvePickSourceComboBox, curveSourceFp, this,
            &TemplateDigitiser::ChangeCurveSource);

    QLabel* curvePickFromLooseLabel = new QLabel();
    curvePickFromLooseLabel->setText(tr("Limit to Loose Booundaries"));
    curveToolbar->addWidget(curvePickFromLooseLabel);
    pickFromBoundariesBox = new QCheckBox();
    pickFromBoundariesBox->setChecked(false);
    curveToolbar->addWidget(pickFromBoundariesBox);
    connect(pickFromBoundariesBox, &QCheckBox::stateChanged, this,
            &TemplateDigitiser::PickFromBoundaries);

    curveToolbar->addSeparator();

    curveClearButton = new QPushButton();
    curveClearButton->setCheckable(false);
    curveClearButton->setText("Clear All Curves");
    // curveClearButton->setEnabled(0);
    curveToolbar->addWidget(curveClearButton);
    curveToolbar->addSeparator();
    connect(curveClearButton, &QPushButton::clicked, this,
            &TemplateDigitiser::ClearCurve);

    //-------------------------------------------------------------------------
    surfaceSliderButton = new QPushButton();
    surfaceSliderButton->setToolTip("Digitize Surface Sliders");
    surfaceSliderButton->setIcon(
        QIcon(":/icons/graphics/icons/surfaceLandmarks.svg"));
    surfaceSliderButton->setCheckable(true);
    mainToolbar->addWidget(surfaceSliderButton);
    connect(surfaceSliderButton, &QPushButton::clicked, this,
            &TemplateDigitiser::SurfaceTool);
    surfacePaintButton = new QPushButton();
    surfacePaintButton->setToolTip("Paint exclusion masks");
    surfacePaintButton->setIcon(QIcon(":/icons/graphics/icons/brush.svg"));
    surfacePaintButton->setCheckable(true);
    surfacePaintButton->setEnabled(1);
    surfaceToolbar->addWidget(surfacePaintButton);
    surfaceToolbar->addSeparator();
    connect(surfacePaintButton, &QPushButton::clicked, this,
            &TemplateDigitiser::BrushTool);
    QLabel* brushSizeLabel = new QLabel();
    brushSizeLabel->setText(tr("Brush Size"));
    surfaceToolbar->addWidget(brushSizeLabel);
    brushSizeSpinBox = new QSpinBox();
    brushSizeSpinBox->setRange(0, 10);
    brushSizeSpinBox->setValue(1);
    surfaceToolbar->addWidget(brushSizeSpinBox);
    surfaceToolbar->addSeparator();
    void (QSpinBox ::*changeBrushSizeFp)(int) = &QSpinBox ::valueChanged;
    connect(brushSizeSpinBox, changeBrushSizeFp, this,
            &TemplateDigitiser::ChangeBrushSize);

    surfaceResampleButton = new QPushButton();
    surfaceResampleButton->setToolTip("Sample Pre-Sliders");
    surfaceResampleButton->setIcon(
        QIcon(":/icons/graphics/icons/resampling.svg"));
    surfaceResampleButton->setCheckable(false);
    surfaceResampleButton->setEnabled(1);
    surfaceToolbar->addWidget(surfaceResampleButton);
    surfaceToolbar->addSeparator();
    connect(surfaceResampleButton, &QPushButton::clicked, this, [this] {
        Resample(m_surfaceNOS, m_meshData);
    });  // give a functor to connect
    QLabel* ignoreIntLabel = new QLabel();
    ignoreIntLabel->setText(tr("Single Mesh"));
    surfaceToolbar->addWidget(ignoreIntLabel);
    ignoreIntCheckbox = new QCheckBox();
    ignoreIntCheckbox->setChecked(1);
    surfaceToolbar->addWidget(ignoreIntCheckbox);
    surfaceToolbar->addSeparator();
    connect(ignoreIntCheckbox, &QCheckBox::stateChanged, this,
            &TemplateDigitiser::IgnoreInternalStat);

    surfaceAddButton = new QPushButton();
    surfaceAddButton->setToolTip("Add a Surface");
    surfaceAddButton->setIcon(QIcon(":/icons/graphics/icons/plus.png"));
    surfaceAddButton->setCheckable(false);
    surfaceAddButton->setEnabled(0);
    surfacePatchToolbar->addWidget(surfaceAddButton);
    surfacePatchToolbar->addSeparator();
    connect(surfaceAddButton, &QPushButton::clicked, this,
            &TemplateDigitiser::AddSurface);

    surfaceLockButton = new QPushButton();
    surfaceLockButton->setToolTip("Lock surface composition");
    surfaceLockButton->setIcon(QIcon(":/icons/graphics/icons/unlocked.svg"));
    surfaceLockButton->setCheckable(true);
    surfacePatchToolbar->addWidget(surfaceLockButton);
    surfacePatchToolbar->addSeparator();
    connect(surfaceLockButton, &QPushButton::clicked, this,
            &TemplateDigitiser::ChangeSurfaceLock);

    surfaceIronButton = new QPushButton();
    surfaceIronButton->setCheckable(true);
    surfaceIronButton->setIcon(QIcon(":/icons/graphics/icons/ironOff.svg"));
    surfaceIronButton->setToolTip("Iron the Nurbs");
    surfaceIronButton->setEnabled(1);
    surfacePatchToolbar->addWidget(surfaceIronButton);
    surfacePatchToolbar->addSeparator();
    connect(surfaceIronButton, &QPushButton::clicked, this,
            &TemplateDigitiser::DrapeToSurface);

    QLabel* surfaceSelectLabel = new QLabel();
    surfaceSelectLabel->setText(tr("Surface Index"));
    surfacePatchToolbar->addWidget(surfaceSelectLabel);
    surfaceSelectComboBox = new QComboBox();
    surfaceSelectComboBox->addItem(
        QString::fromStdString(std::to_string(m_currentSurfaceId)));
    surfacePatchToolbar->addWidget(surfaceSelectComboBox);
    surfacePatchToolbar->addSeparator();
    void (QComboBox ::*surfaceSelectFp)(int) = &QComboBox ::currentIndexChanged;
    connect(surfaceSelectComboBox, surfaceSelectFp, this,
            &TemplateDigitiser::ChangeSurface);

    QLabel* surfacePickLabel = new QLabel();
    surfacePickLabel->setText(tr("From Curve"));
    surfacePatchToolbar->addWidget(surfacePickLabel);
    surfacePickSourceComboBox = new QComboBox();
    surfacePickSourceComboBox->addItem(QString::fromStdString("Index"));
    surfacePatchToolbar->addWidget(surfacePickSourceComboBox);
    surfacePatchToolbar->addSeparator();
    void (QComboBox ::*surfaceSourceFp)(int) = &QComboBox ::currentIndexChanged;
    connect(surfacePickSourceComboBox, surfaceSourceFp, this,
            &TemplateDigitiser::ChangeSurfaceSource);

    QLabel* cyclePatchesLable = new QLabel();
    cyclePatchesLable->setText(tr("Cycle Patches"));
    surfacePatchToolbar->addWidget(cyclePatchesLable);
    cyclePatchesComboBox = new QComboBox();
    // cyclePatchesComboBox->addItem(QString::fromStdString(std::to_string(0)));
    surfacePatchToolbar->addWidget(cyclePatchesComboBox);
    surfacePatchToolbar->addSeparator();
    void (QComboBox ::*cyclePatchesFp)(int) = &QComboBox ::currentIndexChanged;
    connect(cyclePatchesComboBox, cyclePatchesFp, this,
            &TemplateDigitiser::CycleThroughPatches);

    surfaceClearButton = new QPushButton();
    surfaceClearButton->setCheckable(false);
    surfaceClearButton->setText("Clear All Surfaces");
    surfacePatchToolbar->addWidget(surfaceClearButton);
    surfacePatchToolbar->addSeparator();
    connect(surfaceClearButton, &QPushButton::clicked, this,
            &TemplateDigitiser::ClearSurface);

    //-------------------------------------------------------------------------
    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainToolbar->addWidget(spacer);

    QLabel* pointSizeLabel = new QLabel();
    pointSizeLabel->setText(tr("Scale Point"));
    mainToolbar->addWidget(pointSizeLabel);

    pointSizeSpinBox = new QSpinBox();
    pointSizeSpinBox->setRange(-5, 5);
    pointSizeSpinBox->setValue(0);
    mainToolbar->addWidget(pointSizeSpinBox);

    QLabel* lineSizeLabel = new QLabel();
    lineSizeLabel->setText(tr("Scale Curve"));
    mainToolbar->addWidget(lineSizeLabel);
    lineSizeSpinBox = new QSpinBox();
    lineSizeSpinBox->setRange(-5, 5);
    lineSizeSpinBox->setValue(0);
    mainToolbar->addWidget(lineSizeSpinBox);

    void (QSpinBox ::*changePtSizeFp)(int) = &QSpinBox ::valueChanged;
    connect(pointSizeSpinBox, changePtSizeFp, this,
            &TemplateDigitiser::ChangePointSize);

    void (QSpinBox ::*changeLineSizeFp)(int) = &QSpinBox ::valueChanged;
    connect(lineSizeSpinBox, changeLineSizeFp, this,
            &TemplateDigitiser::ChangeLineSize);

    //---------------------------------------------

    counterGroup->setStyleSheet(style);

    counterFixedLabel = new QLabel(tr(u8"\u0025 Type I:"));
    counterCurveNOCLabel = new QLabel(tr(u8"\u0025 Curves:"));
    counterSurfaceNOPLabel = new QLabel(tr(u8"\u0025 Surface Patches:"));
    typeILineEdit = new QLineEdit();
    typeILineEdit->setReadOnly(true);
    curveLineEditNOC = new QLineEdit();
    curveLineEditNOC->setReadOnly(true);
    surfaceLineEditNOP = new QLineEdit();
    surfaceLineEditNOP->setReadOnly(true);

    counterLayout->addWidget(counterFixedLabel);
    counterLayout->addWidget(typeILineEdit);
    counterLayout->addWidget(counterCurveNOCLabel);
    counterLayout->addWidget(curveLineEditNOC);
    counterLayout->addWidget(counterSurfaceNOPLabel);
    counterLayout->addWidget(surfaceLineEditNOP);
    counterLayout->addStretch(1);
    counterGroup->setLayout(counterLayout);
    counterGroup->setContentsMargins(3, 3, 3, 1);
}

/* void TemplateDigitiser::PickCurveAnchor() {
    if (curveAnchorButton->isChecked()) {
        curveAnchorButton->setIcon(
            QIcon(":/icons/graphics/icons/anchorOn.svg"));
        // here needs something?;
    }
    if (!curveAnchorButton->isChecked()) {
        curveAnchorButton->setIcon(
            QIcon(":/icons/graphics/icons/anchorOff.svg"));
        curveLockButton->setChecked(1);
        ChangeCurveLock();
    }
} */

void TemplateDigitiser::IgnoreInternalStat() {
    if (ignoreIntCheckbox->isChecked()) {
        m_ignoreInside = true;

    } else if (!ignoreIntCheckbox->isChecked()) {
        m_ignoreInside = false;
    }
}

void TemplateDigitiser::SetPoly(vtkPolyData* poly) {
    m_meshData = poly;
    m_maskArray->SetName("Masked");
    m_maskArray->SetNumberOfValues(m_meshData->GetNumberOfCells());
    for (int i = 0; i < m_meshData->GetNumberOfCells(); i++) {
        m_maskArray->SetValue(i, 0);
    }
    m_maskArray->Modified();
    m_meshData->GetCellData()->RemoveArray("Masked");
    m_meshData->Modified();
    m_meshData->GetCellData()->AddArray(m_maskArray);

    m_typeINOL = m_parent->GetTypeINOL();
    m_surfaceNOS = m_parent->GetSurfaceNOS();
    std::tie(m_surfacePatchUNOS, m_surfacePatchVNOS) =
        m_parent->GetSurfacePatchResolution();
    m_surfacePatchNOP = m_parent->GetSurfacePatchNOP();
    m_curveNOS = m_parent->GetCurveNOS();
    m_curveNOC = m_parent->GetCurveNOC();
    for (int i = 0; i < m_curveNOC; i++) {
        m_curveType->push_back(1);
    }

    if (m_typeINOL == 0) {
        landmarkButton->setEnabled(0);
        counterLayout->removeWidget(counterFixedLabel);
        counterLayout->removeWidget(typeILineEdit);
        counterFixedLabel->hide();
        typeILineEdit->hide();
    }
    if (m_curveNOS == 0) {
        curveSliderButton->setEnabled(0);
        counterLayout->removeWidget(counterCurveNOCLabel);
        counterLayout->removeWidget(curveLineEditNOC);
        counterCurveNOCLabel->hide();
        curveLineEditNOC->hide();
    }
    if (m_surfaceNOS == 0 &&
        (m_surfacePatchUNOS == 0 || m_surfacePatchVNOS == 0)) {
        surfaceSliderButton->setEnabled(0);
    }
    if (m_surfacePatchUNOS == 0 || m_surfacePatchVNOS == 0) {
        counterLayout->removeWidget(counterSurfaceNOPLabel);
        counterLayout->removeWidget(surfaceLineEditNOP);
        counterSurfaceNOPLabel->hide();
        surfaceLineEditNOP->hide();
    }
    if (m_curveNOS != 0) {
        curveSliderButton->setEnabled(1);
        curveSelectComboBox->addItem(
            QString::fromStdString(std::to_string(m_currentCurveId)));
        curveLineEditNOC->setText(QString::fromStdString(std::to_string(
            m_curveNOC - m_curveCtrlBlock->GetNumberOfBlocks())));
    }
    if (m_typeINOL != 0) {
        landmarkButton->setEnabled(1);
        typeILineEdit->setText(
            QString::fromStdString(std::to_string(m_typeINOL)));
    }
    if (m_surfaceNOS != 0 ||
        (m_surfacePatchUNOS != 0 && m_surfacePatchVNOS != 0)) {
        surfaceSliderButton->setEnabled(1);
        if (m_surfaceNOS == 0) {
            surfaceLineEditNOP->setText(
                QString::fromStdString(std::to_string(m_surfacePatchNOP)));
        }
    }
    Plot();
}

void TemplateDigitiser::ResetCurveScene() {
    m_curveHighlightCtrlPoints->Initialize();
    m_curvectrlPointsPoly->Initialize();
    m_curvectrlVertexFilter->Update();
    m_curvectrlVertexFilter->Modified();

    m_curveHighlightPoints->Initialize();
    m_curvePointsPoly->Initialize();
    // m_curveAnchorIdList->clear();
    if (m_curveBlock->GetNumberOfBlocks() > 0) {
        m_curveVertexFilter->Update();
        m_curveVertexFilter->Modified();
    }
    m_curvePoly->Initialize();
    m_curveTubeFilter->SetInputData(m_curvePoly);
    m_curveTubeFilter->Update();
    m_curveTubeFilter->Modified();
    // m_curveActor->SetVisibility(0);
    m_renderer->GetRenderWindow()->Render();
}

void TemplateDigitiser::ResetSurfaceScene() {
    m_surfaceCurveHighlightCtrlPoints->Initialize();
    m_surfaceCurveCtrlPointsPoly->Initialize();
    m_surfaceCurveCtrlVertexFilter->Update();
    m_surfaceCurveCtrlVertexFilter->Modified();
    m_surfaceCtrlPointsPoly->Initialize();
    m_surfaceCtrlVertexFilter->Update();
    m_surfaceCtrlVertexFilter->Modified();
    m_surfaceCurvePoly->Initialize();
    m_surfacePatchHighlightPoints->Initialize();
    vtkNew<vtkPolyData> emptyPoly;
    m_surfaceTubeFilter->SetInputData(emptyPoly);
    m_surfaceTubeFilter->Update();
    m_surfaceTubeFilter->Modified();

    m_surfaceCurveTubeFilter->SetInputData(emptyPoly);
    m_surfaceCurveTubeFilter->Update();
    m_surfaceCurveTubeFilter->Modified();

    m_surfacePatchVertexFilter->SetInputData(emptyPoly);
    m_surfacePatchVertexFilter->Update();
    m_surfacePatchVertexFilter->Modified();

    cyclePatchesComboBox->blockSignals(true);
    cyclePatchesComboBox->clear();
    cyclePatchesComboBox->blockSignals(false);
    m_cutMeshData->Initialize();
    // m_dynamicRes =0;
    m_surfaceMask->Initialize();
    m_renderer->GetRenderWindow()->Render();
}

void TemplateDigitiser::AddCurve() {
    curveAddButton->setEnabled(0);
    // curveAnchorButton->setEnabled(0);
    if (m_curveCtrlBlock->GetNumberOfBlocks() < m_curveNOC) {
        curvePickSourceComboBox->setEnabled(1);
        m_currentCurveId = m_curveCtrlBlock->GetNumberOfBlocks() - 1;
        m_currentCurveId += 1;
        curveSelectComboBox->addItem(
            QString::fromStdString(std::to_string(m_currentCurveId)));
        curveSelectComboBox->setCurrentIndex(m_currentCurveId);
        /* curveAnchorButton->setChecked(0);
        PickCurveAnchor();
        curveLockButton->setChecked(0);
        ChangeCurveLock(); */
        m_curvePoly->Initialize();
        m_curveTubeFilter->SetInputData(m_curvePoly);
        m_curveTubeFilter->Update();
        m_curveTubeFilter->Modified();
        m_renderer->GetRenderWindow()->Render();
    }
}

void TemplateDigitiser::AddSurface() {
    surfaceAddButton->setEnabled(0);
    if (m_surfaceCurveCtrlBlock->GetNumberOfBlocks() < m_surfacePatchNOP) {
        surfacePickSourceComboBox->setEnabled(1);
        m_surfacePtsIds->push_back(new std::vector<int>);
        surfaceIronButton->setEnabled(1);
        surfaceIronButton->setChecked(0);
        DrapeToSurface();
        surfaceLockButton->setChecked(0);
        ChangeSurfaceLock();
        m_currentSurfaceId = m_surfaceCurveCtrlBlock->GetNumberOfBlocks() - 1;
        m_currentSurfaceId += 1;

        surfaceSelectComboBox->addItem(
            QString::fromStdString(std::to_string(m_currentSurfaceId)));
        surfaceSelectComboBox->setCurrentIndex(m_currentSurfaceId);
    }
}

void TemplateDigitiser::ChangeCurve(int index) {
    if (m_curveCtrlBlock->GetNumberOfBlocks() > 0) {
        m_currentCurveId = index;
        ResetCurveScene();
        UpdateCurveScene(m_currentCurveId);
        if (m_curveType->at(m_currentCurveId) == 1) {
            curveTypeButton->setChecked(0);
            ChangeCurveTypeAuto();
        } else if (m_curveType->at(m_currentCurveId) == 0) {
            curveTypeButton->setChecked(1);
            ChangeCurveTypeAuto();
        }
    }
}

void TemplateDigitiser::ChangeCurveSource(
    int index) {  // for getting curve from surface outline
    std::string textNum = curvePickSourceComboBox->currentText().toStdString();
    int num = -1;
    try {
        num = stoi(textNum);
    } catch (const std::exception& e) {
        num = -1;
    }
    if (num >= 0 && num < m_surfaceCurveCtrlBlock->GetNumberOfBlocks()) {
        // ResetCurveScene();
        auto tempSurfaceCtrlPts =
            dynamic_cast<vtkPolyData*>(m_surfaceCurveCtrlBlock->GetBlock(num));
        m_curveHighlightCtrlPoints->DeepCopy(tempSurfaceCtrlPts->GetPoints());
        m_curveHighlightCtrlPoints->Modified();
        m_curvectrlPointsPoly->SetPoints(m_curveHighlightCtrlPoints);
        vtkNew<vtkPolyData> tempCtrlPoly;
        tempCtrlPoly->DeepCopy(m_curvectrlPointsPoly);
        m_curveCtrlBlock->SetBlock(m_currentCurveId, tempCtrlPoly);
        m_curveCtrlBlock->Modified();
        m_curvectrlVertexFilter->SetInputData(m_curvectrlPointsPoly);
        m_curvectrlVertexFilter->Update();
        m_curvectrlVertexFilter->Modified();
        if (m_curveHighlightCtrlPoints->GetNumberOfPoints() > 2) {
            // curveAnchorButton->setEnabled(1);
            UpdateCurveData(m_curveHighlightCtrlPoints, m_curvePoly,
                            m_curveHighlightPoints, m_meshData);
            m_curvePoly->Modified();
            m_curveTubeFilter->SetInputData(m_curvePoly);
            m_curveTubeFilter->Update();
            m_curveTubeFilter->Modified();
            m_curvePointsPoly->SetPoints(m_curveHighlightPoints);
            // UpdateAnchors(m_curvePointsPoly, m_curveAnchorIdList, 1, 1);
            vtkNew<vtkPolyData> tempCurvePtPoly;
            tempCurvePtPoly->DeepCopy(m_curvePointsPoly);
            m_curveBlock->SetBlock(m_currentCurveId, tempCurvePtPoly);
            m_curveBlock->Modified();
            m_curveVertexFilter->SetInputData(m_curvePointsPoly);
            m_curveVertexFilter->Update();
            m_curveVertexFilter->Modified();
            vtkNew<vtkPolyData> tempCurvePolyLine;
            tempCurvePolyLine->DeepCopy(m_curvePoly);
            m_curvePolyLineBlock->SetBlock(m_currentCurveId, tempCurvePolyLine);
            m_curvePolyLineBlock->Modified();
            vtkNew<vtkPolyData> curveArrowPoly;
            MakeArrow(m_meshData, m_curvePolyLineBlock, 3, curveArrowPoly);
            m_glyphCurveArrow->SetInputData(curveArrowPoly);
            m_glyphCurveArrow->Update();
        }
        m_renderer->GetRenderWindow()->Render();
        if (m_currentCurveId == m_curveCtrlBlock->GetNumberOfBlocks() - 1) {
            curveLineEditNOC->setText(QString::fromStdString(std::to_string(
                m_curveNOC - m_curveCtrlBlock->GetNumberOfBlocks())));
            if (m_curveCtrlBlock->GetNumberOfBlocks() < m_curveNOC) {
                curveAddButton->setEnabled(1);
            }
            surfacePickSourceComboBox->addItem(
                QString::number(m_currentCurveId));
        }
    }
}

void TemplateDigitiser::ChangeSurfaceSource(int index) {
    std::string textNum =
        surfacePickSourceComboBox->currentText().toStdString();
    int num = -1;
    try {
        num = stoi(textNum);
    } catch (const std::exception& e) {
        num = -1;
    }
    if (num >= 0 && num < m_curveCtrlBlock->GetNumberOfBlocks()) {
        auto tempCurveCtrlPts =
            dynamic_cast<vtkPolyData*>(m_curveCtrlBlock->GetBlock(num));
        m_surfaceCurveHighlightCtrlPoints->DeepCopy(
            tempCurveCtrlPts->GetPoints());
        m_surfaceCurveHighlightCtrlPoints->Modified();
        m_surfaceCurveCtrlPointsPoly->SetPoints(
            m_surfaceCurveHighlightCtrlPoints);
        m_surfaceCurveCtrlVertexFilter->SetInputData(
            m_surfaceCurveCtrlPointsPoly);
        m_surfaceCurveCtrlVertexFilter->Update();
        m_surfaceCurveCtrlVertexFilter->Modified();
        if (m_surfaceCurveHighlightCtrlPoints->GetNumberOfPoints() > 2) {
            surfaceIronButton->setEnabled(1);
            vtkNew<vtkPolyData> tempCtrlPoly;
            tempCtrlPoly->DeepCopy(m_surfaceCurveCtrlPointsPoly);
            m_surfaceCurveCtrlBlock->SetBlock(m_currentSurfaceId, tempCtrlPoly);
            m_surfaceCurveCtrlBlock->Modified();
            ConstructSurfaceData(m_surfaceCurveHighlightCtrlPoints,
                                 m_surfacePatchHighlightPoints,
                                 m_surfaceCtrlPointsPoly, m_surfaceCurvePoly,
                                 m_nurbsResolution);
            m_surfaceCurvePoly->Modified();
            if (m_surfaceCtrlPointsPoly->GetNumberOfPoints() > 0) {
                m_surfaceCtrlVertexFilter->SetInputData(
                    m_surfaceCtrlPointsPoly);
                m_surfaceCtrlVertexFilter->Update();
                m_surfaceCtrlVertexFilter->Modified();
                vtkNew<vtkExtractEdges> edgeExtracted;
                edgeExtracted->SetInputData(m_surfaceCtrlPointsPoly);
                edgeExtracted->Update();
                m_surfaceTubeFilter->SetInputData(edgeExtracted->GetOutput());
                m_surfaceTubeFilter->Update();
                m_surfaceTubeFilter->Modified();
                vtkNew<vtkPolyData> tempSurfaceCtrlPoly;
                tempSurfaceCtrlPoly->DeepCopy(m_surfaceCtrlPointsPoly);
                m_surfaceCtrlPtsBlock->SetBlock(m_currentSurfaceId,
                                                tempSurfaceCtrlPoly);
                m_surfaceCurveTubeFilter->SetInputData(m_surfaceCurvePoly);
                m_surfaceCurveTubeFilter->Update();
                m_surfaceCurveTubeFilter->Modified();
                vtkNew<vtkPolyData> tempcurvePolyLine;
                tempcurvePolyLine->DeepCopy(m_surfaceCurvePoly);
                m_surfaceCurveBlock->SetBlock(m_currentSurfaceId,
                                              tempcurvePolyLine);
                vtkNew<vtkPolyData> tempSlidersPoly;
                tempSlidersPoly->SetPoints(m_surfacePatchHighlightPoints);
                m_surfacePatchVertexFilter->SetInputData(tempSlidersPoly);
                m_surfacePatchVertexFilter->Update();
                m_surfacePatchVertexFilter->Modified();

                vtkNew<vtkPolyData> tempSlidersPolyCopy;
                tempSlidersPolyCopy->DeepCopy(tempSlidersPoly);

                m_surfaceBlock->SetBlock(m_currentSurfaceId,
                                         tempSlidersPolyCopy);
                m_surfaceBlock->Modified();

                vtkNew<vtkPolyData> tempMask;
                tempMask->DeepCopy(m_surfaceMask);
                m_surfaceMaskBlock->SetBlock(m_currentSurfaceId, tempMask);
                m_surfaceMaskBlock->Modified();

                vtkNew<vtkPolyData> surfaceArrowPoly;
                MakeArrow(m_meshData, m_surfaceCurveBlock, 4, surfaceArrowPoly);
                m_glyphSurfaceArrow->SetInputData(surfaceArrowPoly);
                m_glyphSurfaceArrow->Update();
            }
        }
        m_renderer->GetRenderWindow()->Render();
        if (m_currentSurfaceId ==
            m_surfaceCurveCtrlBlock->GetNumberOfBlocks() - 1) {
            int numOfPatches =
                m_surfacePatchNOP - (m_surfaceBlock->GetNumberOfBlocks());
            surfaceLineEditNOP->setText(
                QString::fromStdString(std::to_string(numOfPatches)));
            if (m_surfaceCurveCtrlBlock->GetNumberOfBlocks() <
                m_surfacePatchNOP) {
                surfaceAddButton->setEnabled(1);
            }
            curvePickSourceComboBox->addItem(
                QString::number(m_currentSurfaceId));
        }
    }
}

void TemplateDigitiser::ChangeSurface(int index) {
    if (m_surfaceCurveCtrlBlock->GetNumberOfBlocks() > 0) {
        m_currentSurfaceId = index;
        ResetSurfaceScene();
        UpdateSurfaceScene(m_currentSurfaceId);
    }
}

void TemplateDigitiser::ChangeSurfaceLock() {
    if (surfaceLockButton->isChecked()) {
        m_editableSurface = 0;
        surfaceLockButton->setIcon(QIcon(":/icons/graphics/icons/locked.svg"));
    } else {
        m_editableSurface = 1;
        surfaceLockButton->setIcon(
            QIcon(":/icons/graphics/icons/unlocked.svg"));
    }
}

/* void TemplateDigitiser::ChangeCurveLock() {
    if (curveLockButton->isChecked()) {
        m_editableCurve = 0;
        curveLockButton->setIcon(QIcon(":/icons/graphics/icons/locked.svg"));
    } else {
        m_editableCurve = 1;
        curveLockButton->setIcon(QIcon(":/icons/graphics/icons/unlocked.svg"));
    }
} */

/* void TemplateDigitiser::ChangeCurveType() {
    if (m_curveAnchored == 1) {
        if (QMessageBox::Yes ==
            QMessageBox::question(this, "Warning",
                                  "Changing Curve Type Will Earase All "
                                  "Anchors, Do you want to continue?",
                                  QMessageBox::Yes | QMessageBox::No)) {
            UpdateAnchors(m_curvePointsPoly, m_curveAnchorIdList, 1, 0);
            curveLockButton->setChecked(0);
            ChangeCurveLock();
            if (curveTypeButton->isChecked()) {
                curveTypeButton->setIcon(
                    QIcon(":/icons/graphics/icons/open_curve.svg"));
                m_curveType->at(m_currentCurveId) = 0;
                if (m_curveHighlightCtrlPoints->GetNumberOfPoints() > 2) {
                    if (pickFromBoundariesBox->isChecked()) {
                        UpdateCurveData(m_curveHighlightCtrlPoints, m_curvePoly,
                                        m_curveHighlightPoints,
                                        m_meshBoundaries);
                    } else {
                        UpdateCurveData(m_curveHighlightCtrlPoints, m_curvePoly,
                                        m_curveHighlightPoints, m_meshData);
                    }
                    m_curveTubeFilter->SetInputData(m_curvePoly);
                    m_curveTubeFilter->Update();
                    m_curveTubeFilter->Modified();
                    m_curvePointsPoly->SetPoints(m_curveHighlightPoints);
                    //UpdateAnchors(m_curvePointsPoly, m_curveAnchorIdList, 1,
0); m_curveVertexFilter->SetInputData(m_curvePointsPoly);
                    m_curveVertexFilter->Update();
                    m_curveVertexFilter->Modified();
                    vtkNew<vtkPolyData> tempCurvePolyLine;
                    tempCurvePolyLine->DeepCopy(m_curvePoly);
                    m_curvePolyLineBlock->SetBlock(m_currentCurveId,
                                                   tempCurvePolyLine);
                    m_curvePolyLineBlock->Modified();
                    vtkNew<vtkPolyData> tempCurvePtPoly;
                    tempCurvePtPoly->DeepCopy(m_curvePointsPoly);
                    m_curveBlock->SetBlock(m_currentCurveId, tempCurvePtPoly);
                    m_curveBlock->Modified();
                    m_renderer->GetRenderWindow()->Render();
                }
            } else {
                curveTypeButton->setIcon(
                    QIcon(":/icons/graphics/icons/closed_curve.svg"));
                m_curveType->at(m_currentCurveId) = 1;
                if (m_curveHighlightCtrlPoints->GetNumberOfPoints() > 2) {
                    if (pickFromBoundariesBox->isChecked()) {
                        UpdateCurveData(m_curveHighlightCtrlPoints, m_curvePoly,
                                        m_curveHighlightPoints,
                                        m_meshBoundaries);
                    } else {
                        UpdateCurveData(m_curveHighlightCtrlPoints, m_curvePoly,
                                        m_curveHighlightPoints, m_meshData);
                    }
                    m_curveTubeFilter->SetInputData(m_curvePoly);
                    m_curveTubeFilter->Update();
                    m_curveTubeFilter->Modified();
                    m_curvePointsPoly->SetPoints(m_curveHighlightPoints);
                    //UpdateAnchors(m_curvePointsPoly, m_curveAnchorIdList, 1,
0); m_curveVertexFilter->SetInputData(m_curvePointsPoly);
                    m_curveVertexFilter->Update();
                    m_curveVertexFilter->Modified();
                    vtkNew<vtkPolyData> tempCurvePolyLine;
                    tempCurvePolyLine->DeepCopy(m_curvePoly);
                    m_curvePolyLineBlock->SetBlock(m_currentCurveId,
                                                   tempCurvePolyLine);
                    m_curvePolyLineBlock->Modified();
                    vtkNew<vtkPolyData> tempCurvePtPoly;
                    tempCurvePtPoly->DeepCopy(m_curvePointsPoly);
                    m_curveBlock->SetBlock(m_currentCurveId, tempCurvePtPoly);
                    m_curveBlock->Modified();
                    m_renderer->GetRenderWindow()->Render();
                }
            }
        } else {
            if (curveTypeButton->isChecked()) {
                curveTypeButton->setChecked(0);
            } else if (!curveTypeButton->isChecked()) {
                curveTypeButton->setChecked(1);
            }
        }
    } else {
        if (curveTypeButton->isChecked()) {
            curveTypeButton->setIcon(
                QIcon(":/icons/graphics/icons/open_curve.svg"));
            m_curveType->at(m_currentCurveId) = 0;
            if (m_curveHighlightCtrlPoints->GetNumberOfPoints() > 2) {
                if (pickFromBoundariesBox->isChecked()) {
                    UpdateCurveData(m_curveHighlightCtrlPoints, m_curvePoly,
                                    m_curveHighlightPoints, m_meshBoundaries);
                } else {
                    UpdateCurveData(m_curveHighlightCtrlPoints, m_curvePoly,
                                    m_curveHighlightPoints, m_meshData);
                }
                m_curveTubeFilter->SetInputData(m_curvePoly);
                m_curveTubeFilter->Update();
                m_curveTubeFilter->Modified();
                m_curvePointsPoly->SetPoints(m_curveHighlightPoints);
                //UpdateAnchors(m_curvePointsPoly, m_curveAnchorIdList, 1, 0);
                curveLockButton->setChecked(0);
                ChangeCurveLock();
                m_curveVertexFilter->SetInputData(m_curvePointsPoly);
                m_curveVertexFilter->Update();
                m_curveVertexFilter->Modified();
                vtkNew<vtkPolyData> tempCurvePolyLine;
                tempCurvePolyLine->DeepCopy(m_curvePoly);
                m_curvePolyLineBlock->SetBlock(m_currentCurveId,
                                               tempCurvePolyLine);
                m_curvePolyLineBlock->Modified();
                vtkNew<vtkPolyData> tempCurvePtPoly;
                tempCurvePtPoly->DeepCopy(m_curvePointsPoly);
                m_curveBlock->SetBlock(m_currentCurveId, tempCurvePtPoly);
                m_curveBlock->Modified();
                m_renderer->GetRenderWindow()->Render();
            }
        } else {
            curveTypeButton->setIcon(
                QIcon(":/icons/graphics/icons/closed_curve.svg"));
            m_curveType->at(m_currentCurveId) = 1;
            if (m_curveHighlightCtrlPoints->GetNumberOfPoints() > 2) {
                if (pickFromBoundariesBox->isChecked()) {
                    UpdateCurveData(m_curveHighlightCtrlPoints, m_curvePoly,
                                    m_curveHighlightPoints, m_meshBoundaries);
                } else {
                    UpdateCurveData(m_curveHighlightCtrlPoints, m_curvePoly,
                                    m_curveHighlightPoints, m_meshData);
                }
                m_curveTubeFilter->SetInputData(m_curvePoly);
                m_curveTubeFilter->Update();
                m_curveTubeFilter->Modified();
                m_curvePointsPoly->SetPoints(m_curveHighlightPoints);
                //UpdateAnchors(m_curvePointsPoly, m_curveAnchorIdList, 1, 0);
                curveLockButton->setChecked(0);
                ChangeCurveLock();
                m_curveVertexFilter->SetInputData(m_curvePointsPoly);
                m_curveVertexFilter->Update();
                m_curveVertexFilter->Modified();
                vtkNew<vtkPolyData> tempCurvePolyLine;
                tempCurvePolyLine->DeepCopy(m_curvePoly);
                m_curvePolyLineBlock->SetBlock(m_currentCurveId,
                                               tempCurvePolyLine);
                m_curvePolyLineBlock->Modified();
                vtkNew<vtkPolyData> tempCurvePtPoly;
                tempCurvePtPoly->DeepCopy(m_curvePointsPoly);
                m_curveBlock->SetBlock(m_currentCurveId, tempCurvePtPoly);
                m_curveBlock->Modified();
                m_renderer->GetRenderWindow()->Render();
            }
        }
    }
} */

void TemplateDigitiser::ChangeCurveType() {
    if (curveTypeButton->isChecked()) {
        curveTypeButton->setIcon(
            QIcon(":/icons/graphics/icons/open_curve.svg"));
        m_curveType->at(m_currentCurveId) = 0;
        if (m_curveHighlightCtrlPoints->GetNumberOfPoints() > 2) {
            UpdateCurveData(m_curveHighlightCtrlPoints, m_curvePoly,
                            m_curveHighlightPoints, m_meshData);
            m_curveTubeFilter->SetInputData(m_curvePoly);
            m_curveTubeFilter->Update();
            m_curveTubeFilter->Modified();
            m_curvePointsPoly->SetPoints(m_curveHighlightPoints);
            // UpdateAnchors(m_curvePointsPoly, m_curveAnchorIdList, 1, 0);
            // curveLockButton->setChecked(0);
            // ChangeCurveLock();
            m_curveVertexFilter->SetInputData(m_curvePointsPoly);
            m_curveVertexFilter->Update();
            m_curveVertexFilter->Modified();
            vtkNew<vtkPolyData> tempCurvePolyLine;
            tempCurvePolyLine->DeepCopy(m_curvePoly);
            m_curvePolyLineBlock->SetBlock(m_currentCurveId, tempCurvePolyLine);
            m_curvePolyLineBlock->Modified();
            vtkNew<vtkPolyData> tempCurvePtPoly;
            tempCurvePtPoly->DeepCopy(m_curvePointsPoly);
            m_curveBlock->SetBlock(m_currentCurveId, tempCurvePtPoly);
            m_curveBlock->Modified();
            m_renderer->GetRenderWindow()->Render();
        }
    } else {
        curveTypeButton->setIcon(
            QIcon(":/icons/graphics/icons/closed_curve.svg"));
        m_curveType->at(m_currentCurveId) = 1;
        if (m_curveHighlightCtrlPoints->GetNumberOfPoints() > 2) {
            UpdateCurveData(m_curveHighlightCtrlPoints, m_curvePoly,
                            m_curveHighlightPoints, m_meshData);
            m_curveTubeFilter->SetInputData(m_curvePoly);
            m_curveTubeFilter->Update();
            m_curveTubeFilter->Modified();
            m_curvePointsPoly->SetPoints(m_curveHighlightPoints);
            // UpdateAnchors(m_curvePointsPoly, m_curveAnchorIdList, 1, 0);
            // curveLockButton->setChecked(0);
            // ChangeCurveLock();
            m_curveVertexFilter->SetInputData(m_curvePointsPoly);
            m_curveVertexFilter->Update();
            m_curveVertexFilter->Modified();
            vtkNew<vtkPolyData> tempCurvePolyLine;
            tempCurvePolyLine->DeepCopy(m_curvePoly);
            m_curvePolyLineBlock->SetBlock(m_currentCurveId, tempCurvePolyLine);
            m_curvePolyLineBlock->Modified();
            vtkNew<vtkPolyData> tempCurvePtPoly;
            tempCurvePtPoly->DeepCopy(m_curvePointsPoly);
            m_curveBlock->SetBlock(m_currentCurveId, tempCurvePtPoly);
            m_curveBlock->Modified();
            m_renderer->GetRenderWindow()->Render();
        }
    }
}

void TemplateDigitiser::ChangeCurveTypeAuto() {
    if (curveTypeButton->isChecked()) {
        curveTypeButton->setIcon(
            QIcon(":/icons/graphics/icons/open_curve.svg"));
        m_curveType->at(m_currentCurveId) = 0;
        if (m_curveHighlightCtrlPoints->GetNumberOfPoints() > 2) {
            UpdateCurveData(m_curveHighlightCtrlPoints, m_curvePoly,
                            m_curveHighlightPoints, m_meshData);
            m_curveTubeFilter->SetInputData(m_curvePoly);
            m_curveTubeFilter->Update();
            m_curveTubeFilter->Modified();
            m_curvePointsPoly->SetPoints(m_curveHighlightPoints);
            // UpdateAnchors(m_curvePointsPoly, m_curveAnchorIdList, 1, 0);
            // curveLockButton->setChecked(0);
            // ChangeCurveLock();
            m_curveVertexFilter->SetInputData(m_curvePointsPoly);
            m_curveVertexFilter->Update();
            m_curveVertexFilter->Modified();
            vtkNew<vtkPolyData> tempCurvePolyLine;
            tempCurvePolyLine->DeepCopy(m_curvePoly);
            m_curvePolyLineBlock->SetBlock(m_currentCurveId, tempCurvePolyLine);
            m_curvePolyLineBlock->Modified();
            vtkNew<vtkPolyData> tempCurvePtPoly;
            tempCurvePtPoly->DeepCopy(m_curvePointsPoly);
            m_curveBlock->SetBlock(m_currentCurveId, tempCurvePtPoly);
            m_curveBlock->Modified();
            m_renderer->GetRenderWindow()->Render();
        }
    } else {
        curveTypeButton->setIcon(
            QIcon(":/icons/graphics/icons/closed_curve.svg"));
        m_curveType->at(m_currentCurveId) = 1;
        if (m_curveHighlightCtrlPoints->GetNumberOfPoints() > 2) {
            UpdateCurveData(m_curveHighlightCtrlPoints, m_curvePoly,
                            m_curveHighlightPoints, m_meshData);
            m_curveTubeFilter->SetInputData(m_curvePoly);
            m_curveTubeFilter->Update();
            m_curveTubeFilter->Modified();
            m_curvePointsPoly->SetPoints(m_curveHighlightPoints);
            // UpdateAnchors(m_curvePointsPoly, m_curveAnchorIdList, 1, 0);
            // curveLockButton->setChecked(0);
            // ChangeCurveLock();
            m_curveVertexFilter->SetInputData(m_curvePointsPoly);
            m_curveVertexFilter->Update();
            m_curveVertexFilter->Modified();
            vtkNew<vtkPolyData> tempCurvePolyLine;
            tempCurvePolyLine->DeepCopy(m_curvePoly);
            m_curvePolyLineBlock->SetBlock(m_currentCurveId, tempCurvePolyLine);
            m_curvePolyLineBlock->Modified();
            vtkNew<vtkPolyData> tempCurvePtPoly;
            tempCurvePtPoly->DeepCopy(m_curvePointsPoly);
            m_curveBlock->SetBlock(m_currentCurveId, tempCurvePtPoly);
            m_curveBlock->Modified();
            m_renderer->GetRenderWindow()->Render();
        }
    }
}

void TemplateDigitiser::ClearCurve() {
    while (curveSelectComboBox->count() > 1) {
        curveSelectComboBox->removeItem(0);
    }
    while (surfacePickSourceComboBox->count() > 1) {
        surfacePickSourceComboBox->removeItem(1);
    }

    m_currentCurveId = 0;
    curveSelectComboBox->setItemText(
        0, QString::fromStdString(std::to_string(m_currentCurveId)));
    m_curveType->resize(0);
    for (int i = 0; i < m_curveNOC; i++) {
        m_curveType->push_back(1);
    }
    curvePickSourceComboBox->setEnabled(1);
    curvePickSourceComboBox->setCurrentIndex(0);
    m_curvectrlPointsPoly->Initialize();
    vtkNew<vtkPolyData> tempCtrlPtsPoly;
    m_curvectrlVertexFilter->SetInputData(tempCtrlPtsPoly);
    m_curvectrlVertexFilter->Update();
    m_curvectrlVertexFilter->Modified();
    m_curvePointsPoly->Initialize();
    // m_curveAnchorIdList->clear();
    if (m_curveBlock->GetNumberOfBlocks() > 0) {
        m_curveVertexFilter->Update();
        m_curveVertexFilter->Modified();
    }
    m_curvePoly->Initialize();
    m_curveTubeFilter->SetInputData(m_curvePoly);
    m_curveTubeFilter->Update();
    m_curveTubeFilter->Modified();
    m_curveTubeFilterDeactive->SetInputData(m_curvePoly);
    m_curveTubeFilterDeactive->Update();
    m_curveTubeFilterDeactive->Modified();
    m_curveHighlightCtrlPoints->Initialize();
    m_curveHighlightPoints->Initialize();
    m_curveHighlightPointsTotal->Initialize();
    m_curveLabelPoints->Initialize();
    m_curveCtrlBlock->Initialize();
    m_curveBlock->Initialize();
    m_curvePolyLineBlock->Initialize();
    vtkNew<vtkPolyData> tempPtsPoly;
    m_curveVertexFilterDeactive->SetInputData(tempPtsPoly);
    m_curveVertexFilterDeactive->Update();
    m_curveVertexFilterDeactive->Modified();
    vtkNew<vtkPolyData> emptyArrow;
    m_glyphCurveArrow->SetInputData(emptyArrow);
    m_glyphCurveArrow->Update();
    m_glyphCurveArrow->Modified();
    curveAddButton->setEnabled(0);
    // curveAnchorButton->setEnabled(0);
    curveLineEditNOC->setText(QString::fromStdString(
        std::to_string(m_curveNOC - m_curveCtrlBlock->GetNumberOfBlocks())));
    curveTypeButton->setChecked(0);
    ChangeCurveType();
    /* if (curveAnchorButton->isChecked()) {
        curveAnchorButton->setChecked(0);
    }
    if (curveLockButton->isChecked()) {
        curveLockButton->setChecked(0);
        ChangeCurveLock();
    } */
    pickFromBoundariesBox->setChecked(0);
    PickFromBoundaries();

    m_renderer->GetRenderWindow()->Render();
}

void TemplateDigitiser::ClearSurface() {
    while (surfaceSelectComboBox->count() > 1) {
        surfaceSelectComboBox->removeItem(0);
    }
    while (curvePickSourceComboBox->count() > 1) {
        curvePickSourceComboBox->removeItem(1);
    }
    surfacePickSourceComboBox->setEnabled(1);
    surfacePickSourceComboBox->setCurrentIndex(0);
    m_currentSurfaceId = 0;
    surfaceSelectComboBox->setItemText(
        0, QString::fromStdString(std::to_string(m_currentSurfaceId)));
    for (int i = 0; i < m_surfacePtsIds->size(); i++) {
        delete m_surfacePtsIds->at(i);
    }
    m_surfacePtsIds->resize(0);
    m_surfacePtsIds->push_back(new std::vector<int>);

    m_surfaceCurveHighlightCtrlPoints->Initialize();
    m_surfaceCurveCtrlPointsPoly->Initialize();
    m_surfaceCurveCtrlVertexFilter->Update();
    m_surfaceCurveCtrlVertexFilter->Modified();
    m_surfaceCtrlPointsPoly->Initialize();
    m_surfaceCtrlVertexFilter->Update();
    m_surfaceCtrlVertexFilter->Modified();
    m_surfaceCurvePoly->Initialize();
    m_surfacePatchHighlightPoints->Initialize();
    vtkNew<vtkPolyData> emptyPoly;
    m_surfaceTubeFilter->SetInputData(emptyPoly);
    m_surfaceTubeFilter->Update();
    m_surfaceTubeFilter->Modified();

    m_surfaceCurveTubeFilter->SetInputData(emptyPoly);
    m_surfaceCurveTubeFilter->Update();
    m_surfaceCurveTubeFilter->Modified();

    m_surfacePatchVertexFilter->SetInputData(emptyPoly);
    m_surfacePatchVertexFilter->Update();
    m_surfacePatchVertexFilter->Modified();

    m_surfaceSliderPtsTotal->Initialize();
    m_surfaceCurveVertexDeactiveFilter->SetInputData(emptyPoly);
    m_surfaceCurveVertexDeactiveFilter->Update();
    m_surfaceCurveVertexDeactiveFilter->Modified();
    m_surfacePatchVertexDeactiveFilter->SetInputData(emptyPoly);
    m_surfacePatchVertexDeactiveFilter->Update();
    m_surfacePatchVertexDeactiveFilter->Modified();

    m_surfaceCurveCtrlPtsDeactiveActor->GetProperty()->SetOpacity(0.5);

    m_surfaceLabelPoints->Initialize();
    m_surfaceLabelVertexFilter->SetInputData(emptyPoly);
    m_surfaceLabelVertexFilter->Update();
    m_surfaceLabelVertexFilter->Modified();

    m_surfaceBlock->Initialize();
    m_surfaceCurveCtrlBlock->Initialize();
    m_surfaceCtrlPtsBlock->Initialize();
    surfaceAddButton->setEnabled(0);
    int numOfPatches =
        m_surfacePatchNOP - (m_surfaceBlock->GetNumberOfBlocks());
    surfaceLineEditNOP->setText(
        QString::fromStdString(std::to_string(numOfPatches)));
    surfaceIronButton->setEnabled(1);
    surfaceIronButton->setChecked(0);
    DrapeToSurface();
    surfaceLockButton->setChecked(0);
    ChangeSurfaceLock();
    m_surfaceCurveBlock->Initialize();
    m_surfaceCurveTubeFilterDeactive->SetInputData(emptyPoly);
    m_surfaceCurveTubeFilterDeactive->Update();
    m_surfaceCurveTubeFilterDeactive->Modified();
    vtkNew<vtkPolyData> emptyArrow;
    m_glyphSurfaceArrow->SetInputData(emptyArrow);
    m_glyphSurfaceArrow->Update();
    m_glyphSurfaceArrow->Modified();

    cyclePatchesComboBox->blockSignals(true);
    cyclePatchesComboBox->clear();
    cyclePatchesComboBox->blockSignals(false);
    m_cutMeshData->Initialize();
    // m_dynamicRes =0;
    m_surfaceMask->Initialize();
    m_surfaceMaskBlock->Initialize();
    m_renderer->GetRenderWindow()->Render();
}

void TemplateDigitiser::DrapeToSurface() {
    if (surfaceIronButton->isChecked()) {
        surfaceIronButton->setIcon(QIcon(":/icons/graphics/icons/ironOn.svg"));
    }
    if (!surfaceIronButton->isChecked()) {
        surfaceIronButton->setIcon(QIcon(":/icons/graphics/icons/ironOff.svg"));
        surfaceLockButton->setChecked(1);
        ChangeSurfaceLock();
    }
}

void TemplateDigitiser::UpdateCurveScene(int id) {
    vtkNew<vtkPoints> tempHighlightedCtrlPts;
    vtkNew<vtkMultiBlockDataSet> tempCurvePolyBlock;
    vtkNew<vtkDataObjectTreeIterator> iterctrl;
    iterctrl->SetDataSet(m_curveCtrlBlock);
    iterctrl->SkipEmptyNodesOn();
    iterctrl->VisitOnlyLeavesOn();

    int counter = 0;
    for (iterctrl->InitTraversal(); !iterctrl->IsDoneWithTraversal();
         iterctrl->GoToNextItem()) {
        if (counter != id) {
            vtkDataObject* dso = iterctrl->GetCurrentDataObject();
            vtkPolyData* pd = dynamic_cast<vtkPolyData*>(dso);
            for (int j = 0; j < pd->GetNumberOfPoints(); j++) {
                tempHighlightedCtrlPts->InsertNextPoint(pd->GetPoint(j));
            }
            tempCurvePolyBlock->SetBlock(
                counter, m_curvePolyLineBlock->GetBlock(counter));
        } else if (counter == id) {
            /* curveLockButton->setChecked(1);
            ChangeCurveLock(); */
            vtkDataObject* dso = iterctrl->GetCurrentDataObject();
            vtkPolyData* pd = dynamic_cast<vtkPolyData*>(dso);
            if (pd->GetNumberOfPoints() > 0) {
                for (int j = 0; j < pd->GetNumberOfPoints(); j++) {
                    m_curveHighlightCtrlPoints->InsertNextPoint(
                        pd->GetPoint(j));
                }
                m_curveHighlightCtrlPoints->Modified();
                m_curvectrlPointsPoly->SetPoints(m_curveHighlightCtrlPoints);
                m_curvectrlVertexFilter->SetInputData(m_curvectrlPointsPoly);
                m_curvectrlVertexFilter->Update();
                m_curvectrlVertexFilter->Modified();
                if (m_curveHighlightCtrlPoints->GetNumberOfPoints() > 2) {
                    UpdateCurveData(m_curveHighlightCtrlPoints, m_curvePoly,
                                    m_curveHighlightPoints, m_meshData);

                    m_curvePoly->Modified();
                    m_curveTubeFilter->SetInputData(m_curvePoly);
                    m_curveTubeFilter->Update();
                    m_curveTubeFilter->Modified();

                    m_curveHighlightPoints->Modified();
                    m_curvePointsPoly->SetPoints(m_curveHighlightPoints);
                    m_curveVertexFilter->Update();
                    m_curveVertexFilter->Modified();
                }
            }
        }
        counter += 1;
    }

    //------------------------------------
    vtkNew<vtkPoints> tempHighlightedPts;
    vtkNew<vtkDataObjectTreeIterator> iterPts;
    iterPts->SetDataSet(m_curveBlock);
    iterPts->SkipEmptyNodesOn();
    iterPts->VisitOnlyLeavesOn();
    counter = 0;
    for (iterPts->InitTraversal(); !iterPts->IsDoneWithTraversal();
         iterPts->GoToNextItem()) {
        if (counter != id) {
            vtkDataObject* dso = iterPts->GetCurrentDataObject();
            vtkPolyData* pd = dynamic_cast<vtkPolyData*>(dso);
            for (int j = 0; j < pd->GetNumberOfPoints(); j++) {
                tempHighlightedPts->InsertNextPoint(pd->GetPoint(j));
            }
        } else if (counter == id) {
            vtkDataObject* dso = iterPts->GetCurrentDataObject();
            vtkPolyData* pd = dynamic_cast<vtkPolyData*>(dso);
            if (m_curveHighlightCtrlPoints->GetNumberOfPoints() > 2) {
                m_curvePointsPoly->Modified();
                m_curveVertexFilter->SetInputData(m_curvePointsPoly);
                m_curveVertexFilter->Update();
                m_curveVertexFilter->Modified();
            }
        }
        counter += 1;
    }
    tempHighlightedPts->Modified();
    vtkNew<vtkPolyData> tempPtsPoly;
    tempPtsPoly->SetPoints(tempHighlightedPts);
    m_curveVertexFilterDeactive->SetInputData(tempPtsPoly);
    m_curveVertexFilterDeactive->Update();
    m_curveVertexFilterDeactive->Modified();

    tempCurvePolyBlock->Modified();
    vtkNew<vtkCompositeDataGeometryFilter> blockFilter;
    blockFilter->SetInputData(tempCurvePolyBlock);
    blockFilter->Update();
    m_curveTubeFilterDeactive->SetInputData(blockFilter->GetOutput());
    m_curveTubeFilterDeactive->Update();
    m_curveTubeFilterDeactive->Modified();
    m_curveActorDeactive->GetProperty()->SetOpacity(0.5);
    m_curveActorDeactive->Modified();

    m_renderer->GetRenderWindow()->Render();
}

void TemplateDigitiser::UpdateSurfaceScene(int id) {
    vtkNew<vtkPoints> tempHighlightedCurveCtrlPts;
    vtkNew<vtkPoints> tempHighlightedPatchSliderPts;
    vtkNew<vtkMultiBlockDataSet> tempCurvePolyBlock;
    vtkNew<vtkDataObjectTreeIterator> iterctrl;
    iterctrl->SetDataSet(m_surfaceCurveCtrlBlock);
    iterctrl->SkipEmptyNodesOn();
    iterctrl->VisitOnlyLeavesOn();
    int counter = 0;
    for (iterctrl->InitTraversal(); !iterctrl->IsDoneWithTraversal();
         iterctrl->GoToNextItem()) {
        if (counter != id) {
            vtkDataObject* dso = iterctrl->GetCurrentDataObject();
            vtkPolyData* pd = dynamic_cast<vtkPolyData*>(dso);
            for (int i = 0; i < pd->GetNumberOfPoints(); i++) {
                tempHighlightedCurveCtrlPts->InsertNextPoint(pd->GetPoint(i));
            }
            vtkPolyData* tempSliderPoly =
                dynamic_cast<vtkPolyData*>(m_surfaceBlock->GetBlock(counter));
            for (int j = 0; j < tempSliderPoly->GetNumberOfPoints(); j++) {
                tempHighlightedPatchSliderPts->InsertNextPoint(
                    tempSliderPoly->GetPoint(j));
            }
            tempCurvePolyBlock->SetBlock(
                counter, m_surfaceCurveBlock->GetBlock(counter));
        } else if (counter == id) {
            vtkDataObject* dso = iterctrl->GetCurrentDataObject();
            vtkPolyData* pd = dynamic_cast<vtkPolyData*>(dso);
            if (pd->GetNumberOfPoints() > 0) {
                surfaceLockButton->setChecked(1);
                ChangeSurfaceLock();
                surfaceIronButton->setChecked(0);
                DrapeToSurface();
                for (int j = 0; j < pd->GetNumberOfPoints(); j++) {
                    m_surfaceCurveHighlightCtrlPoints->InsertNextPoint(
                        pd->GetPoint(j));
                }
                m_surfaceCurveHighlightCtrlPoints->Modified();
                m_surfaceCurveCtrlPointsPoly->SetPoints(
                    m_surfaceCurveHighlightCtrlPoints);
                m_surfaceCurveCtrlVertexFilter->SetInputData(
                    m_surfaceCurveCtrlPointsPoly);
                m_surfaceCurveCtrlVertexFilter->Update();
                m_surfaceCurveCtrlVertexFilter->Modified();

                auto tempSurfaceSliders =
                    dynamic_cast<vtkPolyData*>(m_surfaceBlock->GetBlock(id));
                for (int i = 0; i < tempSurfaceSliders->GetNumberOfPoints();
                     i++) {
                    m_surfacePatchHighlightPoints->InsertNextPoint(
                        tempSurfaceSliders->GetPoint(i));
                }

                vtkNew<vtkPolyData> tempSlidersPoly;
                tempSlidersPoly->SetPoints(m_surfacePatchHighlightPoints);
                m_surfacePatchVertexFilter->SetInputData(tempSlidersPoly);
                m_surfacePatchVertexFilter->Update();
                m_surfacePatchVertexFilter->Modified();

                auto tempSurfaceCurve = dynamic_cast<vtkPolyData*>(
                    m_surfaceCurveBlock->GetBlock(id));
                m_surfaceCurveTubeFilter->SetInputData(tempSurfaceCurve);
                m_surfaceCurveTubeFilter->Update();
                m_surfaceCurveTubeFilter->Modified();

                auto tempSurfaceCtrlPts = dynamic_cast<vtkPolyData*>(
                    m_surfaceCtrlPtsBlock->GetBlock(id));
                m_surfaceCtrlPointsPoly->DeepCopy(tempSurfaceCtrlPts);
                m_surfaceCtrlVertexFilter->SetInputData(
                    m_surfaceCtrlPointsPoly);
                m_surfaceCtrlVertexFilter->Update();
                m_surfaceCtrlVertexFilter->Modified();
                vtkNew<vtkExtractEdges> edgeExtracted;
                edgeExtracted->SetInputData(m_surfaceCtrlPointsPoly);
                edgeExtracted->Update();
                m_surfaceTubeFilter->SetInputData(edgeExtracted->GetOutput());
                m_surfaceTubeFilter->Update();
                m_surfaceTubeFilter->Modified();

                auto tempSurfaceMask = dynamic_cast<vtkPolyData*>(
                    m_surfaceMaskBlock->GetBlock(id));
                m_surfaceMask->DeepCopy(tempSurfaceMask);
                m_surfaceMask->Modified();
            }
        }
        counter += 1;
    }
    vtkNew<vtkPolyData> HighlightedCurveCtrlPtsDeactivePoly;
    HighlightedCurveCtrlPtsDeactivePoly->SetPoints(tempHighlightedCurveCtrlPts);
    m_surfaceCurveVertexDeactiveFilter->SetInputData(
        HighlightedCurveCtrlPtsDeactivePoly);
    m_surfaceCurveVertexDeactiveFilter->Update();
    m_surfaceCurveVertexDeactiveFilter->Modified();

    vtkNew<vtkPolyData> HighlightedPatchSliderDeactivePoly;
    HighlightedPatchSliderDeactivePoly->SetPoints(
        tempHighlightedPatchSliderPts);
    m_surfacePatchVertexDeactiveFilter->SetInputData(
        HighlightedPatchSliderDeactivePoly);
    m_surfacePatchVertexDeactiveFilter->Update();
    m_surfacePatchVertexDeactiveFilter->Modified();

    vtkNew<vtkCompositeDataGeometryFilter> blockFilter;
    blockFilter->SetInputData(tempCurvePolyBlock);
    blockFilter->Update();
    m_surfaceCurveTubeFilterDeactive->SetInputData(blockFilter->GetOutput());
    m_surfaceCurveTubeFilterDeactive->Update();
    m_surfaceCurveTubeFilterDeactive->Modified();
    m_surfaceCurveActorDeactive->GetProperty()->SetOpacity(0.5);
    m_surfaceCurveActorDeactive->Modified();

    m_renderer->GetRenderWindow()->Render();
}

void TemplateDigitiser::FinalizeCurveScene() {
    if (m_curveBlock->GetNumberOfBlocks() == m_curveNOC) {
        ResetCurveScene();
        m_curveHighlightPointsTotal->Initialize();
        vtkNew<vtkDataObjectTreeIterator> iterPts;
        iterPts->SetDataSet(m_curveBlock);
        iterPts->SkipEmptyNodesOn();
        iterPts->VisitOnlyLeavesOn();
        for (iterPts->InitTraversal(); !iterPts->IsDoneWithTraversal();
             iterPts->GoToNextItem()) {
            vtkDataObject* dso = iterPts->GetCurrentDataObject();
            vtkPolyData* pd = dynamic_cast<vtkPolyData*>(dso);
            if (pd->GetNumberOfPoints() == m_curveNOS) {
                for (int j = 0; j < pd->GetNumberOfPoints(); j++) {
                    m_curveHighlightPointsTotal->InsertNextPoint(
                        pd->GetPoint(j));
                }
            } else {
                QMessageBox warning;
                warning.setText(
                    "Your last curve is not completed. Add more control "
                    "points! Nothing will be registered!");
                warning.exec();
                break;
            }
        }
        m_curveHighlightPointsTotal->Modified();
        vtkNew<vtkPolyData> tempPtsPoly;
        tempPtsPoly->SetPoints(m_curveHighlightPointsTotal);
        m_curveVertexFilterDeactive->SetInputData(tempPtsPoly);
        m_curveVertexFilterDeactive->Update();
        m_curveVertexFilterDeactive->Modified();
        m_curvePointActorDeactive->GetProperty()->SetOpacity(1);

        vtkNew<vtkPolyData> emptyPoly;
        m_curveTubeFilterDeactive->SetInputData(emptyPoly);
        m_curveTubeFilterDeactive->Update();
        m_curveTubeFilterDeactive->Modified();
        /* curveAnchorButton->setChecked(0);
        PickCurveAnchor(); */
        m_renderer->GetRenderWindow()->Render();
    } else {
        QMessageBox warning;
        warning.setText(
            "Number of digitized curves are less than the decided number. "
            "Nothing will be registered!");
        warning.exec();
    }
}

void TemplateDigitiser::FinalizeSurfaceScene() {
    if (m_surfaceBlock->GetNumberOfBlocks() == m_surfacePatchNOP) {
        ResetSurfaceScene();
        m_surfaceCurveCtrlPointsPoly->Initialize();
        m_surfacePatchHighlightPoints->Initialize();
        m_surfaceCurveCtrlVertexFilter->SetInputData(
            m_surfaceCurveCtrlPointsPoly);
        m_surfaceCurveCtrlVertexFilter->Update();
        m_surfaceCurveCtrlVertexFilter->Modified();
        vtkNew<vtkPolyData> emptyPoly;
        m_surfaceCurveVertexDeactiveFilter->SetInputData(emptyPoly);
        m_surfaceCurveVertexDeactiveFilter->Update();
        m_surfaceCurveVertexDeactiveFilter->Modified();

        m_surfaceSliderPtsTotal->Initialize();
        vtkNew<vtkDataObjectTreeIterator> iterPts;
        iterPts->SetDataSet(m_surfaceBlock);
        iterPts->SkipEmptyNodesOn();
        iterPts->VisitOnlyLeavesOn();
        for (iterPts->InitTraversal(); !iterPts->IsDoneWithTraversal();
             iterPts->GoToNextItem()) {
            vtkDataObject* dso = iterPts->GetCurrentDataObject();
            vtkPolyData* pd = dynamic_cast<vtkPolyData*>(dso);
            for (int i = 0; i < pd->GetNumberOfPoints(); i++) {
                m_surfaceSliderPtsTotal->InsertNextPoint(pd->GetPoint(i));
            }
        }
        m_surfaceSliderPtsTotal->Modified();
        vtkNew<vtkPolyData> tempPtsPoly;
        tempPtsPoly->SetPoints(m_surfaceSliderPtsTotal);
        m_surfacePatchVertexDeactiveFilter->SetInputData(tempPtsPoly);
        m_surfacePatchVertexDeactiveFilter->Update();
        m_surfacePatchVertexDeactiveFilter->Modified();
        m_surfacePatchPointDeactiveActor->GetProperty()->SetOpacity(1);
        m_surfacePatchPointDeactiveActor->Modified();

        m_surfaceCurveTubeFilterDeactive->SetInputData(emptyPoly);
        m_surfaceCurveTubeFilterDeactive->Update();
        m_surfaceCurveTubeFilterDeactive->Modified();

        m_renderer->GetRenderWindow()->Render();
    }
    else {
        QMessageBox warning;
        warning.setText(
            "Number of digitized Surface Patches are less than the decided "
            "number. Nothing will be registered!");
        warning.exec();
    }
}

void TemplateDigitiser::UpdateCurveData(vtkPoints* pts, vtkPolyData* outputLine,
                                   vtkPoints* outputPoints,
                                   vtkPolyData* baseMesh) {
    // Here put closed or open curve
    if (pts->GetNumberOfPoints() > 2) {
        // m_curveActor->SetVisibility(1);
        vtkNew<vtkPoints> tempPoints;
        outputPoints->Initialize();
        outputLine->Initialize();

        vtkNew<vtkParametricSpline> tempCurve;
        tempCurve->SetPoints(pts);
        double delta = 1 / (double)(m_curveNOS + 1);
        if (m_curveType->at(m_currentCurveId) == 1) {  // close curve
            tempCurve->SetClosed(1);
            for (double i = 1; i < (double)m_curveNOS + 1; i++) {
                double segmnt = delta * i;
                double u[3] = {segmnt, 0, 0};
                double Pt[3];
                tempCurve->Evaluate(u, Pt, nullptr);
                tempPoints->InsertNextPoint(Pt);
                tempPoints->Modified();
            }
        } else if (m_curveType->at(m_currentCurveId) == 0) {  // open curve
            tempCurve->SetClosed(0);
            for (double i = 1; i < (double)m_curveNOS + 1; i++) {
                double segmnt = delta * i;
                double u[3] = {segmnt, 0, 0};
                double Pt[3];
                tempCurve->Evaluate(u, Pt, nullptr);
                tempPoints->InsertNextPoint(Pt);
                tempPoints->Modified();
            }
        }
        // Project on mesh
        if (baseMesh->GetNumberOfCells() > 0) {
            vtkNew<vtkCellLocator> ptLocator;
            ptLocator->SetDataSet(baseMesh);
            ptLocator->BuildLocator();
            for (int i = 0; i < tempPoints->GetNumberOfPoints(); i++) {
                double closestPoint[3];
                vtkIdType closestCellId = -1;
                int subId = -1;
                double dist = -1;
                ptLocator->FindClosestPoint(tempPoints->GetPoint(i),
                                            closestPoint, closestCellId, subId,
                                            dist);
                outputPoints->InsertNextPoint(closestPoint);
            }
            outputPoints->Modified();
        }
        vtkNew<vtkPoints> resampledCurvePts;
        delta = 1 / 101.0;
        for (float i = 1; i < 101; i++) {
            double segmnt = delta * i;
            double u[3] = {segmnt, 0, 0};
            double Pt[3];
            tempCurve->Evaluate(u, Pt, nullptr);
            resampledCurvePts->InsertNextPoint(Pt);
            resampledCurvePts->Modified();
        }

        vtkNew<vtkCellArray> line;
        if (m_curveType->at(m_currentCurveId) == 1) {  // closed curve
            line->InsertNextCell(resampledCurvePts->GetNumberOfPoints() + 1);
            for (int i = 0; i < resampledCurvePts->GetNumberOfPoints(); i++) {
                line->InsertCellPoint(i);
            }
            line->InsertCellPoint(0);
        } else if (m_curveType->at(m_currentCurveId) == 0) {  // open curve
            line->InsertNextCell(resampledCurvePts->GetNumberOfPoints());
            for (int i = 0; i < resampledCurvePts->GetNumberOfPoints(); i++) {
                line->InsertCellPoint(i);
            }
        }
        vtkNew<vtkPolyData> curvePoly;
        curvePoly->SetPoints(resampledCurvePts);
        curvePoly->SetLines(line);
        curvePoly->Modified();
        outputLine->DeepCopy(curvePoly);

        vtkNew<vtkPolyData> labelPoly;
        m_curveLabelPoints->InsertPoint(m_currentCurveId,
                                        outputPoints->GetPoint(0));
        labelPoly->SetPoints(m_curveLabelPoints);
        m_curveLabelVertexFilter->SetInputData(labelPoly);
        m_curveLabelVertexFilter->Update();
        m_curveLabelVertexFilter->Modified();
    }
}

void TemplateDigitiser::ConstructSurfaceData(vtkPolyData* CtrlPtsPoly,
                                        vtkPoints* outputSliders) {
    outputSliders->Initialize();
    std::vector<int>* outlineIds = new std::vector<int>();
    OutlineIdFinder(m_surfacePatchUNOS, m_surfacePatchVNOS, outlineIds);
    vtkNew<vtkPoints> tempSliders;
    for (int i = 0; i < CtrlPtsPoly->GetNumberOfPoints(); i++) {
        if (std::find(outlineIds->begin(), outlineIds->end(), i) !=
            outlineIds->end()) {
        } else {
            tempSliders->InsertNextPoint(CtrlPtsPoly->GetPoint(i));
        }
    }
    tempSliders->Modified();

    ProjectOnMesh(tempSliders, m_surfaceMask);
    delete outlineIds;
    outputSliders->DeepCopy(tempSliders);
    outputSliders->Modified();
}

void TemplateDigitiser::ConstructSurfaceData(vtkPoints* pts,
                                        vtkPoints* outputSliders,
                                        vtkPolyData* outputCtrlPtsPoly,
                                        vtkPolyData* outputCurvePoly,
                                        int resolution) {
    if (pts->GetNumberOfPoints() > 2) {
        outputSliders->Initialize();
        outputCtrlPtsPoly->Initialize();
        outputCurvePoly->Initialize();
        int uRes = m_surfacePatchUNOS + 2;
        int vRes = m_surfacePatchVNOS + 2;
        int resampleNumber = (((uRes + 1) - 2) * 2) + (((vRes + 1) - 2) * 2);
        vtkNew<vtkPoints> curveResmplPts;
        double delta = 1 / (double)resampleNumber;
        vtkNew<vtkParametricSpline> tempCurve;
        tempCurve->SetPoints(pts);
        tempCurve->SetClosed(1);
        for (double i = 0; i < (double)resampleNumber; i++) {
            double segmnt = delta * i;
            double u[3] = {segmnt, 0, 0};
            double Pt[3];
            tempCurve->Evaluate(u, Pt, nullptr);
            curveResmplPts->InsertNextPoint(Pt);
            curveResmplPts->Modified();
        }
        MeshCutter(m_meshData, curveResmplPts, m_surfaceMask);
        vtkNew<vtkPolyData> planePolyInit;
        MakeCage(pts, planePolyInit);
        std::vector<int>* outlineIds = new std::vector<int>();
        OutlineIdFinder(m_surfacePatchUNOS, m_surfacePatchVNOS, outlineIds);
        /* vtkNew<vtkPoints> planeOutlinePts;
        for(int i =0; i < outlineIds->size(); i++){
            planeOutlinePts->InsertNextPoint(planePolyInit->GetPoint(outlineIds->at(i)));
        }
        planeOutlinePts->Modified(); */

        vtkPolyData* planePolyDeformed = planePolyInit;
        outputCtrlPtsPoly->DeepCopy(planePolyDeformed);

        vtkNew<vtkCellArray> line;
        line->InsertNextCell(curveResmplPts->GetNumberOfPoints() + 1);
        for (int i = 0; i < curveResmplPts->GetNumberOfPoints(); i++) {
            line->InsertCellPoint(i);
        }
        line->InsertCellPoint(0);
        vtkNew<vtkPolyData> curvePoly;
        curvePoly->SetPoints(curveResmplPts);
        curvePoly->SetLines(line);
        vtkNew<vtkSplineFilter> splineFilter;
        splineFilter->SetInputData(curvePoly);
        splineFilter->SetNumberOfSubdivisions(
            curveResmplPts->GetNumberOfPoints());
        splineFilter->Update();
        outputCurvePoly->DeepCopy(splineFilter->GetOutput());

        vtkNew<vtkPoints> tempSliders;
        for (int i = 0; i < outputCtrlPtsPoly->GetNumberOfPoints(); i++) {
            auto it = std::find(outlineIds->begin(), outlineIds->end(), i);
            if (it != outlineIds->end()) {
                /* int index = it - outlineIds->begin();
                outputCtrlPtsPoly->GetPoints()->SetPoint(i,
                curveResmplPts->GetPoint(outlineIds->at(index))); */
            } else {
                tempSliders->InsertNextPoint(outputCtrlPtsPoly->GetPoint(i));
            }
        }
        /* for(int i = 0; i < outlineIds->size(); i++){
            outputCtrlPtsPoly->GetPoints()->SetPoint(outlineIds->at(i),
        curveResmplPts->GetPoint((outlineIds->size() - 1) - i));
        } */
        if (outputCtrlPtsPoly->GetNumberOfPoints() > 0) {
            vtkNew<vtkCellLocator> ptLocator;
            ptLocator->SetDataSet(outputCurvePoly);
            ptLocator->BuildLocator();
            for (int i = 0; i < outlineIds->size(); i++) {
                double closestPoint[3];
                vtkIdType closestCellId = -1;
                int subId = -1;
                double dist = -1;
                ptLocator->FindClosestPoint(
                    outputCtrlPtsPoly->GetPoint(outlineIds->at(i)),
                    closestPoint, closestCellId, subId, dist);
                outputCtrlPtsPoly->GetPoints()->SetPoint(outlineIds->at(i),
                                                         closestPoint);
            }
        }
        tempSliders->Modified();
        outputCtrlPtsPoly->Modified();
        ProjectOnMesh(tempSliders, m_surfaceMask);
        delete outlineIds;

        outputSliders->DeepCopy(tempSliders);
        outputSliders->Modified();
        //---------------------
        /* vtkNew<vtkXMLPolyDataWriter> writer;
        writer->SetInputData(transform->GetOutput());
        writer->SetFileName("testDeformed.vtp");
        writer->Update();
        writer->Write(); */
        if (outputCtrlPtsPoly->GetNumberOfPoints() > 0) {
            vtkNew<vtkPolyData> labelPoly;
            m_surfaceLabelPoints->InsertPoint(m_currentSurfaceId,
                                              outputSliders->GetPoint(0));
            labelPoly->SetPoints(m_surfaceLabelPoints);
            m_surfaceLabelVertexFilter->SetInputData(labelPoly);
            m_surfaceLabelVertexFilter->Update();
            m_surfaceLabelVertexFilter->Modified();
        }
    }
}

void TemplateDigitiser::MakeCage(vtkPoints* pts, vtkPolyData* outPlanePoly) {
    outPlanePoly->Initialize();
    double delta = 1 / 3.0;
    vtkNew<vtkPoints> curveInit3Pts;
    vtkNew<vtkParametricSpline> tempCurve;
    tempCurve->SetPoints(pts);
    tempCurve->SetClosed(1);
    for (double i = 0; i < 3.0; i++) {
        double segmnt = delta * i;
        double u[3] = {segmnt, 0, 0};
        double Pt[3];
        tempCurve->Evaluate(u, Pt, nullptr);
        curveInit3Pts->InsertNextPoint(Pt);
        curveInit3Pts->Modified();
    }

    vtkNew<vtkPolyLine> measureLine;
    measureLine->GetPointIds()->SetNumberOfIds(4);
    for (int i = 0; i < 4; i++) {
        measureLine->GetPointIds()->SetId(i, i);
    }
    vtkNew<vtkCellArray> measureCells;
    measureCells->InsertNextCell(measureLine);
    vtkNew<vtkPoints> measurePoints;
    measurePoints->DeepCopy(curveInit3Pts);
    measurePoints->InsertNextPoint(curveInit3Pts->GetPoint(0));
    vtkNew<vtkPolyData> measurePoly;
    measurePoly->SetPoints(measurePoints);
    measurePoly->SetLines(measureCells);
    vtkNew<vtkAppendArcLength> measureFilter;
    measureFilter->SetInputData(measurePoly);
    measureFilter->Update();
    int dynamicRes = std::sqrt(measureFilter->GetOutput()
                                   ->GetPointData()
                                   ->GetArray("arc_length")
                                   ->GetTuple(3)[0]) *
                     0.5;
    if (dynamicRes % 2 == 0) {
        dynamicRes += 1;
    }
    vtkNew<vtkPlaneSource> planeSource;
    planeSource->SetXResolution(dynamicRes);
    planeSource->SetYResolution(dynamicRes);
    planeSource->SetOrigin(curveInit3Pts->GetPoint(0));
    planeSource->SetPoint1(curveInit3Pts->GetPoint(1));
    planeSource->SetPoint2(curveInit3Pts->GetPoint(2));
    planeSource->Update();

    vtkPolyData* planePolyInit = planeSource->GetOutput();
    int resampleNumber = ((dynamicRes + 1) * 2) + (((dynamicRes + 1) - 2) * 2);
    vtkNew<vtkPoints> curveResmplPts;
    delta = 1 / (double)resampleNumber;
    for (double i = 0; i < (double)resampleNumber; i++) {
        double segmnt = delta * i;
        double u[3] = {segmnt, 0, 0};
        double Pt[3];
        tempCurve->Evaluate(u, Pt, nullptr);
        curveResmplPts->InsertNextPoint(Pt);
        curveResmplPts->Modified();
    }
    // this is a bit confusing to find ids of the plane outline points in the
    // same order as the curve, it only works in case of square planes, which we
    // have here
    std::vector<int>* outlineIds = new std::vector<int>();
    for (int i = 0; i < dynamicRes + 1; i++) {
        if (i == 0) {
            for (int j = 0; j < dynamicRes + 1; j++) {
                outlineIds->push_back(j);
            }
        }
        if (0 < i && i < dynamicRes) {
            outlineIds->push_back(((dynamicRes * i) + i) + dynamicRes);
        }
        if (i == dynamicRes) {
            for (int j = ((dynamicRes + 1) * (dynamicRes + 1)) - 1;
                 j >= (dynamicRes + 1) * dynamicRes; j--) {
                outlineIds->push_back(j);
            }
        }
    }
    for (int i = dynamicRes - 1; i > 0; i--) {
        outlineIds->push_back((dynamicRes * i) + i);
    }

    vtkNew<vtkPoints> planeOutlinePts;
    for (int i = 0; i < outlineIds->size(); i++) {
        planeOutlinePts->InsertNextPoint(
            planePolyInit->GetPoint(outlineIds->at(i)));
    }

    vtkNew<vtkThinPlateSplineTransform> tpsTrans;
    tpsTrans->SetTargetLandmarks(curveResmplPts);
    tpsTrans->SetSourceLandmarks(planeOutlinePts);
    tpsTrans->SetBasisToR();
    tpsTrans->SetSigma(1);
    tpsTrans->Update();
    vtkNew<vtkTransformPolyDataFilter> transform;
    transform->SetInputData(planePolyInit);
    transform->SetTransform(tpsTrans);
    transform->Update();

    vtkNew<vtkWindowedSincPolyDataFilter> repeller;
    repeller->SetInputData(transform->GetOutput());
    repeller->SetNumberOfIterations(50);
    repeller->SetPassBand(0.1);
    repeller->SetFeatureEdgeSmoothing(1);
    repeller->SetFeatureAngle(5);
    repeller->SetBoundarySmoothing(0);
    repeller->NormalizeCoordinatesOn();
    repeller->Update();

    vtkPolyData* planePolyDeformed = repeller->GetOutput();
    ProjectOnMesh(planePolyDeformed, m_surfaceMask, outlineIds);

    vtkNew<vtkSmoothPolyDataFilter> repeller2;
    repeller2->SetInputData(planePolyDeformed);
    repeller2->SetNumberOfIterations(200);
    repeller2->SetConvergence(0.05);
    repeller2->Update();

    vtkPolyData* ctrlPtsPoly = repeller2->GetOutput();
    TColgp_Array2OfPnt* planePointArray =
        new TColgp_Array2OfPnt(1, dynamicRes + 1, 1, dynamicRes + 1);
    for (int i = 0; i < dynamicRes + 1; i++) {
        for (int j = 0; j < dynamicRes + 1; j++) {
            double pt[3];
            ctrlPtsPoly->GetPoint((dynamicRes * i) + (i + j), pt);
            planePointArray->SetValue(i + 1, j + 1,
                                      gp_Pnt(pt[0], pt[1], pt[2]));
        }
    }

    try {
        Handle(Geom_BezierSurface) BZPlane =
            new Geom_BezierSurface(*planePointArray);
        double UDelta = 1 / static_cast<double>((m_surfacePatchUNOS + 1));
        double VDelta = 1 / static_cast<double>((m_surfacePatchVNOS + 1));
        vtkNew<vtkPoints> tempSliders;
        for (double i = 0; i < m_surfacePatchVNOS + 2; i++) {
            for (double j = 0; j < m_surfacePatchUNOS + 2; j++) {
                double USegmnt = UDelta * j;
                double VSegmnt = VDelta * i;
                tempSliders->InsertNextPoint(
                    BZPlane->Value(USegmnt, VSegmnt).Coord(1),
                    BZPlane->Value(USegmnt, VSegmnt).Coord(2),
                    BZPlane->Value(USegmnt, VSegmnt).Coord(3));
            }
        }
        ProjectOnMesh(tempSliders, m_surfaceMask);
        int uRes = m_surfacePatchUNOS + 2;
        int vRes = m_surfacePatchVNOS + 2;
        vtkNew<vtkPlaneSource> outPlaneSource;
        outPlaneSource->SetXResolution(uRes - 1);
        outPlaneSource->SetYResolution(vRes - 1);
        outPlaneSource->SetOrigin(curveInit3Pts->GetPoint(0));
        outPlaneSource->SetPoint1(curveInit3Pts->GetPoint(1));
        outPlaneSource->SetPoint2(curveInit3Pts->GetPoint(2));
        outPlaneSource->Update();
        vtkPolyData* planePoly = outPlaneSource->GetOutput();
        planePoly->SetPoints(tempSliders);
        planePoly->Modified();
        outPlanePoly->DeepCopy(planePoly);
    } catch (Standard_Failure e) {
        std::cout << "Can't Interpolate the Plane" << std::endl;
    }
    delete outlineIds;
    delete planePointArray;
    /* vtkNew<vtkXMLPolyDataWriter> writer;
    writer->SetInputData(planePoly);
    writer->SetFileName("testDeformed.vtp");
    writer->Update();
    writer->Write(); */
}

/* void TemplateDigitiser::MakeSurface(vtkPolyData* mask, vtkPoints* curve,
vtkPolyData* plane, int ures, int vres){
    //int res = ures * vres;
    //std::cout<< res<<std::endl;
    // We work with the odd resolution to parametrize the plane
    vtkNew<vtkFeatureEdges> edgeFilter;
    edgeFilter->SetInputData(mask);
    edgeFilter->BoundaryEdgesOn();
    edgeFilter->FeatureEdgesOff();
    edgeFilter->NonManifoldEdgesOff();
    edgeFilter->ManifoldEdgesOff();
    edgeFilter->Update();

    vtkNew<vtkDoubleArray> distArray;
    distArray->SetName("Distance");
    distArray->SetNumberOfComponents(1);
    distArray->SetNumberOfTuples(mask->GetNumberOfPoints());
    vtkNew<vtkCellLocator> curvePtLocator;
    curvePtLocator->SetDataSet(edgeFilter->GetOutput());
    curvePtLocator->Update();
    curvePtLocator->BuildLocator();
    for(int i =0; i < mask->GetNumberOfPoints(); i++){
        double closestPoint[3];
        vtkIdType closestCellId = -1;
        int subId = -1;
        double tempDist = -1;
        curvePtLocator->FindClosestPoint(mask->GetPoint(i), closestPoint,
closestCellId, subId, tempDist); double dist = EucDist(mask->GetPoint(i)[0],
mask->GetPoint(i)[1], mask->GetPoint(i)[2], closestPoint[0], closestPoint[1],
closestPoint[2]); distArray->SetTuple1(i, dist);
    }
    mask->GetPointData()->SetScalars(distArray);
    int NumContours = (ures/2) + 1;
    double minDistance = 0.99 *
mask->GetPointData()->GetScalars()->GetRange()[0]; double maxDistance = 0.99 *
mask->GetPointData()->GetScalars()->GetRange()[1]; vtkNew<vtkContourFilter> cF;
    cF->SetInputData(mask);
    cF->GenerateValues(NumContours, minDistance,maxDistance);
    cF->Update();
    std::cout<< NumContours<<std::endl;
    vtkNew<vtkPolyData>curvePoly;
    curvePoly->SetPoints(curve);
    vtkNew<vtkMultiBlockDataSet> contourBlock;
    contourBlock->SetBlock(0, curvePoly);

    for(int i = 1; i < NumContours; i++){
        vtkNew<vtkThreshold> selector;
        selector->SetInputData(cF->GetOutput());
        selector->ThresholdByLower((i + 1) * minDistance);
        selector->ThresholdByUpper((i + 1) * minDistance);
        selector->Update();

        if (selector->GetOutput()->GetNumberOfPoints() > 0){
            vtkNew<vtkPoints> tempContourPts;
            vtkDataObject* dso = contourBlock->GetBlock(i -1);
            vtkPolyData* pd = dynamic_cast<vtkPolyData*>(dso);
            vtkNew<vtkPointLocator> ptLocator;
            ptLocator->SetDataSet(selector->GetOutput());
            ptLocator->BuildLocator();
            for(int j=0; j < pd->GetNumberOfPoints(); j++){
                vtkIdType id = ptLocator->FindClosestPoint(pd->GetPoint(j));
                double closestPoint[3];
                ptLocator->GetDataSet()->GetPoint(id, closestPoint);
                tempContourPts->InsertNextPoint(closestPoint);
            }

            vtkNew<vtkCellArray> tempContourLine;
            tempContourLine->InsertNextCell(tempContourPts->GetNumberOfPoints()
+ 1); for(int j=0; j < tempContourPts->GetNumberOfPoints(); j++){
                tempContourLine->InsertCellPoint(j);
            }
            tempContourLine->InsertCellPoint(0);
            vtkNew<vtkPolyData> tempContourPoly;
            tempContourPoly->SetPoints(tempContourPts);
            tempContourPoly->SetLines(tempContourLine);


            contourBlock->SetBlock(i, tempContourPoly);
            contourBlock->Modified();


        }
        else{
            std::cout<<"Problem"<<std::endl;
            break;
        }
    }

    vtkNew<vtkXMLPolyDataWriter> writer;
    writer->SetInputData(plane);
    writer->SetFileName("testDeformed.vtp");
    writer->Update();
    writer->Write();

    vtkNew<vtkXMLPolyDataWriter> writer2;
    writer2->SetInputData(cF->GetOutput());
    writer2->SetFileName("testcontour.vtp");
    writer2->Update();
    writer2->Write();

    vtkNew<vtkXMLMultiBlockDataWriter> writer3;
    writer3->SetInputData(contourBlock);
    writer3->SetFileName("testcontour.vtmb");
    writer3->Update();
    writer3->Write();

} */

/* void TemplateDigitiser::MakeSurface(vtkPoints* outerCurve, int Ures, int Vres,
vtkPoints *outputSliders){ Standard_Integer Degree=3; Standard_Integer
NbPtsOnCur=15; Standard_Integer NbIter=2; Standard_Boolean
Anisotropie=Standard_False; Standard_Real Tol2d=0.00001; Standard_Real
Tol3d=0.0001; Standard_Real TolAng=0.01; Standard_Real TolCurv=0.01;
    Standard_Integer MaxDeg=8;
    Standard_Integer MaxSegments=9;

    vtkNew<vtkPoints> tempSliders;
    outputSliders->Initialize();

    double UDelta = 1/static_cast<double>((Ures + 1));
    double VDelta = 1/static_cast<double>((Vres + 1));

    Handle(TColgp_HArray1OfPnt)pointArray = new TColgp_HArray1OfPnt(1,
outerCurve->GetNumberOfPoints()); for(int i =0; i <
outerCurve->GetNumberOfPoints(); i++){ pointArray->SetValue(i+1,
gp_Pnt(outerCurve->GetPoint(i)[0], outerCurve->GetPoint(i)[1],
outerCurve->GetPoint(i)[2]));
        }
    Standard_Real tol = 1.0e-7;
    GeomAPI_Interpolate *intp = new GeomAPI_Interpolate(pointArray, 1, tol);
//always here we interpolate a closed curve! intp->Perform();
    Handle(Geom_BSplineCurve) tempCurve = intp->Curve();

    TopoDS_Edge aEdge=BRepBuilderAPI_MakeEdge(tempCurve);
    BRepOffsetAPI_MakeFilling
fillingOp(Degree,NbPtsOnCur,NbIter,Anisotropie,Tol2d,Tol3d,TolAng,TolCurv,MaxDeg,MaxSegments);
    fillingOp.Add(aEdge,GeomAbs_C0,true);
    fillingOp.Build();
    if(fillingOp.IsDone()){
        TopoDS_Shape face = fillingOp.Shape();
        TopoDS_Face theFace = TopoDS::Face(face);
        BRepAdaptor_Surface aSurface(theFace);
        Handle(BRepAdaptor_HSurface) hSurface = new
BRepAdaptor_HSurface(aSurface); StdPrs_ToolRFace aToolRst (hSurface); int
theNbUIso = 10; //num of u iso curves int theNbVIso = 10; //num of v iso curves
        // Compute bounds of the restriction
        Standard_Real anUMin,anUMax,aVMin,aVMax;
        Standard_Real anU,aV,aStep;
        Standard_Integer anI,anNbP = 10;
        bool theDrawUIso = true;
        bool theDrawVIso = true;
        anUMin = aVMin = RealLast();
        anUMax = aVMax = RealFirst();
        gp_Pnt2d aPoint1,aPoint2;
        for(aToolRst.Init(); aToolRst.More(); aToolRst.Next()){
            Adaptor2d_Curve2d aRCurve = aToolRst.Value();
            anU = aRCurve.FirstParameter();
            aV = aRCurve.LastParameter();
            if (aRCurve.GetType() != GeomAbs_Line){
                aStep = ( aV - anU) / anNbP;
                for (anI = 0; anI <= anNbP; ++anI){
                    gp_Pnt2d aRCurvePoint = aRCurve.Value(anU);
                    if (aRCurvePoint.X() < anUMin) anUMin = aRCurvePoint.X();
                    if (aRCurvePoint.X() > anUMax) anUMax = aRCurvePoint.X();
                    if (aRCurvePoint.Y() < aVMin) aVMin = aRCurvePoint.Y();
                    if (aRCurvePoint.Y() > aVMax) aVMax = aRCurvePoint.Y();
                    anU += aStep;
                }
            }
            else{
                aPoint1 = aRCurve.Value(anU);
                if (aPoint1.X() < anUMin) anUMin = aPoint1.X();
                if (aPoint1.X() > anUMax) anUMax = aPoint1.X();
                if (aPoint1.Y() < aVMin) aVMin = aPoint1.Y();
                if (aPoint1.Y() > aVMax) aVMax = aPoint1.Y();

                aPoint2 = aRCurve.Value(aV);
                if (aPoint2.X() < anUMin) anUMin = aPoint2.X();
                if (aPoint2.X() > anUMax) anUMax = aPoint2.X();
                if (aPoint2.Y() < aVMin) aVMin = aPoint2.Y();
                if (aPoint2.Y() > aVMax) aVMax = aPoint2.Y();
            }
        }
        // Load the isos
        Hatch_Hatcher anIsoBuild(1.e-5,aToolRst.IsOriented());
        Standard_Boolean anUClosed = hSurface->IsUClosed();
        Standard_Boolean aVClosed = hSurface->IsVClosed();
        if(!anUClosed){
            anUMin = anUMin + ( anUMax - anUMin) /1000.;
            anUMax = anUMax - ( anUMax - anUMin) /1000.;
        }
        if(!aVClosed){
            aVMin = aVMin + ( aVMax - aVMin) /1000.;
            aVMax = aVMax - ( aVMax - aVMin) /1000.;
        }
        if(theDrawVIso){
            if (theNbVIso > 0){
                aVClosed = Standard_False;
                Standard_Real dv= aVClosed ?(aVMax-aVMin)/theNbVIso :
(aVMax-aVMin)/(1+theNbVIso); for (anI=1; anI<=theNbVIso;anI++){
                    anIsoBuild.AddYLine(aVMin+dv*anI);
                }
            }
        }
        // Trim the isos
        Standard_Real anU1, anU2, aDU;
        for(aToolRst.Init(); aToolRst.More(); aToolRst.Next()){
            TopAbs_Orientation Orient = aToolRst.Orientation();
            if (Orient == TopAbs_FORWARD || Orient == TopAbs_REVERSED){
                Adaptor2d_Curve2d aRCurve = aToolRst.Value();
                anU1 = aRCurve.FirstParameter();
                anU2 = aRCurve.LastParameter();
                if(aRCurve.GetType() != GeomAbs_Line){
                    aDU = (anU2-anU1)/(anNbP-1);
                    aPoint2 = aRCurve.Value(anU1);
                    for (anI = 2; anI <= anNbP; anI++){
                        anU = anU1 + (anI-1)*aDU;
                        aPoint1 = aPoint2;
                        aPoint2 = aRCurve.Value(anU);
                        std::cout<< aPoint1.X()<<std::endl;
                        std::cout<< aPoint2.Y() <<std::endl;
                        if(Orient == TopAbs_FORWARD ){
                            anIsoBuild.Trim(aPoint1,aPoint2);
                        }
                        else{
                            anIsoBuild.Trim(aPoint2,aPoint1);
                        }
                    }
                }
                else{
                    aPoint1 = aRCurve.Value(anU1);
                    aPoint2 = aRCurve.Value(anU2);
                    if(Orient == TopAbs_FORWARD ){
                        anIsoBuild.Trim(aPoint1,aPoint2);
                    }
                    else{
                        anIsoBuild.Trim(aPoint2,aPoint1);
                    }
                }
            }
        }


        //For debug
        IGESControl_Controller::Init();
        IGESControl_Writer writer("MM", 0);
        writer.AddShape(face);
        writer.ComputeModel();
        writer.Write("test.igs");
    }

}
 */
void TemplateDigitiser::Plot() {
    // Setting up render scene
    vtkNew<vtkNamedColors> colors;
    m_renWin = m_vtkRenderWidget->GetRenderWindow();
    m_renWin->AddRenderer(m_renderer);
    m_style->SetCurrentRenderer(m_renderer);
    m_PointPickerStyle->SetCurrentRenderer(m_renderer);
    m_PointPainterStyle->SetCurrentRenderer(m_renderer);
    m_2Dstyle->SetCurrentRenderer(m_renderer);
    m_PointMoverStyle->SetCurrentRenderer(m_renderer);
    m_iren->SetInteractorStyle(m_style);
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

    vtkNew<vtkDataSetMapper> cutMeshmapper;
    cutMeshmapper->SetInputData(m_surfaceMask);
    cutMeshmapper->SetScalarVisibility(0);
    cutMeshmapper->SetResolveCoincidentTopologyToDefault();
    m_cutMeshActor->SetMapper(cutMeshmapper);
    m_cutMeshActor->GetProperty()->SetColor(1, 0.843, 0.0);
    m_cutMeshActor->GetProperty()->SetOpacity(1);
    m_cutMeshActor->SetPickable(0);
    m_renderer->AddActor(m_cutMeshActor);

    // Point properties and color etc
    vtkNew<vtkMassProperties> prop;
    prop->SetInputData(m_meshData);
    prop->Update();
    m_area = prop->GetSurfaceArea();
    m_area = std::sqrt(m_area);
    m_sizeConstant =
        (m_typeINOL + m_surfaceNOS +
         (m_surfacePatchNOP * m_surfacePatchUNOS * m_surfacePatchVNOS) +
         (m_curveNOS * m_curveNOC));
    if (m_sizeConstant < 100) {
        m_sizeConstant = 100;
    }
    if (m_sizeConstant > 300) {
        m_sizeConstant = 300;
    }
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetRadius(m_area / (m_sizeConstant));
    vtkNew<vtkSphereSource> sphereSource2;
    sphereSource2->SetRadius((m_area / m_sizeConstant) * 1.5);
    vtkNew<vtkSphereSource> sphereSource3;
    sphereSource3->SetRadius((m_area / m_sizeConstant) * 0.5);
    //--------------------------------------------
    vtkNew<vtkGlyph3DMapper> fixedPointMapper;
    fixedPointMapper->SetInputData(m_fixedVertexFilter->GetOutput());
    fixedPointMapper->SetSourceConnection(sphereSource->GetOutputPort());
    fixedPointMapper->ScalingOff();
    fixedPointMapper->ScalarVisibilityOff();
    m_fixedPointActor->GetProperty()->SetColor(1.0, 0.0, 0.0);
    m_fixedPointActor->GetProperty()->SetDiffuse(0.8);
    m_fixedPointActor->GetProperty()->SetSpecular(0.5);
    m_fixedPointActor->GetProperty()->SetSpecularPower(30);
    m_fixedPointActor->SetMapper(fixedPointMapper);
    m_renderer->AddActor(m_fixedPointActor);
    //--------------------------------------------
    vtkNew<vtkGlyph3DMapper> surfaceCtrlPointMapper;
    surfaceCtrlPointMapper->SetInputData(
        m_surfaceCtrlVertexFilter->GetOutput());
    surfaceCtrlPointMapper->SetSourceConnection(sphereSource3->GetOutputPort());
    surfaceCtrlPointMapper->ScalingOff();
    surfaceCtrlPointMapper->ScalarVisibilityOff();
    m_surfaceCtrlPointActor->GetProperty()->SetColor(0.0, 0.0, 1.0);
    m_surfaceCtrlPointActor->GetProperty()->SetDiffuse(0.8);
    m_surfaceCtrlPointActor->GetProperty()->SetSpecular(0.5);
    m_surfaceCtrlPointActor->GetProperty()->SetSpecularPower(80);
    m_surfaceCtrlPointActor->SetPickable(0);
    m_surfaceCtrlPointActor->Modified();
    m_surfaceCtrlPointActor->SetMapper(surfaceCtrlPointMapper);
    m_renderer->AddActor(m_surfaceCtrlPointActor);

    vtkNew<vtkGlyph3DMapper> surfaceCurveCtrlPointMapper;
    surfaceCurveCtrlPointMapper->SetInputData(
        m_surfaceCurveCtrlVertexFilter->GetOutput());
    surfaceCurveCtrlPointMapper->SetSourceConnection(
        sphereSource2->GetOutputPort());
    surfaceCurveCtrlPointMapper->ScalingOff();
    surfaceCurveCtrlPointMapper->ScalarVisibilityOff();
    m_surfaceCurveCtrlPointActor->GetProperty()->SetColor(1.0, 0.0, 1.0);
    m_surfaceCurveCtrlPointActor->GetProperty()->SetDiffuse(0.8);
    m_surfaceCurveCtrlPointActor->GetProperty()->SetSpecular(0.5);
    m_surfaceCurveCtrlPointActor->GetProperty()->SetSpecularPower(80);
    m_surfaceCurveCtrlPointActor->SetMapper(surfaceCurveCtrlPointMapper);
    m_renderer->AddActor(m_surfaceCurveCtrlPointActor);

    vtkNew<vtkPolyDataMapper> surfaceEdgeMapper;
    m_surfaceTubeFilter->SetRadius(0.1);
    m_surfaceTubeFilter->SetNumberOfSides(10);
    surfaceEdgeMapper->SetInputData(m_surfaceTubeFilter->GetOutput());
    surfaceEdgeMapper->SetScalarVisibility(0);
    m_surfaceEdgeActor->SetMapper(surfaceEdgeMapper);
    m_surfaceEdgeActor->GetProperty()->SetColor(1.0, 0.0, 1.0);
    m_surfaceEdgeActor->GetProperty()->SetDiffuse(0.8);
    m_surfaceEdgeActor->GetProperty()->SetAmbient(0.2);
    m_surfaceEdgeActor->GetProperty()->SetSpecular(0.3);
    m_surfaceEdgeActor->GetProperty()->SetSpecularPower(20);
    m_surfaceEdgeActor->SetPickable(0);
    m_renderer->AddActor(m_surfaceEdgeActor);

    vtkNew<vtkPolyDataMapper> surfaceCurveMapperDeactive;
    m_surfaceCurveTubeFilterDeactive->SetRadius(0.3);
    m_surfaceCurveTubeFilterDeactive->SetNumberOfSides(10);
    surfaceCurveMapperDeactive->SetInputData(
        m_surfaceCurveTubeFilterDeactive->GetOutput());
    surfaceCurveMapperDeactive->SetScalarVisibility(0);
    m_surfaceCurveActorDeactive->SetMapper(surfaceCurveMapperDeactive);
    m_surfaceCurveActorDeactive->GetProperty()->SetColor(0.0, 0.0, 1.0);
    m_surfaceCurveActorDeactive->GetProperty()->SetDiffuse(0.8);
    m_surfaceCurveActorDeactive->GetProperty()->SetAmbient(0.2);
    m_surfaceCurveActorDeactive->GetProperty()->SetSpecular(0.3);
    m_surfaceCurveActorDeactive->GetProperty()->SetSpecularPower(20);
    m_surfaceCurveActorDeactive->SetPickable(0);
    m_renderer->AddActor(m_surfaceCurveActorDeactive);

    vtkNew<vtkPolyDataMapper> surfaceCurveMapper;
    m_surfaceCurveTubeFilter->SetRadius(0.3);
    m_surfaceCurveTubeFilter->SetNumberOfSides(10);
    surfaceCurveMapper->SetInputData(m_surfaceCurveTubeFilter->GetOutput());
    surfaceCurveMapper->SetScalarVisibility(0);
    m_surfaceCurveActor->SetMapper(surfaceCurveMapper);
    m_surfaceCurveActor->GetProperty()->SetColor(
        colors->GetColor3d("RosyBrown").GetData());
    m_surfaceCurveActor->GetProperty()->SetDiffuse(0.8);
    m_surfaceCurveActor->GetProperty()->SetAmbient(0.2);
    m_surfaceCurveActor->GetProperty()->SetSpecular(0.3);
    m_surfaceCurveActor->GetProperty()->SetSpecularPower(20);
    m_surfaceCurveActor->SetPickable(0);
    m_renderer->AddActor(m_surfaceCurveActor);

    m_surfaceVertexFilter->SetInputData(m_surfacePointsPoly);
    m_surfaceVertexFilter->Update();
    m_surfaceVertexFilter->Modified();
    vtkNew<vtkGlyph3DMapper> surfacePointMapper;
    surfacePointMapper->SetInputData(m_surfaceVertexFilter->GetOutput());
    surfacePointMapper->SetSourceConnection(sphereSource->GetOutputPort());
    surfacePointMapper->ScalingOff();
    surfacePointMapper->ScalarVisibilityOff();
    m_surfacePointActor->GetProperty()->SetColor(0.0, 0.0, 1.0);
    m_surfacePointActor->GetProperty()->SetDiffuse(0.8);
    m_surfacePointActor->GetProperty()->SetSpecular(0.5);
    m_surfacePointActor->GetProperty()->SetSpecularPower(30);
    m_surfacePointActor->SetMapper(surfacePointMapper);
    m_surfacePointActor->SetPickable(0);
    m_renderer->AddActor(m_surfacePointActor);

    vtkNew<vtkGlyph3DMapper> surfacePatchPointMapper;
    surfacePatchPointMapper->SetInputData(
        m_surfacePatchVertexFilter->GetOutput());
    surfacePatchPointMapper->SetSourceConnection(sphereSource->GetOutputPort());
    surfacePatchPointMapper->ScalingOff();
    surfacePatchPointMapper->ScalarVisibilityOff();
    m_surfacePatchPointActor->GetProperty()->SetColor(0.0, 0.0, 1.0);
    m_surfacePatchPointActor->GetProperty()->SetDiffuse(0.8);
    m_surfacePatchPointActor->GetProperty()->SetSpecular(0.5);
    m_surfacePatchPointActor->GetProperty()->SetSpecularPower(30);
    m_surfacePatchPointActor->GetProperty()->SetOpacity(0.5);
    m_surfacePatchPointActor->SetMapper(surfacePatchPointMapper);
    m_surfacePatchPointActor->SetPickable(0);
    m_renderer->AddActor(m_surfacePatchPointActor);

    vtkNew<vtkGlyph3DMapper> surfacePatchPointDeactiveMapper;
    surfacePatchPointDeactiveMapper->SetInputData(
        m_surfacePatchVertexDeactiveFilter->GetOutput());
    surfacePatchPointDeactiveMapper->SetSourceConnection(
        sphereSource->GetOutputPort());
    surfacePatchPointDeactiveMapper->ScalingOff();
    surfacePatchPointDeactiveMapper->ScalarVisibilityOff();
    m_surfacePatchPointDeactiveActor->GetProperty()->SetColor(0.0, 0.0, 1.0);
    m_surfacePatchPointDeactiveActor->GetProperty()->SetDiffuse(0.8);
    m_surfacePatchPointDeactiveActor->GetProperty()->SetSpecular(0.5);
    m_surfacePatchPointDeactiveActor->GetProperty()->SetSpecularPower(30);
    m_surfacePatchPointDeactiveActor->GetProperty()->SetOpacity(0.5);
    m_surfacePatchPointDeactiveActor->SetMapper(
        surfacePatchPointDeactiveMapper);
    m_surfacePatchPointDeactiveActor->SetPickable(0);
    m_renderer->AddActor(m_surfacePatchPointDeactiveActor);

    vtkNew<vtkGlyph3DMapper> surfaceCurveCtrlPtsDeactiveMapper;
    surfaceCurveCtrlPtsDeactiveMapper->SetInputData(
        m_surfaceCurveVertexDeactiveFilter->GetOutput());
    surfaceCurveCtrlPtsDeactiveMapper->SetSourceConnection(
        sphereSource->GetOutputPort());
    surfaceCurveCtrlPtsDeactiveMapper->ScalingOff();
    surfaceCurveCtrlPtsDeactiveMapper->ScalarVisibilityOff();
    m_surfaceCurveCtrlPtsDeactiveActor->GetProperty()->SetColor(0.0, 1.0, 1.0);
    m_surfaceCurveCtrlPtsDeactiveActor->GetProperty()->SetDiffuse(0.8);
    m_surfaceCurveCtrlPtsDeactiveActor->GetProperty()->SetSpecular(0.8);
    m_surfaceCurveCtrlPtsDeactiveActor->GetProperty()->SetSpecularPower(30);
    m_surfaceCurveCtrlPtsDeactiveActor->GetProperty()->SetOpacity(0.5);
    m_surfaceCurveCtrlPtsDeactiveActor->SetMapper(
        surfaceCurveCtrlPtsDeactiveMapper);
    m_surfaceCurveCtrlPtsDeactiveActor->SetPickable(0);
    m_renderer->AddActor(m_surfaceCurveCtrlPtsDeactiveActor);

    vtkNew<vtkArrowSource> surfaceArrow;
    surfaceArrow->SetTipResolution(16);
    surfaceArrow->Update();
    vtkNew<vtkPolyData> emptyArrow;
    m_glyphSurfaceArrow->SetInputData(emptyArrow);
    m_glyphSurfaceArrow->SetSourceData(surfaceArrow->GetOutput());
    m_glyphSurfaceArrow->SetVectorModeToUseVector();
    m_glyphSurfaceArrow->SetScaleModeToScaleByVector();
    m_glyphSurfaceArrow->SetScaleFactor(10);
    m_glyphSurfaceArrow->Update();

    vtkNew<vtkPolyDataMapper> glyph3DSurfaceMapper;
    glyph3DSurfaceMapper->SetInputData(m_glyphSurfaceArrow->GetOutput());
    m_surfaceArrowActor->SetMapper(glyph3DSurfaceMapper);
    m_surfaceArrowActor->GetProperty()->SetColor(0.0, 0.0, 1.0);
    m_surfaceArrowActor->SetPickable(0);
    m_renderer->AddActor(m_surfaceArrowActor);

    //--------------------------------------------
    vtkNew<vtkGlyph3DMapper> curvectrlPointMapper;
    curvectrlPointMapper->SetInputData(m_curvectrlVertexFilter->GetOutput());
    curvectrlPointMapper->SetSourceConnection(sphereSource2->GetOutputPort());
    curvectrlPointMapper->ScalingOff();
    curvectrlPointMapper->ScalarVisibilityOff();
    m_curvectrlPointActor->GetProperty()->SetColor(1.0, 0.0, 1.0);
    m_curvectrlPointActor->GetProperty()->SetDiffuse(0.8);
    m_curvectrlPointActor->GetProperty()->SetSpecular(0.5);
    m_curvectrlPointActor->GetProperty()->SetSpecularPower(80);
    m_curvectrlPointActor->SetMapper(curvectrlPointMapper);
    m_renderer->AddActor(m_curvectrlPointActor);
    //--------------------------------------------
    vtkNew<vtkDataSetMapper> curveMapperDeactive;
    m_curveTubeFilterDeactive->SetRadius(0.3);
    m_curveTubeFilterDeactive->SetNumberOfSides(10);
    curveMapperDeactive->ScalarVisibilityOff();
    curveMapperDeactive->SetInputData(m_curveTubeFilterDeactive->GetOutput());
    m_curveActorDeactive->SetMapper(curveMapperDeactive);
    m_curveActorDeactive->GetProperty()->SetColor(
        colors->GetColor3d("Green").GetData());
    m_curveActorDeactive->GetProperty()->SetLineWidth(3.0);
    m_curveActorDeactive->GetProperty()->SetOpacity(0.5);
    m_curveActorDeactive->SetPickable(0);

    m_renderer->AddActor(m_curveActorDeactive);
    //--------------------------------------------
    vtkNew<vtkDataSetMapper> curveMapper;
    m_curveTubeFilter->SetRadius(0.3);
    m_curveTubeFilter->SetNumberOfSides(10);
    curveMapper->ScalarVisibilityOff();
    curveMapper->SetInputData(m_curveTubeFilter->GetOutput());
    m_curveActor->SetMapper(curveMapper);
    m_curveActor->GetProperty()->SetColor(
        colors->GetColor3d("RosyBrown").GetData());
    m_curveActor->GetProperty()->SetLineWidth(3.0);
    m_curveActor->SetPickable(0);
    m_renderer->AddActor(m_curveActor);
    //--------------------------------------------
    vtkNew<vtkGlyph3DMapper> curvePointMapper;
    curvePointMapper->SetInputData(m_curveVertexFilter->GetOutput());
    curvePointMapper->SetSourceConnection(sphereSource->GetOutputPort());
    curvePointMapper->ScalingOff();
    curvePointMapper->ScalarVisibilityOff();

    m_curvePointActor->GetProperty()->SetColor(0.0, 1.0, 0.0);
    m_curvePointActor->SetMapper(curvePointMapper);
    m_curvePointActor->SetPickable(0);
    m_renderer->AddActor(m_curvePointActor);
    //--------------------------------------------
    vtkNew<vtkGlyph3DMapper> curvePointMapperDeactive;
    curvePointMapperDeactive->SetInputData(
        m_curveVertexFilterDeactive->GetOutput());
    curvePointMapperDeactive->SetSourceConnection(
        sphereSource->GetOutputPort());
    curvePointMapperDeactive->ScalingOff();
    curvePointMapperDeactive->ScalarVisibilityOff();
    m_curvePointActorDeactive->GetProperty()->SetColor(0.0, 1.0, 0.0);
    m_curvePointActorDeactive->GetProperty()->SetOpacity(0.5);
    m_curvePointActorDeactive->SetMapper(curvePointMapperDeactive);
    m_curvePointActorDeactive->SetPickable(0);
    m_renderer->AddActor(m_curvePointActorDeactive);

    //--------------------------------------------
    vtkNew<vtkArrowSource> curveArrow;
    curveArrow->SetTipResolution(16);
    curveArrow->Update();
    m_glyphCurveArrow->SetInputData(emptyArrow);
    m_glyphCurveArrow->SetSourceData(curveArrow->GetOutput());
    m_glyphCurveArrow->SetVectorModeToUseVector();
    m_glyphCurveArrow->SetScaleModeToScaleByVector();
    m_glyphCurveArrow->SetScaleFactor(10);
    m_glyphCurveArrow->Update();

    vtkNew<vtkPolyDataMapper> glyph3DCurveMapper;
    glyph3DCurveMapper->SetInputData(m_glyphCurveArrow->GetOutput());
    m_curveArrowActor->SetMapper(glyph3DCurveMapper);
    m_curveArrowActor->GetProperty()->SetColor(0.0, 1.0, 0.0);
    m_curveArrowActor->SetPickable(0);
    m_renderer->AddActor(m_curveArrowActor);
    //--------------------------------------------
    // Label properties
    vtkNew<vtkLabeledDataMapper> labelMapper;
    labelMapper->SetInputData(m_fixedVertexFilter->GetOutput());
    labelMapper->GetLabelTextProperty()->SetFontSize(15);
    vtkNew<vtkActor2D> labelActor;
    labelActor->SetMapper(labelMapper);
    labelActor->GetProperty()->SetColor(colors->GetColor3d("Orange").GetData());
    labelActor->SetPickable(0);
    m_renderer->AddActor(labelActor);

    vtkNew<vtkLabeledDataMapper> surfaceLabelMapper;
    surfaceLabelMapper->SetInputData(m_surfaceLabelVertexFilter->GetOutput());
    surfaceLabelMapper->GetLabelTextProperty()->SetFontSize(15);
    vtkNew<vtkActor2D> surfaceLabelActor;
    surfaceLabelActor->SetMapper(surfaceLabelMapper);
    surfaceLabelActor->GetProperty()->SetColor(
        colors->GetColor3d("cadetblue").GetData());
    surfaceLabelActor->SetPickable(0);
    m_renderer->AddActor(surfaceLabelActor);

    vtkNew<vtkLabeledDataMapper> curveLabelMapper;
    curveLabelMapper->SetInputData(m_curveLabelVertexFilter->GetOutput());
    curveLabelMapper->GetLabelTextProperty()->SetFontSize(15);
    vtkNew<vtkActor2D> curveLabelActor;
    curveLabelActor->SetMapper(curveLabelMapper);
    curveLabelActor->GetProperty()->SetColor(
        colors->GetColor3d("palegreen").GetData());
    curveLabelActor->SetPickable(0);
    m_renderer->AddActor(curveLabelActor);

    // Ambient properties
    m_renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

    // Observers
    m_PointPickerStyle->AddObserver(vtkCommand::LeftButtonPressEvent, this,
                                    &TemplateDigitiser::PickFunc, 1);
    m_PointPickerStyle->AddObserver(vtkCommand::LeftButtonReleaseEvent, this,
                                    &TemplateDigitiser::resetLeftClck, 1);
    m_PointPickerStyle->AddObserver(vtkCommand::MiddleButtonPressEvent, this,
                                    &TemplateDigitiser::MoveFunc, 1);
    m_PointMoverStyle->AddObserver(vtkCommand::MiddleButtonReleaseEvent, this,
                                   &TemplateDigitiser::resetMouseMove, 1);
    m_PointMoverStyle->AddObserver(vtkCommand::MouseMoveEvent, this,
                                   &TemplateDigitiser::CoordinateFunc, 1);
    //--------------------------------------------------------------------
    m_PointPainterStyle->AddObserver(vtkCommand::MouseMoveEvent, this,
                                     &TemplateDigitiser::PaintFunc, 1);
    m_PointPainterStyle->AddObserver(vtkCommand::LeftButtonPressEvent, this,
                                     &TemplateDigitiser::PaintMouseClicked, 1);
    m_PointPainterStyle->AddObserver(vtkCommand::LeftButtonReleaseEvent, this,
                                     &TemplateDigitiser::PaintMouseReleased, 1);

    QShortcut* selectAllShortcut =
        new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_A), this);
    QObject::connect(selectAllShortcut, &QShortcut::activated, this,
                     &TemplateDigitiser::SelectAll);
    QShortcut* deSelectAllShortcut =
        new QShortcut(QKeySequence(Qt::SHIFT + Qt::Key_A), this);
    QObject::connect(deSelectAllShortcut, &QShortcut::activated, this,
                     &TemplateDigitiser::DeSelectAll);
    //--------------------------------------------------------------------

    m_renWin->Render();
    m_iren->Start();
}

void TemplateDigitiser::PaintMouseClicked() {
    m_mouseIsClicked = 1;
    m_PointPainterStyle->OnLeftButtonDown();
}

void TemplateDigitiser::PaintMouseReleased() {
    m_mouseIsClicked = 0;
    m_PointPainterStyle->OnLeftButtonUp();
}

void TemplateDigitiser::Resample(int resolution, vtkPolyData* mesh) {
    std::vector<int>* idList = new std::vector<int>;
    std::vector<double>* probab = new std::vector<double>;
    std::vector<double>* distance = new std::vector<double>;
    std::vector<int>* tempPairedPts = new std::vector<int>;
    std::vector<int>* pairedPts = new std::vector<int>;

    vtkNew<vtkPolyData> paintedPoly;
    if (m_ignoreInside) {  // just in case, if the mesh has inner parts
        vtkNew<vtkPolyDataConnectivityFilter> Selector;
        Selector->SetInputData(mesh);
        Selector->SetExtractionModeToLargestRegion();
        Selector->Update();

        vtkNew<vtkThreshold> threshold;
        threshold->SetInputData(Selector->GetOutput());
        threshold->ThresholdBetween(0, 0);
        threshold->SetInputArrayToProcess(
            0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_CELLS, "Masked");
        threshold->Update();
        vtkNew<vtkGeometryFilter> geometry;
        geometry->SetInputData(threshold->GetOutput());
        geometry->Update();
        vtkNew<vtkCleanPolyData> cleanFilter;
        cleanFilter->SetInputData(geometry->GetOutput());
        cleanFilter->Update();
        paintedPoly->DeepCopy(cleanFilter->GetOutput());
    } else if (!m_ignoreInside) {
        vtkNew<vtkThreshold> threshold;
        threshold->SetInputData(mesh);
        threshold->ThresholdBetween(0, 0);
        threshold->SetInputArrayToProcess(
            0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_CELLS, "Masked");
        threshold->Update();
        vtkNew<vtkGeometryFilter> geometry;
        geometry->SetInputData(threshold->GetOutput());
        geometry->Update();
        vtkNew<vtkCleanPolyData> cleanFilter;
        cleanFilter->SetInputData(geometry->GetOutput());
        cleanFilter->Update();
        paintedPoly->DeepCopy(cleanFilter->GetOutput());
    }

    std::vector<int>* shuffledIds =
        new std::vector<int>(paintedPoly->GetNumberOfPoints());
    std::generate(shuffledIds->begin(), shuffledIds->end(),
                  [n = 0]() mutable { return n++; });
    auto rng = std::default_random_engine{};
    std::shuffle(std::begin(*shuffledIds), std::end(*shuffledIds), rng);
    vtkNew<vtkPoints> resampledPt;
    vtkNew<vtkPoints> tempPt;
    double totalArea = GetMeshCellArea(probab, paintedPoly);

    if (paintedPoly->GetNumberOfPoints() > 5000) {
        RandomChoice(paintedPoly, 5000, probab, idList, resampledPt);
        resampledPt->Modified();
        vtkNew<vtkPolyData> resampledPtsPoly;
        resampledPtsPoly->SetPoints(resampledPt);
        resampledPtsPoly->Modified();
        if (paintedPoly->GetNumberOfPoints() > (resolution * 5)) {
            PoissonDisk(resampledPtsPoly, m_fixedHighlightPoints,
                        m_curveHighlightPointsTotal, totalArea, resolution,
                        tempPt);
        } else {
            QMessageBox warning;
            warning.setText(
                "Number of vertices in the mesh is too few for resampling."
                "Reduce number of surface sliders or "
                "Remesh your geometry with higher resolution!");
            warning.exec();
        }
    } else {
        if (paintedPoly->GetNumberOfPoints() > (resolution * 5)) {
            PoissonDisk(paintedPoly, m_fixedHighlightPoints,
                        m_curveHighlightPointsTotal, totalArea, resolution,
                        tempPt);
        } else {
            QMessageBox warning;
            warning.setText(
                "Number of vertices in the mesh is too few for resampling."
                "Reduce number of surface sliders or "
                "Remesh your geometry with higher resolution!");
            warning.exec();
        }
    }

    delete idList;
    delete probab;
    delete distance;
    delete pairedPts;
    delete tempPairedPts;
    delete shuffledIds;
}

void TemplateDigitiser::PoissonDisk(vtkPolyData* inputPoly, vtkPoints* fixedLm,
                               vtkPoints* curveSliders, double area, int resol,
                               vtkPoints* outPutPts) {
    bool hasFixed = 1;
    bool hasCurve = 1;

    if (m_typeINOL > 0 && fixedLm->GetNumberOfPoints() != m_typeINOL) {
        hasFixed = false;
    }
    if (m_curveNOS > 0 && curveSliders->GetNumberOfPoints() != m_curveNOS) {
        hasCurve = false;
    }
    if (!hasFixed || !hasCurve) {
        if (QMessageBox::Yes ==
            QMessageBox::question(this, "Warning",
                                  "To avoid landmark coincident, first "
                                  "digitize fixed and / or curve landmarks."
                                  "Do you want to continue?",
                                  QMessageBox::Yes | QMessageBox::No)) {
            PrepareBNSampling(1);
            delete m_BlueNoiseThread;
            m_BlueNoiseThread = new BlueNoiseThread(inputPoly, area, resol,
                                                    m_surfaceHighlightPoints, m_mutex);
            m_BlueNoiseThread->setParent(this);

            connect(m_BlueNoiseThread, &BlueNoiseThread::SamplingIsDone, this,
                    &TemplateDigitiser::OnSamplingIsDone);
            RunStatThread(m_BlueNoiseThread);
            m_BlueNoiseThread->start();
        }
    } else {
        PrepareBNSampling(1);
        delete m_BlueNoiseThread;
        m_BlueNoiseThread =
            new BlueNoiseThread(inputPoly, fixedLm, curveSliders, area, resol,
                                m_surfaceHighlightPoints, m_mutex);
        m_BlueNoiseThread->setParent(this);

        connect(m_BlueNoiseThread, &BlueNoiseThread::SamplingIsDone, this,
                &TemplateDigitiser::OnSamplingIsDone);
        RunStatThread(m_BlueNoiseThread);

        m_BlueNoiseThread->start();
    }
}

void ::TemplateDigitiser::PrepareBNSampling(bool stat) {
    if (stat) {
        m_surfacePointsPoly->Initialize();
        m_surfacePointsPoly->Modified();
        m_surfaceVertexFilter->Update();
        m_surfaceVertexFilter->Modified();
        m_renderer->GetRenderWindow()->Render();
        surfaceSliderButton->setEnabled(0);
        surfacePaintButton->setEnabled(0);
        surfaceResampleButton->setEnabled(0);
        ignoreIntCheckbox->setEnabled(0);
    } else {
        m_surfaceVertexFilter->Update();
        m_surfaceVertexFilter->Modified();
        m_renderer->GetRenderWindow()->Render();
        surfaceSliderButton->setEnabled(1);
        surfacePaintButton->setEnabled(1);
        surfaceResampleButton->setEnabled(1);
        ignoreIntCheckbox->setEnabled(1);
    }
}

void ::TemplateDigitiser::OnSamplingIsDone() {
    m_mutex->lock();
    if (m_surfaceHighlightPoints->GetNumberOfPoints() == m_surfaceNOS) {
        m_surfacePointsPoly->Initialize();
        m_surfacePointsPoly->SetPoints(m_surfaceHighlightPoints);
        m_surfacePointsPoly->Modified();
        PrepareBNSampling(0);
    }
    m_mutex->unlock();
}

void TemplateDigitiser::OnStatusChanged(int status) {
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
        // progressLineEdit->hide();
    }
}

void TemplateDigitiser::RunStatThread(QThread* thread) {
    delete m_statThread;
    m_statThread = new StatusReporterThread(thread);
    connect(m_statThread, &StatusReporterThread::StatusChanged, this,
            &TemplateDigitiser::OnStatusChanged);
    m_statThread->setParent(this);
    m_statThread->start();
}

double TemplateDigitiser::GetMeshCellArea(std::vector<double>* probab,
                                     vtkPolyData* inputMesh) {
    vtkNew<vtkMeshQuality> qualityFilter;
    qualityFilter->SetInputData(inputMesh);
    qualityFilter->SetTriangleQualityMeasureToArea();
    qualityFilter->Update();
    auto areaArray = dynamic_cast<vtkDoubleArray*>(
        qualityFilter->GetOutput()->GetCellData()->GetArray("Quality"));
    double totalArea = 0;
    for (vtkIdType i = 0; i < areaArray->GetNumberOfTuples(); i++) {
        totalArea += areaArray->GetValue(i);
    }
    for (vtkIdType i = 0; i < areaArray->GetNumberOfTuples(); i++) {
        double val = areaArray->GetValue(i);
        probab->push_back(val / totalArea);
    }
    return totalArea;
}

void TemplateDigitiser::RandomChoice(vtkPolyData* inputMesh, int outputSize,
                                std::vector<double>* probab,
                                std::vector<int>* idList, vtkPoints* points) {
    std::discrete_distribution distribution(probab->begin(), probab->end());
    std::vector<decltype(distribution)::result_type> indices;
    indices.reserve(outputSize);
    std::generate_n(back_inserter(indices), outputSize,
                    [distribution = std::move(
                         distribution),  // could also capture by reference (&)
                                         // or construct in the capture list
                     generator = std::default_random_engine{}
                     // pseudo random. Fixed seed! Always same output.
    ]() mutable {  // mutable required for generator
                        return distribution(generator);
                    });

    for (auto const index : indices) {
        double u = RandomFloat(1), v = RandomFloat(1);
        double A = 1 - sqrt(u);
        double B = sqrt(u) * (1 - v);
        double C = v * sqrt(u);
        double X1 = inputMesh->GetCell(index)->GetPoints()->GetPoint(0)[0];
        double Y1 = inputMesh->GetCell(index)->GetPoints()->GetPoint(0)[1];
        double Z1 = inputMesh->GetCell(index)->GetPoints()->GetPoint(0)[2];

        double X2 = inputMesh->GetCell(index)->GetPoints()->GetPoint(1)[0];
        double Y2 = inputMesh->GetCell(index)->GetPoints()->GetPoint(1)[1];
        double Z2 = inputMesh->GetCell(index)->GetPoints()->GetPoint(1)[2];

        double X3 = inputMesh->GetCell(index)->GetPoints()->GetPoint(2)[0];
        double Y3 = inputMesh->GetCell(index)->GetPoints()->GetPoint(2)[1];
        double Z3 = inputMesh->GetCell(index)->GetPoints()->GetPoint(2)[2];

        double x = A * X1 + B * X2 + C * X3;
        double y = A * Y1 + B * Y2 + C * Y3;
        double z = A * Z1 + B * Z3 + C * Z3;
        points->InsertNextPoint(x, y, z);
        idList->push_back(index);
    }
}

double TemplateDigitiser::RandomFloat(double maximum) {
    return (double)rand() / (double)(RAND_MAX / maximum);
}

double TemplateDigitiser::EucDist(double Ax, double Ay, double Az, double Bx,
                             double By, double Bz) {
    double dx = Ax - Bx;
    double dy = Ay - By;
    double dz = Az - Bz;
    double dist = sqrt((dx * dx) + (dy * dy) + (dz * dz));
    return dist;
}

void TemplateDigitiser::ProjectOnMesh(vtkPoints* point) {
    vtkNew<vtkCellLocator> ptLocator;
    ptLocator->SetDataSet(m_meshData);
    ptLocator->BuildLocator();
    for (int i = 0; i < point->GetNumberOfPoints(); i++) {
        double closestPoint[3];
        vtkIdType closestCellId = -1;
        int subId = -1;
        double dist = -1;
        ptLocator->FindClosestPoint(point->GetPoint(i), closestPoint,
                                    closestCellId, subId, dist);
        point->SetPoint(i, closestPoint);
    }
    point->Modified();
}

void TemplateDigitiser::ProjectOnMesh(vtkPoints* point, vtkPolyData* mask) {
    vtkNew<vtkCellLocator> ptLocator;
    ptLocator->SetDataSet(mask);
    ptLocator->BuildLocator();
    for (int i = 0; i < point->GetNumberOfPoints(); i++) {
        double closestPoint[3];
        vtkIdType closestCellId = -1;
        int subId = -1;
        double dist = -1;
        ptLocator->FindClosestPoint(point->GetPoint(i), closestPoint,
                                    closestCellId, subId, dist);
        point->SetPoint(i, closestPoint);
    }
    point->Modified();
}

void TemplateDigitiser::ProjectOnMesh(vtkPolyData* Poly, vtkPolyData* mask,
                                 std::vector<int>* ids) {
    /* vtkNew<vtkSelectPolyDataModified> loop;
    loop->SetLoop(tempPts);
    loop->SetInputData(m_meshData);
    loop->GenerateSelectionScalarsOn();
    loop->SetSelectionModeToLargestRegion();
    loop->SetInsideOut(0);
    loop->SetEdgeSearchModeToDijkstra();
    loop->Update();

    vtkNew<vtkClipPolyData> clip;
    clip->SetInputConnection(loop->GetOutputPort());
    clip->SetValue(0.0);
    clip->Update(); */

    vtkNew<vtkCellLocator> ptLocator;
    ptLocator->SetDataSet(mask);
    ptLocator->BuildLocator();
    if (ids) {
        for (int i = 0; i < Poly->GetNumberOfPoints(); i++) {
            if (std::find(ids->begin(), ids->end(), i) != ids->end()) {
            } else {
                double closestPoint[3];
                vtkIdType closestCellId = -1;
                int subId = -1;
                double dist = -1;
                ptLocator->FindClosestPoint(Poly->GetPoint(i), closestPoint,
                                            closestCellId, subId, dist);
                Poly->GetPoints()->SetPoint(i, closestPoint);
            }
        }
    } else {
        for (int i = 0; i < Poly->GetNumberOfPoints(); i++) {
            double closestPoint[3];
            vtkIdType closestCellId = -1;
            int subId = -1;
            double dist = -1;
            ptLocator->FindClosestPoint(Poly->GetPoint(i), closestPoint,
                                        closestCellId, subId, dist);
            Poly->GetPoints()->SetPoint(i, closestPoint);
        }
    }

    /* vtkNew<vtkOBJWriter> writer;
    writer->SetInputData(mask);
    writer->SetFileName("testClipped.obj");
    writer->Update();
    writer->Write(); */
}

void TemplateDigitiser::DijkstraEdgeSearch(vtkPolyData* mesh,
                                      vtkPolyData* closedCurve,
                                      vtkIdList* edgePointIds) {
    vtkNew<vtkDijkstraGraphGeodesicPath> edgeSearchFilter;
    edgeSearchFilter->StopWhenEndReachedOn();
    edgeSearchFilter->SetInputData(mesh);

    vtkNew<vtkPointLocator> pointLocator;
    pointLocator->SetDataSet(mesh);

    vtkPoints* inPts = mesh->GetPoints();
    vtkIdType numLoopPts = closedCurve->GetNumberOfPoints();

    vtkIdType currentId = 0;
    double xLoop[3];
    closedCurve->GetPoint(0, xLoop);
    vtkIdType nextId = pointLocator->FindClosestPoint(xLoop);
    for (vtkIdType i = 0; i < numLoopPts; i++) {
        currentId = nextId;
        closedCurve->GetPoint((i + 1) % numLoopPts, xLoop);
        nextId = pointLocator->FindClosestPoint(xLoop);

        edgeSearchFilter->SetStartVertex(currentId);
        edgeSearchFilter->SetEndVertex(nextId);
        edgeSearchFilter->Update();
        vtkPolyData* outputPath = edgeSearchFilter->GetOutput();
        double x0[3];
        inPts->GetPoint(currentId, x0);
        for (int j = outputPath->GetNumberOfPoints() - 1; j >= 0; --j) {
            double x[3];
            outputPath->GetPoint(j, x);
            double dist2 = vtkMath::Distance2BetweenPoints(x, x0);
            if (dist2 > 0.0) {
                // Find point ID to add in the input mesh to remember the next
                // edge point
                edgePointIds->InsertNextId(pointLocator->FindClosestPoint(x));
                for (int k = 0; k < 3; ++k) {
                    // Remember last added point so that it does not get added
                    // twice
                    x0[k] = x[k];
                }
            }
        }
    }
}

void TemplateDigitiser::MeshCutter(vtkPolyData* poly, vtkPoints* curvePts,
                              vtkPolyData* outMask) {
    outMask->Initialize();
    vtkNew<vtkIdList> cutterIdList;
    GetCutterCurve(poly, curvePts, cutterIdList);
    m_cutMeshData->Initialize();
    m_cutMeshData->DeepCopy(poly);
    m_cutMeshData->BuildLinks();
    for (int i = 0; i < cutterIdList->GetNumberOfIds(); i++) {
        m_cutMeshData->DeleteCell(cutterIdList->GetId(i));
    }
    m_cutMeshData->RemoveDeletedCells();

    vtkNew<vtkConnectivityFilter> cF;
    cF->SetInputData(m_cutMeshData);
    cF->SetExtractionModeToAllRegions();
    cF->ColorRegionsOn();
    cF->Update();
    cyclePatchesComboBox->blockSignals(true);
    cyclePatchesComboBox->clear();

    for (int i = 0; i < cF->GetNumberOfExtractedRegions(); i++) {
        cyclePatchesComboBox->addItem(
            QString::fromStdString(std::to_string(i)));
    }
    vtkNew<vtkThreshold> patchSelector;
    patchSelector->SetInputData(cF->GetOutput());
    if (cF->GetNumberOfExtractedRegions() > 1) {
        patchSelector->ThresholdBetween(1, 1);
        patchSelector->Update();
        cyclePatchesComboBox->setCurrentIndex(1);
    } else {
        patchSelector->ThresholdBetween(0, 0);
        patchSelector->Update();
    }
    cyclePatchesComboBox->blockSignals(false);
    vtkNew<vtkGeometryFilter> geometryF;
    geometryF->SetInputConnection(patchSelector->GetOutputPort());
    geometryF->Update();
    outMask->DeepCopy(geometryF->GetOutput());
}

void TemplateDigitiser::GetCutterCurve(vtkPolyData* Poly, vtkPoints* curvePts,
                                  vtkIdList* outCurveIds) {
    /* vtkNew<vtkCellArray> tempLine;
    tempLine->InsertNextCell(curvePts->GetNumberOfPoints() + 1);
    for(int i=0; i < curvePts->GetNumberOfPoints(); i++){
        tempLine->InsertCellPoint(i);
    }
    tempLine->InsertCellPoint(0);
    vtkNew<vtkPolyData> tempCurvePoly;
    tempCurvePoly->SetPoints(curvePts);
    tempCurvePoly->SetLines(tempLine);
    vtkNew<vtkTubeFilter> tubeFilter;
    tubeFilter->SetRadius(0.1);
    tubeFilter->SetNumberOfSides(10);
    tubeFilter->SetInputData(tempCurvePoly);
    tubeFilter->Update();
    vtkNew<vtkIntersectionPolyDataFilter> intF;
    intF->SetInputDataObject(0, Poly);
    intF->SetInputDataObject(1, tubeFilter->GetOutput());
    intF->Update();  */
    // intF->GetOutputDataObject(1); for remeshed m_meshData

    outCurveIds->Initialize();
    vtkNew<vtkPoints> tempPts;
    vtkNew<vtkPointLocator> curvePtLocator;
    curvePtLocator->SetDataSet(Poly);

    for (int i = 0; i < curvePts->GetNumberOfPoints(); i++) {
        vtkIdType id = curvePtLocator->FindClosestPoint(curvePts->GetPoint(i));
        double closestPoint[3];
        curvePtLocator->GetDataSet()->GetPoint(id, closestPoint);
        tempPts->InsertNextPoint(closestPoint);
    }

    vtkNew<vtkCellArray> line;
    line->InsertNextCell(tempPts->GetNumberOfPoints() + 1);
    for (int i = 0; i < tempPts->GetNumberOfPoints(); i++) {
        line->InsertCellPoint(i);
    }
    line->InsertCellPoint(0);
    vtkNew<vtkPolyData> curvePoly;
    curvePoly->SetPoints(tempPts);
    curvePoly->SetLines(line);

    vtkNew<vtkIdList> edgePointIds;
    edgePointIds->Allocate(curvePoly->GetNumberOfPoints() * 10, 1000);
    DijkstraEdgeSearch(Poly, curvePoly, edgePointIds);

    for (int i = 0; i < Poly->GetNumberOfCells(); i++) {
        auto tempCellIds = Poly->GetCell(i)->GetPointIds();
        for (int j = 0; j < edgePointIds->GetNumberOfIds(); j++) {
            if (tempCellIds->IsId(edgePointIds->GetId(j)) != -1) {
                outCurveIds->InsertNextId(i);
                break;
            }
        }
    }

    /* vtkNew<vtkOBJWriter> writer;
    writer->SetInputData(intF->GetOutputDataObject(1));
    writer->SetFileName("testRemeshed.obj");
    writer->Update();
    writer->Write(); */
}

void TemplateDigitiser::ChangeBrushSize(int index) {
    if (m_meshData->GetNumberOfCells() > index) {
        m_brushSize = index;
    } else {
        brushSizeSpinBox->setValue(1);
        m_brushSize = 1;
    }
}

void TemplateDigitiser::ChangePointSize(int index) {
    index *= 2;
    // Point properties and color etc
    vtkNew<vtkMassProperties> prop;
    prop->SetInputData(m_meshData);
    prop->Update();
    double area = prop->GetSurfaceArea();
    area = std::sqrt(area);
    double sizeConstant =
        (m_typeINOL + m_surfaceNOS +
         (m_surfacePatchNOP * m_surfacePatchUNOS * m_surfacePatchVNOS) +
         (m_curveNOS * m_curveNOC));
    if (sizeConstant < 100) {
        sizeConstant = 100;
    }
    if (sizeConstant > 300) {
        sizeConstant = 300;
    }
    vtkNew<vtkSphereSource> sphereSource;
    vtkNew<vtkSphereSource> sphereSource2;
    vtkNew<vtkSphereSource> sphereSource3;
    if (index < 0) {
        sizeConstant -= index * 9;
        sphereSource->SetRadius(area / sizeConstant);
        sphereSource2->SetRadius(area / sizeConstant * 1.5);
        sphereSource3->SetRadius(area / sizeConstant * 0.5);
    }
    if (index > 0) {
        sizeConstant -= index * 9;

        sphereSource->SetRadius(area / sizeConstant);
        sphereSource2->SetRadius(area / sizeConstant * 1.5);
        sphereSource3->SetRadius(area / sizeConstant * 0.5);
    }
    if (index == 0) {
        sphereSource->SetRadius(area / sizeConstant);
        sphereSource2->SetRadius(area / sizeConstant * 1.5);
        sphereSource3->SetRadius(area / sizeConstant * 0.5);
    }

    vtkNew<vtkGlyph3DMapper> fixedPointMapper;
    fixedPointMapper->SetInputData(m_fixedVertexFilter->GetOutput());
    fixedPointMapper->SetSourceConnection(sphereSource->GetOutputPort());
    fixedPointMapper->ScalingOff();
    fixedPointMapper->ScalarVisibilityOff();
    m_fixedPointActor->SetMapper(fixedPointMapper);

    vtkNew<vtkGlyph3DMapper> surfaceCtrlPointMapper;
    surfaceCtrlPointMapper->SetInputData(
        m_surfaceCtrlVertexFilter->GetOutput());
    surfaceCtrlPointMapper->SetSourceConnection(sphereSource3->GetOutputPort());
    surfaceCtrlPointMapper->ScalingOff();
    surfaceCtrlPointMapper->ScalarVisibilityOff();
    m_surfaceCtrlPointActor->SetMapper(surfaceCtrlPointMapper);

    vtkNew<vtkGlyph3DMapper> surfaceCurveCtrlPointMapper;
    surfaceCurveCtrlPointMapper->SetInputData(
        m_surfaceCurveCtrlVertexFilter->GetOutput());
    surfaceCurveCtrlPointMapper->SetSourceConnection(
        sphereSource2->GetOutputPort());
    surfaceCurveCtrlPointMapper->ScalingOff();
    surfaceCurveCtrlPointMapper->ScalarVisibilityOff();
    m_surfaceCurveCtrlPointActor->SetMapper(surfaceCurveCtrlPointMapper);

    vtkNew<vtkGlyph3DMapper> surfacePointMapper;
    surfacePointMapper->SetInputData(m_surfaceVertexFilter->GetOutput());
    surfacePointMapper->SetSourceConnection(sphereSource->GetOutputPort());
    surfacePointMapper->ScalingOff();
    surfacePointMapper->ScalarVisibilityOff();
    m_surfacePointActor->SetMapper(surfacePointMapper);

    vtkNew<vtkGlyph3DMapper> surfacePatchPointMapper;
    surfacePatchPointMapper->SetInputData(
        m_surfacePatchVertexFilter->GetOutput());
    surfacePatchPointMapper->SetSourceConnection(sphereSource->GetOutputPort());
    surfacePatchPointMapper->ScalingOff();
    surfacePatchPointMapper->ScalarVisibilityOff();
    m_surfacePatchPointActor->SetMapper(surfacePatchPointMapper);

    vtkNew<vtkGlyph3DMapper> surfacePatchPointDeactiveMapper;
    surfacePatchPointDeactiveMapper->SetInputData(
        m_surfacePatchVertexDeactiveFilter->GetOutput());
    surfacePatchPointDeactiveMapper->SetSourceConnection(
        sphereSource->GetOutputPort());
    surfacePatchPointDeactiveMapper->ScalingOff();
    surfacePatchPointDeactiveMapper->ScalarVisibilityOff();
    m_surfacePatchPointDeactiveActor->SetMapper(
        surfacePatchPointDeactiveMapper);

    vtkNew<vtkGlyph3DMapper> surfaceCurveCtrlPtsDeactiveMapper;
    surfaceCurveCtrlPtsDeactiveMapper->SetInputData(
        m_surfaceCurveVertexDeactiveFilter->GetOutput());
    surfaceCurveCtrlPtsDeactiveMapper->SetSourceConnection(
        sphereSource->GetOutputPort());
    surfaceCurveCtrlPtsDeactiveMapper->ScalingOff();
    surfaceCurveCtrlPtsDeactiveMapper->ScalarVisibilityOff();
    m_surfaceCurveCtrlPtsDeactiveActor->SetMapper(
        surfaceCurveCtrlPtsDeactiveMapper);

    vtkNew<vtkGlyph3DMapper> curvectrlPointMapper;
    curvectrlPointMapper->SetInputData(m_curvectrlVertexFilter->GetOutput());
    curvectrlPointMapper->SetSourceConnection(sphereSource2->GetOutputPort());
    curvectrlPointMapper->ScalingOff();
    curvectrlPointMapper->ScalarVisibilityOff();
    m_curvectrlPointActor->SetMapper(curvectrlPointMapper);

    vtkNew<vtkGlyph3DMapper> curvePointMapper;
    curvePointMapper->SetInputData(m_curveVertexFilter->GetOutput());
    curvePointMapper->SetSourceConnection(sphereSource->GetOutputPort());
    curvePointMapper->ScalingOff();
    curvePointMapper->ScalarVisibilityOff();
    m_curvePointActor->SetMapper(curvePointMapper);

    vtkNew<vtkGlyph3DMapper> curvePointMapperDeactive;
    curvePointMapperDeactive->SetInputData(
        m_curveVertexFilterDeactive->GetOutput());
    curvePointMapperDeactive->SetSourceConnection(
        sphereSource->GetOutputPort());
    curvePointMapperDeactive->ScalingOff();
    curvePointMapperDeactive->ScalarVisibilityOff();
    m_curvePointActorDeactive->SetMapper(curvePointMapperDeactive);

    m_renderer->GetRenderWindow()->Render();
}

void TemplateDigitiser::ChangeLineSize(int index) {
    index *= 2;
    if (index > 0) {
        if (m_surfaceTubeFilter->GetOutput()->GetNumberOfPoints() > 0) {
            m_surfaceTubeFilter->SetRadius(0.1 * index);
            m_surfaceTubeFilter->Update();
        }
        if (m_surfaceCurveTubeFilterDeactive->GetOutput()->GetNumberOfPoints() >
            0) {
            m_surfaceCurveTubeFilterDeactive->SetRadius(0.3 * index);
            m_surfaceCurveTubeFilterDeactive->Update();
        }
        if (m_surfaceCurveTubeFilter->GetOutput()->GetNumberOfPoints() > 0) {
            m_surfaceCurveTubeFilter->SetRadius(0.3 * index);
            m_surfaceCurveTubeFilter->Update();
        }
        if (m_curveTubeFilterDeactive->GetOutput()->GetNumberOfPoints() > 0) {
            m_curveTubeFilterDeactive->SetRadius(0.3 * index);
            m_curveTubeFilterDeactive->Update();
        }
        if (m_curveTubeFilter->GetOutput()->GetNumberOfPoints() > 0) {
            m_curveTubeFilter->SetRadius(0.3 * index);
            m_curveTubeFilter->Update();
        }
    }

    if (index < 0) {
        index = std::abs(index);
        if (m_surfaceTubeFilter->GetOutput()->GetNumberOfPoints() > 0) {
            m_surfaceTubeFilter->SetRadius(0.1 / index);
            m_surfaceTubeFilter->Update();
        }
        if (m_surfaceCurveTubeFilterDeactive->GetOutput()->GetNumberOfPoints() >
            0) {
            m_surfaceCurveTubeFilterDeactive->SetRadius(0.3 / index);
            m_surfaceCurveTubeFilterDeactive->Update();
        }
        if (m_surfaceCurveTubeFilter->GetOutput()->GetNumberOfPoints() > 0) {
            m_surfaceCurveTubeFilter->SetRadius(0.3 / index);
            m_surfaceCurveTubeFilter->Update();
        }
        if (m_curveTubeFilterDeactive->GetOutput()->GetNumberOfPoints() > 0) {
            m_curveTubeFilterDeactive->SetRadius(0.3 / index);
            m_curveTubeFilterDeactive->Update();
        }
        if (m_curveTubeFilter->GetOutput()->GetNumberOfPoints() > 0) {
            m_curveTubeFilter->SetRadius(0.3 / index);
            m_curveTubeFilter->Update();
        }
    }

    if (index == 0) {
        if (m_surfaceTubeFilter->GetOutput()->GetNumberOfPoints() > 0) {
            m_surfaceTubeFilter->SetRadius(0.1);
            m_surfaceTubeFilter->Update();
        }
        if (m_surfaceCurveTubeFilterDeactive->GetOutput()->GetNumberOfPoints() >
            0) {
            m_surfaceCurveTubeFilterDeactive->SetRadius(0.3);
            m_surfaceCurveTubeFilterDeactive->Update();
        }
        if (m_surfaceCurveTubeFilter->GetOutput()->GetNumberOfPoints() > 0) {
            m_surfaceCurveTubeFilter->SetRadius(0.3);
            m_surfaceCurveTubeFilter->Update();
        }
        if (m_curveTubeFilterDeactive->GetOutput()->GetNumberOfPoints() > 0) {
            m_curveTubeFilterDeactive->SetRadius(0.3);
            m_curveTubeFilterDeactive->Update();
        }
        if (m_curveTubeFilter->GetOutput()->GetNumberOfPoints() > 0) {
            m_curveTubeFilter->SetRadius(0.3);
            m_curveTubeFilter->Update();
        }
    }

    m_renderer->GetRenderWindow()->Render();
}

void TemplateDigitiser::CycleThroughPatches(int index) {
    m_surfaceMask->Initialize();
    vtkNew<vtkConnectivityFilter> cF;
    cF->SetInputData(m_cutMeshData);
    cF->SetExtractionModeToAllRegions();
    cF->ColorRegionsOn();
    cF->Update();

    vtkNew<vtkThreshold> patchSelector;
    patchSelector->SetInputData(cF->GetOutput());
    /* patchSelector->ThresholdByLower(index);
    patchSelector->ThresholdByUpper(index); */
    patchSelector->ThresholdBetween(index, index);
    patchSelector->Update();

    vtkNew<vtkGeometryFilter> geometryF;
    geometryF->SetInputData(patchSelector->GetOutput());
    geometryF->Update();
    m_surfaceMask->DeepCopy(geometryF->GetOutput());

    Handle(TColgp_HArray1OfPnt) pointArray = new TColgp_HArray1OfPnt(
        1, m_surfaceCurveHighlightCtrlPoints->GetNumberOfPoints());
    for (int i = 0; i < m_surfaceCurveHighlightCtrlPoints->GetNumberOfPoints();
         i++) {
        pointArray->SetValue(
            i + 1, gp_Pnt(m_surfaceCurveHighlightCtrlPoints->GetPoint(i)[0],
                          m_surfaceCurveHighlightCtrlPoints->GetPoint(i)[1],
                          m_surfaceCurveHighlightCtrlPoints->GetPoint(i)[2]));
    }
    Standard_Real tol = 1.0e-7;
    GeomAPI_Interpolate* intp = new GeomAPI_Interpolate(
        pointArray, 1, tol);  // always here we interpolate a closed curve!
    intp->Perform();
    Handle(Geom_BSplineCurve) tempCurve = intp->Curve();

    float delta =
        (tempCurve->LastParameter() - tempCurve->FirstParameter()) / 3;
    vtkNew<vtkPoints> curveInit3Pts;
    for (float i = tempCurve->FirstParameter(); i < 3; i++) {
        float segmnt = tempCurve->FirstParameter() + delta * i;
        curveInit3Pts->InsertNextPoint(tempCurve->Value(segmnt).Coord(1),
                                       tempCurve->Value(segmnt).Coord(2),
                                       tempCurve->Value(segmnt).Coord(3));
        curveInit3Pts->Modified();
    }

    int uRes = m_surfacePatchUNOS + 2;
    int vRes = m_surfacePatchVNOS + 2;

    vtkNew<vtkPolyData> planePolyInit;
    MakeCage(m_surfaceCurveHighlightCtrlPoints, planePolyInit);

    // Now we resample the creatded curve, and move the plane outlines to the
    // resampled points
    int resampleNumber = (((uRes + 1) - 2) * 2) + (((vRes + 1) - 2) * 2);
    delta = (tempCurve->LastParameter() - tempCurve->FirstParameter()) /
            resampleNumber;
    vtkNew<vtkPoints> curveResmplPts;
    for (float i = tempCurve->FirstParameter(); i < resampleNumber; i++) {
        float segmnt = tempCurve->FirstParameter() + delta * i;
        curveResmplPts->InsertNextPoint(tempCurve->Value(segmnt).Coord(1),
                                        tempCurve->Value(segmnt).Coord(2),
                                        tempCurve->Value(segmnt).Coord(3));
        curveResmplPts->Modified();
    }

    std::vector<int>* outlineIds = new std::vector<int>();
    OutlineIdFinder(m_surfacePatchUNOS, m_surfacePatchVNOS, outlineIds);
    vtkNew<vtkCellArray> line;
    line->InsertNextCell(curveResmplPts->GetNumberOfPoints() + 1);
    for (int i = 0; i < curveResmplPts->GetNumberOfPoints(); i++) {
        line->InsertCellPoint(i);
    }
    line->InsertCellPoint(0);
    vtkNew<vtkPolyData> curvePoly;
    curvePoly->SetPoints(curveResmplPts);
    curvePoly->SetLines(line);
    vtkNew<vtkSplineFilter> splineFilter;
    splineFilter->SetInputData(curvePoly);
    splineFilter->SetNumberOfSubdivisions(curveResmplPts->GetNumberOfPoints());
    splineFilter->Update();
    vtkPolyData* tempCtrlPtsPoly = splineFilter->GetOutput();

    m_surfaceCtrlPointsPoly->Initialize();
    m_surfaceCtrlPointsPoly->DeepCopy(planePolyInit);

    vtkNew<vtkPoints> tempSliders;
    for (int i = 0; i < m_surfaceCtrlPointsPoly->GetNumberOfPoints(); i++) {
        if (std::find(outlineIds->begin(), outlineIds->end(), i) !=
            outlineIds->end()) {
        } else {
            tempSliders->InsertNextPoint(m_surfaceCtrlPointsPoly->GetPoint(i));
        }
    }
    if (m_surfaceCtrlPointsPoly->GetNumberOfPoints() > 0) {
        vtkNew<vtkCellLocator> ptLocator;
        ptLocator->SetDataSet(tempCtrlPtsPoly);
        ptLocator->BuildLocator();
        for (int i = 0; i < outlineIds->size(); i++) {
            double closestPoint[3];
            vtkIdType closestCellId = -1;
            int subId = -1;
            double dist = -1;
            ptLocator->FindClosestPoint(
                m_surfaceCtrlPointsPoly->GetPoint(outlineIds->at(i)),
                closestPoint, closestCellId, subId, dist);
            m_surfaceCtrlPointsPoly->GetPoints()->SetPoint(outlineIds->at(i),
                                                           closestPoint);
        }
        m_surfaceCtrlPointsPoly->Modified();
        ProjectOnMesh(tempSliders, m_surfaceMask);
        tempSliders->Modified();
        m_surfacePatchHighlightPoints->Initialize();
        m_surfacePatchHighlightPoints->DeepCopy(tempSliders);
        m_surfacePatchHighlightPoints->Modified();

        vtkNew<vtkPolyData> labelPoly;
        m_surfaceLabelPoints->InsertPoint(
            m_currentSurfaceId, m_surfacePatchHighlightPoints->GetPoint(0));
        labelPoly->SetPoints(m_surfaceLabelPoints);
        m_surfaceLabelVertexFilter->SetInputData(labelPoly);
        m_surfaceLabelVertexFilter->Update();
        m_surfaceLabelVertexFilter->Modified();

        m_surfaceCtrlVertexFilter->SetInputData(m_surfaceCtrlPointsPoly);
        m_surfaceCtrlVertexFilter->Update();
        m_surfaceCtrlVertexFilter->Modified();
        vtkNew<vtkExtractEdges> edgeExtracted;
        edgeExtracted->SetInputData(m_surfaceCtrlPointsPoly);
        edgeExtracted->Update();
        m_surfaceTubeFilter->SetInputData(edgeExtracted->GetOutput());
        m_surfaceTubeFilter->Update();
        m_surfaceTubeFilter->Modified();
        vtkNew<vtkPolyData> tempSurfaceCtrlPoly;
        tempSurfaceCtrlPoly->DeepCopy(m_surfaceCtrlPointsPoly);
        m_surfaceCtrlPtsBlock->SetBlock(m_currentSurfaceId,
                                        tempSurfaceCtrlPoly);

        vtkNew<vtkPolyData> tempSlidersPoly;
        tempSlidersPoly->SetPoints(m_surfacePatchHighlightPoints);
        m_surfacePatchVertexFilter->SetInputData(tempSlidersPoly);
        m_surfacePatchVertexFilter->Update();
        m_surfacePatchVertexFilter->Modified();

        vtkNew<vtkPolyData> tempSlidersPolyCopy;
        tempSlidersPolyCopy->DeepCopy(tempSlidersPoly);
        m_surfaceBlock->SetBlock(m_currentSurfaceId, tempSlidersPolyCopy);
        m_surfaceBlock->Modified();

        vtkNew<vtkPolyData> tempMask;
        tempMask->DeepCopy(m_surfaceMask);
        m_surfaceMaskBlock->SetBlock(m_currentSurfaceId, tempMask);
        m_surfaceMaskBlock->Modified();
    }
    delete outlineIds;

    m_renderer->GetRenderWindow()->Render();
}

void TemplateDigitiser::MakeArrow(vtkPolyData* inputMesh,
                             vtkMultiBlockDataSet* inputCurveBlock,
                             int liftScale, vtkPolyData* output) {
    vtkNew<vtkPointLocator> ptLocator;
    ptLocator->SetDataSet(inputMesh);
    ptLocator->BuildLocator();
    vtkNew<vtkPolyDataNormals> normalFilter;
    normalFilter->SetInputData(inputMesh);
    normalFilter->Update();
    vtkNew<vtkDoubleArray> u;
    u->SetName("u");
    u->SetNumberOfComponents(3);
    u->SetNumberOfTuples(inputCurveBlock->GetNumberOfBlocks());
    vtkNew<vtkDataObjectTreeIterator> iterPts;
    vtkNew<vtkPoints> curveArrowPts;
    curveArrowPts->SetNumberOfPoints(inputCurveBlock->GetNumberOfBlocks());
    iterPts->SetDataSet(inputCurveBlock);
    iterPts->SkipEmptyNodesOn();
    iterPts->VisitOnlyLeavesOn();
    int counter = 0;
    for (iterPts->InitTraversal(); !iterPts->IsDoneWithTraversal();
         iterPts->GoToNextItem()) {
        vtkDataObject* dso = iterPts->GetCurrentDataObject();
        vtkPolyData* pd = dynamic_cast<vtkPolyData*>(dso);
        if (pd->GetNumberOfPoints() > 2) {
            double x1 = pd->GetPoint(1)[0] - pd->GetPoint(0)[0];
            double y1 = pd->GetPoint(1)[1] - pd->GetPoint(0)[1];
            double z1 = pd->GetPoint(1)[2] - pd->GetPoint(0)[2];
            Eigen::VectorXd nromVect(3);
            nromVect.operator()(0) = x1;
            nromVect.operator()(1) = y1;
            nromVect.operator()(2) = z1;
            nromVect.normalize();
            u->SetTuple3(counter, nromVect(0), nromVect(1), nromVect(2));
            vtkIdType id = ptLocator->FindClosestPoint(pd->GetPoint(0));
            double closestPoint[3];
            ptLocator->GetDataSet()->GetPoint(id, closestPoint);
            vtkDataArray* normalArray =
                normalFilter->GetOutput()->GetPointData()->GetNormals();
            double* normalVector = normalArray->GetTuple(id);
            double finalX = pd->GetPoint(0)[0] + (normalVector[0] * liftScale);
            double finalY = pd->GetPoint(0)[1] + (normalVector[1] * liftScale);
            double finalZ = pd->GetPoint(0)[2] + (normalVector[2] * liftScale);
            curveArrowPts->SetPoint(counter, finalX, finalY, finalZ);
        }
        counter += 1;
    }
    output->Initialize();
    output->SetPoints(curveArrowPts);
    output->GetPointData()->SetVectors(u);
    output->Modified();
}

void TemplateDigitiser::TypeITool() {
    if (landmarkButton->isChecked()) {
        curveSliderButton->setChecked(false);
        surfaceSliderButton->setChecked(false);
        SurfaceTool();
        CurveTool();
        m_iren->SetInteractorStyle(m_PointPickerStyle);
        landmarkToolbar->setVisible(true);
    } else {
        m_iren->SetInteractorStyle(m_style);
        landmarkToolbar->setVisible(false);
        if (m_fixedHighlightPoints->GetNumberOfPoints() > 0) {
            if (m_typeINOL == m_fixedHighlightPoints->GetNumberOfPoints()) {
            } else {
                QMessageBox warning;
                warning.setText(
                    "Number of digitized Fixed Landmarks are less than the "
                    "decided number. Nothing will be registered!");
                warning.exec();
            }
        }
    }
}

void TemplateDigitiser::SurfaceTool() {
    if (surfaceSliderButton->isChecked()) {
        curveSliderButton->setChecked(false);
        landmarkButton->setChecked(false);
        TypeITool();
        CurveTool();
        if (m_surfaceNOS != 0) {
            m_iren->SetInteractorStyle(m_PointPainterStyle);
            surfacePatchToolbar->setVisible(false);
            surfaceToolbar->setVisible(true);
        } else {
            m_iren->SetInteractorStyle(m_PointPickerStyle);
            surfacePatchToolbar->setVisible(true);
            surfaceToolbar->setVisible(false);
            if (m_surfaceBlock->GetNumberOfBlocks() > 0) {
                ResetSurfaceScene();
                UpdateSurfaceScene(m_currentSurfaceId);
                surfaceSelectComboBox->setCurrentIndex(m_currentSurfaceId);
                m_surfacePatchPointDeactiveActor->GetProperty()->SetOpacity(
                    0.5);
                m_surfacePatchPointDeactiveActor->Modified();
                m_renderer->GetRenderWindow()->Render();
            }
        }
    } else {
        surfacePatchToolbar->setVisible(false);
        surfaceToolbar->setVisible(false);
        m_iren->SetInteractorStyle(m_style);
        if (m_surfaceBlock->GetNumberOfBlocks() > 0) {
            FinalizeSurfaceScene();
        }
    }
}

void TemplateDigitiser::CurveTool() {
    if (curveSliderButton->isChecked()) {
        surfaceSliderButton->setChecked(false);
        landmarkButton->setChecked(false);
        SurfaceTool();
        TypeITool();
        m_iren->SetInteractorStyle(m_PointPickerStyle);
        curveToolbar->setVisible(true);
        pickFromBoundariesBox->setChecked(0);
        PickFromBoundaries();
        if (m_curveCtrlBlock->GetNumberOfBlocks() > 0) {
            ResetCurveScene();
            UpdateCurveScene(m_currentCurveId);
            curveSelectComboBox->setCurrentIndex(m_currentCurveId);
            m_curvePointActorDeactive->GetProperty()->SetOpacity(0.5);
            m_renderer->GetRenderWindow()->Render();
        }
    } else {
        curveToolbar->setVisible(false);
        pickFromBoundariesBox->setChecked(0);
        PickFromBoundaries();
        m_iren->SetInteractorStyle(m_style);
        if (m_curveCtrlBlock->GetNumberOfBlocks() > 0) {
            FinalizeCurveScene();
        }
    }
}

void TemplateDigitiser::CosmeticCurve(vtkPoints* ctrlPts, vtkPolyData* outputCurve) {
    outputCurve->Initialize();
    if (ctrlPts->GetNumberOfPoints() > 1) {
        if (surfaceSliderButton->isChecked()) {
            vtkNew<vtkCellArray> line;
            line->InsertNextCell(ctrlPts->GetNumberOfPoints() + 1);
            for (int i = 0; i < ctrlPts->GetNumberOfPoints(); i++) {
                line->InsertCellPoint(i);
            }
            line->InsertCellPoint(0);
            vtkNew<vtkPolyData> curvePoly;
            curvePoly->SetPoints(ctrlPts);
            curvePoly->SetLines(line);
            vtkNew<vtkSplineFilter> splineFilter;
            splineFilter->SetInputData(curvePoly);
            // splineFilter->SetNumberOfSubdivisions(ctrlPts->GetNumberOfPoints()*10);
            splineFilter->SetSubdivideToLength();
            splineFilter->Update();
            outputCurve->DeepCopy(splineFilter->GetOutput());
        } else if (curveSliderButton->isChecked()) {
            vtkNew<vtkCellArray> line;
            line->InsertNextCell(ctrlPts->GetNumberOfPoints());
            for (int i = 0; i < ctrlPts->GetNumberOfPoints(); i++) {
                line->InsertCellPoint(i);
            }
            vtkNew<vtkPolyData> curvePoly;
            curvePoly->SetPoints(ctrlPts);
            curvePoly->SetLines(line);
            outputCurve->DeepCopy(curvePoly);
        }
    }
}

void TemplateDigitiser::PickFunc(vtkObject* caller, long unsigned int eventId,
                            void* callData) {
    if (landmarkButton->isChecked()) {
        if (m_iren->GetControlKey()) {
            if (m_typeINOL -
                    m_fixedVertexFilter->GetOutput()->GetNumberOfPoints() !=
                0) {
                m_iren->SetInteractorStyle(m_2Dstyle);
                m_iren->Modified();
                m_meshActor->SetPickable(1);
                m_meshActor->Modified();
                m_fixedPointActor->SetPickable(0);
                m_fixedPointActor->Modified();
                m_curvectrlPointActor->SetPickable(0);
                m_curvectrlPointActor->Modified();
                /* m_curvePointActor->SetPickable(0);
                m_curvePointActor->Modified(); */
                m_surfaceCurveCtrlPointActor->SetPickable(0);
                m_surfaceCurveCtrlPointActor->Modified();
                auto clickPos = m_iren->GetEventPosition();
                vtkNew<vtkCellPicker> picker;
                picker->Pick(clickPos[0], clickPos[1], 0, m_renderer);
                if (picker->GetPointId() != -1) {
                    m_fixedPtsIds->InsertNextValue(picker->GetPointId());
                    m_fixedHighlightPoints->InsertNextPoint(
                        m_meshData->GetPoint(picker->GetPointId()));
                    m_fixedHighlightPoints->Modified();
                    m_fixedPointsPoly->SetPoints(m_fixedHighlightPoints);
                    m_fixedPointsPoly->GetPointData()->SetScalars(
                        m_fixedPtsIds);
                    m_fixedVertexFilter->SetInputData(m_fixedPointsPoly);
                    m_fixedVertexFilter->Update();
                    m_fixedVertexFilter->Modified();
                    m_renderer->GetRenderWindow()->Render();
                }
            }
        }
        if (m_iren->GetShiftKey()) {
            m_meshActor->SetPickable(0);
            m_meshActor->Modified();
            m_fixedPointActor->SetPickable(1);
            m_fixedPointActor->Modified();
            m_curvectrlPointActor->SetPickable(0);
            m_curvectrlPointActor->Modified();
            /* m_curvePointActor->SetPickable(0);
            m_curvePointActor->Modified(); */
            m_surfaceCurveCtrlPointActor->SetPickable(0);
            m_surfaceCurveCtrlPointActor->Modified();
            auto clickPos = m_iren->GetEventPosition();
            vtkNew<vtkPointPicker> picker;
            picker->Pick(clickPos[0], clickPos[1], 0, m_renderer);
            if (picker->GetPointId() != -1) {
                auto temp = m_fixedVertexFilter->GetOutput();
                temp->GetPoints()->GetData()->RemoveTuple(picker->GetPointId());
                temp->GetPoints()->GetData()->Modified();
                std::vector<int> tempList;
                for (int i = 0; i < m_fixedPtsIds->GetNumberOfTuples(); i++) {
                    tempList.push_back(m_fixedPtsIds->GetValue(i));
                }
                tempList.erase(tempList.begin() + picker->GetPointId());
                m_fixedPtsIds->Initialize();
                for (int items : tempList) {
                    m_fixedPtsIds->InsertNextValue(items);
                }
                m_fixedVertexFilter->Update();
                m_fixedVertexFilter->Modified();
                if (tempList.empty()) {
                    std::cout << "it's empty \n";
                }
            }
        }
    } else if (curveSliderButton->isChecked() /* && m_editableCurve */) {
        if (m_iren->GetControlKey() /* && !curveAnchorButton->isChecked() */) {
            m_iren->SetInteractorStyle(m_2Dstyle);
            m_iren->Modified();
            m_meshActor->SetPickable(1);
            m_meshActor->Modified();
            m_fixedPointActor->SetPickable(0);
            m_fixedPointActor->Modified();
            m_curvectrlPointActor->SetPickable(0);
            m_curvectrlPointActor->Modified();
            /* m_curvePointActor->SetPickable(0);
            m_curvePointActor->Modified(); */
            m_surfaceCurveCtrlPointActor->SetPickable(0);
            m_surfaceCurveCtrlPointActor->Modified();
            auto clickPos = m_iren->GetEventPosition();
            vtkNew<vtkCellPicker> picker;
            picker->Pick(clickPos[0], clickPos[1], 0, m_renderer);
            if (picker->GetPointId() != -1) {
                if (pickFromBoundariesBox->isChecked()) {
                    if (m_meshBoundaries->GetNumberOfCells() > 0) {
                        vtkNew<vtkCellLocator> ptLocator;
                        ptLocator->SetDataSet(m_meshBoundaries);
                        ptLocator->BuildLocator();
                        double closestPoint[3];
                        vtkIdType closestCellId = -1;
                        int subId = -1;
                        double dist = -1;
                        ptLocator->FindClosestPoint(
                            m_meshData->GetPoint(picker->GetPointId()),
                            closestPoint, closestCellId, subId, dist);
                        m_curveHighlightCtrlPoints->InsertNextPoint(
                            closestPoint);
                        m_curveHighlightCtrlPoints->Modified();
                    }
                } else {
                    m_curveHighlightCtrlPoints->InsertNextPoint(
                        m_meshData->GetPoint(picker->GetPointId()));
                    m_curveHighlightCtrlPoints->Modified();
                }
                m_curvectrlPointsPoly->SetPoints(m_curveHighlightCtrlPoints);
                vtkNew<vtkPolyData> tempCtrlPoly;
                tempCtrlPoly->DeepCopy(m_curvectrlPointsPoly);
                m_curveCtrlBlock->SetBlock(m_currentCurveId, tempCtrlPoly);
                m_curveCtrlBlock->Modified();
                m_curvectrlVertexFilter->SetInputData(m_curvectrlPointsPoly);
                m_curvectrlVertexFilter->Update();
                m_curvectrlVertexFilter->Modified();
                if (m_curveHighlightCtrlPoints->GetNumberOfPoints() == 2) {
                    CosmeticCurve(m_curveHighlightCtrlPoints,
                                  m_cosmeticCurvePoly);
                    m_curveTubeFilter->SetInputData(m_cosmeticCurvePoly);
                    m_curveTubeFilter->Update();
                    m_curveTubeFilter->Modified();
                }
                if (m_curveHighlightCtrlPoints->GetNumberOfPoints() > 2) {
                    // curveAnchorButton->setEnabled(1);
                    if (pickFromBoundariesBox->isChecked()) {
                        UpdateCurveData(m_curveHighlightCtrlPoints, m_curvePoly,
                                        m_curveHighlightPoints,
                                        m_meshBoundaries);
                    } else {
                        UpdateCurveData(m_curveHighlightCtrlPoints, m_curvePoly,
                                        m_curveHighlightPoints, m_meshData);
                    }
                    m_curvePoly->Modified();
                    m_curveTubeFilter->SetInputData(m_curvePoly);
                    m_curveTubeFilter->Update();
                    m_curveTubeFilter->Modified();
                    m_curvePointsPoly->SetPoints(m_curveHighlightPoints);
                    // UpdateAnchors(m_curvePointsPoly, m_curveAnchorIdList, 1,
                    // 0);
                    vtkNew<vtkPolyData> tempCurvePtPoly;
                    tempCurvePtPoly->DeepCopy(m_curvePointsPoly);
                    m_curveBlock->SetBlock(m_currentCurveId, tempCurvePtPoly);
                    m_curveBlock->Modified();
                    m_curveVertexFilter->SetInputData(m_curvePointsPoly);
                    m_curveVertexFilter->Update();
                    m_curveVertexFilter->Modified();
                    vtkNew<vtkPolyData> tempCurvePolyLine;
                    tempCurvePolyLine->DeepCopy(m_curvePoly);
                    m_curvePolyLineBlock->SetBlock(m_currentCurveId,
                                                   tempCurvePolyLine);
                    m_curvePolyLineBlock->Modified();
                    vtkNew<vtkPolyData> curveArrowPoly;
                    MakeArrow(m_meshData, m_curvePolyLineBlock, 3,
                              curveArrowPoly);
                    m_glyphCurveArrow->SetInputData(curveArrowPoly);
                    m_glyphCurveArrow->Update();
                }
                m_renderer->GetRenderWindow()->Render();
                if (m_currentCurveId ==
                    m_curveCtrlBlock->GetNumberOfBlocks() - 1) {
                    if (m_curveHighlightCtrlPoints->GetNumberOfPoints() == 3) {
                        curveLineEditNOC->setText(
                            QString::fromStdString(std::to_string(
                                m_curveNOC -
                                m_curveCtrlBlock->GetNumberOfBlocks())));
                        if (m_curveCtrlBlock->GetNumberOfBlocks() <
                            m_curveNOC) {
                            curveAddButton->setEnabled(1);
                        }
                        surfacePickSourceComboBox->addItem(
                            QString::number(m_currentCurveId));
                    }
                }
            }
        }
        if (m_iren->GetShiftKey() /* && !curveAnchorButton->isChecked() */) {
            m_meshActor->SetPickable(0);
            m_meshActor->Modified();
            m_fixedPointActor->SetPickable(0);
            m_fixedPointActor->Modified();
            m_curvectrlPointActor->SetPickable(1);
            m_curvectrlPointActor->Modified();
            /* m_curvePointActor->SetPickable(0);
            m_curvePointActor->Modified(); */
            m_surfaceCurveCtrlPointActor->SetPickable(0);
            m_surfaceCurveCtrlPointActor->Modified();
            auto clickPos = m_iren->GetEventPosition();
            vtkNew<vtkPointPicker> picker;
            picker->Pick(clickPos[0], clickPos[1], 0, m_renderer);
            if (picker->GetPointId() > 0) {
                auto temp = m_curvectrlVertexFilter->GetOutput();
                if (temp->GetNumberOfPoints() > 3) {
                    temp->GetPoints()->GetData()->RemoveTuple(
                        picker->GetPointId());
                    temp->GetPoints()->GetData()->Modified();
                    m_curvectrlVertexFilter->Update();
                    m_curvectrlVertexFilter->Modified();
                    if (pickFromBoundariesBox->isChecked()) {
                        UpdateCurveData(m_curveHighlightCtrlPoints, m_curvePoly,
                                        m_curveHighlightPoints,
                                        m_meshBoundaries);
                    } else {
                        UpdateCurveData(m_curveHighlightCtrlPoints, m_curvePoly,
                                        m_curveHighlightPoints, m_meshData);
                    }
                    m_curvePoly->Modified();
                    m_curveTubeFilter->SetInputData(m_curvePoly);
                    m_curveTubeFilter->Update();
                    m_curveTubeFilter->Modified();
                    m_curvePointsPoly->SetPoints(m_curveHighlightPoints);
                    // UpdateAnchors(m_curvePointsPoly, m_curveAnchorIdList, 1,
                    // 0);
                    m_curveVertexFilter->SetInputData(m_curvePointsPoly);
                    m_curveVertexFilter->Update();
                    m_curveVertexFilter->Modified();
                    vtkNew<vtkPolyData> tempCtrlPoly;
                    tempCtrlPoly->DeepCopy(temp);
                    m_curveCtrlBlock->SetBlock(m_currentCurveId, tempCtrlPoly);
                    m_curveCtrlBlock->Modified();
                    vtkNew<vtkPolyData> tempCurvePtPoly;
                    tempCurvePtPoly->DeepCopy(m_curvePointsPoly);
                    m_curveBlock->SetBlock(m_currentCurveId, tempCurvePtPoly);
                    m_curveBlock->Modified();
                    vtkNew<vtkPolyData> tempCurvePolyLine;
                    tempCurvePolyLine->DeepCopy(m_curvePoly);
                    m_curvePolyLineBlock->SetBlock(m_currentCurveId,
                                                   tempCurvePolyLine);
                    m_curvePolyLineBlock->Modified();
                    vtkNew<vtkPolyData> curveArrowPoly;
                    MakeArrow(m_meshData, m_curvePolyLineBlock, 3,
                              curveArrowPoly);
                    m_glyphCurveArrow->SetInputData(curveArrowPoly);
                    m_glyphCurveArrow->Update();
                }
            }
        }
    } else if (surfaceSliderButton->isChecked()) {
        if (m_iren->GetControlKey() && m_editableSurface == 1 &&
            !surfaceIronButton->isChecked()) {
            m_iren->SetInteractorStyle(m_2Dstyle);
            m_iren->Modified();
            m_meshActor->SetPickable(1);
            m_meshActor->Modified();
            m_fixedPointActor->SetPickable(0);
            m_fixedPointActor->Modified();
            m_curvectrlPointActor->SetPickable(0);
            m_curvectrlPointActor->Modified();
            /* m_curvePointActor->SetPickable(0);
            m_curvePointActor->Modified(); */
            m_surfaceCurveCtrlPointActor->SetPickable(0);
            m_surfaceCurveCtrlPointActor->Modified();
            auto clickPos = m_iren->GetEventPosition();
            vtkNew<vtkCellPicker> picker;
            picker->Pick(clickPos[0], clickPos[1], 0, m_renderer);
            if (picker->GetPointId() != -1 &&
                std::find(m_surfacePtsIds->at(m_currentSurfaceId)->begin(),
                          m_surfacePtsIds->at(m_currentSurfaceId)->end(),
                          picker->GetPointId()) !=
                    m_surfacePtsIds->at(m_currentSurfaceId)->end() == 0) {
                m_surfacePtsIds->at(m_currentSurfaceId)
                    ->push_back(picker->GetPointId());
                m_surfaceCurveHighlightCtrlPoints->InsertNextPoint(
                    m_meshData->GetPoint(picker->GetPointId()));
                m_surfaceCurveHighlightCtrlPoints->Modified();
                m_surfaceCurveCtrlPointsPoly->SetPoints(
                    m_surfaceCurveHighlightCtrlPoints);
                m_surfaceCurveCtrlVertexFilter->SetInputData(
                    m_surfaceCurveCtrlPointsPoly);
                m_surfaceCurveCtrlVertexFilter->Update();
                m_surfaceCurveCtrlVertexFilter->Modified();
                CosmeticCurve(m_surfaceCurveHighlightCtrlPoints,
                              m_cosmeticCurvePoly);
                m_surfaceCurveTubeFilter->SetInputData(m_cosmeticCurvePoly);
                m_surfaceCurveTubeFilter->Update();
                m_surfaceCurveTubeFilter->Modified();
                if (m_surfaceCurveHighlightCtrlPoints->GetNumberOfPoints() >
                    2) {
                    m_surfaceChanged = 1;
                }
                m_renderer->GetRenderWindow()->Render();

            }
        }
        if (m_iren->GetShiftKey() && m_editableSurface == 1 &&
            !surfaceIronButton->isChecked()) {
            m_meshActor->SetPickable(0);
            m_meshActor->Modified();
            m_fixedPointActor->SetPickable(0);
            m_fixedPointActor->Modified();
            m_curvectrlPointActor->SetPickable(0);
            m_curvectrlPointActor->Modified();
            /* m_curvePointActor->SetPickable(0);
            m_curvePointActor->Modified(); */
            m_surfaceCurveCtrlPointActor->SetPickable(1);
            m_surfaceCurveCtrlPointActor->Modified();
            auto clickPos = m_iren->GetEventPosition();
            vtkNew<vtkPointPicker> picker;
            picker->Pick(clickPos[0], clickPos[1], 0, m_renderer);
            if (picker->GetPointId() > 0) {
                if (m_surfaceCurveHighlightCtrlPoints->GetNumberOfPoints() >
                    3) {
                    m_surfacePtsIds->at(m_currentSurfaceId)
                        ->push_back(picker->GetPointId());
                    m_surfaceCurveHighlightCtrlPoints->GetData()->RemoveTuple(
                        picker->GetPointId());
                    m_surfaceCurveHighlightCtrlPoints->GetData()->Modified();
                    m_surfaceCurveCtrlPointsPoly->SetPoints(
                        m_surfaceCurveHighlightCtrlPoints);
                    m_surfaceCurveCtrlVertexFilter->SetInputData(
                        m_surfaceCurveCtrlPointsPoly);
                    m_surfaceCurveCtrlVertexFilter->Update();
                    m_surfaceCurveCtrlVertexFilter->Modified();
                    vtkNew<vtkPolyData> tempCtrlPoly;
                    tempCtrlPoly->DeepCopy(m_surfaceCurveCtrlPointsPoly);
                    m_surfaceCurveCtrlBlock->SetBlock(m_currentSurfaceId,
                                                      tempCtrlPoly);
                    m_surfaceCurveCtrlBlock->Modified();
                    ConstructSurfaceData(m_surfaceCurveHighlightCtrlPoints,
                                         m_surfacePatchHighlightPoints,
                                         m_surfaceCtrlPointsPoly,
                                         m_surfaceCurvePoly, m_nurbsResolution);
                    m_surfaceCurvePoly->Modified();
                    if (m_surfaceCtrlPointsPoly->GetNumberOfPoints() > 0) {
                        m_surfacePatchHighlightPoints->Modified();
                        m_surfaceCtrlVertexFilter->SetInputData(
                            m_surfaceCtrlPointsPoly);
                        m_surfaceCtrlVertexFilter->Update();
                        m_surfaceCtrlVertexFilter->Modified();

                        vtkNew<vtkExtractEdges> edgeExtracted;
                        edgeExtracted->SetInputData(m_surfaceCtrlPointsPoly);
                        edgeExtracted->Update();
                        m_surfaceTubeFilter->SetInputData(
                            edgeExtracted->GetOutput());
                        m_surfaceTubeFilter->Update();
                        m_surfaceTubeFilter->Modified();

                        vtkNew<vtkPolyData> tempSurfaceCtrlPoly;
                        tempSurfaceCtrlPoly->DeepCopy(m_surfaceCtrlPointsPoly);
                        m_surfaceCtrlPtsBlock->SetBlock(m_currentSurfaceId,
                                                        tempSurfaceCtrlPoly);

                        vtkNew<vtkPolyData> tempcurvePolyLine;
                        tempcurvePolyLine->DeepCopy(m_surfaceCurvePoly);
                        m_surfaceCurveBlock->SetBlock(m_currentSurfaceId,
                                                      tempcurvePolyLine);

                        m_surfaceCurveTubeFilter->SetInputData(
                            m_surfaceCurvePoly);
                        m_surfaceCurveTubeFilter->Update();
                        m_surfaceCurveTubeFilter->Modified();

                        vtkNew<vtkPolyData> tempSlidersPoly;
                        tempSlidersPoly->SetPoints(
                            m_surfacePatchHighlightPoints);
                        m_surfacePatchVertexFilter->SetInputData(
                            tempSlidersPoly);
                        m_surfacePatchVertexFilter->Update();
                        m_surfacePatchVertexFilter->Modified();

                        vtkNew<vtkPolyData> tempSlidersPolyCopy;
                        tempSlidersPolyCopy->DeepCopy(tempSlidersPoly);
                        m_surfaceBlock->SetBlock(m_currentSurfaceId,
                                                 tempSlidersPolyCopy);
                        m_surfaceBlock->Modified();

                        vtkNew<vtkPolyData> tempMask;
                        tempMask->DeepCopy(m_surfaceMask);
                        m_surfaceMaskBlock->SetBlock(m_currentSurfaceId,
                                                     tempMask);
                        m_surfaceMaskBlock->Modified();

                        vtkNew<vtkPolyData> surfaceArrowPoly;
                        MakeArrow(m_meshData, m_surfaceCurveBlock, 4,
                                  surfaceArrowPoly);
                        m_glyphSurfaceArrow->SetInputData(surfaceArrowPoly);
                        m_glyphSurfaceArrow->Update();
                    }

                    m_renderer->GetRenderWindow()->Render();
                }
            }
        }
    }

    if (m_iren->GetInteractorStyle()->GetClassName() ==
        m_PointPickerStyle->GetClassName()) {
        m_PointPickerStyle->OnLeftButtonDown();
    } else if (m_iren->GetInteractorStyle()->GetClassName() ==
               m_2Dstyle->GetClassName()) {
        m_iren->SetInteractorStyle(m_PointPickerStyle);
    }
    typeILineEdit->setText(QString::fromStdString(std::to_string(
        m_typeINOL - m_fixedVertexFilter->GetOutput()->GetNumberOfPoints())));
}

void TemplateDigitiser::DeSelectAll() {
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

void TemplateDigitiser::SelectAll() {
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

void TemplateDigitiser::PaintFunc(vtkObject* caller, long unsigned int eventId,
                             void* callData) {
    if (m_iren->GetControlKey() && m_mouseIsClicked) {
        if (surfacePaintButton->isChecked()) {
            m_iren->SetInteractorStyle(m_2Dstyle);
            m_iren->Modified();
            m_meshActor->SetPickable(1);
            m_meshActor->Modified();
            m_fixedPointActor->SetPickable(0);
            m_fixedPointActor->Modified();
            m_curvectrlPointActor->SetPickable(0);
            m_curvectrlPointActor->Modified();
            /* m_curvePointActor->SetPickable(0);
            m_curvePointActor->Modified(); */
            m_surfaceCurveCtrlPointActor->SetPickable(0);
            m_surfaceCurveCtrlPointActor->Modified();
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
            m_fixedPointActor->SetPickable(0);
            m_fixedPointActor->Modified();
            m_curvectrlPointActor->SetPickable(0);
            m_curvectrlPointActor->Modified();
            /* m_curvePointActor->SetPickable(0);
            m_curvePointActor->Modified(); */
            m_surfaceCurveCtrlPointActor->SetPickable(0);
            m_surfaceCurveCtrlPointActor->Modified();
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

void TemplateDigitiser::BrushTool() {
    if (surfacePaintButton->isChecked()) {
        surfaceResampleButton->setEnabled(0);
    } else {
        surfaceResampleButton->setEnabled(1);
    }
}

void TemplateDigitiser::NeighborFinder(int initId, int brushSize,
                                  std::vector<int>& outList) {
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
                std::cout << "Cell Id is out of bound, Debug!" << std::endl;
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

void TemplateDigitiser::resetLeftClck(vtkObject* caller, long unsigned int eventId,
                                 void* callData) {
    m_PointPickerStyle->OnLeftButtonUp();
    // m_PointPainterStyle->OnLeftButtonUp();
}

void TemplateDigitiser::MoveFunc(vtkObject* caller, long unsigned int eventId,
                            void* callData) {
    if (m_iren->GetControlKey()) {
        m_meshActor->SetPickable(0);
        m_meshActor->Modified();
        m_fixedPointActor->SetPickable(0);
        m_fixedPointActor->Modified();
        m_curvectrlPointActor->SetPickable(0);
        m_curvectrlPointActor->Modified();
        /* m_curvePointActor->SetPickable(0);
        m_curvePointActor->Modified(); */
        m_surfaceCurveCtrlPointActor->SetPickable(0);
        m_surfaceCurveCtrlPointActor->Modified();

        if (landmarkButton->isChecked()) {
            m_fixedPointActor->SetPickable(1);
            m_fixedPointActor->Modified();
        } else if (surfaceSliderButton->isChecked()) {
            if (surfaceIronButton->isChecked()) {
                m_surfaceCurveCtrlPointActor->SetPickable(0);
                m_surfaceCurveCtrlPointActor->Modified();
                m_surfaceCtrlPointActor->SetPickable(1);
                m_surfaceCtrlPointActor->Modified();
            } else {
                m_surfaceCurveCtrlPointActor->SetPickable(1);
                m_surfaceCurveCtrlPointActor->Modified();
                m_surfaceCtrlPointActor->SetPickable(0);
                m_surfaceCtrlPointActor->Modified();
                if (m_surfaceCurveHighlightCtrlPoints->GetNumberOfPoints() >
                    2) {
                    m_surfaceChanged = 1;
                }
            }
        } else if (curveSliderButton->isChecked()) {
            m_curvectrlPointActor->SetPickable(1);
            m_curvectrlPointActor->Modified();
        }
        auto clickPos = m_iren->GetEventPosition();
        m_grabPicker->Pick(clickPos[0], clickPos[1], 0, m_renderer);

        if (m_grabPicker->GetPointId() != -1) {
            m_iren->SetInteractorStyle(m_PointMoverStyle);
        }
    } else {
        m_PointPickerStyle->OnMiddleButtonDown();
    }
}

void TemplateDigitiser::resetMouseMove(vtkObject* caller, long unsigned int eventId,
                                  void* callData) {
    m_PointMoverStyle->OnMiddleButtonUp();
    m_iren->SetInteractorStyle(m_PointPickerStyle);
}

void TemplateDigitiser::CoordinateFunc(vtkObject* caller, long unsigned int eventId,
                                  void* callData) {
    m_meshActor->SetPickable(1);
    m_meshActor->Modified();
    auto clickPos = m_iren->GetEventPosition();
    vtkNew<vtkCellPicker> meshPicker;
    meshPicker->Pick(clickPos[0], clickPos[1], 0, m_renderer);
    double* newLocation = meshPicker->GetPickPosition();
    if (pickFromBoundariesBox->isChecked()) {
        if (m_meshBoundaries->GetNumberOfCells() > 0) {
            vtkNew<vtkCellLocator> ptLocator;
            ptLocator->SetDataSet(m_meshBoundaries);
            ptLocator->BuildLocator();
            double closestPoint[3];
            vtkIdType closestCellId = -1;
            int subId = -1;
            double dist = -1;
            ptLocator->FindClosestPoint(newLocation, closestPoint,
                                        closestCellId, subId, dist);
            newLocation = closestPoint;
        }
    }
    if (meshPicker->GetPointId() != -1) {
        auto id = m_grabPicker->GetPointId();
        if (landmarkButton->isChecked()) {
            m_fixedHighlightPoints->SetPoint(id, newLocation);
            m_fixedHighlightPoints->Modified();
            m_fixedPointsPoly->Modified();
            m_fixedVertexFilter->Update();
            m_fixedVertexFilter->Modified();
            m_renderer->GetRenderWindow()->Render();
        } else if (surfaceSliderButton->isChecked() && m_editableSurface == 1) {
            if (m_surfaceNOS == 0 && !surfaceIronButton->isChecked()) {
                if (std::find(m_surfacePtsIds->at(m_currentSurfaceId)->begin(),
                              m_surfacePtsIds->at(m_currentSurfaceId)->end(),
                              meshPicker->GetPointId()) !=
                    m_surfacePtsIds->at(m_currentSurfaceId)->end() == 0) {
                    m_surfaceCurveHighlightCtrlPoints->SetPoint(id,
                                                                newLocation);
                    m_surfaceCurveCtrlPointsPoly->SetPoints(
                        m_surfaceCurveHighlightCtrlPoints);
                    m_surfaceCurveCtrlVertexFilter->SetInputData(
                        m_surfaceCurveCtrlPointsPoly);
                    m_surfaceCurveCtrlVertexFilter->Update();
                    m_surfaceCurveCtrlVertexFilter->Modified();
                    vtkNew<vtkPolyData> tempCtrlPoly;
                    tempCtrlPoly->DeepCopy(m_surfaceCurveCtrlPointsPoly);
                    m_surfaceCurveCtrlBlock->SetBlock(m_currentSurfaceId,
                                                      tempCtrlPoly);
                    m_surfaceCurveCtrlBlock->Modified();

                    CosmeticCurve(m_surfaceCurveHighlightCtrlPoints,
                                  m_cosmeticCurvePoly);
                    m_surfaceCurveTubeFilter->SetInputData(m_cosmeticCurvePoly);
                    m_surfaceCurveTubeFilter->Update();
                    m_surfaceCurveTubeFilter->Modified();
                    m_renderer->GetRenderWindow()->Render();
                }
            } else if (m_surfaceNOS == 0 && surfaceIronButton->isChecked()) {
                std::vector<int>* outlineIds = new std::vector<int>();
                OutlineIdFinder(m_surfacePatchUNOS, m_surfacePatchVNOS,
                                outlineIds);

                if (std::find(outlineIds->begin(), outlineIds->end(), id) ==
                    outlineIds->end()) {
                    m_surfaceCtrlPointsPoly->GetPoints()->SetPoint(id,
                                                                   newLocation);
                    m_surfaceCtrlPointsPoly->Modified();
                }

                vtkNew<vtkWindowedSincPolyDataFilter> repeller;
                repeller->SetInputData(m_surfaceCtrlPointsPoly);
                repeller->SetNumberOfIterations(10);
                repeller->SetPassBand(0.1);
                repeller->SetFeatureEdgeSmoothing(0);
                // repeller->SetFeatureAngle(40);
                repeller->SetBoundarySmoothing(0);
                repeller->NormalizeCoordinatesOn();
                repeller->Update();

                m_surfaceCtrlPointsPoly->DeepCopy(repeller->GetOutput());

                vtkNew<vtkCellLocator> ptLocator;
                ptLocator->SetDataSet(m_surfaceMask);
                ptLocator->BuildLocator();

                for (int i = 0;
                     i < m_surfaceCtrlPointsPoly->GetNumberOfPoints(); i++) {
                    if (std::find(outlineIds->begin(), outlineIds->end(), i) !=
                        outlineIds->end()) {
                    } else {
                        double closestPoint[3];
                        vtkIdType closestCellId = -1;
                        int subId = -1;
                        double dist = -1;
                        ptLocator->FindClosestPoint(
                            m_surfaceCtrlPointsPoly->GetPoint(i), closestPoint,
                            closestCellId, subId, dist);
                        m_surfaceCtrlPointsPoly->GetPoints()->SetPoint(
                            i, closestPoint);
                    }
                }

                delete outlineIds;
                ConstructSurfaceData(m_surfaceCtrlPointsPoly,
                                     m_surfacePatchHighlightPoints);

                vtkNew<vtkPolyData> tempSlidersPoly;
                tempSlidersPoly->SetPoints(m_surfacePatchHighlightPoints);
                m_surfacePatchVertexFilter->SetInputData(tempSlidersPoly);
                m_surfacePatchVertexFilter->Update();
                m_surfacePatchVertexFilter->Modified();

                vtkNew<vtkPolyData> tempSlidersPolyCopy;
                tempSlidersPolyCopy->DeepCopy(tempSlidersPoly);
                m_surfaceBlock->SetBlock(m_currentSurfaceId,
                                         tempSlidersPolyCopy);
                m_surfaceBlock->Modified();

                m_surfaceCtrlVertexFilter->SetInputData(
                    m_surfaceCtrlPointsPoly);
                m_surfaceCtrlVertexFilter->Update();
                m_surfaceCtrlVertexFilter->Modified();

                vtkNew<vtkExtractEdges> edgeExtracted;
                edgeExtracted->SetInputData(m_surfaceCtrlPointsPoly);
                edgeExtracted->Update();
                m_surfaceTubeFilter->SetInputData(edgeExtracted->GetOutput());
                m_surfaceTubeFilter->Update();
                m_surfaceTubeFilter->Modified();
                vtkNew<vtkPolyData> tempSurfaceCtrlPoly;
                tempSurfaceCtrlPoly->DeepCopy(m_surfaceCtrlPointsPoly);
                m_surfaceCtrlPtsBlock->SetBlock(m_currentSurfaceId,
                                                tempSurfaceCtrlPoly);
                m_renderer->GetRenderWindow()->Render();
            } else {
                m_surfaceHighlightPoints->SetPoint(id, newLocation);
                m_surfaceHighlightPoints->Modified();
                m_surfacePointsPoly->Modified();
                m_surfaceVertexFilter->Update();
                m_surfaceVertexFilter->Modified();
                m_renderer->GetRenderWindow()->Render();
            }
        } 
        else if (curveSliderButton->isChecked() /* &&
                   !curveAnchorButton->isChecked() && m_editableCurve */) {
            m_curveHighlightCtrlPoints->SetPoint(id, newLocation);
            m_curveHighlightCtrlPoints->Modified();
            m_curvectrlPointsPoly->Modified();
            m_curvectrlVertexFilter->Update();
            m_curvectrlVertexFilter->Modified();
            if (pickFromBoundariesBox->isChecked()) {
                UpdateCurveData(m_curveHighlightCtrlPoints, m_curvePoly,
                                m_curveHighlightPoints, m_meshBoundaries);
            } else {
                UpdateCurveData(m_curveHighlightCtrlPoints, m_curvePoly,
                                m_curveHighlightPoints, m_meshData);
            }
            m_curvePoly->Modified();
            m_curveTubeFilter->SetInputData(m_curvePoly);
            m_curveTubeFilter->Update();
            m_curveTubeFilter->Modified();
            m_curvePointsPoly->SetPoints(m_curveHighlightPoints);
            // UpdateAnchors(m_curvePointsPoly, m_curveAnchorIdList, 1, 0);
            m_curveVertexFilter->SetInputData(m_curvePointsPoly);
            m_curveVertexFilter->Update();
            m_curveVertexFilter->Modified();
            vtkNew<vtkPolyData> tempCtrlPoly;
            tempCtrlPoly->DeepCopy(m_curvectrlPointsPoly);
            m_curveCtrlBlock->SetBlock(m_currentCurveId, tempCtrlPoly);
            m_curveCtrlBlock->Modified();
            if (m_curveHighlightCtrlPoints->GetNumberOfPoints() > 2) {
                vtkNew<vtkPolyData> tempCurvePtPoly;
                tempCurvePtPoly->DeepCopy(m_curvePointsPoly);
                m_curveBlock->SetBlock(m_currentCurveId, tempCurvePtPoly);
                m_curveBlock->Modified();
                vtkNew<vtkPolyData> tempCurvePolyLine;
                tempCurvePolyLine->DeepCopy(m_curvePoly);
                m_curvePolyLineBlock->SetBlock(m_currentCurveId,
                                               tempCurvePolyLine);
                m_curvePolyLineBlock->Modified();
                vtkNew<vtkPolyData> curveArrowPoly;
                MakeArrow(m_meshData, m_curvePolyLineBlock, 3, curveArrowPoly);
                m_glyphCurveArrow->SetInputData(curveArrowPoly);
                m_glyphCurveArrow->Update();
            }
            m_renderer->GetRenderWindow()->Render();
        }
    }
}

void TemplateDigitiser::OutlineIdFinder(int u, int v, std::vector<int>* output) {
    int uRes = u + 2;
    int vRes = v + 2;

    for (int i = 0; i < uRes; i++) {
        output->push_back(i);
    }
    for (int i = 1; i < vRes; i++) {
        output->push_back((uRes * i) + (uRes - 1));
    }
    for (int i = 1; i < uRes; i++) {
        output->push_back((((uRes * (vRes - 1)) - 1) + uRes) - i);
    }
    for (int i = 1; i < vRes - 1; i++) {
        output->push_back((uRes * (vRes - 1)) - (uRes * i));
    }
}

void TemplateDigitiser::PrepareClosing(QCloseEvent* event) {
    bool typeI = 0;
    bool surfaceSliders = 0;
    bool surfacePatchSliders = 0;
    bool curveSliders = 0;
    if (m_typeINOL == m_fixedHighlightPoints->GetNumberOfPoints()) {
        typeI = 1;
    }
    if (m_surfaceNOS == m_surfaceHighlightPoints->GetNumberOfPoints()) {
        surfaceSliders = 1;
    }
    if (m_surfacePatchUNOS != 0 &&
        m_surfacePatchNOP == m_surfaceBlock->GetNumberOfBlocks()) {
        surfacePatchSliders = 1;
    }
    if (m_surfacePatchUNOS == 0) {
        surfacePatchSliders = 1;
    }
    if (m_curveNOS != 0 && m_curveNOC == m_curveBlock->GetNumberOfBlocks()) {
        curveSliders = 1;
    }
    if (m_curveNOS == 0) {
        curveSliders = 1;
    }

    if (typeI == 1 && surfaceSliders == 1 && surfacePatchSliders == 1 &&
        curveSliders == 1) {
        m_parent->SetIgnorInternals(m_ignoreInside);
        m_parent->SetTemplateTypeI(m_fixedHighlightPoints);
        m_parent->SetTemplateCurveSliders(m_curveBlock, m_curvePolyLineBlock);
        m_parent->SetTemplateSurfaceSliders(m_surfaceHighlightPoints);
        m_parent->SetTemplatePatchSurfaceSliders(m_surfaceBlock,
                                                 m_surfaceCurveBlock);
        m_parent->Refresh(1);
        event->accept();
    } else {
        if (QMessageBox::Yes ==
            QMessageBox::question(this, "Warning",
                                  "Number of digitized Landmarks / "
                                  "SemiLandmarks are less than "
                                  "the decided number.\n Nothing will be "
                                  "registered! Do you want "
                                  "to close the digitizer?",
                                  QMessageBox::Yes | QMessageBox::No)) {
            m_parent->Refresh(0);
            event->accept();
        }
    }
}

void TemplateDigitiser::closeEvent(QCloseEvent* event) {
    event->ignore();
    if (m_BlueNoiseThread) {
        if (m_BlueNoiseThread->isRunning()) {
            QMessageBox msgBox;
            msgBox.setText(
                "The Resampling Process is running. Please Be Patient!");
            msgBox.exec();
        } else {
            PrepareClosing(event);
        }
    }
    else{
        PrepareClosing(event);
    }
}

void TemplateDigitiser::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Control) {
        if (surfaceSliderButton->isChecked()) {
            if (m_editableSurface == 1 && !surfaceIronButton->isChecked()) {
                vtkNew<vtkPolyData> epmptyPoly;
                m_surfaceTubeFilter->SetInputData(epmptyPoly);
                m_surfaceTubeFilter->Update();
                m_surfaceTubeFilter->Modified();
                m_surfacePatchVertexFilter->SetInputData(epmptyPoly);
                m_surfacePatchVertexFilter->Update();
                m_surfacePatchVertexFilter->Modified();
                m_surfaceCtrlVertexFilter->SetInputData(epmptyPoly);
                m_surfaceCtrlVertexFilter->Update();
                m_surfaceCtrlVertexFilter->Modified();
                m_glyphSurfaceArrow->SetInputData(epmptyPoly);
                m_glyphSurfaceArrow->Update();
                m_surfaceLabelVertexFilter->SetInputData(epmptyPoly);
                m_surfaceLabelVertexFilter->Update();
                m_surfaceLabelVertexFilter->Modified();
                m_cutMeshActor->GetProperty()->SetOpacity(0);
                m_renderer->GetRenderWindow()->Render();
            }
        }
    }
}

void TemplateDigitiser::keyReleaseEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Control) {
        if (surfaceSliderButton->isChecked()) {
            if (m_iren->GetInteractorStyle()->GetClassName() ==
                m_PointPickerStyle->GetClassName()) {
                if (m_surfaceCurveHighlightCtrlPoints->GetNumberOfPoints() >
                        2 &&
                    m_surfaceChanged == 1) {
                    surfaceIronButton->setEnabled(1);
                    vtkNew<vtkPolyData> tempCtrlPoly;
                    tempCtrlPoly->DeepCopy(m_surfaceCurveCtrlPointsPoly);
                    m_surfaceCurveCtrlBlock->SetBlock(m_currentSurfaceId,
                                                      tempCtrlPoly);
                    m_surfaceCurveCtrlBlock->Modified();
                    ConstructSurfaceData(m_surfaceCurveHighlightCtrlPoints,
                                         m_surfacePatchHighlightPoints,
                                         m_surfaceCtrlPointsPoly,
                                         m_surfaceCurvePoly, m_nurbsResolution);
                    if (m_surfaceCtrlPointsPoly->GetNumberOfPoints() > 0) {
                        m_surfacePatchHighlightPoints->Modified();
                        m_surfaceCtrlPointsPoly->Modified();
                        m_surfaceCtrlVertexFilter->SetInputData(
                            m_surfaceCtrlPointsPoly);
                        m_surfaceCtrlVertexFilter->Update();
                        m_surfaceCtrlVertexFilter->Modified();
                        vtkNew<vtkExtractEdges> edgeExtracted;
                        edgeExtracted->SetInputData(m_surfaceCtrlPointsPoly);
                        edgeExtracted->Update();
                        m_surfaceTubeFilter->SetInputData(
                            edgeExtracted->GetOutput());
                        m_surfaceTubeFilter->Update();
                        m_surfaceTubeFilter->Modified();

                        vtkNew<vtkPolyData> tempcurvePolyLine;
                        tempcurvePolyLine->DeepCopy(m_surfaceCurvePoly);
                        m_surfaceCurveBlock->SetBlock(m_currentSurfaceId,
                                                      tempcurvePolyLine);

                        m_surfaceCurveTubeFilter->SetInputData(
                            m_surfaceCurvePoly);
                        m_surfaceCurveTubeFilter->Update();
                        m_surfaceCurveTubeFilter->Modified();

                        vtkNew<vtkPolyData> tempSurfaceCtrlPoly;
                        tempSurfaceCtrlPoly->DeepCopy(m_surfaceCtrlPointsPoly);
                        m_surfaceCtrlPtsBlock->SetBlock(m_currentSurfaceId,
                                                        tempSurfaceCtrlPoly);

                        vtkNew<vtkPolyData> tempSlidersPoly;
                        tempSlidersPoly->SetPoints(
                            m_surfacePatchHighlightPoints);
                        m_surfacePatchVertexFilter->SetInputData(
                            tempSlidersPoly);
                        m_surfacePatchVertexFilter->Update();
                        m_surfacePatchVertexFilter->Modified();

                        vtkNew<vtkPolyData> tempSlidersPolyCopy;
                        tempSlidersPolyCopy->DeepCopy(tempSlidersPoly);
                        m_surfaceBlock->SetBlock(m_currentSurfaceId,
                                                 tempSlidersPolyCopy);
                        m_surfaceBlock->Modified();

                        vtkNew<vtkPolyData> tempMask;
                        tempMask->DeepCopy(m_surfaceMask);
                        m_surfaceMaskBlock->SetBlock(m_currentSurfaceId,
                                                     tempMask);
                        m_surfaceMaskBlock->Modified();

                        vtkNew<vtkPolyData> surfaceArrowPoly;
                        MakeArrow(m_meshData, m_surfaceCurveBlock, 4,
                                  surfaceArrowPoly);
                        m_glyphSurfaceArrow->SetInputData(surfaceArrowPoly);
                        m_glyphSurfaceArrow->Update();
                        m_surfaceChanged = 0;
                        int numOfPatches =
                            m_surfacePatchNOP -
                            (m_surfaceBlock->GetNumberOfBlocks());
                        surfaceLineEditNOP->setText(QString::fromStdString(
                            std::to_string(numOfPatches)));
                        if (m_surfaceBlock->GetNumberOfBlocks() <
                            m_surfacePatchNOP) {
                            surfaceAddButton->setEnabled(1);
                        }
                        curvePickSourceComboBox->addItem(
                            QString::number(m_currentSurfaceId));
                    }
                } else if (m_surfaceCurveHighlightCtrlPoints
                                   ->GetNumberOfPoints() > 2 &&
                           m_surfaceChanged == 0) {
                    vtkNew<vtkExtractEdges> edgeExtracted;
                    edgeExtracted->SetInputData(m_surfaceCtrlPointsPoly);
                    edgeExtracted->Update();
                    m_surfaceTubeFilter->SetInputData(
                        edgeExtracted->GetOutput());
                    m_surfaceTubeFilter->Update();
                    m_surfaceTubeFilter->Modified();
                    m_surfaceCurveTubeFilter->SetInputData(m_surfaceCurvePoly);
                    m_surfaceCurveTubeFilter->Update();
                    m_surfaceCurveTubeFilter->Modified();
                    vtkNew<vtkPolyData> tempSlidersPoly;
                    tempSlidersPoly->SetPoints(m_surfacePatchHighlightPoints);
                    m_surfacePatchVertexFilter->SetInputData(tempSlidersPoly);
                    m_surfacePatchVertexFilter->Update();
                    m_surfacePatchVertexFilter->Modified();
                    m_surfaceCtrlVertexFilter->SetInputData(
                        m_surfaceCtrlPointsPoly);
                    m_surfaceCtrlVertexFilter->Update();
                    m_surfaceCtrlVertexFilter->Modified();
                    vtkNew<vtkPolyData> labelPoly;
                    labelPoly->SetPoints(m_surfaceLabelPoints);
                    m_surfaceLabelVertexFilter->SetInputData(labelPoly);
                    m_surfaceLabelVertexFilter->Update();
                    m_surfaceLabelVertexFilter->Modified();
                }
                m_cutMeshActor->GetProperty()->SetOpacity(1);
                m_renderer->GetRenderWindow()->Render();
            }
            if (m_iren->GetInteractorStyle()->GetClassName() ==
                m_PointPainterStyle->GetClassName()) {
                if (surfacePaintButton->isChecked()) {
                }
            }
        }
    }
}

/* void TemplateDigitiser::UpdateAnchors(vtkPolyData* landmarksPoly,
                                 std::vector<int>* anchorId, int sliderType,
                                 bool status) {
    if (sliderType == 1) {  // Curve sliders
        if (status) {       // Update anchors
            m_curveAnchored = 1;
            vtkNew<vtkIntArray> array;
            array->SetName("AnchorStatus");
            array->SetNumberOfValues(landmarksPoly->GetNumberOfPoints());
            for (int i = 0; i < landmarksPoly->GetNumberOfPoints(); i++) {
                array->SetValue(i, 0);
            }
            array->Modified();
            std::vector<int>::iterator iter;
            for (iter = anchorId->begin(); iter != anchorId->end(); ++iter) {
                auto indx = *iter;
                array->SetValue(indx, 1);
                // std::cout<< indx<<std::endl;
            }
            array->Modified();
            landmarksPoly->GetPointData()->RemoveArray("AnchorStatus");
            landmarksPoly->Modified();
            landmarksPoly->GetPointData()->AddArray(array);
            landmarksPoly->Modified();
            if (anchorId->size() == 0) {
                m_curveAnchored = 0;
            }
            //update the database
            vtkNew<vtkPolyData> tempCurvePtPoly;
            tempCurvePtPoly->DeepCopy(landmarksPoly);
            m_curveBlock->SetBlock(m_currentCurveId, tempCurvePtPoly);
            m_curveBlock->Modified();

        } else {  // Reset anchors
            anchorId->clear();
            m_curveAnchored = 0;
            vtkNew<vtkIntArray> array;
            array->SetName("AnchorStatus");
            array->SetNumberOfValues(landmarksPoly->GetNumberOfPoints());
            for (int i = 0; i < landmarksPoly->GetNumberOfPoints(); i++) {
                array->SetValue(i, 0);
            }
            array->Modified();
            landmarksPoly->GetPointData()->RemoveArray("AnchorStatus");
            landmarksPoly->Modified();
            landmarksPoly->GetPointData()->AddArray(array);
            landmarksPoly->Modified();

            //update the database
            vtkNew<vtkPolyData> tempCurvePtPoly;
            tempCurvePtPoly->DeepCopy(landmarksPoly);
            m_curveBlock->SetBlock(m_currentCurveId, tempCurvePtPoly);
            m_curveBlock->Modified();
        }
    }
} */

/* void TemplateDigitiser::RedistCurveLM(vtkPolyData* curvePolyData,
                                 vtkPolyData* curveLandmarksPoly, int id,
                                 double* location) {
    // std::cout << "Redistributing" << std::endl;
    std::vector<int> curveAnchorIdList = *m_curveAnchorIdList;

    curveAnchorIdList.push_back(0);
    std::sort(curveAnchorIdList.begin(), curveAnchorIdList.end());
    curveAnchorIdList.erase(
        std::unique(curveAnchorIdList.begin(), curveAnchorIdList.end()),
        curveAnchorIdList.end());
    std::vector<int>::iterator findIter =
        std::find(curveAnchorIdList.begin(), curveAnchorIdList.end(), id);
    const int pos = std::distance(curveAnchorIdList.begin(), findIter);
    int firstCurveId = pos - 1;
    int secondCurveId = 0;
    if (pos + 1 < curveAnchorIdList.size()) {
        secondCurveId = pos + 1;
    }
    if (pos - 1 < 0) {
        firstCurveId = curveAnchorIdList.size() - 1;
    }
    int numPts1stCurve =
        (curveAnchorIdList[pos] - curveAnchorIdList[firstCurveId]) - 1;
    if (numPts1stCurve < 0) {
        numPts1stCurve = (curveLandmarksPoly->GetNumberOfPoints() -
                          curveAnchorIdList[firstCurveId]) -
                         1;
    }
    int numPts2ndCurve =
        (curveAnchorIdList[secondCurveId] - curveAnchorIdList[pos]) - 1;
    if (numPts2ndCurve < 0) {
        numPts2ndCurve =
            (curveLandmarksPoly->GetNumberOfPoints() - curveAnchorIdList[pos]) -
            1;
    }

    std::vector<int> firstCurveIdList;
    std::vector<int> secondCurveIdList;

    // std::cout<< "1st Curve Landmarks ids are:"<<std::endl;
    for (int i = 1; i < numPts1stCurve + 1; i++) {
        // std::cout<< i + curveAnchorIdList[firstCurveId] <<std::endl;
        firstCurveIdList.push_back(i + curveAnchorIdList[firstCurveId]);
    }
    //std::cout << "2nd Curve Landmarks ids are:" << std::endl;
    for (int i = 1; i < numPts2ndCurve + 1; i++) {
        // std::cout<< i + curveAnchorIdList[pos]<<std::endl;
        secondCurveIdList.push_back(i + curveAnchorIdList[pos]);
    }
    // std::cout<< "End"<<std::endl;


    double firstPtCoord[3];
    double secPtCoord[3];
    curveLandmarksPoly->GetPoint(curveAnchorIdList[firstCurveId], firstPtCoord);
    curveLandmarksPoly->GetPoint(curveAnchorIdList[secondCurveId], secPtCoord);

    // std::vector<int> mainCurveAnchorIdList;
    vtkNew<vtkPointLocator> ptLocator;
    ptLocator->SetDataSet(curvePolyData);
    ptLocator->BuildLocator();

    vtkIdType startPtId;
    vtkIdType endPtId;
    vtkIdType locPtId;

    startPtId = ptLocator->FindClosestPoint(firstPtCoord);
    if (secondCurveId == 0) {
        endPtId = curvePolyData->GetNumberOfPoints() - 1;
    }
    else {
        endPtId = ptLocator->FindClosestPoint(secPtCoord);
    }
    locPtId = ptLocator->FindClosestPoint(location);

    vtkNew<vtkPoints> anchorPts;
    Redistributor(m_curvePoly, startPtId, endPtId, locPtId, firstCurveIdList,
                  secondCurveIdList, curveLandmarksPoly, anchorPts);
    if (anchorPts->GetNumberOfPoints() > 0) {
        curveLandmarksPoly->GetPoints()->SetPoint(id,
                                                  anchorPts->GetPoint(0));
        vtkNew<vtkPolyData> tempCurvePtPoly;
        tempCurvePtPoly->DeepCopy(curveLandmarksPoly);
        m_curveBlock->SetBlock(m_currentCurveId, tempCurvePtPoly);
        m_curveBlock->Modified();
        m_curveVertexFilter->SetInputData(curveLandmarksPoly);
        m_curveVertexFilter->Update();
        m_curveVertexFilter->Modified();
        m_renderer->GetRenderWindow()->Render();
    }
}

void TemplateDigitiser::Redistributor(vtkPolyData* curvePolyData, int startId,
                                 int endId, int locationId,
                                 std::vector<int>& firstCurveIdList,
                                 std::vector<int>& secondCurveIdList,
                                 vtkPolyData* outPutPtsPoly, vtkPoints*
outPutPts) {

    vtkNew<vtkPoints> tempPts1;
    vtkNew<vtkPoints> tempPts2;
    vtkNew<vtkPoints> tempLMPts1;
    vtkNew<vtkPoints> tempLMPts2;
    vtkNew<vtkPoints> lMPts1;
    vtkNew<vtkPoints> lMPts2;
    vtkNew<vtkParametricSpline> tempCurve1;
    tempCurve1->SetClosed(0);
    vtkNew<vtkParametricSpline> tempCurve2;
    tempCurve2->SetClosed(0);

    if (startId < locationId && locationId < endId) {
        for (int i = startId; i <= locationId; i++) {
            tempPts1->InsertNextPoint(curvePolyData->GetPoint(i));
        }
        tempPts1->Modified();
        tempCurve1->SetPoints(tempPts1);
        tempCurve1->Modified();

        for (int i = locationId; i <= endId; i++) {
            tempPts2->InsertNextPoint(curvePolyData->GetPoint(i));
        }
        if(m_curveType->at(m_currentCurveId) == 1){//closed curve
            if(endId == curvePolyData->GetNumberOfPoints() - 1){
                tempPts2->InsertNextPoint(curvePolyData->GetPoint(0));
            }
        }
        tempPts2->Modified();
        tempCurve2->SetPoints(tempPts2);
        tempCurve2->Modified();


        double delta1 = 1 / (double)(firstCurveIdList.size() + 1);
        double delta2 = 1 / (double)(secondCurveIdList.size() + 1);

        for (double i = 1; i < (double)firstCurveIdList.size() + 1; i++) {
            double segmnt = delta1 * i;
            double u[3] = {segmnt, 0, 0};
            double Pt[3];
            tempCurve1->Evaluate(u, Pt, nullptr);
            tempLMPts1->InsertNextPoint(Pt);
            tempLMPts1->Modified();
        }

        for (double i = 1; i < (double)secondCurveIdList.size() + 1; i++) {
            double segmnt = delta2 * i;
            double u[3] = {segmnt, 0, 0};
            double Pt[3];
            tempCurve2->Evaluate(u, Pt, nullptr);
            tempLMPts2->InsertNextPoint(Pt);
            tempLMPts2->Modified();
        }

        vtkNew<vtkCellLocator> ptLocator;
        ptLocator->SetDataSet(m_meshData);
        ptLocator->BuildLocator();
        for (int i = 0; i < tempLMPts1->GetNumberOfPoints(); i++) {
            double closestPoint[3];
            vtkIdType closestCellId = -1;
            int subId = -1;
            double dist = -1;
            ptLocator->FindClosestPoint(tempLMPts1->GetPoint(i),
                                            closestPoint, closestCellId, subId,
                                            dist);
            lMPts1->InsertNextPoint(closestPoint);
        }
        lMPts1->Modified();

        for (int i = 0; i < tempLMPts2->GetNumberOfPoints(); i++) {
            double closestPoint[3];
            vtkIdType closestCellId = -1;
            int subId = -1;
            double dist = -1;
            ptLocator->FindClosestPoint(tempLMPts2->GetPoint(i),
                                            closestPoint, closestCellId, subId,
                                            dist);
            lMPts2->InsertNextPoint(closestPoint);
        }
        lMPts2->Modified();
        if(lMPts1->GetNumberOfPoints() == firstCurveIdList.size() &&
lMPts2->GetNumberOfPoints() == secondCurveIdList.size()){ for(int i =0; i <
lMPts1->GetNumberOfPoints(); i++){
                outPutPtsPoly->GetPoints()->SetPoint(firstCurveIdList[i],
lMPts1->GetPoint(i));
            }
            for(int i =0; i < lMPts2->GetNumberOfPoints(); i++){
                outPutPtsPoly->GetPoints()->SetPoint(secondCurveIdList[i],
lMPts2->GetPoint(i));
            }
            outPutPtsPoly->Modified();

            vtkNew<vtkPoints> anchor;
            anchor->InsertNextPoint(curvePolyData->GetPoint(locationId));
            anchor->Modified();
            ProjectOnMesh(anchor);
            outPutPts->DeepCopy(anchor);
        }
        else{
            std::cout<< "Error, Something is wrong! Debug!"<<std::endl;
        }

    }
    else {
        //std::cout << "End of the movement Limit" << std::endl;
    }
} */

void TemplateDigitiser::ShowLargestDiameter() {
    if (showDiameterBox->isChecked()) {
        vtkNew<vtkFeatureEdges> featureEdges;
        featureEdges->SetInputData(m_meshData);
        featureEdges->BoundaryEdgesOn();
        featureEdges->FeatureEdgesOff();
        featureEdges->ManifoldEdgesOff();
        featureEdges->NonManifoldEdgesOff();
        featureEdges->ColoringOn();
        featureEdges->Update();
        if (featureEdges->GetOutput()->GetNumberOfCells() > 0) {
            vtkPoints* meshPoints = featureEdges->GetOutput()->GetPoints();
            if (meshPoints->GetNumberOfPoints() > 1000) {
                if (QMessageBox::Yes ==
                    QMessageBox::question(
                        this, "Warning",
                        "Your mesh is large (more than 1000 vertices), this "
                        "can "
                        "take long time to calculate, or even crash your OS "
                        "due to "
                        "the RAM overload. Do you want to continue?",
                        QMessageBox::Yes | QMessageBox::No)) {
                    DrawDiameter(meshPoints);
                }
            } else {
                DrawDiameter(meshPoints);
            }

        } else {
            auto errorDialogue = QMessageBox();
            errorDialogue.setIcon(QMessageBox::Information);
            errorDialogue.setWindowTitle("Attention");
            errorDialogue.setText("The mesh doesn't have loose edges");
            errorDialogue.exec();
            m_renderer->RemoveActor(m_largestDiamTubeActor);
            m_renderer->GetRenderWindow()->Render();
            showDiameterBox->setChecked(0);
            showDiameterBox->setStatusTip("The mesh doesn't have loose edges");
            showDiameterBox->setEnabled(0);
        }
    } else {
        m_renderer->RemoveActor(m_largestDiamTubeActor);
        m_renderer->GetRenderWindow()->Render();
        m_renderer->RemoveActor(m_largestDiamTubeActor);
        m_renderer->GetRenderWindow()->Render();
    }
}

void TemplateDigitiser::DrawDiameter(vtkPoints* meshPoints) {
    Eigen::MatrixXd distMat;
    distMat.resize(0, 0);
    distMat.setZero(meshPoints->GetNumberOfPoints(),
                    meshPoints->GetNumberOfPoints());
    PDist(meshPoints, distMat);
    // DebugPrintMatrix(distMat);
    if (!distMat.isZero()) {
        Eigen::MatrixXd::Index maxRow, maxCol;
        float max = distMat.maxCoeff(&maxRow, &maxCol);
        // std::cout<< maxRow<<std::endl;
        // std::cout<< maxCol<<std::endl;
        vtkNew<vtkLineSource> lineSource;
        lineSource->SetPoint1(meshPoints->GetPoint(maxRow));
        lineSource->SetPoint2(meshPoints->GetPoint(maxCol));
        lineSource->Update();
        vtkNew<vtkTubeFilter> tubeFilter;
        tubeFilter->SetInputData(lineSource->GetOutput());
        tubeFilter->SetRadius(0.1);  // default is .5
        tubeFilter->SetNumberOfSides(50);
        tubeFilter->Update();
        // Create a mapper and actor
        vtkNew<vtkPolyDataMapper> tubeMapper;
        tubeMapper->SetInputData(tubeFilter->GetOutput());
        tubeMapper->Update();
        m_largestDiamTubeActor->GetProperty()->SetColor(1.0, 0.0, 0.0);
        m_largestDiamTubeActor->SetMapper(tubeMapper);
        m_largestDiamTubeActor->Modified();
        m_renderer->AddActor(m_largestDiamTubeActor);
        m_renderer->GetRenderWindow()->Render();
    } else {
        std::cout << "Mesh is corrupted? Debug!" << std::endl;
    }
}

void TemplateDigitiser::PDist(vtkPoints* points, Eigen::MatrixXd& output) {
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

void TemplateDigitiser::DebugPrintMatrix(Eigen::MatrixXd matrix) {
    Eigen::IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");
    std::string sep = "\n----------------------------------------\n";
    std::cout << matrix.format(CleanFmt) << sep;
}

void TemplateDigitiser::PickFromBoundaries() {
    if (pickFromBoundariesBox->isChecked()) {
        vtkNew<vtkFeatureEdges> featureEdges;
        featureEdges->SetInputData(m_meshData);
        featureEdges->BoundaryEdgesOn();
        featureEdges->FeatureEdgesOff();
        featureEdges->ManifoldEdgesOff();
        featureEdges->NonManifoldEdgesOff();
        featureEdges->ColoringOn();
        featureEdges->Update();
        if (featureEdges->GetOutput()->GetNumberOfCells() > 0) {
            vtkNew<vtkTubeFilter> tubeFilter;
            tubeFilter->SetInputData(featureEdges->GetOutput());
            tubeFilter->SetRadius(0.1);  // default is .5
            tubeFilter->SetNumberOfSides(50);
            tubeFilter->Update();

            m_meshBoundaries->Initialize();
            m_meshBoundaries->DeepCopy(tubeFilter->GetOutput());
            vtkNew<vtkPolyDataMapper> edgeMapper;
            edgeMapper->SetInputData(m_meshBoundaries);
            edgeMapper->SetScalarModeToUseCellData();
            edgeMapper->Update();
            m_meshBoundActor->SetMapper(edgeMapper);
            m_meshBoundActor->Modified();
            m_meshActor->GetProperty()->SetOpacity(0.5);
            m_meshActor->Modified();

            m_renderer->AddActor(m_meshBoundActor);
            m_renderer->GetRenderWindow()->Render();
        } else {
            auto errorDialogue = QMessageBox();
            errorDialogue.setIcon(QMessageBox::Information);
            errorDialogue.setWindowTitle("Attention");
            errorDialogue.setText("The mesh doesn't have loose edges");
            errorDialogue.exec();
            pickFromBoundariesBox->setChecked(0);
            pickFromBoundariesBox->setStatusTip(
                "The mesh doesn't have loose edges");
            pickFromBoundariesBox->setEnabled(0);
            m_meshBoundaries->Initialize();
            m_renderer->RemoveActor(m_meshBoundActor);
            m_meshActor->GetProperty()->SetOpacity(1);
            m_meshActor->Modified();
            m_renderer->GetRenderWindow()->Render();
        }
    } else {
        m_meshBoundaries->Initialize();
        m_renderer->RemoveActor(m_meshBoundActor);
        m_meshActor->GetProperty()->SetOpacity(1);
        m_meshActor->Modified();
        m_renderer->GetRenderWindow()->Render();
    }
}

TemplateDigitiser::~TemplateDigitiser() {
    delete m_vtkRenderWidget;
    delete m_curveType;
    for (int i = 0; i < m_surfacePtsIds->size(); i++) {
        delete m_surfacePtsIds->at(i);
    }
    delete m_surfacePtsIds;
    delete m_BlueNoiseThread;
    delete m_statThread;
    delete m_mutex;
    // delete m_curveAnchorIdList;
}
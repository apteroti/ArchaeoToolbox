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

#include "../include/SpecimenDigitiser.h"

#include "../include/MainWindow.h"

SpecimenDigitiser::SpecimenDigitiser(vtkPolyData* data, MainWindow* parent)
    : m_meshData(data), m_parent(parent) {
    m_ignoreInside = m_parent->GetIgnorSetting();
    this->setWindowTitle("Digitizer");
    this->resize(650, 500);
    m_curveType = new std::vector<int>;
    m_vtkRenderWidget = new QVTKOpenGLWidget();
    m_renWin = vtkSmartPointer<vtkRenderWindow>::New();
    m_renderer = vtkSmartPointer<vtkRenderer>::New();
    m_iren = vtkSmartPointer<vtkGenericRenderWindowInteractor>::New();
    m_style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    m_PointPickerStyle =
        vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    m_PointMoverStyle =
        vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    m_2Dstyle = vtkSmartPointer<vtkInteractorStyleImage>::New();
    m_meshActor = vtkSmartPointer<vtkActor>::New();
    m_fixedPointActor = vtkSmartPointer<vtkActor>::New();
    m_fixedPointsPoly = vtkSmartPointer<vtkPolyData>::New();
    m_fixedVertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    m_fixedLandmarks = vtkSmartPointer<vtkPoints>::New();
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
    m_surfaceCurveCtrlVertexFilter =
        vtkSmartPointer<vtkVertexGlyphFilter>::New();
    m_surfaceCurveHighlightCtrlPoints = vtkSmartPointer<vtkPoints>::New();
    m_surfacePointActor = vtkSmartPointer<vtkActor>::New();
    m_surfaceVertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    m_surfaceLandmarks = vtkSmartPointer<vtkPoints>::New();
    m_surfaceCurveActor = vtkSmartPointer<vtkActor>::New();
    m_surfaceCurvePoly = vtkSmartPointer<vtkPolyData>::New();
    m_surfaceCurveTubeFilter = vtkSmartPointer<vtkTubeFilter>::New();

    m_surfacePatchLandmarks = vtkSmartPointer<vtkPoints>::New();
    m_surfacePatchVertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    m_surfacePatchPointActor = vtkSmartPointer<vtkActor>::New();
    m_surfaceTubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
    m_surfaceEdgeActor = vtkSmartPointer<vtkActor>::New();

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
    m_surfacePointDeactiveActor = vtkSmartPointer<vtkActor>::New();
    m_surfaceVertexDeactiveFilter =
        vtkSmartPointer<vtkVertexGlyphFilter>::New();

    m_surfaceCtrlVertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    m_surfaceCtrlPointActor = vtkSmartPointer<vtkActor>::New();
    m_surfaceCtrlPointsPoly = vtkSmartPointer<vtkPolyData>::New();
    m_surfacePatchVertexActiveFilter =
        vtkSmartPointer<vtkVertexGlyphFilter>::New();
    m_surfacePatchPointActiveActor = vtkSmartPointer<vtkActor>::New();
    m_surfaceArrowActor = vtkSmartPointer<vtkActor>::New();
    m_glyphSurfaceArrow = vtkSmartPointer<vtkGlyph3D>::New();
    m_surfaceMask = vtkSmartPointer<vtkPolyData>::New();

    m_surfaceBlock = vtkSmartPointer<vtkMultiBlockDataSet>::New();
    m_surfaceCurveCtrlBlock = vtkSmartPointer<vtkMultiBlockDataSet>::New();
    m_surfaceCurveBlock = vtkSmartPointer<vtkMultiBlockDataSet>::New();
    m_surfaceMaskBlock = vtkSmartPointer<vtkMultiBlockDataSet>::New();
    m_surfaceCtrlPtsBlock = vtkSmartPointer<vtkMultiBlockDataSet>::New();
    //-------------------------------------------------------------------------
    m_curveVertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    m_curvectrlPointActor = vtkSmartPointer<vtkActor>::New();
    m_curvectrlPointsPoly = vtkSmartPointer<vtkPolyData>::New();
    m_curvectrlVertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    m_curveHighlightCtrlPoints = vtkSmartPointer<vtkPoints>::New();
    m_curveLandmarks = vtkSmartPointer<vtkPoints>::New();
    m_curveTotalLandmarks = vtkSmartPointer<vtkPoints>::New();
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
    m_curveVertexFilterActive = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    m_curvePointActorActive = vtkSmartPointer<vtkActor>::New();
    m_curveArrowActor = vtkSmartPointer<vtkActor>::New();
    m_glyphCurveArrow = vtkSmartPointer<vtkGlyph3D>::New();
    m_curveCtrlBlock = vtkSmartPointer<vtkMultiBlockDataSet>::New();
    m_curveBlock = vtkSmartPointer<vtkMultiBlockDataSet>::New();
    m_curvePolyLineBlock = vtkSmartPointer<vtkMultiBlockDataSet>::New();
    m_meshBoundActor = vtkSmartPointer<vtkActor>::New();
    m_meshBoundActor->SetPickable(0);
    m_meshBoundaries = vtkSmartPointer<vtkPolyData>::New();
    //-------------------------------------------------------------------------

    this->setCentralWidget(m_vtkRenderWidget);
    mainToolbar = this->addToolBar("Main Toolbar");
    mainToolbar->setMovable(false);
    this->addToolBarBreak();
    fixedLandmarkToolbar = this->addToolBar("Landmark Toolbar");
    fixedLandmarkToolbar->setVisible(false);
    surfacePatchToolbar = this->addToolBar("Surface Toolbar");
    surfacePatchToolbar->setVisible(false);
    curveToolbar = this->addToolBar("Curve Toolbar");
    curveToolbar->setVisible(false);
    counterLayout = new QHBoxLayout();
    counterGroup = new QGroupBox(tr("Counter"));
    this->statusBar()->addWidget(counterGroup, 0);
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
    //-------------------------------------------------------------------------
    landmarkButton = new QPushButton();
    landmarkButton->setToolTip("Digitize Fixed Landmarks");
    landmarkButton->setIcon(QIcon(":/icons/graphics/icons/landmarks.svg"));
    landmarkButton->setCheckable(true);
    landmarkButton->setEnabled(0);
    mainToolbar->addWidget(landmarkButton);
    connect(landmarkButton, &QPushButton::clicked, this,
            &SpecimenDigitiser::TypeITool);

    QLabel* showLargestDiamLabel = new QLabel();
    showLargestDiamLabel->setText(tr("Show Largest Diameter"));
    fixedLandmarkToolbar->addWidget(showLargestDiamLabel);
    showDiameterBox = new QCheckBox();
    showDiameterBox->setChecked(false);
    fixedLandmarkToolbar->addWidget(showDiameterBox);
    fixedLandmarkToolbar->addSeparator();
    connect(showDiameterBox, &QCheckBox::stateChanged, this,
            &SpecimenDigitiser::ShowLargestDiameter);
    //--------------------------------------------------------------------------
    curveSliderButton = new QPushButton();
    curveSliderButton->setToolTip("Digitize Curve Sliders");
    curveSliderButton->setIcon(
        QIcon(":/icons/graphics/icons/curveLandmarks.svg"));
    curveSliderButton->setCheckable(true);
    curveSliderButton->setEnabled(0);
    mainToolbar->addWidget(curveSliderButton);
    connect(curveSliderButton, &QPushButton::clicked, this,
            &SpecimenDigitiser::CurveTool);

    curveAddButton = new QPushButton();
    curveAddButton->setToolTip("Add a Curve");
    curveAddButton->setIcon(QIcon(":/icons/graphics/icons/plus.png"));
    curveAddButton->setCheckable(false);
    curveAddButton->setEnabled(0);
    curveToolbar->addWidget(curveAddButton);
    curveToolbar->addSeparator();
    connect(curveAddButton, &QPushButton::clicked, this,
            &SpecimenDigitiser::AddCurve);
    curveTypeButton = new QPushButton();
    curveTypeButton->setToolTip("Change Curve Type");
    curveTypeButton->setIcon(QIcon(":/icons/graphics/icons/closed_curve.svg"));
    curveTypeButton->setCheckable(true);
    curveToolbar->addWidget(curveTypeButton);
    curveToolbar->addSeparator();
    connect(curveTypeButton, &QPushButton::clicked, this,
            &SpecimenDigitiser::ChangeCurveType);
    QLabel* curveSelectLabel = new QLabel();
    curveSelectLabel->setText(tr("Curve Index"));
    curveToolbar->addWidget(curveSelectLabel);
    curveSelectComboBox = new QComboBox();
    curveToolbar->addWidget(curveSelectComboBox);
    curveToolbar->addSeparator();
    void (QComboBox ::*curveSelectFp)(int) = &QComboBox ::currentIndexChanged;
    connect(curveSelectComboBox, curveSelectFp, this,
            &SpecimenDigitiser::ChangeCurve);

    QLabel* curvePickLabel = new QLabel();
    curvePickLabel->setText(tr("From Surface"));
    curveToolbar->addWidget(curvePickLabel);
    curvePickSourceComboBox = new QComboBox();
    curvePickSourceComboBox->addItem(QString::fromStdString("Index"));
    curveToolbar->addWidget(curvePickSourceComboBox);
    curveToolbar->addSeparator();
    void (QComboBox ::*curveSourceFp)(int) = &QComboBox ::currentIndexChanged;
    connect(curvePickSourceComboBox, curveSourceFp, this,
            &SpecimenDigitiser::ChangeCurveSource);

    QLabel* curvePickFromLooseLabel = new QLabel();
    curvePickFromLooseLabel->setText(tr("Limit to Loose Booundaries"));
    curveToolbar->addWidget(curvePickFromLooseLabel);
    pickFromBoundariesBox = new QCheckBox();
    pickFromBoundariesBox->setChecked(false);
    curveToolbar->addWidget(pickFromBoundariesBox);
    connect(pickFromBoundariesBox, &QCheckBox::stateChanged, this,
            &SpecimenDigitiser::PickFromBoundaries);

    curveToolbar->addSeparator();

    curveClearButton = new QPushButton();
    curveClearButton->setCheckable(false);
    curveClearButton->setText("Clear All Curves");
    // curveClearButton->setEnabled(0);
    curveToolbar->addWidget(curveClearButton);
    curveToolbar->addSeparator();
    connect(curveClearButton, &QPushButton::clicked, this,
            &SpecimenDigitiser::ClearCurve);

    surfaceSliderButton = new QPushButton();
    surfaceSliderButton->setToolTip("Digitize Surface Sliders");
    surfaceSliderButton->setIcon(
        QIcon(":/icons/graphics/icons/surfaceLandmarks.svg"));
    surfaceSliderButton->setCheckable(true);
    surfaceSliderButton->setEnabled(0);
    mainToolbar->addWidget(surfaceSliderButton);
    connect(surfaceSliderButton, &QPushButton::clicked, this,
            &SpecimenDigitiser::SurfaceTool);

    slidingButton = new QPushButton();
    slidingButton->setIcon(QIcon(":/icons/graphics/icons/sliding.svg"));
    slidingButton->setToolTip("Slide Semi-Landmarks");
    slidingButton->setCheckable(false);
    slidingButton->setEnabled(0);
    mainToolbar->addWidget(slidingButton);
    connect(slidingButton, &QPushButton::clicked, this,
            &SpecimenDigitiser::MakeSlide);
    // Create opacity effect
    QGraphicsOpacityEffect* effectForSliding =
        new QGraphicsOpacityEffect(slidingButton);
    slidingButton->setGraphicsEffect(effectForSliding);
    // Create animation
    m_slidingAnimation = new QPropertyAnimation(effectForSliding, "opacity");
    m_slidingAnimation->setDuration(1000);   // 1 second cycle
    m_slidingAnimation->setStartValue(1.0);  // Fully visible
    m_slidingAnimation->setEndValue(0.2);    // Almost transparent
    m_slidingAnimation->setEasingCurve(QEasingCurve::InOutQuad);
    m_slidingAnimation->setLoopCount(-1);  // Infinite loop
    m_slidingAnimation->stop();
    //---------------------------

    surfaceAddButton = new QPushButton();
    surfaceAddButton->setToolTip("Add a Surface");
    surfaceAddButton->setIcon(QIcon(":/icons/graphics/icons/plus.png"));
    surfaceAddButton->setCheckable(false);
    surfaceAddButton->setEnabled(0);
    surfacePatchToolbar->addWidget(surfaceAddButton);
    connect(surfaceAddButton, &QPushButton::clicked, this,
            &SpecimenDigitiser::AddSurface);

    surfaceLockButton = new QPushButton();
    surfaceLockButton->setToolTip("Lock surface composition");
    surfaceLockButton->setIcon(QIcon(":/icons/graphics/icons/unlocked.svg"));
    surfaceLockButton->setCheckable(true);
    surfacePatchToolbar->addWidget(surfaceLockButton);
    connect(surfaceLockButton, &QPushButton::clicked, this,
            &SpecimenDigitiser::ChangeSurfaceLock);

    surfaceIronButton = new QPushButton();
    surfaceIronButton->setCheckable(true);
    surfaceIronButton->setIcon(QIcon(":/icons/graphics/icons/ironOff.svg"));
    surfaceIronButton->setToolTip("Iron Out Sliders");
    surfaceIronButton->setEnabled(0);
    surfacePatchToolbar->addWidget(surfaceIronButton);
    surfacePatchToolbar->addSeparator();
    connect(surfaceIronButton, &QPushButton::clicked, this,
            &SpecimenDigitiser::DrapeToSurface);
    // Create opacity effect
    QGraphicsOpacityEffect* effectForIron =
        new QGraphicsOpacityEffect(surfaceIronButton);
    surfaceIronButton->setGraphicsEffect(effectForIron);
    // Create animation
    m_ironAnimation = new QPropertyAnimation(effectForIron, "opacity");
    m_ironAnimation->setDuration(1000);   // 1 second cycle
    m_ironAnimation->setStartValue(1.0);  // Fully visible
    m_ironAnimation->setEndValue(0.2);    // Almost transparent
    m_ironAnimation->setEasingCurve(QEasingCurve::InOutQuad);
    m_ironAnimation->setLoopCount(-1);  // Infinite loop
    m_ironAnimation->stop();

    surfaceInterpolateButton = new QPushButton();
    surfaceInterpolateButton->setToolTip("Place Pre-Sliders");
    surfaceInterpolateButton->setCheckable(false);
    surfaceInterpolateButton->setIcon(
        QIcon(":/icons/graphics/icons/Bspline.png"));
    // Create opacity effect
    QGraphicsOpacityEffect* effect =
        new QGraphicsOpacityEffect(surfaceInterpolateButton);
    surfaceInterpolateButton->setGraphicsEffect(effect);
    // Create animation
    m_interpolationAnimation = new QPropertyAnimation(effect, "opacity");
    m_interpolationAnimation->setDuration(1000);   // 1 second cycle
    m_interpolationAnimation->setStartValue(1.0);  // Fully visible
    m_interpolationAnimation->setEndValue(0.2);    // Almost transparent
    m_interpolationAnimation->setEasingCurve(QEasingCurve::InOutQuad);
    m_interpolationAnimation->setLoopCount(-1);  // Infinite loop

    surfaceInterpolateButton->setEnabled(0);
    m_interpolationAnimation->stop();
    surfacePatchToolbar->addWidget(surfaceInterpolateButton);
    surfacePatchToolbar->addSeparator();
    connect(surfaceInterpolateButton, &QPushButton::clicked, this,
            &SpecimenDigitiser::InterpolateSurface);

    QLabel* surfaceSelectLabel = new QLabel();
    surfaceSelectLabel->setText(tr("Surface Index"));
    surfacePatchToolbar->addWidget(surfaceSelectLabel);
    surfaceSelectComboBox = new QComboBox();
    surfacePatchToolbar->addWidget(surfaceSelectComboBox);
    surfacePatchToolbar->addSeparator();
    void (QComboBox ::*surfaceSelectFp)(int) = &QComboBox ::currentIndexChanged;
    connect(surfaceSelectComboBox, surfaceSelectFp, this,
            &SpecimenDigitiser::ChangeSurface);

    QLabel* surfacePickLabel = new QLabel();
    surfacePickLabel->setText(tr("From Curve"));
    surfacePatchToolbar->addWidget(surfacePickLabel);
    surfacePickSourceComboBox = new QComboBox();
    surfacePickSourceComboBox->addItem(QString::fromStdString("Index"));
    surfacePatchToolbar->addWidget(surfacePickSourceComboBox);
    surfacePatchToolbar->addSeparator();
    void (QComboBox ::*surfaceSourceFp)(int) = &QComboBox ::currentIndexChanged;
    connect(surfacePickSourceComboBox, surfaceSourceFp, this,
            &SpecimenDigitiser::ChangeSurfaceSource);

    surfaceClearButton = new QPushButton();
    surfaceClearButton->setCheckable(false);
    surfaceClearButton->setText("Clear All Surfaces");
    surfacePatchToolbar->addWidget(surfaceClearButton);
    surfacePatchToolbar->addSeparator();
    connect(surfaceClearButton, &QPushButton::clicked, this,
            &SpecimenDigitiser::ClearSurface);
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
            &SpecimenDigitiser::ChangePointSize);

    void (QSpinBox ::*changeLineSizeFp)(int) = &QSpinBox ::valueChanged;
    connect(lineSizeSpinBox, changeLineSizeFp, this,
            &SpecimenDigitiser::ChangeLineSize);

    //---------------------------------------------
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
        "}";

    counterGroup->setStyleSheet(style);

    counterFixedLabel = new QLabel(tr(u8"\u0025 Type I-II:"));
    counterCurveNOCLabel = new QLabel(tr(u8"\u0025 Curves:"));
    counterSurfaceNOPLabel = new QLabel(tr(u8"\u0025 Surface Patches:"));
    typeILineEdit = new QLineEdit();
    typeILineEdit->setReadOnly(true);
    curveLineEditNOC = new QLineEdit();
    curveLineEditNOC->setReadOnly(true);
    surfaceLineEditNOP = new QLineEdit();
    surfaceLineEditNOP->setReadOnly(true);

    counterLayout = new QHBoxLayout();
    counterLayout->addWidget(counterFixedLabel);
    counterLayout->addWidget(typeILineEdit);
    counterLayout->addWidget(counterCurveNOCLabel);
    counterLayout->addWidget(curveLineEditNOC);
    counterLayout->addWidget(counterSurfaceNOPLabel);
    counterLayout->addWidget(surfaceLineEditNOP);
    counterLayout->addStretch(1);
    counterGroup->setLayout(counterLayout);
    counterGroup->setContentsMargins(3, 3, 3, 1);
    this->statusBar()->addWidget(counterGroup, 0);

    if (m_parent->GetTemplateMesh()->GetNumberOfPoints() > 0) {
        m_templateMesh = m_parent->GetTemplateMesh();
        if (m_parent->GetTemplateTypeI()->GetNumberOfPoints() > 0) {
            m_templateTypeI = m_parent->GetTemplateTypeI();
            m_typeINOL = m_parent->GetTemplateTypeI()->GetNumberOfPoints();
            landmarkButton->setEnabled(1);
            typeILineEdit->setText(
                QString::fromStdString(std::to_string(m_typeINOL)));
        }
        if (m_parent->GetTemplateTypeI()->GetNumberOfPoints() == 0) {
            landmarkButton->setEnabled(0);
            counterLayout->removeWidget(counterFixedLabel);
            counterLayout->removeWidget(typeILineEdit);
            counterFixedLabel->hide();
            typeILineEdit->hide();
        }
        if (m_parent->GetTemplateSurfaceSliders()->GetNumberOfPoints() > 0) {
            m_templateSurfaceSliders = m_parent->GetTemplateSurfaceSliders();
            m_surfaceNOS =
                m_parent->GetTemplateSurfaceSliders()->GetNumberOfPoints();
            surfaceSliderButton->setEnabled(1);
            counterLayout->removeWidget(counterSurfaceNOPLabel);
            counterLayout->removeWidget(surfaceLineEditNOP);
            counterSurfaceNOPLabel->hide();
            surfaceLineEditNOP->hide();
        }
        if (m_parent->GetTemplateSurfacePatchSliders()->GetNumberOfBlocks() >
            0) {
            m_templatePatchSurfaceSliders =
                m_parent->GetTemplateSurfacePatchSliders();
            m_surfacePatchNOP =
                m_parent->GetTemplateSurfacePatchSliders()->GetNumberOfBlocks();
            m_surfacePatchUNOS = m_parent->GetURes();
            m_surfacePatchVNOS = m_parent->GetVRes();
            surfaceSliderButton->setEnabled(1);
            surfaceLineEditNOP->setText(
                QString::fromStdString(std::to_string(m_surfacePatchNOP)));
            surfaceSelectComboBox->addItem(
                QString::fromStdString(std::to_string(m_currentSurfaceId)));
        }
        if (m_parent->GetTemplateSurfacePatchSliders()->GetNumberOfBlocks() ==
            0) {
            if (m_parent->GetTemplateSurfaceSliders()->GetNumberOfPoints() ==
                0) {
                surfaceSliderButton->setEnabled(0);
            }
            counterLayout->removeWidget(counterSurfaceNOPLabel);
            counterLayout->removeWidget(surfaceLineEditNOP);
            counterSurfaceNOPLabel->hide();
            surfaceLineEditNOP->hide();
        }
        if (m_parent->GetTemplateCurveSliders()->GetNumberOfBlocks() > 0) {
            m_templateCurveSliders = m_parent->GetTemplateCurveSliders();
            m_curveNOC =
                m_parent->GetTemplateCurveSliders()->GetNumberOfBlocks();
            m_curveNOS = m_parent->GetCurveNOS();
            curveSliderButton->setEnabled(1);
            curveSelectComboBox->addItem(
                QString::fromStdString(std::to_string(m_currentCurveId)));
            curveLineEditNOC->setText(QString::fromStdString(std::to_string(
                m_curveNOC - m_curveCtrlBlock->GetNumberOfBlocks())));
        }
        if (m_parent->GetTemplateCurveSliders()->GetNumberOfBlocks() == 0) {
            counterLayout->removeWidget(counterCurveNOCLabel);
            counterLayout->removeWidget(curveLineEditNOC);
            counterCurveNOCLabel->hide();
            curveLineEditNOC->hide();
            curveSliderButton->setEnabled(0);
        }
        if (m_parent->GetTemplateTypeI()->GetNumberOfPoints() == 0 &&
            m_parent->GetTemplateSurfacePatchSliders()->GetNumberOfBlocks() ==
                0 &&
            m_parent->GetTemplateCurveSliders()->GetNumberOfBlocks() == 0 &&
            m_parent->GetTemplateSurfaceSliders()->GetNumberOfPoints() > 0) {
            this->statusBar()->removeWidget(counterGroup);
            counterGroup->hide();
            surfaceSliderButton->setEnabled(1);
        }
    }
    for (int i = 0; i < m_curveNOC; i++) {
        m_curveType->push_back(1);
    }
    Plot();
    this->show();
}

void SpecimenDigitiser::Plot() {
    // Setting up render scene
    vtkNew<vtkNamedColors> colors;
    m_renWin = m_vtkRenderWidget->GetRenderWindow();
    m_renWin->AddRenderer(m_renderer);
    m_style->SetCurrentRenderer(m_renderer);
    m_PointPickerStyle->SetCurrentRenderer(m_renderer);
    m_2Dstyle->SetCurrentRenderer(m_renderer);
    m_PointMoverStyle->SetCurrentRenderer(m_renderer);
    m_iren->SetInteractorStyle(m_style);
    m_iren->SetRenderWindow(m_renWin);
    // Mesh properties and color etc
    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetInputData(m_meshData);
    mapper->SetResolveCoincidentTopologyToOff();
    mapper->ScalarVisibilityOff();
    m_meshActor->SetMapper(mapper);
    m_meshActor->GetProperty()->SetColor(1, 0.992, 0.815);
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
    const double area = prop->GetSurfaceArea();
    const double diagonal =
        std::sqrt(area);  // Approximate characteristic length
    // Compute size factor based on application-specific parameters
    // Normalized between 0-1 range first, then scaled
    double sizeFactor =
        (m_typeINOL * 0.03 + m_surfaceNOS * 0.025 +
         m_surfacePatchNOP * m_surfacePatchUNOS * m_surfacePatchVNOS * 0.025 +
         m_curveNOS * m_curveNOC * 0.025);

    // Apply sigmoid function for smooth clamping
    sizeFactor =
        1.0 /
        (1.0 + std::exp(-0.1 * (sizeFactor - 50.0)));  // Sigmoid normalization

    // Map to reasonable visual range (1%-5% of characteristic length)
    const double minSize = 0.01 * diagonal;
    const double maxSize = 0.05 * diagonal;
    m_sizeConstant = minSize + sizeFactor * (maxSize - minSize);
    // Apply to sphere source
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetRadius(m_sizeConstant);
    vtkNew<vtkSphereSource> sphereSource2;
    sphereSource2->SetRadius(m_sizeConstant * 1.5);
    vtkNew<vtkSphereSource> sphereSource3;
    sphereSource3->SetRadius(m_sizeConstant * 0.5);
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

    vtkNew<vtkGlyph3DMapper> surfacePointDeactiveMapper;
    surfacePointDeactiveMapper->SetInputData(
        m_surfaceVertexDeactiveFilter->GetOutput());
    surfacePointDeactiveMapper->SetSourceConnection(
        sphereSource->GetOutputPort());
    m_surfacePointDeactiveActor->GetProperty()->SetColor(0.0, 0.0, 1.0);
    m_surfacePointDeactiveActor->GetProperty()->SetDiffuse(0.8);
    m_surfacePointDeactiveActor->GetProperty()->SetSpecular(0.5);
    m_surfacePointDeactiveActor->GetProperty()->SetSpecularPower(30);
    m_surfacePointDeactiveActor->SetMapper(surfacePointDeactiveMapper);
    m_surfacePointDeactiveActor->SetPickable(0);
    m_renderer->AddActor(m_surfacePointDeactiveActor);

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

    vtkNew<vtkGlyph3DMapper> surfacePatchPointActiveMapper;
    surfacePatchPointActiveMapper->SetInputData(
        m_surfacePatchVertexActiveFilter->GetOutput());
    surfacePatchPointActiveMapper->SetSourceConnection(
        sphereSource->GetOutputPort());
    surfacePatchPointActiveMapper->ScalingOff();
    surfacePatchPointActiveMapper->ScalarVisibilityOff();
    m_surfacePatchPointActiveActor->GetProperty()->SetColor(0.0, 0.0, 1.0);
    m_surfacePatchPointActiveActor->GetProperty()->SetDiffuse(0.8);
    m_surfacePatchPointActiveActor->GetProperty()->SetSpecular(0.5);
    m_surfacePatchPointActiveActor->GetProperty()->SetSpecularPower(30);
    m_surfacePatchPointActiveActor->GetProperty()->SetOpacity(1);
    m_surfacePatchPointActiveActor->SetMapper(surfacePatchPointActiveMapper);
    m_surfacePatchPointActiveActor->SetPickable(0);
    m_renderer->AddActor(m_surfacePatchPointActiveActor);

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
    //-------------------------------------------
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
    vtkNew<vtkGlyph3DMapper> curvePointMapperActive;
    curvePointMapperActive->SetInputData(
        m_curveVertexFilterActive->GetOutput());
    curvePointMapperActive->SetSourceConnection(sphereSource->GetOutputPort());
    curvePointMapperActive->ScalingOff();
    curvePointMapperActive->ScalarVisibilityOff();
    m_curvePointActorActive->GetProperty()->SetColor(0.0, 1.0, 0.0);
    m_curvePointActorActive->GetProperty()->SetDiffuse(0.8);
    m_curvePointActorActive->GetProperty()->SetSpecular(0.5);
    m_curvePointActorActive->GetProperty()->SetSpecularPower(30);
    m_curvePointActorActive->GetProperty()->SetOpacity(1);
    m_curvePointActorActive->SetMapper(curvePointMapperActive);
    m_curvePointActorActive->SetPickable(0);
    m_renderer->AddActor(m_curvePointActorActive);
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
    m_renderer->ResetCamera();
    // Observers
    m_PointPickerStyle->AddObserver(vtkCommand::LeftButtonPressEvent, this,
                                    &SpecimenDigitiser::PickFunc, 1);
    m_PointPickerStyle->AddObserver(vtkCommand::LeftButtonReleaseEvent, this,
                                    &SpecimenDigitiser::resetLeftClck, 1);
    m_PointPickerStyle->AddObserver(vtkCommand::MiddleButtonPressEvent, this,
                                    &SpecimenDigitiser::MoveFunc, 1);
    m_PointMoverStyle->AddObserver(vtkCommand::MiddleButtonReleaseEvent, this,
                                   &SpecimenDigitiser::resetMouseMove, 1);
    m_PointMoverStyle->AddObserver(vtkCommand::MouseMoveEvent, this,
                                   &SpecimenDigitiser::CoordinateFunc, 1);

    m_renWin->Render();
    m_iren->Start();
}

void SpecimenDigitiser::TypeITool() {
    if (landmarkButton->isChecked()) {
        curveSliderButton->setChecked(false);
        surfaceSliderButton->setChecked(false);
        SurfaceTool();
        CurveTool();
        m_iren->SetInteractorStyle(m_PointPickerStyle);
        fixedLandmarkToolbar->setVisible(true);
        slidingButton->setEnabled(0);
        m_slidingAnimation->stop();
        slidingButton->setToolTip(
            "Can not do sliding when the Fixed LM digitising tool is active");
    } else {
        fixedLandmarkToolbar->setVisible(false);
        slidingButton->setToolTip("Slide Semi-Landmarks");
        m_iren->SetInteractorStyle(m_style);
        // typeIToolbar->setVisible(false);
        if (m_fixedLandmarks->GetNumberOfPoints() > 0) {
            if (m_typeINOL == m_fixedLandmarks->GetNumberOfPoints()) {
                PrepareSliding();
            } else {
                QMessageBox warning(this);
                warning.setText(
                    "Number of digitized Fixed Landmarks are less than the "
                    "decided number. Nothing will be registered!");
                warning.exec();
            }
        }
    }
}

void SpecimenDigitiser::SurfaceTool() {
    if (surfaceSliderButton->isChecked()) {
        curveSliderButton->setChecked(false);
        landmarkButton->setChecked(false);
        TypeITool();
        CurveTool();
        slidingButton->setEnabled(0);
        m_slidingAnimation->stop();
        slidingButton->setToolTip(
            "Can not do sliding when the surface digitising tool is active");
        if (m_surfaceNOS != 0) {
            bool hasFixed = 1;
            bool hasCurve = 1;

            vtkPoints* fixedLM = GetTypeI();
            vtkNew<vtkPoints> curveSliders;
            GetCurveSliders(curveSliders);

            vtkPoints* templateFixedLM = GetTemplateTypeI();
            vtkNew<vtkPoints> templateCurveSliders;
            GetTemplateCurveSliders(templateCurveSliders);

            if (m_typeINOL > 0 && fixedLM->GetNumberOfPoints() != m_typeINOL) {
                hasFixed = false;
            }
            if (m_curveNOS > 0 &&
                (curveSliders->GetNumberOfPoints()) != m_curveNOS * m_curveNOC) {
                hasCurve = false;
            }
            
            if (!hasFixed || !hasCurve) {
                if (QMessageBox::Yes ==
                    QMessageBox::question(
                        this, "Warning",
                        "To make the automated registration fast and accurate, "
                        "first "
                        "digitize all fixed and / or curve landmarks."
                        "Do you want to continue regardless?",
                        QMessageBox::Yes | QMessageBox::No)) {
                    m_iren->SetInteractorStyle(m_style);
                    statusLabel->setText("Status: Busy");
                    progressLabel->setPixmap(
                        QPixmap(":/icons/graphics/icons/busy.svg"));

                    vtkNew<vtkThreshold> threshold;
                    threshold->SetInputData(m_templateMesh);
                    threshold->ThresholdBetween(
                        1, 1);  // Extract cells with Masked == 1
                    threshold->SetInputArrayToProcess(
                        0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_CELLS,
                        "Masked");
                    threshold->Update();
                    if (threshold->GetOutput()->GetNumberOfPoints() > 0) {
                        auto choice = QMessageBox::question(
                            this, "Information",
                            "Do you want to apply Exclusion Paint?",
                            QMessageBox::Yes | QMessageBox::No);
                        if (choice == QMessageBox::Yes) {
                            this->hide();
                            delete m_exclusionPainter;
                            m_exclusionPainter = new ExclusionPaint(m_meshData);
                            // Create an event loop to block here until the
                            // window is closed
                            QEventLoop loop;
                            QObject::connect(m_exclusionPainter,
                                             &ExclusionPaint::windowClosed,
                                             &loop, &QEventLoop::quit);
                            loop.exec();  // blocks here until
                                          // m_exclusionPainter is closed
                            this->show();
                        }
                    }

                    delete m_regPlot;
                    m_regPlot =
                        new Registration(m_meshData, m_templateMesh,
                                         m_templateSurfaceSliders, this);
                } else {
                    surfaceSliderButton->setChecked(false);
                    SurfaceTool();
                }
            } else {
                vtkNew<vtkPoints> totalTamplate;
                for (int i = 0; i < templateFixedLM->GetNumberOfPoints(); i++) {
                    totalTamplate->InsertNextPoint(
                        templateFixedLM->GetPoint(i));
                }
                for (int i = 0; i < templateCurveSliders->GetNumberOfPoints();
                     i++) {
                    totalTamplate->InsertNextPoint(
                        templateCurveSliders->GetPoint(i));
                }
                totalTamplate->Modified();

                vtkNew<vtkPoints> totalSource;
                for (int i = 0; i < fixedLM->GetNumberOfPoints(); i++) {
                    totalSource->InsertNextPoint(fixedLM->GetPoint(i));
                }
                for (int i = 0; i < curveSliders->GetNumberOfPoints(); i++) {
                    totalSource->InsertNextPoint(curveSliders->GetPoint(i));
                }
                totalSource->Modified();
                m_iren->SetInteractorStyle(m_style);
                statusLabel->setText("Status: Busy");
                progressLabel->setPixmap(
                    QPixmap(":/icons/graphics/icons/busy.svg"));

                vtkNew<vtkThreshold> threshold;
                threshold->SetInputData(m_templateMesh);
                threshold->ThresholdBetween(
                    1, 1);  // Extract cells with Masked == 1
                threshold->SetInputArrayToProcess(
                    0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_CELLS, "Masked");
                threshold->Update();
                if (threshold->GetOutput()->GetNumberOfPoints() > 0) {
                    auto choice = QMessageBox::question(
                        this, "Information",
                        "Do you want to apply Exclusion Paint?",
                        QMessageBox::Yes | QMessageBox::No);
                    if (choice == QMessageBox::Yes) {
                        this->hide();
                        delete m_exclusionPainter;
                        m_exclusionPainter = new ExclusionPaint(m_meshData);
                        // Create an event loop to block here until the window
                        // is closed
                        QEventLoop loop;
                        QObject::connect(m_exclusionPainter,
                                         &ExclusionPaint::windowClosed, &loop,
                                         &QEventLoop::quit);
                        loop.exec();  // blocks here until m_exclusionPainter is
                                      // closed
                        this->show();
                    }
                }

                delete m_regPlot;
                m_regPlot = new Registration(m_meshData, m_templateMesh,
                                             m_templateSurfaceSliders, this);
                m_regPlot->SetAnchors(totalSource, totalTamplate);
                if (m_typeINOL == 0 && m_curveNOS == 0) {
                    m_regPlot->SetAccuracy(0);
                }
            }

        } else {
            m_iren->SetInteractorStyle(m_PointPickerStyle);
            surfacePatchToolbar->setVisible(true);
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
        slidingButton->setToolTip("Slide Semi-Landmarks");
        m_iren->SetInteractorStyle(m_style);
        delete m_regPlot;
        m_regPlot = nullptr;
        if (m_surfaceBlock->GetNumberOfBlocks() > 0 ||
            m_surfaceLandmarks->GetNumberOfPoints() > 0) {
            FinalizeSurfaceScene();
            PrepareSliding();
        }
    }
}

void SpecimenDigitiser::CurveTool() {
    if (curveSliderButton->isChecked()) {
        surfaceSliderButton->setChecked(false);
        landmarkButton->setChecked(false);
        SurfaceTool();
        TypeITool();
        m_iren->SetInteractorStyle(m_PointPickerStyle);
        curveToolbar->setVisible(true);
        slidingButton->setEnabled(0);
        m_slidingAnimation->stop();
        slidingButton->setToolTip(
            "Can not do sliding when the curve digitising tool is active");
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
        slidingButton->setToolTip("Slide Semi-Landmarks");
        m_iren->SetInteractorStyle(m_style);
        pickFromBoundariesBox->setChecked(0);
        PickFromBoundaries();
        if (m_curveCtrlBlock->GetNumberOfBlocks() > 0) {
            FinalizeCurveScene();
            PrepareSliding();
        }
    }
}

void SpecimenDigitiser::PrepareSliding() {
    int numCurveSliders = 0;
    int numSurfaceSliders = 0;
    int numTotalSliders = 0;
    bool conditionOK = 1;
    if (m_fixedLandmarks->GetNumberOfPoints() == m_typeINOL) {
        if (m_curveNOS != 0) {
            if (m_curveBlock->GetNumberOfBlocks() == m_curveNOC) {
                numCurveSliders = m_curveNOS * m_curveNOC;
            } else {
                conditionOK = 0;
                slidingButton->setEnabled(0);
                m_slidingAnimation->stop();
            }
        }
        if (m_surfacePatchUNOS != 0 && m_surfacePatchVNOS != 0 &&
            m_surfaceNOS == 0) {
            if (m_surfaceBlock->GetNumberOfBlocks() == m_surfacePatchNOP) {
                numSurfaceSliders =
                    m_surfacePatchUNOS * m_surfacePatchVNOS * m_surfacePatchNOP;
            } else {
                conditionOK = 0;
                slidingButton->setEnabled(0);
                m_slidingAnimation->stop();
            }
        }
        if (m_surfacePatchUNOS == 0 && m_surfacePatchVNOS == 0 &&
            m_surfaceNOS != 0) {
            if (m_surfaceLandmarks->GetNumberOfPoints() == m_surfaceNOS) {
                numSurfaceSliders = m_surfaceLandmarks->GetNumberOfPoints();
            } else {
                conditionOK = 0;
                slidingButton->setEnabled(0);
                m_slidingAnimation->stop();
            }
        }
        if (conditionOK) {
            numTotalSliders = numCurveSliders + numSurfaceSliders;
        }
        if (numTotalSliders > 0) {
            slidingButton->setEnabled(1);
            m_slidingAnimation->start();
        }
    }
    if (m_fixedLandmarks->GetNumberOfPoints() == m_typeINOL &&
        m_curveNOS == 0 && m_surfacePatchUNOS == 0 && m_surfacePatchVNOS == 0 &&
        m_surfaceNOS == 0) {
        m_dataDigitized = 1;
        m_parent->SetTypeI(m_fixedLandmarks);
        landmarkButton->setEnabled(0);
        surfaceSliderButton->setEnabled(0);
        curveSliderButton->setEnabled(0);
    }
}

void SpecimenDigitiser::MakeSlide() {
    this->statusBar()->removeWidget(counterGroup);
    progressLineEdit->show();
    progressLineEdit->setText("Prepare Sliding...");
    statusLabel->setText("Status: Busy Preparing");
    progressLabel->setPixmap(QPixmap(":/icons/graphics/icons/busy.svg"));

    landmarkButton->setEnabled(0);
    surfaceSliderButton->setEnabled(0);
    curveSliderButton->setEnabled(0);
    slidingButton->setEnabled(0);
    m_slidingAnimation->stop();
    int numTotalSliders =
        (m_curveNOS * m_curveNOC) +
        (m_surfacePatchUNOS * m_surfacePatchVNOS * m_surfacePatchNOP) +
        m_surfaceNOS;
    // Take target coordinates to Eigen
    int numTotalLndmrks = numTotalSliders + m_typeINOL;
    Eigen::MatrixXd totalCoordinates(numTotalLndmrks, 3);
    for (int i = 0; i < m_typeINOL; i++) {
        totalCoordinates.operator()(i, 0) = m_fixedLandmarks->GetPoint(i)[0];
        totalCoordinates.operator()(i, 1) = m_fixedLandmarks->GetPoint(i)[1];
        totalCoordinates.operator()(i, 2) = m_fixedLandmarks->GetPoint(i)[2];
    }
    int curveStartIds = m_typeINOL;
    vtkNew<vtkDataObjectTreeIterator> curveIterPts;
    curveIterPts->SetDataSet(m_curveBlock);
    curveIterPts->SkipEmptyNodesOn();
    curveIterPts->VisitOnlyLeavesOn();
    for (curveIterPts->InitTraversal(); !curveIterPts->IsDoneWithTraversal();
         curveIterPts->GoToNextItem()) {
        vtkDataObject* dso = curveIterPts->GetCurrentDataObject();
        vtkPolyData* pd = dynamic_cast<vtkPolyData*>(dso);
        for (int i = 0; i < pd->GetNumberOfPoints(); i++) {
            totalCoordinates.operator()(i + curveStartIds, 0) =
                pd->GetPoint(i)[0];
            totalCoordinates.operator()(i + curveStartIds, 1) =
                pd->GetPoint(i)[1];
            totalCoordinates.operator()(i + curveStartIds, 2) =
                pd->GetPoint(i)[2];
        }
        curveStartIds += m_curveNOS;
    }
    int surfaceStartIds = m_typeINOL + (m_curveNOS * m_curveNOC);
    if (m_surfaceNOS == 0 && m_surfacePatchUNOS * m_surfacePatchVNOS != 0) {
        vtkNew<vtkDataObjectTreeIterator> surfaceIterPts;
        surfaceIterPts->SetDataSet(m_surfaceBlock);
        surfaceIterPts->SkipEmptyNodesOn();
        surfaceIterPts->VisitOnlyLeavesOn();
        for (surfaceIterPts->InitTraversal();
             !surfaceIterPts->IsDoneWithTraversal();
             surfaceIterPts->GoToNextItem()) {
            vtkDataObject* dso = surfaceIterPts->GetCurrentDataObject();
            vtkPolyData* pd = dynamic_cast<vtkPolyData*>(dso);
            for (int i = 0; i < pd->GetNumberOfPoints(); i++) {
                totalCoordinates.operator()(i + surfaceStartIds, 0) =
                    pd->GetPoint(i)[0];
                totalCoordinates.operator()(i + surfaceStartIds, 1) =
                    pd->GetPoint(i)[1];
                totalCoordinates.operator()(i + surfaceStartIds, 2) =
                    pd->GetPoint(i)[2];
            }
            surfaceStartIds += (m_surfacePatchUNOS * m_surfacePatchVNOS);
        }
    }
    if (m_surfaceNOS != 0 && m_surfacePatchUNOS * m_surfacePatchVNOS == 0) {
        for (int i = 0; i < m_surfaceNOS; i++) {
            totalCoordinates.operator()(i + surfaceStartIds, 0) =
                m_surfaceLandmarks->GetPoint(i)[0];
            totalCoordinates.operator()(i + surfaceStartIds, 1) =
                m_surfaceLandmarks->GetPoint(i)[1];
            totalCoordinates.operator()(i + surfaceStartIds, 2) =
                m_surfaceLandmarks->GetPoint(i)[2];
        }
    }
    //--------------------
    // Take template coordinates to Eigen

    Eigen::MatrixXd totalTemplateCoordinates(numTotalLndmrks, 3);
    for (int i = 0; i < m_typeINOL; i++) {
        totalTemplateCoordinates.operator()(i, 0) =
            m_templateTypeI->GetPoint(i)[0];
        totalTemplateCoordinates.operator()(i, 1) =
            m_templateTypeI->GetPoint(i)[1];
        totalTemplateCoordinates.operator()(i, 2) =
            m_templateTypeI->GetPoint(i)[2];
    }
    vtkNew<vtkDataObjectTreeIterator> templateCurveIterPts;
    templateCurveIterPts->SetDataSet(m_templateCurveSliders);
    templateCurveIterPts->SkipEmptyNodesOn();
    templateCurveIterPts->VisitOnlyLeavesOn();
    curveStartIds = m_typeINOL;
    for (templateCurveIterPts->InitTraversal();
         !templateCurveIterPts->IsDoneWithTraversal();
         templateCurveIterPts->GoToNextItem()) {
        vtkDataObject* dso = templateCurveIterPts->GetCurrentDataObject();
        vtkPolyData* pd = dynamic_cast<vtkPolyData*>(dso);
        for (int i = 0; i < pd->GetNumberOfPoints(); i++) {
            totalTemplateCoordinates.operator()(i + curveStartIds, 0) =
                pd->GetPoint(i)[0];
            totalTemplateCoordinates.operator()(i + curveStartIds, 1) =
                pd->GetPoint(i)[1];
            totalTemplateCoordinates.operator()(i + curveStartIds, 2) =
                pd->GetPoint(i)[2];
        }
        curveStartIds += m_curveNOS;
    }
    if (m_surfaceNOS == 0 && m_surfacePatchUNOS * m_surfacePatchVNOS != 0) {
        vtkNew<vtkDataObjectTreeIterator> templateSurfaceIterPts;
        templateSurfaceIterPts->SetDataSet(m_templatePatchSurfaceSliders);
        templateSurfaceIterPts->SkipEmptyNodesOn();
        templateSurfaceIterPts->VisitOnlyLeavesOn();
        surfaceStartIds = m_typeINOL + (m_curveNOS * m_curveNOC);
        for (templateSurfaceIterPts->InitTraversal();
             !templateSurfaceIterPts->IsDoneWithTraversal();
             templateSurfaceIterPts->GoToNextItem()) {
            vtkDataObject* dso = templateSurfaceIterPts->GetCurrentDataObject();
            vtkPolyData* pd = dynamic_cast<vtkPolyData*>(dso);
            for (int i = 0; i < pd->GetNumberOfPoints(); i++) {
                totalTemplateCoordinates.operator()(i + surfaceStartIds, 0) =
                    pd->GetPoint(i)[0];
                totalTemplateCoordinates.operator()(i + surfaceStartIds, 1) =
                    pd->GetPoint(i)[1];
                totalTemplateCoordinates.operator()(i + surfaceStartIds, 2) =
                    pd->GetPoint(i)[2];
            }
            surfaceStartIds += (m_surfacePatchUNOS * m_surfacePatchVNOS);
        }
    }
    if (m_surfaceNOS != 0 && m_surfacePatchUNOS * m_surfacePatchVNOS == 0) {
        auto templateSlider = m_parent->GetTemplateSurfaceSliders();
        for (int i = 0; i < m_surfaceNOS; i++) {
            totalTemplateCoordinates.operator()(i + surfaceStartIds, 0) =
                templateSlider->GetPoint(i)[0];
            totalTemplateCoordinates.operator()(i + surfaceStartIds, 1) =
                templateSlider->GetPoint(i)[1];
            totalTemplateCoordinates.operator()(i + surfaceStartIds, 2) =
                templateSlider->GetPoint(i)[2];
        }
    }
    m_curvePointActorDeactive->GetProperty()->SetOpacity(0.1);
    m_surfacePatchPointDeactiveActor->GetProperty()->SetOpacity(0.1);
    m_surfacePointDeactiveActor->GetProperty()->SetOpacity(0.1);
    m_renderer->GetRenderWindow()->Render();

    qRegisterMetaType<Eigen::MatrixXd>("Eigen::MatrixXd");
    delete m_slidingThread;
    m_slidingThread = new SlidingThread(
        m_meshData, m_typeINOL, m_curveNOS, m_curveNOC, m_curveType,
        m_surfaceNOS, m_surfacePatchUNOS, m_surfacePatchVNOS, m_surfacePatchNOP,
        m_curvePolyLineBlock, m_surfaceMaskBlock, totalTemplateCoordinates,
        totalCoordinates);
    m_slidingThread->setParent(this);

    connect(m_slidingThread, &SlidingThread::CoordinateChanged, this,
            &SpecimenDigitiser::OnCoordinateChanged);
    connect(m_slidingThread, &SlidingThread::CoordinateNotChanged, this,
            &SpecimenDigitiser::OnCoordinateNotChanged);

    m_slidingThread->start();
    SlidingStatus();
}

void SpecimenDigitiser::OnStatusChanged(int status) {
    double scale = m_slidingThread->GetScalingFactor();
    double BE = m_slidingThread->GetBE();
    bool improving = m_slidingThread->GetImprovement();
    std::string solverType = m_slidingThread->GetSolverType();
    int loopCount = m_slidingThread->GetImprovementLoop();
    std::stringstream scaleStream;
    scaleStream << std::fixed << std::setprecision(3) << scale;

    std::string progressText = "";
    if (!improving) {
        progressText = "Solver: " + solverType + " | " +
                       "Damping Factor: " + scaleStream.str() + " | " +
                       "Bending Energy: " + std::to_string(BE);
    }
    if (improving) {
        progressText = "Solver: " + solverType + " | " +
                       " Improving BE minimisation (Attempts: " +
                       std::to_string(loopCount) + ")";
    }
    progressLineEdit->setText(QString::fromUtf8(progressText.c_str()));

    if (status > 0) {
        statusLabel->setText("Status: Sliding");
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

void SpecimenDigitiser::SlidingStatus() {
    if (m_slidingThread) {
        delete m_slidingStatThread;
        m_slidingStatThread = new StatusReporterThread(m_slidingThread);
        m_slidingStatThread->setParent(this);
        connect(m_slidingStatThread, &StatusReporterThread::StatusChanged, this,
                &SpecimenDigitiser::OnStatusChanged);
        m_slidingStatThread->start();
    }
}

void SpecimenDigitiser::OnCoordinateChanged(Eigen::MatrixXd sendOffData) {
    FinalizeDigitization(sendOffData, 1);
}
void SpecimenDigitiser::OnCoordinateNotChanged(Eigen::MatrixXd sendOffData) {
    FinalizeDigitization(sendOffData, 0);
}

void SpecimenDigitiser::FinalizeDigitization(Eigen::MatrixXd& Lndmrks,
                                             bool sendOffData) {
    int numCurveSliders = 0;
    int numSurfaceSliders = 0;
    vtkNew<vtkPoints> fixedPts;
    vtkNew<vtkPoints> curvePts;
    vtkNew<vtkPoints> surfacePts;

    if (m_typeINOL != 0) {
        auto fixedCoordsBlock = Lndmrks.block(0, 0, m_typeINOL, 3);
        for (int i = 0; i < m_typeINOL; i++) {
            fixedPts->InsertNextPoint(fixedCoordsBlock(i, 0),
                                      fixedCoordsBlock(i, 1),
                                      fixedCoordsBlock(i, 2));
        }
        vtkNew<vtkPolyData> tempFixedPtsPoly;
        tempFixedPtsPoly->SetPoints(fixedPts);
        m_fixedVertexFilter->SetInputData(tempFixedPtsPoly);
        m_fixedVertexFilter->Update();
        m_fixedVertexFilter->Modified();
    }

    if (m_curveNOS != 0) {
        numCurveSliders = m_curveNOS * m_curveNOC;
        int start = m_typeINOL;
        auto curveCoordsBlock = Lndmrks.block(start, 0, numCurveSliders, 3);
        for (int i = 0; i < numCurveSliders; i++) {
            curvePts->InsertNextPoint(curveCoordsBlock(i, 0),
                                      curveCoordsBlock(i, 1),
                                      curveCoordsBlock(i, 2));
        }
        curvePts->Modified();
        vtkNew<vtkPolyData> tempCurvePtsPoly;
        tempCurvePtsPoly->SetPoints(curvePts);
        m_curveVertexFilterActive->SetInputData(tempCurvePtsPoly);
        m_curveVertexFilterActive->Update();
        m_curveVertexFilterActive->Modified();
    }

    if (m_surfacePatchUNOS != 0 && m_surfacePatchVNOS != 0 &&
        m_surfaceNOS == 0) {
        numSurfaceSliders =
            (m_surfacePatchUNOS * m_surfacePatchVNOS) * m_surfacePatchNOP;
        int start = m_typeINOL + numCurveSliders;
        auto surfaceCoordsBlock = Lndmrks.block(start, 0, numSurfaceSliders, 3);
        for (int i = 0; i < numSurfaceSliders; i++) {
            surfacePts->InsertNextPoint(surfaceCoordsBlock(i, 0),
                                        surfaceCoordsBlock(i, 1),
                                        surfaceCoordsBlock(i, 2));
        }
        surfacePts->Modified();
        vtkNew<vtkPolyData> tempSurfacePtsPoly;
        tempSurfacePtsPoly->SetPoints(surfacePts);
        m_surfacePatchVertexActiveFilter->SetInputData(tempSurfacePtsPoly);
        m_surfacePatchVertexActiveFilter->Update();
        m_surfacePatchVertexActiveFilter->Modified();
    }
    if (m_surfacePatchUNOS == 0 && m_surfacePatchVNOS == 0 &&
        m_surfaceNOS != 0) {
        numSurfaceSliders = m_surfaceNOS;
        int start = m_typeINOL + numCurveSliders;
        auto surfaceCoordsBlock = Lndmrks.block(start, 0, numSurfaceSliders, 3);
        for (int i = 0; i < numSurfaceSliders; i++) {
            surfacePts->InsertNextPoint(surfaceCoordsBlock(i, 0),
                                        surfaceCoordsBlock(i, 1),
                                        surfaceCoordsBlock(i, 2));
        }
        surfacePts->Modified();
        vtkNew<vtkPolyData> tempSurfacePtsPoly;
        tempSurfacePtsPoly->SetPoints(surfacePts);

        m_surfaceVertexFilter->SetInputData(tempSurfacePtsPoly);
        m_surfaceVertexFilter->Update();
        m_surfaceVertexFilter->Modified();
    }
    m_renderer->GetRenderWindow()->Render();
    if (sendOffData) {
        m_dataDigitized = 1;
        m_parent->SetSliders(fixedPts, curvePts, surfacePts);
    }
}

void SpecimenDigitiser::DebugPrintMatrix(Eigen::MatrixXd matrix) {
    Eigen::IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");
    std::string sep = "\n----------------------------------------\n";
    std::cout << matrix.format(CleanFmt) << sep;
}

void SpecimenDigitiser::PDist(vtkPoints* points, Eigen::MatrixXd& output) {
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

void SpecimenDigitiser::PDist(Eigen::MatrixXd& points,
                              Eigen::MatrixXd& output) {
    int dim = points.rows();
    for (int i = 0; i < dim; i++) {
        for (int j = i + 1; j < dim; j++) {
            double dist = EucDist(points(i, 0), points(i, 1), points(i, 2),
                                  points(j, 0), points(j, 1), points(j, 2));
            output.operator()(i, j) = dist;
        }
    }
    output.triangularView<Eigen::Lower>() = output.transpose();
}

double SpecimenDigitiser::EucDist(double Ax, double Ay, double Az, double Bx,
                                  double By, double Bz) {
    double dx = Ax - Bx;
    double dy = Ay - By;
    double dz = Az - Bz;
    double dist = sqrt((dx * dx) + (dy * dy) + (dz * dz));
    return dist;
}

void SpecimenDigitiser::AddSurface() {
    surfaceAddButton->setEnabled(0);
    if (m_surfaceCurveCtrlBlock->GetNumberOfBlocks() < m_surfacePatchNOP) {
        surfacePickSourceComboBox->setEnabled(1);
        m_surfacePtsIds->push_back(new std::vector<int>);
        surfaceIronButton->setEnabled(0);
        surfaceIronButton->setChecked(0);
        m_ironAnimation->stop();
        DrapeToSurface();
        surfaceInterpolateButton->setEnabled(0);
        m_interpolationAnimation->stop();
        surfaceLockButton->setChecked(0);
        ChangeSurfaceLock();
        m_currentSurfaceId = m_surfaceCurveCtrlBlock->GetNumberOfBlocks() - 1;
        m_currentSurfaceId += 1;

        surfaceSelectComboBox->addItem(
            QString::fromStdString(std::to_string(m_currentSurfaceId)));
        surfaceSelectComboBox->setCurrentIndex(m_currentSurfaceId);
    }
}

void SpecimenDigitiser::AddCurve() {
    curveAddButton->setEnabled(0);
    if (m_curveCtrlBlock->GetNumberOfBlocks() < m_curveNOC) {
        curvePickSourceComboBox->setEnabled(1);
        m_currentCurveId = m_curveCtrlBlock->GetNumberOfBlocks() - 1;
        m_currentCurveId += 1;
        curveSelectComboBox->addItem(
            QString::fromStdString(std::to_string(m_currentCurveId)));
        curveSelectComboBox->setCurrentIndex(m_currentCurveId);
        m_curvePoly->Initialize();
        m_curveTubeFilter->SetInputData(m_curvePoly);
        m_curveTubeFilter->Update();
        m_curveTubeFilter->Modified();
        m_renderer->GetRenderWindow()->Render();
    }
}

void SpecimenDigitiser::ChangeCurve(int index) {
    if (m_curveCtrlBlock->GetNumberOfBlocks() > 0) {
        m_currentCurveId = index;
        ResetCurveScene();
        UpdateCurveScene(m_currentCurveId);
        if (m_curveType->at(m_currentCurveId) == 1) {
            curveTypeButton->setChecked(0);
            ChangeCurveType();
        } else if (m_curveType->at(m_currentCurveId) == 0) {
            curveTypeButton->setChecked(1);
            ChangeCurveType();
        }
    }
}

void SpecimenDigitiser::ChangeCurveSource(int index) {
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
            UpdateCurveData(m_curveHighlightCtrlPoints, m_curvePoly,
                            m_curveLandmarks, m_meshData);
            m_curvePoly->Modified();
            m_curveTubeFilter->SetInputData(m_curvePoly);
            m_curveTubeFilter->Update();
            m_curveTubeFilter->Modified();
            m_curvePointsPoly->SetPoints(m_curveLandmarks);
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

void SpecimenDigitiser::ChangeSurfaceSource(int index) {
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
            surfaceIronButton->setEnabled(0);
            surfaceIronButton->setChecked(0);
            m_ironAnimation->stop();
            vtkNew<vtkPolyData> tempCtrlPoly;
            tempCtrlPoly->DeepCopy(m_surfaceCurveCtrlPointsPoly);
            m_surfaceCurveCtrlBlock->SetBlock(m_currentSurfaceId, tempCtrlPoly);
            m_surfaceCurveCtrlBlock->Modified();
            MeshCutter(m_surfaceCurveHighlightCtrlPoints);
            if (!surfaceInterpolateButton->isEnabled()) {
                surfaceInterpolateButton->setEnabled(1);
                m_interpolationAnimation->start();
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

void SpecimenDigitiser::ChangeSurface(int index) {
    if (m_surfaceCurveCtrlBlock->GetNumberOfBlocks() > 0) {
        m_currentSurfaceId = index;
        ResetSurfaceScene();
        UpdateSurfaceScene(m_currentSurfaceId);
    }
}

void SpecimenDigitiser::ChangeSurfaceLock() {
    if (surfaceLockButton->isChecked()) {
        m_editableSurface = 0;
        surfaceLockButton->setIcon(QIcon(":/icons/graphics/icons/locked.svg"));
    } else {
        m_editableSurface = 1;
        surfaceLockButton->setIcon(
            QIcon(":/icons/graphics/icons/unlocked.svg"));
    }
}

void SpecimenDigitiser::ChangeCurveType() {
    if (curveTypeButton->isChecked()) {
        curveTypeButton->setIcon(
            QIcon(":/icons/graphics/icons/open_curve.svg"));
        m_curveType->at(m_currentCurveId) = 0;
        if (m_curveHighlightCtrlPoints->GetNumberOfPoints() > 2) {
            if (pickFromBoundariesBox->isChecked()) {
                UpdateCurveData(m_curveHighlightCtrlPoints, m_curvePoly,
                                m_curveLandmarks, m_meshBoundaries);
            } else {
                UpdateCurveData(m_curveHighlightCtrlPoints, m_curvePoly,
                                m_curveLandmarks, m_meshData);
            }
            m_curveTubeFilter->SetInputData(m_curvePoly);
            m_curveTubeFilter->Update();
            m_curveTubeFilter->Modified();
            m_curvePointsPoly->SetPoints(m_curveLandmarks);
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
            if (pickFromBoundariesBox->isChecked()) {
                UpdateCurveData(m_curveHighlightCtrlPoints, m_curvePoly,
                                m_curveLandmarks, m_meshBoundaries);
            } else {
                UpdateCurveData(m_curveHighlightCtrlPoints, m_curvePoly,
                                m_curveLandmarks, m_meshData);
            }
            m_curveTubeFilter->SetInputData(m_curvePoly);
            m_curveTubeFilter->Update();
            m_curveTubeFilter->Modified();
            m_curvePointsPoly->SetPoints(m_curveLandmarks);
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

void SpecimenDigitiser::ClearCurve() {
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
    m_curveLandmarks->Initialize();
    m_curveTotalLandmarks->Initialize();
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
    curveLineEditNOC->setText(QString::fromStdString(
        std::to_string(m_curveNOC - m_curveCtrlBlock->GetNumberOfBlocks())));
    curveTypeButton->setChecked(0);
    ChangeCurveType();
    pickFromBoundariesBox->setChecked(0);
    PickFromBoundaries();
    m_renderer->GetRenderWindow()->Render();
}

void SpecimenDigitiser::ClearSurface() {
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
    m_surfaceCurvePoly->Initialize();

    m_surfaceCtrlPointsPoly->Initialize();
    m_surfaceCtrlVertexFilter->Update();
    m_surfaceCtrlVertexFilter->Modified();
    m_surfacePatchLandmarks->Initialize();
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
    m_surfaceMaskBlock->Initialize();
    surfaceAddButton->setEnabled(0);
    int numOfPatches =
        m_surfacePatchNOP - (m_surfaceBlock->GetNumberOfBlocks());
    surfaceLineEditNOP->setText(
        QString::fromStdString(std::to_string(numOfPatches)));
    surfaceIronButton->setEnabled(0);
    surfaceIronButton->setChecked(0);
    m_ironAnimation->stop();
    DrapeToSurface();
    surfaceInterpolateButton->setEnabled(0);
    m_interpolationAnimation->stop();
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

    m_surfaceMask->Initialize();
    m_surfaceMaskBlock->Initialize();

    m_renderer->GetRenderWindow()->Render();
}

void SpecimenDigitiser::DrapeToSurface() {
    if (surfaceIronButton->isChecked()) {
        // Set opacity to 1.0 immediately
        QGraphicsOpacityEffect* effect = qobject_cast<QGraphicsOpacityEffect*>(surfaceIronButton->graphicsEffect());
        if (effect) {
            effect->setOpacity(1.0);
        }
        m_ironAnimation->stop();
        surfaceIronButton->setIcon(QIcon(":/icons/graphics/icons/ironOn.svg"));
    }
    if (!surfaceIronButton->isChecked()) {
        m_ironAnimation->stop();
        surfaceIronButton->setIcon(QIcon(":/icons/graphics/icons/ironOff.svg"));
        surfaceLockButton->setChecked(1);
        ChangeSurfaceLock();
    }
}

void SpecimenDigitiser::ResetCurveScene() {
    m_curveHighlightCtrlPoints->Initialize();
    m_curvectrlPointsPoly->Initialize();
    m_curvectrlVertexFilter->Update();
    m_curvectrlVertexFilter->Modified();

    m_curveLandmarks->Initialize();
    m_curvePointsPoly->Initialize();
    if (m_curveBlock->GetNumberOfBlocks() > 0) {
        m_curveVertexFilter->Update();
        m_curveVertexFilter->Modified();
    }
    m_curvePoly->Initialize();
    m_curveTubeFilter->SetInputData(m_curvePoly);
    m_curveTubeFilter->Update();
    m_curveTubeFilter->Modified();
    m_renderer->GetRenderWindow()->Render();
}

void SpecimenDigitiser::ResetSurfaceScene() {
    if (m_surfaceCurvePoly->GetNumberOfPoints() > 0 &&
        m_surfacePatchLandmarks->GetNumberOfPoints() == 0) {
        QMessageBox::information(
            this, "Information",
            "Pre-sliders will be digitised automatically!");
        InterpolateSurface();
    }
    m_surfaceCurveHighlightCtrlPoints->Initialize();
    m_surfaceCurveCtrlPointsPoly->Initialize();
    m_surfaceCurveCtrlVertexFilter->Update();
    m_surfaceCurveCtrlVertexFilter->Modified();
    m_surfaceCurvePoly->Initialize();
    m_surfacePatchLandmarks->Initialize();
    m_surfaceCtrlPointsPoly->Initialize();
    m_surfaceCtrlVertexFilter->Update();
    m_surfaceCtrlVertexFilter->Modified();
    vtkNew<vtkPolyData> emptyPoly;
    m_surfaceCurveTubeFilter->SetInputData(emptyPoly);
    m_surfaceCurveTubeFilter->Update();
    m_surfaceCurveTubeFilter->Modified();

    m_surfaceTubeFilter->SetInputData(emptyPoly);
    m_surfaceTubeFilter->Update();
    m_surfaceTubeFilter->Modified();

    m_surfacePatchVertexFilter->SetInputData(emptyPoly);
    m_surfacePatchVertexFilter->Update();
    m_surfacePatchVertexFilter->Modified();

    m_surfaceMask->Initialize();

    m_renderer->GetRenderWindow()->Render();
}

void SpecimenDigitiser::UpdateCurveScene(int id) {
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
                                    m_curveLandmarks, m_meshData);
                    m_curvePoly->Modified();
                    m_curveTubeFilter->SetInputData(m_curvePoly);
                    m_curveTubeFilter->Update();
                    m_curveTubeFilter->Modified();
                    m_curveLandmarks->Modified();
                    m_curvePointsPoly->SetPoints(m_curveLandmarks);
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

void SpecimenDigitiser::UpdateSurfaceScene(int id) {
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
                surfaceIronButton->setEnabled(0);
                surfaceIronButton->setChecked(0);
                m_ironAnimation->stop();
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
                    m_surfacePatchLandmarks->InsertNextPoint(
                        tempSurfaceSliders->GetPoint(i));
                }

                vtkNew<vtkPolyData> tempSlidersPoly;
                tempSlidersPoly->SetPoints(m_surfacePatchLandmarks);
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

void SpecimenDigitiser::FinalizeCurveScene() {
    if (m_curveBlock->GetNumberOfBlocks() == m_curveNOC) {
        ResetCurveScene();
        m_curveTotalLandmarks->Initialize();
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
                    m_curveTotalLandmarks->InsertNextPoint(pd->GetPoint(j));
                }
            } else {
                QMessageBox warning(this);
                warning.setText(
                    "Your last curve is not completed. Add more control "
                    "points! Nothing will be registered!");
                warning.exec();
                break;
            }
        }
        m_curveTotalLandmarks->Modified();
        vtkNew<vtkPolyData> tempPtsPoly;
        tempPtsPoly->SetPoints(m_curveTotalLandmarks);
        m_curveVertexFilterDeactive->SetInputData(tempPtsPoly);
        m_curveVertexFilterDeactive->Update();
        m_curveVertexFilterDeactive->Modified();
        m_curvePointActorDeactive->GetProperty()->SetOpacity(1);

        vtkNew<vtkPolyData> emptyPoly;
        m_curveTubeFilterDeactive->SetInputData(emptyPoly);
        m_curveTubeFilterDeactive->Update();
        m_curveTubeFilterDeactive->Modified();

        m_renderer->GetRenderWindow()->Render();
    } else {
        QMessageBox warning(this);
        warning.setText(
            "Number of digitized curves are less than the decided number. "
            "Nothing will be registered!");
        warning.exec();
    }
}

void SpecimenDigitiser::FinalizeSurfaceScene() {
    if (m_surfaceBlock->GetNumberOfBlocks() == m_surfacePatchNOP &&
        m_surfaceNOS == 0) {
        ResetSurfaceScene();
        m_surfaceCurveCtrlPointsPoly->Initialize();
        m_surfacePatchLandmarks->Initialize();
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

        /* m_glyphSurfaceArrow->SetInputData(emptyPoly);
        m_glyphSurfaceArrow->Update();
        m_glyphSurfaceArrow->Modified(); */

        m_renderer->GetRenderWindow()->Render();
    }
    if (m_surfaceBlock->GetNumberOfBlocks() == 0 &&
        m_surfaceNOS == m_surfaceLandmarks->GetNumberOfPoints()) {
    }
    if ((!m_surfaceBlock->GetNumberOfBlocks() == m_surfacePatchNOP) &&
        m_surfaceNOS == 0) {
        QMessageBox warning(this);
        warning.setText(
            "Number of digitized Surface Patches are less than the decided "
            "number. Nothing will be registered!");
        warning.exec();
    }
}

void SpecimenDigitiser::UpdateCurveData(vtkPoints* pts, vtkPolyData* outputLine,
                                        vtkPoints* outputPoints,
                                        vtkPolyData* baseMesh) {
    // Here put closed or open curve
    if (pts->GetNumberOfPoints() > 2) {
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
            vtkNew<vtkCellLocator> m_ptLocator;
            m_ptLocator->SetDataSet(baseMesh);
            m_ptLocator->BuildLocator();
            for (int i = 0; i < tempPoints->GetNumberOfPoints(); i++) {
                double closestPoint[3];
                vtkIdType closestCellId = -1;
                int subId = -1;
                double dist = -1;
                m_ptLocator->FindClosestPoint(tempPoints->GetPoint(i),
                                              closestPoint, closestCellId,
                                              subId, dist);
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

        // delete intp;
    }
}

void SpecimenDigitiser::ConstructSurfaceData(vtkPolyData* CtrlPtsPoly,
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

void SpecimenDigitiser::ConstructSurfaceData(vtkPoints* pts,
                                             vtkPoints* outputSliders,
                                             vtkPolyData* outputCtrlPtsPoly) {
    if (pts->GetNumberOfPoints() > 2) {
        outputSliders->Initialize();
        outputCtrlPtsPoly->Initialize();
        vtkNew<vtkPolyData> planePolyInit;
        MakeCage(pts, planePolyInit);
        std::vector<int>* outlineIds = new std::vector<int>();
        OutlineIdFinder(m_surfacePatchUNOS, m_surfacePatchVNOS, outlineIds);

        vtkPolyData* planePolyDeformed = planePolyInit;
        outputCtrlPtsPoly->DeepCopy(planePolyDeformed);
        vtkNew<vtkPoints> tempSliders;
        for (int i = 0; i < outputCtrlPtsPoly->GetNumberOfPoints(); i++) {
            auto it = std::find(outlineIds->begin(), outlineIds->end(), i);
            if (it != outlineIds->end()) {
            } else {
                tempSliders->InsertNextPoint(outputCtrlPtsPoly->GetPoint(i));
            }
        }
        if (outputCtrlPtsPoly->GetNumberOfPoints() > 0) {
            vtkNew<vtkCellLocator> ptLocator;
            ptLocator->SetDataSet(m_surfaceCurvePoly);
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

void SpecimenDigitiser::MakeCage(vtkPoints* inputPts,
                                 vtkPolyData* outPlanePoly) {
    outPlanePoly->Initialize();

    // 1. Create and configure the spline
    vtkNew<vtkParametricSpline> spline;
    spline->SetPoints(inputPts);
    spline->SetClosed(1);

    // 2. Sample 3 initial points
    vtkNew<vtkPoints> curveInit3Pts;
    double delta = 1.0 / 3.0;
    for (int i = 0; i < 3; i++) {
        double u[3] = {delta * i, 0, 0};
        double pt[3];
        spline->Evaluate(u, pt, nullptr);
        curveInit3Pts->InsertNextPoint(pt);
    }

    // 3. Calculate curve length
    vtkNew<vtkPolyData> curvePoly;
    vtkNew<vtkCellArray> lines;
    lines->InsertNextCell(inputPts->GetNumberOfPoints());
    for (vtkIdType i = 0; i < inputPts->GetNumberOfPoints(); i++) {
        lines->InsertCellPoint(i);
    }
    curvePoly->SetPoints(inputPts);
    curvePoly->SetLines(lines);

    vtkNew<vtkAppendArcLength> arcLengthFilter;
    arcLengthFilter->SetInputData(curvePoly);
    arcLengthFilter->Update();
    double totalLength = arcLengthFilter->GetOutput()
                             ->GetPointData()
                             ->GetArray("arc_length")
                             ->GetTuple1(inputPts->GetNumberOfPoints() - 1);

    // 4. Determine resolution
    int dynamicRes = static_cast<int>(std::sqrt(totalLength) * 0.5);
    dynamicRes = (dynamicRes % 2 == 0) ? dynamicRes + 1 : dynamicRes;

    // 5. Create initial plane
    vtkNew<vtkPlaneSource> plane;
    plane->SetXResolution(dynamicRes);
    plane->SetYResolution(dynamicRes);
    plane->SetOrigin(curveInit3Pts->GetPoint(0));
    plane->SetPoint1(curveInit3Pts->GetPoint(1));
    plane->SetPoint2(curveInit3Pts->GetPoint(2));
    plane->Update();

    // 6. Resample curve points
    int resampleNumber = ((dynamicRes + 1) * 2) + (((dynamicRes + 1) - 2) * 2);
    vtkNew<vtkPoints> curveResampledPts;
    delta = 1.0 / resampleNumber;
    for (int i = 0; i < resampleNumber; i++) {
        double u[3] = {delta * i, 0, 0};
        double pt[3];
        spline->Evaluate(u, pt, nullptr);
        curveResampledPts->InsertNextPoint(pt);
    }

    // 7. Get plane boundary points in order
    vtkNew<vtkPoints> planeBoundaryPts;
    GetPlaneBoundaryPoints(plane->GetOutput(), planeBoundaryPts);

    // 8. Apply Thin Plate Spline transform
    vtkNew<vtkThinPlateSplineTransform> tps;
    tps->SetSourceLandmarks(planeBoundaryPts);
    tps->SetTargetLandmarks(curveResampledPts);
    tps->SetBasisToR();
    tps->SetSigma(1.0);

    vtkNew<vtkTransformPolyDataFilter> transform;
    transform->SetInputConnection(plane->GetOutputPort());
    transform->SetTransform(tps);
    transform->Update();

    // 9. Apply windowed sinc smoothing
    vtkNew<vtkWindowedSincPolyDataFilter> smoother;
    smoother->SetInputConnection(transform->GetOutputPort());
    smoother->SetNumberOfIterations(50);
    smoother->SetPassBand(0.1);
    smoother->SetFeatureEdgeSmoothing(1);
    smoother->SetFeatureAngle(5);
    smoother->SetBoundarySmoothing(0);
    smoother->NormalizeCoordinatesOn();
    smoother->Update();
    vtkPolyData* temp = smoother->GetOutput();
    // 10. Final projection and output
    ProjectOnMesh(temp, m_surfaceMask);
    vtkNew<vtkSmoothPolyDataFilter> repeller;
    repeller->SetInputData(temp);
    repeller->SetNumberOfIterations(200);
    repeller->SetConvergence(0.05);
    repeller->Update();

    // Create output grid
    int uRes = m_surfacePatchUNOS + 2;
    int vRes = m_surfacePatchVNOS + 2;

    vtkNew<vtkPoints> outputPoints;
    vtkNew<vtkFloatArray> parametricCoords;
    parametricCoords->SetNumberOfComponents(2);

    for (int v = 0; v < vRes; v++) {
        for (int u = 0; u < uRes; u++) {
            double uv[2] = {static_cast<double>(u) / (uRes - 1),
                            static_cast<double>(v) / (vRes - 1)};
            parametricCoords->InsertNextTuple(uv);

            // Map to deformed plane
            int srcU = static_cast<int>(uv[0] * dynamicRes);
            int srcV = static_cast<int>(uv[1] * dynamicRes);
            int idx = srcV * (dynamicRes + 1) + srcU;

            double pt[3];
            repeller->GetOutput()->GetPoint(idx, pt);
            outputPoints->InsertNextPoint(pt);
        }
    }

    vtkNew<vtkPolyData> result;
    result->SetPoints(outputPoints);
    result->GetPointData()->SetTCoords(parametricCoords);

    // Create grid topology
    vtkNew<vtkCellArray> polys;
    for (int v = 0; v < vRes - 1; v++) {
        for (int u = 0; u < uRes - 1; u++) {
            vtkIdType pts[4] = {v * uRes + u, v * uRes + u + 1,
                                (v + 1) * uRes + u + 1, (v + 1) * uRes + u};
            polys->InsertNextCell(4, pts);
        }
    }
    result->SetPolys(polys);

    outPlanePoly->DeepCopy(result);
}

// Helper function to get ordered boundary points
void SpecimenDigitiser::GetPlaneBoundaryPoints(vtkPolyData* plane,
                                               vtkPoints* boundaryPoints) {
    int dim = static_cast<int>(
        std::sqrt(plane->GetNumberOfPoints()));  // Assumes square grid

    // Top edge (left to right)
    for (int i = 0; i < dim; i++) {
        boundaryPoints->InsertNextPoint(plane->GetPoint(i));
    }

    // Right edge (top to bottom, skip first)
    for (int i = 1; i < dim; i++) {
        boundaryPoints->InsertNextPoint(plane->GetPoint(i * dim - 1));
    }

    // Bottom edge (right to left, skip first)
    for (int i = 1; i < dim; i++) {
        boundaryPoints->InsertNextPoint(plane->GetPoint(dim * dim - 1 - i));
    }

    // Left edge (bottom to top, skip first and last)
    for (int i = 1; i < dim - 1; i++) {
        boundaryPoints->InsertNextPoint(plane->GetPoint((dim - 1 - i) * dim));
    }
}

void SpecimenDigitiser::MeshCutter(vtkPoints* pts) {
    if (pts->GetNumberOfPoints() > 2) {
        m_surfaceMask->Initialize();
        m_surfaceCurvePoly->Initialize();
        int uRes = m_surfacePatchUNOS + 2;
        int vRes = m_surfacePatchVNOS + 2;
        int resampleNumber = (uRes - 1) * 2 + (vRes - 1) * 2;
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
        vtkNew<vtkPolyData> tempPoly;
        CutMeshWithCurve(m_meshData, curveResmplPts, tempPoly);
        m_surfaceMask->DeepCopy(tempPoly);

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
        m_surfaceCurvePoly->DeepCopy(splineFilter->GetOutput());
        m_surfaceCurvePoly->Modified();
        vtkNew<vtkPolyData> tempcurvePolyLine;
        tempcurvePolyLine->DeepCopy(m_surfaceCurvePoly);
        m_surfaceCurveBlock->SetBlock(m_currentSurfaceId, tempcurvePolyLine);

        m_surfaceCurveTubeFilter->SetInputData(m_surfaceCurvePoly);
        m_surfaceCurveTubeFilter->Update();
        m_surfaceCurveTubeFilter->Modified();
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

void SpecimenDigitiser::CutMeshWithCurve(vtkPolyData* inputMesh,
                                         vtkPoints* curvePoints,
                                         vtkPolyData* outputCutMesh) {
    if (!inputMesh || !curvePoints || curvePoints->GetNumberOfPoints() < 3) {
        std::cerr << "Invalid mesh or curve points." << std::endl;
        return;
    }
    // --- Step 0: Extract UNIQUE Touched Regions ---
    vtkNew<vtkConnectivityFilter> preSelector;
    preSelector->SetInputData(inputMesh);
    preSelector->SetExtractionModeToAllRegions();
    preSelector->ColorRegionsOn();
    preSelector->Update();

    // Find UNIQUE regions using first 3 curve points
    std::unordered_set<vtkIdType> touchedRegions;
    vtkNew<vtkPointLocator> bodyLocator;
    bodyLocator->SetDataSet(preSelector->GetOutput());
    bodyLocator->BuildLocator();

    const vtkIdType pointsToCheck =
        std::min<vtkIdType>(3, curvePoints->GetNumberOfPoints());
    for (int i = 0; i < pointsToCheck; i++) {
        double pt[3];
        curvePoints->GetPoint(i, pt);
        vtkIdType closestPt = bodyLocator->FindClosestPoint(pt);
        vtkIdType regionId =
            static_cast<vtkIdType>(preSelector->GetOutput()
                                       ->GetPointData()
                                       ->GetScalars()
                                       ->GetComponent(closestPt, 0));
        touchedRegions.insert(
            regionId);  // Set automatically handles duplicates
    }

    // Combine UNIQUE regions
    vtkNew<vtkAppendPolyData> regionCombiner;
    for (vtkIdType regionId : touchedRegions) {
        vtkNew<vtkThreshold> regionExtractor;
        regionExtractor->SetInputConnection(preSelector->GetOutputPort());
        regionExtractor->SetInputArrayToProcess(
            0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "RegionId");
        regionExtractor->ThresholdBetween(regionId, regionId);

        // Convert unstructured grid to polydata
        vtkNew<vtkGeometryFilter> converter;
        converter->SetInputConnection(regionExtractor->GetOutputPort());

        // Add the POLYDATA (not unstructured grid) to combiner
        regionCombiner->AddInputConnection(converter->GetOutputPort());
    }

    regionCombiner->Update();  // Combines all regions

    // Prepare final mesh for cutting
    vtkNew<vtkPolyData> relevantBodies;
    relevantBodies->DeepCopy(regionCombiner->GetOutput());
    relevantBodies->GetPointData()->RemoveArray("RegionId");

    // Step 1: Build a closed loop from curvePoints
    vtkIdType numPoints = curvePoints->GetNumberOfPoints();
    double first[3], last[3];
    curvePoints->GetPoint(0, first);
    curvePoints->GetPoint(numPoints - 1, last);

    vtkNew<vtkPoints> closedPoints;
    closedPoints->DeepCopy(curvePoints);

    if (vtkMath::Distance2BetweenPoints(first, last) > 1e-6) {
        closedPoints->InsertNextPoint(first);
        ++numPoints;
    }

    vtkNew<vtkIdList> cutterIdList;
    GetCutterCurve(relevantBodies, closedPoints, cutterIdList);
    relevantBodies->BuildLinks();
    for (int i = 0; i < cutterIdList->GetNumberOfIds(); i++) {
        relevantBodies->DeleteCell(cutterIdList->GetId(i));
    }
    relevantBodies->RemoveDeletedCells();

    // Step 2: Select the largest region outside the curve
    vtkNew<vtkConnectivityFilter> selector;
    selector->SetInputData(relevantBodies);
    selector->SetExtractionModeToLargestRegion();
    selector->ColorRegionsOn();
    selector->Update();

    // Step 3: Get the selection output and its scalars
    auto coloredRegions = selector->GetOutput();
    vtkDataArray* regionColors = coloredRegions->GetPointData()->GetScalars();

    if (!regionColors) {
        std::cerr << "No selection scalars found." << std::endl;
        return;
    }

    // Step 4: Create inverse selection array
    vtkNew<vtkIntArray> inverseSelection;
    inverseSelection->SetName("InverseSelection");
    inverseSelection->SetNumberOfComponents(1);
    inverseSelection->SetNumberOfTuples(relevantBodies->GetNumberOfPoints());
    inverseSelection->Fill(1);  // Initialize all as inside (1)

    // Create locator to map points between original and modified mesh
    vtkNew<vtkPointLocator> locator;
    locator->SetDataSet(coloredRegions);
    locator->BuildLocator();

    // Mark points belonging to the largest region (color=1) as outside (0)
    for (vtkIdType i = 0; i < relevantBodies->GetNumberOfPoints(); ++i) {
        double pt[3];
        relevantBodies->GetPoint(i, pt);
        vtkIdType closestId = locator->FindClosestPoint(pt);

        // Largest region will have scalar value = 1
        if (regionColors->GetComponent(closestId, 0) == 1.0) {
            inverseSelection->SetValue(i, 0);  // Mark as outside
        }
    }

    // Step 5: Add the array to our copied mesh
    relevantBodies->GetPointData()->AddArray(inverseSelection);

    // Step 6: Threshold to get only the inside region (where value == 0,
    // inverse of the largest region)
    vtkNew<vtkThreshold> threshold;
    threshold->SetInputData(relevantBodies);
    threshold->SetInputArrayToProcess(
        0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "InverseSelection");
    threshold->ThresholdBetween(0, 0);
    threshold->Update();

    // Convert back to PolyData and clean
    vtkNew<vtkGeometryFilter> geometryFilter;
    geometryFilter->SetInputConnection(threshold->GetOutputPort());
    geometryFilter->Update();

    vtkNew<vtkCleanPolyData> cleaner;
    cleaner->SetInputConnection(geometryFilter->GetOutputPort());
    cleaner->Update();

    outputCutMesh->ShallowCopy(cleaner->GetOutput());
    outputCutMesh->GetPointData()->RemoveArray("InverseSelection");
}

void SpecimenDigitiser::GetCutterCurve(vtkPolyData* Poly, vtkPoints* curvePts,
                                       vtkIdList* outCurveIds) {
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
}

void SpecimenDigitiser::DijkstraEdgeSearch(vtkPolyData* mesh,
                                           vtkPolyData* closedCurve,
                                           vtkIdList* edgePointIds) {
    mesh->BuildLinks();  // Critical for performance
    vtkNew<vtkDijkstraGraphGeodesicPath> edgeSearchFilter;
    edgeSearchFilter->StopWhenEndReachedOn();
    edgeSearchFilter->SetInputData(mesh);

    vtkNew<vtkStaticPointLocator> pointLocator;
    pointLocator->SetDataSet(mesh);
    pointLocator->BuildLocator();

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

void SpecimenDigitiser::ChangePointSize(int index) {
    // Clamp index to reasonable range
    index = std::clamp(index, -5, 5);
    // Calculate size with minimum bound
    double sizeConstant = m_sizeConstant;  // Start with default size

    vtkNew<vtkSphereSource> sphereSource;
    vtkNew<vtkSphereSource> sphereSource2;
    vtkNew<vtkSphereSource> sphereSource3;
    if (index < 0) {
        // Negative index: divide size exponentially (-1 → 1/2, -2 → 1/4)
        sizeConstant /= (1 << (-index));  // Bit shift for power-of-2 division
        sphereSource->SetRadius(sizeConstant);
        sphereSource2->SetRadius(sizeConstant * 1.5);
        sphereSource3->SetRadius(sizeConstant * 0.5);
    }
    if (index > 0) {
        // Positive indices: 1=1.5x, 2=2x, 3=3x, etc.
        sizeConstant *= (index == 1) ? 1.5 : index;
        sphereSource->SetRadius(sizeConstant);
        sphereSource2->SetRadius(sizeConstant * 1.5);
        sphereSource3->SetRadius(sizeConstant * 0.5);
    }
    if (index == 0) {
        sizeConstant = m_sizeConstant;
        sphereSource->SetRadius(sizeConstant);
        sphereSource2->SetRadius(sizeConstant * 1.5);
        sphereSource3->SetRadius(sizeConstant * 0.5);
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

void SpecimenDigitiser::ChangeLineSize(int index) {
    int magnifier = 2;
    // Clamp index to reasonable range
    index = std::clamp(index, -5, 5);
    if (index > 0) {
        if (m_surfaceTubeFilter->GetOutput()->GetNumberOfPoints() > 0) {
            m_surfaceTubeFilter->SetRadius(0.1 * (index * magnifier));
            m_surfaceTubeFilter->Update();
        }
        if (m_surfaceCurveTubeFilterDeactive->GetOutput()->GetNumberOfPoints() >
            0) {
            m_surfaceCurveTubeFilterDeactive->SetRadius(0.3 *
                                                        (index * magnifier));
            m_surfaceCurveTubeFilterDeactive->Update();
        }
        if (m_surfaceCurveTubeFilter->GetOutput()->GetNumberOfPoints() > 0) {
            m_surfaceCurveTubeFilter->SetRadius(0.3 * (index * magnifier));
            m_surfaceCurveTubeFilter->Update();
        }
        if (m_curveTubeFilterDeactive->GetOutput()->GetNumberOfPoints() > 0) {
            m_curveTubeFilterDeactive->SetRadius(0.3 * (index * magnifier));
            m_curveTubeFilterDeactive->Update();
        }
        if (m_curveTubeFilter->GetOutput()->GetNumberOfPoints() > 0) {
            m_curveTubeFilter->SetRadius(0.3 * (index * magnifier));
            m_curveTubeFilter->Update();
        }
    }

    if (index < 0) {
        index = std::abs(index);
        if (m_surfaceTubeFilter->GetOutput()->GetNumberOfPoints() > 0) {
            m_surfaceTubeFilter->SetRadius(0.1 / (index * magnifier));
            m_surfaceTubeFilter->Update();
        }
        if (m_surfaceCurveTubeFilterDeactive->GetOutput()->GetNumberOfPoints() >
            0) {
            m_surfaceCurveTubeFilterDeactive->SetRadius(0.3 /
                                                        (index * magnifier));
            m_surfaceCurveTubeFilterDeactive->Update();
        }
        if (m_surfaceCurveTubeFilter->GetOutput()->GetNumberOfPoints() > 0) {
            m_surfaceCurveTubeFilter->SetRadius(0.3 / (index * magnifier));
            m_surfaceCurveTubeFilter->Update();
        }
        if (m_curveTubeFilterDeactive->GetOutput()->GetNumberOfPoints() > 0) {
            m_curveTubeFilterDeactive->SetRadius(0.3 / (index * magnifier));
            m_curveTubeFilterDeactive->Update();
        }
        if (m_curveTubeFilter->GetOutput()->GetNumberOfPoints() > 0) {
            m_curveTubeFilter->SetRadius(0.3 / (index * magnifier));
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

void SpecimenDigitiser::MakeArrow(vtkPolyData* inputMesh,
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
        counter += 1;
    }
    output->Initialize();
    output->SetPoints(curveArrowPts);
    output->GetPointData()->SetVectors(u);
    output->Modified();
}

void SpecimenDigitiser::CosmeticCurve(vtkPoints* ctrlPts,
                                      vtkPolyData* outputCurve) {
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

void SpecimenDigitiser::PickFunc(vtkObject* caller, long unsigned int eventId,
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
                m_surfaceCurveCtrlPointActor->SetPickable(0);
                m_surfaceCurveCtrlPointActor->Modified();
                auto clickPos = m_iren->GetEventPosition();
                vtkNew<vtkCellPicker> picker;
                picker->Pick(clickPos[0], clickPos[1], 0, m_renderer);
                if (picker->GetPointId() != -1) {
                    m_fixedPtsIds->InsertNextValue(picker->GetPointId());
                    m_fixedLandmarks->InsertNextPoint(
                        m_meshData->GetPoint(picker->GetPointId()));
                    m_fixedLandmarks->Modified();
                    m_fixedPointsPoly->SetPoints(m_fixedLandmarks);
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
    } else if (curveSliderButton->isChecked()) {
        if (m_iren->GetControlKey()) {
            m_iren->SetInteractorStyle(m_2Dstyle);
            m_iren->Modified();
            m_meshActor->SetPickable(1);
            m_meshActor->Modified();
            m_fixedPointActor->SetPickable(0);
            m_fixedPointActor->Modified();
            m_curvectrlPointActor->SetPickable(0);
            m_curvectrlPointActor->Modified();
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
                    if (pickFromBoundariesBox->isChecked()) {
                        UpdateCurveData(m_curveHighlightCtrlPoints, m_curvePoly,
                                        m_curveLandmarks, m_meshBoundaries);
                    } else {
                        UpdateCurveData(m_curveHighlightCtrlPoints, m_curvePoly,
                                        m_curveLandmarks, m_meshData);
                    }
                    m_curvePoly->Modified();
                    m_curveTubeFilter->SetInputData(m_curvePoly);
                    m_curveTubeFilter->Update();
                    m_curveTubeFilter->Modified();
                    m_curvePointsPoly->SetPoints(m_curveLandmarks);
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
        if (m_iren->GetShiftKey()) {
            m_meshActor->SetPickable(0);
            m_meshActor->Modified();
            m_fixedPointActor->SetPickable(0);
            m_fixedPointActor->Modified();
            m_curvectrlPointActor->SetPickable(1);
            m_curvectrlPointActor->Modified();
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
                                        m_curveLandmarks, m_meshBoundaries);
                    } else {
                        UpdateCurveData(m_curveHighlightCtrlPoints, m_curvePoly,
                                        m_curveLandmarks, m_meshData);
                    }
                    m_curvePoly->Modified();
                    m_curveTubeFilter->SetInputData(m_curvePoly);
                    m_curveTubeFilter->Update();
                    m_curveTubeFilter->Modified();
                    m_curvePointsPoly->SetPoints(m_curveLandmarks);
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
        if (m_iren->GetControlKey() && m_editableSurface == 1) {
            m_iren->SetInteractorStyle(m_2Dstyle);
            m_iren->Modified();
            m_meshActor->SetPickable(1);
            m_meshActor->Modified();
            m_fixedPointActor->SetPickable(0);
            m_fixedPointActor->Modified();
            m_curvectrlPointActor->SetPickable(0);
            m_curvectrlPointActor->Modified();
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
        if (m_iren->GetShiftKey() && m_editableSurface == 1) {
            m_meshActor->SetPickable(0);
            m_meshActor->Modified();
            m_fixedPointActor->SetPickable(0);
            m_fixedPointActor->Modified();
            m_curvectrlPointActor->SetPickable(0);
            m_curvectrlPointActor->Modified();
            m_surfaceCurveCtrlPointActor->SetPickable(1);
            m_surfaceCurveCtrlPointActor->Modified();
            auto clickPos = m_iren->GetEventPosition();
            vtkNew<vtkPointPicker> picker;
            picker->Pick(clickPos[0], clickPos[1], 0, m_renderer);
            if (picker->GetPointId() > 0) {
                if (m_surfaceCurveHighlightCtrlPoints->GetNumberOfPoints() >
                    3) {
                    m_surfaceChanged = 1;
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
                    MeshCutter(m_surfaceCurveHighlightCtrlPoints);
                    if (!surfaceInterpolateButton->isEnabled()) {
                        surfaceInterpolateButton->setEnabled(1);
                        m_interpolationAnimation->start();
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

void SpecimenDigitiser::resetLeftClck(vtkObject* caller,
                                      long unsigned int eventId,
                                      void* callData) {
    m_PointPickerStyle->OnLeftButtonUp();
}

void SpecimenDigitiser::MoveFunc(vtkObject* caller, long unsigned int eventId,
                                 void* callData) {
    if (m_iren->GetControlKey()) {
        m_meshActor->SetPickable(0);
        m_meshActor->Modified();
        m_fixedPointActor->SetPickable(0);
        m_fixedPointActor->Modified();
        m_curvectrlPointActor->SetPickable(0);
        m_curvectrlPointActor->Modified();
        m_surfaceCurveCtrlPointActor->SetPickable(0);
        m_surfaceCurveCtrlPointActor->Modified();
        bool surfaceIsGoingToChange = 0;
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
                if (m_editableSurface == 1) {
                    surfaceIsGoingToChange = 1;
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
            if (m_surfaceCurveHighlightCtrlPoints->GetNumberOfPoints() > 2 &&
                surfaceIsGoingToChange) {
                m_surfaceChanged = 1;
            }
        }
    } else {
        m_PointPickerStyle->OnMiddleButtonDown();
    }
}

void SpecimenDigitiser::resetMouseMove(vtkObject* caller,
                                       long unsigned int eventId,
                                       void* callData) {
    m_PointMoverStyle->OnMiddleButtonUp();
    m_iren->SetInteractorStyle(m_PointPickerStyle);
}

void SpecimenDigitiser::CoordinateFunc(vtkObject* caller,
                                       long unsigned int eventId,
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
            m_fixedLandmarks->SetPoint(id, newLocation);
            m_fixedLandmarks->Modified();
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
                int planeRes =
                    std::sqrt(m_surfaceCtrlPointsPoly->GetNumberOfCells());
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
                                     m_surfacePatchLandmarks);

                vtkNew<vtkPolyData> tempSlidersPoly;
                tempSlidersPoly->SetPoints(m_surfacePatchLandmarks);
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
                std::cout << "Come here to Debug" << std::endl;
            }
        } else if (curveSliderButton->isChecked()) {
            m_curveHighlightCtrlPoints->SetPoint(id, newLocation);
            m_curveHighlightCtrlPoints->Modified();
            m_curvectrlPointsPoly->Modified();
            m_curvectrlVertexFilter->Update();
            m_curvectrlVertexFilter->Modified();
            if (pickFromBoundariesBox->isChecked()) {
                UpdateCurveData(m_curveHighlightCtrlPoints, m_curvePoly,
                                m_curveLandmarks, m_meshBoundaries);
            } else {
                UpdateCurveData(m_curveHighlightCtrlPoints, m_curvePoly,
                                m_curveLandmarks, m_meshData);
            }
            m_curvePoly->Modified();
            m_curveTubeFilter->SetInputData(m_curvePoly);
            m_curveTubeFilter->Update();
            m_curveTubeFilter->Modified();
            m_curvePointsPoly->SetPoints(m_curveLandmarks);
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

void SpecimenDigitiser::ProjectOnMesh(vtkPoints* point, vtkPolyData* mask) {
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

void SpecimenDigitiser::ProjectOnMesh(vtkPolyData* Poly, vtkPolyData* mask,
                                      std::vector<int>* ids) {
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
}

void SpecimenDigitiser::closeEvent(QCloseEvent* event) {
    event->ignore();
    if (m_dataDigitized == 1) {
        m_parent->SetStatus(STATUS::outdated);
        m_parent->UpdateDataBase();
        event->accept();
    } else {
        if (m_regPlot || m_slidingThread) {
            if (m_regPlot) {
                if (m_regPlot->IsRunning()) {
                    QMessageBox warning(this);
                    warning.setText(
                        "Cannot close this window! \n Registration is in the "
                        "process!");
                    warning.exec();
                    event->ignore();
                }
            }
            if (m_slidingThread) {
                if (m_slidingThread->isRunning()) {
                    QMessageBox warning(this);
                    warning.setText(
                        "Sliding is in the process, be patient please!");
                    warning.exec();
                    event->ignore();
                }
                if (!m_slidingThread->isRunning()) {
                    m_parent->SetStatus(STATUS::neutral);
                    m_parent->UpdateDataBase();
                    CleanUp();
                    event->accept();
                }
            }
        } else {
            std::string query(
                "Number of the digitized landmarks do not agree with the "
                "decided number \n or the digitized semi-landmarks haven't "
                "been slid yet!. Nothing will be registered!\n Do you want to "
                "closed the digitizer?");
            if (QMessageBox::Yes ==
                QMessageBox::question(this, "Close Confirmation",
                                      QString::fromStdString(query),
                                      QMessageBox::Yes | QMessageBox::No)) {
                m_parent->SetStatus(STATUS::neutral);
                m_parent->UpdateDataBase();
                CleanUp();
                event->accept();
            }
        }
    }
}

void SpecimenDigitiser::CleanUp() {
    if (m_curveType) {
        delete m_curveType;
        m_curveType = nullptr;
    }
    if (m_surfacePtsIds) {
        for (int i = 0; i < m_surfacePtsIds->size(); i++) {
            delete m_surfacePtsIds->at(i);
        }
        delete m_surfacePtsIds;
        m_surfacePtsIds = nullptr;
    }
    if (m_regPlot) {
        delete m_regPlot;
        m_regPlot = nullptr;
    }
}

void SpecimenDigitiser::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Control || event->key() == Qt::Key_Shift) {
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

void SpecimenDigitiser::OutlineIdFinder(int u, int v,
                                        std::vector<int>* output) {
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

void SpecimenDigitiser::keyReleaseEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Control || event->key() == Qt::Key_Shift) {
        if (surfaceSliderButton->isChecked()) {
            if (m_surfaceCurveHighlightCtrlPoints->GetNumberOfPoints() > 2 &&
                m_surfaceChanged == 1) {
                surfaceIronButton->setEnabled(0);
                surfaceIronButton->setChecked(0);
                m_ironAnimation->stop();
                vtkNew<vtkPolyData> tempCtrlPoly;
                tempCtrlPoly->DeepCopy(m_surfaceCurveCtrlPointsPoly);
                m_surfaceCurveCtrlBlock->SetBlock(m_currentSurfaceId,
                                                  tempCtrlPoly);
                m_surfaceCurveCtrlBlock->Modified();
                MeshCutter(m_surfaceCurveHighlightCtrlPoints);
                if (!surfaceInterpolateButton->isEnabled()) {
                    surfaceInterpolateButton->setEnabled(1);
                    m_interpolationAnimation->start();
                }
            } else if (m_surfaceCurveHighlightCtrlPoints->GetNumberOfPoints() >
                           2 &&
                       m_surfaceChanged == 0) {
                vtkNew<vtkExtractEdges> edgeExtracted;
                edgeExtracted->SetInputData(m_surfaceCtrlPointsPoly);
                edgeExtracted->Update();
                m_surfaceTubeFilter->SetInputData(edgeExtracted->GetOutput());
                m_surfaceTubeFilter->Update();
                m_surfaceTubeFilter->Modified();
                m_surfaceCurveTubeFilter->SetInputData(m_surfaceCurvePoly);
                m_surfaceCurveTubeFilter->Update();
                m_surfaceCurveTubeFilter->Modified();
                vtkNew<vtkPolyData> tempSlidersPoly;
                tempSlidersPoly->SetPoints(m_surfacePatchLandmarks);
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
    }
}

void SpecimenDigitiser::ShowLargestDiameter() {
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
            auto errorDialogue = QMessageBox(this);
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

void SpecimenDigitiser::DrawDiameter(vtkPoints* meshPoints) {
    Eigen::MatrixXd distMat;
    distMat.resize(0, 0);
    distMat.setZero(meshPoints->GetNumberOfPoints(),
                    meshPoints->GetNumberOfPoints());
    PDist(meshPoints, distMat);
    // DebugPrintMatrix(distMat);
    if (!distMat.isZero()) {
        Eigen::MatrixXd::Index maxRow, maxCol;
        float max = distMat.maxCoeff(&maxRow, &maxCol);
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

void SpecimenDigitiser::PickFromBoundaries() {
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
            auto errorDialogue = QMessageBox(this);
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

void SpecimenDigitiser::SetSurfaceSlider(vtkPoints* pts) {
    m_surfaceLandmarks->Initialize();
    m_surfaceLandmarks->DeepCopy(pts);
    vtkNew<vtkPolyData> tempPoly;
    tempPoly->SetPoints(m_surfaceLandmarks);
    tempPoly->Modified();
    m_surfaceVertexDeactiveFilter->SetInputData(tempPoly);
    m_surfaceVertexDeactiveFilter->Update();
    m_surfaceVertexDeactiveFilter->Modified();
    m_renderer->GetRenderWindow()->Render();
}

void SpecimenDigitiser::FlipSurfaceButton() {
    surfaceSliderButton->setChecked(0);
    SurfaceTool();
}

int SpecimenDigitiser::GetTemplateNOL() {
    return m_typeINOL + m_surfaceNOS +
           (m_surfacePatchNOP * m_surfacePatchUNOS * m_surfacePatchVNOS) +
           (m_curveNOS * m_curveNOC);
}

vtkPoints* SpecimenDigitiser::GetTemplateTypeI() {
    return m_parent->GetTemplateTypeI();
}

vtkPoints* SpecimenDigitiser::GetTypeI() { return m_fixedLandmarks; }

void SpecimenDigitiser::GetTemplateCurveSliders(vtkPoints* Output) {
    Output->Initialize();
    auto temp = m_parent->GetTemplateCurveSliders();
    vtkNew<vtkDataObjectTreeIterator> iterPts;
    iterPts->SetDataSet(temp);
    iterPts->SkipEmptyNodesOn();
    iterPts->VisitOnlyLeavesOn();
    for (iterPts->InitTraversal(); !iterPts->IsDoneWithTraversal();
         iterPts->GoToNextItem()) {
        vtkDataObject* dso = iterPts->GetCurrentDataObject();
        vtkPolyData* pd = dynamic_cast<vtkPolyData*>(dso);
        for (int i = 0; i < pd->GetNumberOfPoints(); i++) {
            Output->InsertNextPoint(pd->GetPoint(i));
        }
    }
}

void SpecimenDigitiser::GetCurveSliders(vtkPoints* Output) {
    Output->Initialize();
    auto temp = m_curveBlock;
    vtkNew<vtkDataObjectTreeIterator> iterPts;
    iterPts->SetDataSet(temp);
    iterPts->SkipEmptyNodesOn();
    iterPts->VisitOnlyLeavesOn();
    for (iterPts->InitTraversal(); !iterPts->IsDoneWithTraversal();
         iterPts->GoToNextItem()) {
        vtkDataObject* dso = iterPts->GetCurrentDataObject();
        vtkPolyData* pd = dynamic_cast<vtkPolyData*>(dso);
        for (int i = 0; i < pd->GetNumberOfPoints(); i++) {
            Output->InsertNextPoint(pd->GetPoint(i));
        }
    }
}

bool SpecimenDigitiser::GetIgnorSetting() { return m_ignoreInside; }

void SpecimenDigitiser::InterpolateSurface() {
    ConstructSurfaceData(m_surfaceCurveHighlightCtrlPoints,
                         m_surfacePatchLandmarks, m_surfaceCtrlPointsPoly);
    if (m_surfaceCtrlPointsPoly->GetNumberOfPoints() > 0) {
        surfaceIronButton->setEnabled(1);
        surfaceIronButton->setChecked(0);
        m_ironAnimation->start();
        m_surfacePatchLandmarks->Modified();
        m_surfaceCtrlPointsPoly->Modified();
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
        tempSlidersPoly->SetPoints(m_surfacePatchLandmarks);
        m_surfacePatchVertexFilter->SetInputData(tempSlidersPoly);
        m_surfacePatchVertexFilter->Update();
        m_surfacePatchVertexFilter->Modified();

        vtkNew<vtkPolyData> tempSlidersPolyCopy;
        tempSlidersPolyCopy->DeepCopy(tempSlidersPoly);
        m_surfaceBlock->SetBlock(m_currentSurfaceId, tempSlidersPolyCopy);
        m_surfaceBlock->Modified();

        m_surfaceChanged = 0;
        int numOfPatches =
            m_surfacePatchNOP - (m_surfaceBlock->GetNumberOfBlocks());
        surfaceLineEditNOP->setText(
            QString::fromStdString(std::to_string(numOfPatches)));
        if (m_surfaceBlock->GetNumberOfBlocks() < m_surfacePatchNOP) {
            surfaceAddButton->setEnabled(1);
        }
        curvePickSourceComboBox->addItem(QString::number(m_currentSurfaceId));
    }

    surfaceInterpolateButton->setEnabled(0);
    m_interpolationAnimation->stop();
    m_renderer->GetRenderWindow()->Render();
}

SpecimenDigitiser::~SpecimenDigitiser() {
    delete m_vtkRenderWidget;
    delete m_exclusionPainter;
    CleanUp();
}
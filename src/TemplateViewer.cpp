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

#include "../include/TemplateViewer.h"

#include "../include/MainWindow.h"

TemplateViewer::TemplateViewer(MainWindow* parent) : m_parent(parent) {
    this->setWindowTitle("Template Viewer");
    this->resize(600, 500);
    m_meshData = vtkSmartPointer<vtkPolyData>::New();
    m_typeI = vtkSmartPointer<vtkPoints>::New();
    m_surfaceSliders = vtkSmartPointer<vtkPoints>::New();
    m_surfacePatchSliders = vtkSmartPointer<vtkMultiBlockDataSet>::New();
    m_curveSliders = vtkSmartPointer<vtkMultiBlockDataSet>::New();
    m_curvePtsPoly = vtkSmartPointer<vtkMultiBlockDataSet>::New();
    m_surfacePatchCurve = vtkSmartPointer<vtkMultiBlockDataSet>::New();
    m_vtkRenderWidget = new QVTKOpenGLWidget();
    m_renderer = vtkSmartPointer<vtkRenderer>::New();
    m_renWin = vtkSmartPointer<vtkRenderWindow>::New();
    m_iren = vtkSmartPointer<vtkGenericRenderWindowInteractor>::New();
    m_style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    m_meshActor = vtkSmartPointer<vtkActor>::New();
    m_fixedPointActor = vtkSmartPointer<vtkActor>::New();
    m_surfacePointActor = vtkSmartPointer<vtkActor>::New();
    m_curvePointActor = vtkSmartPointer<vtkActor>::New();
    m_surfacePatchPointActor = vtkSmartPointer<vtkActor>::New();
    m_labelActor = vtkSmartPointer<vtkActor2D>::New();
    m_curveLabelActor = vtkSmartPointer<vtkActor2D>::New();
    m_surfaceLabelActor = vtkSmartPointer<vtkActor2D>::New();
    m_curveArrowActor = vtkSmartPointer<vtkActor>::New();
    m_surfaceArrowActor = vtkSmartPointer<vtkActor>::New();

    m_totalLandmarks = vtkSmartPointer<vtkPolyData>::New();
    m_totalLabelActor = vtkSmartPointer<vtkActor2D>::New();
    // Setting up render scene
    vtkNew<vtkNamedColors> colors;
    m_renWin = m_vtkRenderWidget->GetRenderWindow();
    m_renWin->AddRenderer(m_renderer);
    m_style->SetCurrentRenderer(m_renderer);
    m_iren->SetInteractorStyle(m_style);
    m_iren->SetRenderWindow(m_renWin);

    // Ambient properties
    m_renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
    m_renWin->Render();
    m_iren->Start();

    this->setCentralWidget(m_vtkRenderWidget);
    QToolBar* mainToolbar = this->addToolBar("Main Toolbar");
    mainToolbar->setMovable(false);
    this->addToolBarBreak();

    QLabel* showPtsIdsLabel = new QLabel();
    showPtsIdsLabel->setText(tr("Show Landmark ids"));
    mainToolbar->addWidget(showPtsIdsLabel);

    showPtsIdsBox = new QCheckBox();
    showPtsIdsBox->setChecked(false);

    mainToolbar->addWidget(showPtsIdsBox);
    screenCapAction = mainToolbar->addAction(
        QIcon(":/icons/graphics/icons/save.svg"), "Capture the Screen");

    connect(showPtsIdsBox, &QCheckBox::stateChanged, this,
            &TemplateViewer::ShowIds);
    connect(screenCapAction, &QAction::triggered, this,
            &TemplateViewer::CaptureScreen);
}

void TemplateViewer::ShowIds() {
    if (showPtsIdsBox->isChecked()) {
        m_renderer->RemoveActor2D(m_labelActor);
        m_renderer->RemoveActor2D(m_curveLabelActor);
        m_renderer->RemoveActor2D(m_surfaceLabelActor);
        m_renderer->AddActor(m_totalLabelActor);
        m_renderer->GetRenderWindow()->Render();
    } else {
        m_renderer->RemoveActor2D(m_totalLabelActor);
        m_renderer->AddActor(m_labelActor);
        m_renderer->AddActor(m_curveLabelActor);
        m_renderer->AddActor(m_surfaceLabelActor);
        m_renderer->GetRenderWindow()->Render();
    }
}

void TemplateViewer::CaptureScreen() {
    auto filter = "png(*.png)";
    QString filename =
        QFileDialog::getSaveFileName(this, "Save image", "", filter, nullptr,
                                     QFileDialog::DontUseNativeDialog);
    QFileInfo fi(filename);
    QString ext = fi.completeSuffix();
    if (filename.isEmpty()) {
        return;
    }
    if (ext != "png") {
        filename += ".png";
    }
    if (!filename.isEmpty()) {
        vtkNew<vtkNamedColors> colors;
        auto oldSB = m_renWin->GetSwapBuffers();
        m_renWin->SwapBuffersOff();
        // Hide the background (set visibility to false or whatever)
        m_renderer->SetBackground(colors->GetColor3d("White").GetData());
        m_renderer->Modified();
        m_renderer->GetRenderWindow()->Render();
        auto windowToImageFilter =
            vtkSmartPointer<vtkWindowToImageFilter>::New();
        windowToImageFilter->SetInput(m_renWin);

        windowToImageFilter->SetScale(3);
        windowToImageFilter->SetInputBufferTypeToRGBA();

        windowToImageFilter->ReadFrontBufferOff();
        windowToImageFilter->Update();  // Issues a render on input

        m_renWin->SetSwapBuffers(oldSB);
        m_renWin->SwapBuffersOn();

        // Show background again (set visibility to true or whatever)
        m_renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
        m_renderer->Modified();
        m_renderer->GetRenderWindow()->Render();

        auto img = windowToImageFilter->GetOutput();
        vtkNew<vtkPNGWriter> writer;
        writer->SetFileName(filename.toStdString().c_str());
        writer->SetInputConnection(windowToImageFilter->GetOutputPort());
        writer->Write();
    }
}

void TemplateViewer::SetPloyData(vtkPolyData* data) {
    m_meshData = data;
    m_typeI = m_parent->GetTemplateTypeI();
    m_surfaceSliders = m_parent->GetTemplateSurfaceSliders();
    m_surfacePatchSliders = m_parent->GetTemplateSurfacePatchSliders();
    m_surfacePatchCurve = m_parent->GetTemplateSurfacePatchCurve();
    m_curveSliders = m_parent->GetTemplateCurveSliders();
    m_curvePtsPoly = m_parent->GetTemplateCurvePoly();
    m_totalLandmarks = m_parent->GetTemplateTotalLandmarks();

    Plot();
}

void TemplateViewer::Plot() {
    vtkNew<vtkNamedColors> colors;
    // Mesh properties and color etc
    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetInputData(m_meshData);
    mapper->ScalarVisibilityOff();  // <- disables scalar-based coloring
    m_meshActor->SetMapper(mapper);
    m_meshActor->GetProperty()->SetColor(1, 0.992, 0.815);
    m_renderer->AddActor(m_meshActor);

    vtkNew<vtkMassProperties> prop;
    prop->SetInputData(m_meshData);
    prop->Update();
    const double area = prop->GetSurfaceArea();
    const double diagonal =
        std::sqrt(area);  // Approximate characteristic length
    // Compute size factor based on application-specific parameters
    // Normalized between 0-1 range first, then scaled
    int fixNOL = m_typeI->GetNumberOfPoints();
    int surfaceNOS = m_surfaceSliders->GetNumberOfPoints() +
                     m_surfacePatchSliders->GetNumberOfPoints();
    int curveNOS = m_curveSliders->GetNumberOfPoints();
    double sizeFactor = (fixNOL * 0.03 + surfaceNOS * 0.025 + curveNOS * 0.025);
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

    vtkNew<vtkSphereSource> sphereSource2;
    sphereSource2->SetRadius(landmarkSize * 1.5);

    //--------------------------------------------
    vtkNew<vtkGlyph3DMapper> fixedPointMapper;
    vtkNew<vtkVertexGlyphFilter> fixedVertexFilter;
    vtkNew<vtkPolyData> typeIPoly;
    typeIPoly->SetPoints(m_typeI);
    fixedVertexFilter->SetInputData(typeIPoly);
    fixedVertexFilter->Update();
    fixedPointMapper->SetInputData(fixedVertexFilter->GetOutput());
    fixedPointMapper->SetSourceConnection(sphereSource2->GetOutputPort());
    fixedPointMapper->ScalingOff();
    fixedPointMapper->ScalarVisibilityOff();
    m_fixedPointActor->GetProperty()->SetColor(1.0, 0.0, 0.0);
    m_fixedPointActor->GetProperty()->SetDiffuse(0.8);
    m_fixedPointActor->GetProperty()->SetSpecular(0.5);
    m_fixedPointActor->GetProperty()->SetSpecularPower(30);
    m_fixedPointActor->SetMapper(fixedPointMapper);
    m_renderer->AddActor(m_fixedPointActor);
    //--------------------------------------------
    vtkNew<vtkGlyph3DMapper> surfacePointMapper;
    vtkNew<vtkVertexGlyphFilter> surfaceVertexFilter;
    vtkNew<vtkPolyData> surfacePtsPoly;
    surfacePtsPoly->SetPoints(m_surfaceSliders);
    surfaceVertexFilter->SetInputData(surfacePtsPoly);
    surfaceVertexFilter->Update();
    surfacePointMapper->SetInputData(surfaceVertexFilter->GetOutput());
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
    //--------------------------------------------
    vtkNew<vtkGlyph3DMapper> curvePointMapper;
    vtkNew<vtkVertexGlyphFilter> curveVertexFilter;
    vtkNew<vtkPoints> curvePts;
    vtkNew<vtkPoints> curveLabels;
    vtkNew<vtkPolyData> curvePtsPoly;

    vtkNew<vtkDataObjectTreeIterator> iterCurvePts;
    iterCurvePts->SetDataSet(m_curveSliders);
    iterCurvePts->SkipEmptyNodesOn();
    iterCurvePts->VisitOnlyLeavesOn();
    for (iterCurvePts->InitTraversal(); !iterCurvePts->IsDoneWithTraversal();
         iterCurvePts->GoToNextItem()) {
        vtkDataObject* dso = iterCurvePts->GetCurrentDataObject();
        vtkPolyData* pd = dynamic_cast<vtkPolyData*>(dso);
        for (int i = 0; i < pd->GetNumberOfPoints(); i++) {
            curvePts->InsertNextPoint(pd->GetPoint(i));
        }
        curveLabels->InsertNextPoint(pd->GetPoint(0));
    }
    curveLabels->Modified();
    curvePts->Modified();
    curvePtsPoly->SetPoints(curvePts);
    curveVertexFilter->SetInputData(curvePtsPoly);
    curveVertexFilter->Update();

    curvePointMapper->SetInputData(curveVertexFilter->GetOutput());
    curvePointMapper->SetSourceConnection(sphereSource->GetOutputPort());
    curvePointMapper->ScalingOff();
    curvePointMapper->ScalarVisibilityOff();
    m_curvePointActor->GetProperty()->SetColor(0.0, 1.0, 0.0);
    m_curvePointActor->GetProperty()->SetDiffuse(0.8);
    m_curvePointActor->GetProperty()->SetSpecular(0.5);
    m_curvePointActor->GetProperty()->SetSpecularPower(30);
    m_curvePointActor->SetMapper(curvePointMapper);
    m_curvePointActor->SetPickable(0);
    m_renderer->AddActor(m_curvePointActor);

    vtkNew<vtkArrowSource> curveArrow;
    curveArrow->SetTipResolution(16);
    curveArrow->Update();

    vtkNew<vtkGlyph3D> glyphCurveArrow;
    vtkNew<vtkPolyData> curveArrowPoly;
    MakeArrow(m_meshData, m_curvePtsPoly, 3, curveArrowPoly);
    glyphCurveArrow->SetInputData(curveArrowPoly);
    glyphCurveArrow->SetSourceData(curveArrow->GetOutput());
    glyphCurveArrow->SetVectorModeToUseVector();
    glyphCurveArrow->SetScaleModeToScaleByVector();
    glyphCurveArrow->SetScaleFactor(10);
    glyphCurveArrow->Update();

    vtkNew<vtkPolyDataMapper> glyph3DCurveMapper;
    glyph3DCurveMapper->SetInputData(glyphCurveArrow->GetOutput());
    m_curveArrowActor->SetMapper(glyph3DCurveMapper);
    m_curveArrowActor->GetProperty()->SetColor(0.0, 1.0, 0.0);
    m_curveArrowActor->SetPickable(0);
    m_renderer->AddActor(m_curveArrowActor);

    //--------------------------------------------
    vtkNew<vtkGlyph3DMapper> surfacePatchPointMapper;
    vtkNew<vtkVertexGlyphFilter> surfacePatchVertexFilter;
    vtkNew<vtkPoints> surfacePatchPts;
    vtkNew<vtkPoints> surfacePatchLabels;
    vtkNew<vtkPolyData> surfacePatchPtsPoly;

    vtkNew<vtkDataObjectTreeIterator> iterSurfacePts;
    iterSurfacePts->SetDataSet(m_surfacePatchSliders);
    iterSurfacePts->SkipEmptyNodesOn();
    iterSurfacePts->VisitOnlyLeavesOn();
    for (iterSurfacePts->InitTraversal();
         !iterSurfacePts->IsDoneWithTraversal();
         iterSurfacePts->GoToNextItem()) {
        vtkDataObject* dso = iterSurfacePts->GetCurrentDataObject();
        vtkPolyData* pd = dynamic_cast<vtkPolyData*>(dso);
        for (int i = 0; i < pd->GetNumberOfPoints(); i++) {
            surfacePatchPts->InsertNextPoint(pd->GetPoint(i));
        }
        surfacePatchLabels->InsertNextPoint(pd->GetPoint(0));
    }
    surfacePatchLabels->Modified();
    surfacePatchPts->Modified();
    surfacePatchPtsPoly->SetPoints(surfacePatchPts);
    surfacePatchVertexFilter->SetInputData(surfacePatchPtsPoly);
    surfacePatchVertexFilter->Update();

    surfacePatchPointMapper->SetInputData(
        surfacePatchVertexFilter->GetOutput());
    surfacePatchPointMapper->SetSourceConnection(sphereSource->GetOutputPort());
    surfacePatchPointMapper->ScalingOff();
    surfacePatchPointMapper->ScalarVisibilityOff();
    m_surfacePatchPointActor->GetProperty()->SetColor(0.0, 0.0, 1.0);
    m_surfacePatchPointActor->GetProperty()->SetDiffuse(0.8);
    m_surfacePatchPointActor->GetProperty()->SetSpecular(0.5);
    m_surfacePatchPointActor->GetProperty()->SetSpecularPower(30);
    m_surfacePatchPointActor->SetMapper(surfacePatchPointMapper);
    m_surfacePatchPointActor->SetPickable(0);
    m_renderer->AddActor(m_surfacePatchPointActor);

    vtkNew<vtkGlyph3D> glyphSurfaceArrow;
    vtkNew<vtkPolyData> surfaceArrowPoly;
    MakeArrow(m_meshData, m_surfacePatchCurve, 4, surfaceArrowPoly);
    glyphSurfaceArrow->SetInputData(surfaceArrowPoly);
    glyphSurfaceArrow->SetSourceData(curveArrow->GetOutput());
    glyphSurfaceArrow->SetVectorModeToUseVector();
    glyphSurfaceArrow->SetScaleModeToScaleByVector();
    glyphSurfaceArrow->SetScaleFactor(10);
    glyphSurfaceArrow->Update();
    vtkNew<vtkPolyDataMapper> glyph3DSurfaceMapper;
    glyph3DSurfaceMapper->SetInputData(glyphSurfaceArrow->GetOutput());
    m_surfaceArrowActor->SetMapper(glyph3DSurfaceMapper);
    m_surfaceArrowActor->GetProperty()->SetColor(0.0, 0.0, 1.0);
    m_surfaceArrowActor->SetPickable(0);
    m_renderer->AddActor(m_surfaceArrowActor);
    //-----------------------------------------------

    // Label properties
    vtkNew<vtkLabeledDataMapper> labelMapper;
    labelMapper->SetInputData(fixedVertexFilter->GetOutput());
    labelMapper->GetLabelTextProperty()->SetFontSize(15);
    m_labelActor->SetMapper(labelMapper);
    m_labelActor->GetProperty()->SetColor(
        colors->GetColor3d("Orange").GetData());
    m_labelActor->SetPickable(0);
    m_renderer->AddActor(m_labelActor);
    //-------
    vtkNew<vtkLabeledDataMapper> curveLabelMapper;
    vtkNew<vtkVertexGlyphFilter> curveLabelVertexFilter;
    vtkNew<vtkPolyData> curveLabelsPoly;
    curveLabelsPoly->SetPoints(curveLabels);
    curveLabelVertexFilter->SetInputData(curveLabelsPoly);
    curveLabelVertexFilter->Update();
    curveLabelMapper->SetInputData(curveLabelVertexFilter->GetOutput());
    curveLabelMapper->GetLabelTextProperty()->SetFontSize(15);
    m_curveLabelActor->SetMapper(curveLabelMapper);
    m_curveLabelActor->GetProperty()->SetColor(
        colors->GetColor3d("palegreen").GetData());
    m_curveLabelActor->SetPickable(0);
    m_renderer->AddActor(m_curveLabelActor);
    //-------
    vtkNew<vtkLabeledDataMapper> surfaceLabelMapper;
    vtkNew<vtkVertexGlyphFilter> surfaceLabelVertexFilter;
    vtkNew<vtkPolyData> surfacePatchLabelsPoly;
    surfacePatchLabelsPoly->SetPoints(surfacePatchLabels);
    surfaceLabelVertexFilter->SetInputData(surfacePatchLabelsPoly);
    surfaceLabelVertexFilter->Update();
    surfaceLabelMapper->SetInputData(surfaceLabelVertexFilter->GetOutput());
    surfaceLabelMapper->GetLabelTextProperty()->SetFontSize(15);
    m_surfaceLabelActor->SetMapper(surfaceLabelMapper);
    m_surfaceLabelActor->GetProperty()->SetColor(
        colors->GetColor3d("cadetblue").GetData());
    m_surfaceLabelActor->SetPickable(0);
    m_renderer->AddActor(m_surfaceLabelActor);
    //--------
    vtkNew<vtkLabeledDataMapper> totalLabelMapper;
    vtkNew<vtkVertexGlyphFilter> totalLabelVertexFilter;
    totalLabelVertexFilter->SetInputData(m_totalLandmarks);
    totalLabelVertexFilter->Update();
    totalLabelMapper->SetInputData(totalLabelVertexFilter->GetOutput());
    totalLabelMapper->GetLabelTextProperty()->SetFontSize(15);
    m_totalLabelActor->SetMapper(totalLabelMapper);
    m_totalLabelActor->GetProperty()->SetColor(
        colors->GetColor3d("black").GetData());
    m_totalLabelActor->SetPickable(0);
    // m_renderer->AddActor(m_totalLabelActor);

    //--------

    m_renderer->ResetCamera();
    m_renderer->GetRenderWindow()->Render();
}

void TemplateViewer::Initialize() {
    m_meshData->Initialize();
    m_typeI->Initialize();
    m_surfaceSliders->Initialize();
    m_surfacePatchSliders->Initialize();
    m_curveSliders->Initialize();
    m_renderer->RemoveActor(m_meshActor);
    m_renderer->RemoveActor(m_fixedPointActor);
    m_renderer->RemoveActor(m_surfacePointActor);
    m_renderer->RemoveActor(m_curvePointActor);
    m_renderer->RemoveActor(m_surfacePatchPointActor);
    m_renderer->RemoveActor2D(m_labelActor);
    m_renderer->RemoveActor2D(m_curveLabelActor);
    m_renderer->RemoveActor2D(m_surfaceLabelActor);
    m_renderer->RemoveActor2D(m_totalLabelActor);
    m_renderer->RemoveActor(m_curveArrowActor);
}

void TemplateViewer::MakeArrow(vtkPolyData* inputMesh,
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

TemplateViewer::~TemplateViewer() { delete m_vtkRenderWidget; }
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

#include "../include/RegistrationThread.h"

RegistrationThread::RegistrationThread(vtkPolyData* templateMesh,
                                       vtkPolyData* sourceMesh,
                                       vtkPolyData* outMesh, bool ignoreInside,
                                       int res, QMutex* mutex, bool preAlign)
    : m_outMesh(outMesh),
      m_ignoreInside(ignoreInside),
      m_res(res),
      m_mutex(mutex),
      m_preAlign(preAlign) {
    m_outMesh->Initialize();
    m_templateMesh = vtkSmartPointer<vtkPolyData>::New();
    m_sourceMesh = vtkSmartPointer<vtkPolyData>::New();

    if (m_ignoreInside) {
        vtkNew<vtkPolyDataConnectivityFilter>
            templateSelector;  // just in case, if the mesh has inner parts
        templateSelector->SetInputData(templateMesh);
        templateSelector->SetExtractionModeToLargestRegion();
        templateSelector->Update();
        m_templateMesh->DeepCopy(templateSelector->GetOutput());

        vtkNew<vtkPolyDataConnectivityFilter>
            targetSelector;  // just in case, if the mesh has inner parts
        targetSelector->SetInputData(sourceMesh);
        targetSelector->SetExtractionModeToLargestRegion();
        targetSelector->Update();
        m_sourceMesh->DeepCopy(targetSelector->GetOutput());

    } else if (!m_ignoreInside) {
        m_templateMesh->DeepCopy(templateMesh);
        m_sourceMesh->DeepCopy(sourceMesh);
    }
}

void RegistrationThread::run() {
    // In vtk, target means template
    vtkNew<vtkPolyData> alignedMesh;
    if (!m_preAlign) {
        PrealignMesh(m_sourceMesh, m_templateMesh, alignedMesh);
    }
    
    int resampleResolution = m_res;

    if(m_templateMesh->GetNumberOfPoints() <  alignedMesh->GetNumberOfPoints() || 
        m_templateMesh->GetNumberOfPoints() <  m_sourceMesh->GetNumberOfPoints()){
        if(m_templateMesh->GetNumberOfPoints() < m_res){
            resampleResolution = m_templateMesh->GetNumberOfPoints() / 1.0;
        }
    }
    else{
        if (!m_preAlign){
            if(alignedMesh->GetNumberOfPoints() < m_res){
                resampleResolution = alignedMesh->GetNumberOfPoints() / 1.0;
            }
        }
        else{
            if(m_sourceMesh->GetNumberOfPoints() < m_res){
                resampleResolution = m_sourceMesh->GetNumberOfPoints() / 1.0;
            }
        }
    }
    
    vtkNew<vtkPoints> templateResampled;
    Resample(m_templateMesh, resampleResolution, templateResampled);
    
    vtkNew<vtkPoints> sourceResampled;
    if (!m_preAlign) {
        Resample(alignedMesh, resampleResolution, sourceResampled);
    } else {
        Resample(m_sourceMesh, resampleResolution, sourceResampled);
    }
    Eigen::MatrixXd templateMatrix =
        Eigen::MatrixXd::Zero(templateResampled->GetNumberOfPoints(), 3);
    for (int i = 0; i < templateResampled->GetNumberOfPoints(); i++) {
        for (int j = 0; j < 3; j++) {
            templateMatrix.operator()(i, j) = templateResampled->GetPoint(i)[j];
        }
    }
    Eigen::MatrixXd sourceMatrix =
        Eigen::MatrixXd::Zero(sourceResampled->GetNumberOfPoints(), 3);
    for (int i = 0; i < sourceResampled->GetNumberOfPoints(); i++) {
        for (int j = 0; j < 3; j++) {
            sourceMatrix.operator()(i, j) = sourceResampled->GetPoint(i)[j];
        }
    }
    
    cpd::NonrigidResult results = cpd::nonrigid(templateMatrix, sourceMatrix);
    auto deformedSourceMatrix = results.points;
    
    vtkNew<vtkPoints> sourceResampledDeformed;
    for (int i = 0; i < deformedSourceMatrix.rows(); i++) {
        sourceResampledDeformed->InsertNextPoint(
            deformedSourceMatrix.coeff(i, 0), deformedSourceMatrix.coeff(i, 1),
            deformedSourceMatrix.coeff(i, 2));
    }
    vtkNew<vtkThinPlateSplineTransform> tpsTrans;
    tpsTrans->SetTargetLandmarks(sourceResampledDeformed);
    tpsTrans->SetSourceLandmarks(sourceResampled);
    tpsTrans->SetBasisToR();
    tpsTrans->Update();
    vtkNew<vtkTransformPolyDataFilter> transform2;
    if (!m_preAlign) {
        transform2->SetInputData(alignedMesh);
    } else {
        transform2->SetInputData(m_sourceMesh);
    }
    transform2->SetTransform(tpsTrans);
    transform2->Update();
    m_mutex->lock();
    m_outMesh->DeepCopy(transform2->GetOutput());
    m_mutex->unlock();
    emit MeshMorphed();
}

void RegistrationThread::DebugPrintMatrix(Eigen::MatrixXd matrix) {
    Eigen::IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");
    std::string sep = "\n----------------------------------------\n";
    std::cout << matrix.format(CleanFmt) << sep;
}

void RegistrationThread::PrealignMesh(
    vtkPolyData* sourceMesh, vtkPolyData* templateMesh, vtkPolyData* alignedOutput) {
    // Step 1: Compute centroids
    double sourceCentroid[3];
    double templateCentroid[3];
    ComputeCentroid(sourceMesh, sourceCentroid);
    ComputeCentroid(templateMesh, templateCentroid);

    // Step 2: Compute eigenvectors of covariance matrices
    Eigen::Matrix3d covSource = ComputeCovarianceMatrix(sourceMesh, sourceCentroid);
    Eigen::Matrix3d covTemplate = ComputeCovarianceMatrix(templateMesh, templateCentroid);

    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> eigSource(covSource);
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> eigTemplate(covTemplate);

    Eigen::Matrix3d axesSource = eigSource.eigenvectors();
    Eigen::Matrix3d axesTemplate = eigTemplate.eigenvectors();

    // Fix PCA sign ambiguity: align source axes with template axes
    for (int i = 0; i < 3; ++i) {
        if (axesTemplate.col(i).dot(axesSource.col(i)) < 0) {
            axesSource.col(i) *= -1;
        }
    }

    // Step 3: Compute rotation matrix
    Eigen::Matrix3d rotation = axesTemplate * axesSource.transpose();

    // Fix reflection if needed
    if (rotation.determinant() < 0) {
        axesTemplate.col(2) *= -1;
        rotation = axesTemplate * axesSource.transpose();
    }

    // Step 4: Build transform: translate to origin → rotate → move to template
    vtkNew<vtkTransform> transform;
    transform->PostMultiply();

    // Move to origin
    transform->Translate(-sourceCentroid[0], -sourceCentroid[1], -sourceCentroid[2]);

    // Apply rotation using vtkMatrix4x4
    vtkNew<vtkMatrix4x4> vtkRotMatrix;
    vtkRotMatrix->Identity();
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            vtkRotMatrix->SetElement(i, j, rotation(i, j));
    transform->Concatenate(vtkRotMatrix);

    // Move to template centroid
    transform->Translate(templateCentroid);

    // Step 5: Apply transform to source mesh
    vtkNew<vtkTransformPolyDataFilter> filter;
    filter->SetInputData(sourceMesh);
    filter->SetTransform(transform);
    filter->Update();

    // Step 6: Output the aligned mesh
    alignedOutput->ShallowCopy(filter->GetOutput());
}

Eigen::Matrix3d RegistrationThread::ComputeCovarianceMatrix(vtkPolyData* mesh, const double centroid[3]) {
    vtkPoints* points = mesh->GetPoints();
    vtkIdType numPoints = points->GetNumberOfPoints();
    Eigen::MatrixXd centered(3, numPoints);

    for (vtkIdType i = 0; i < numPoints; ++i) {
        double p[3];
        points->GetPoint(i, p);
        centered(0, i) = p[0] - centroid[0];
        centered(1, i) = p[1] - centroid[1];
        centered(2, i) = p[2] - centroid[2];
    }

    // Covariance matrix
    return (centered * centered.transpose()) / static_cast<double>(numPoints);
}

void RegistrationThread::ComputeCentroid(vtkPolyData* mesh, double centroid[3]) {
    vtkPoints* points = mesh->GetPoints();
    vtkIdType numPoints = points->GetNumberOfPoints();

    centroid[0] = centroid[1] = centroid[2] = 0.0;
    for (vtkIdType i = 0; i < numPoints; ++i) {
        double p[3];
        points->GetPoint(i, p);
        centroid[0] += p[0];
        centroid[1] += p[1];
        centroid[2] += p[2];
    }

    centroid[0] /= numPoints;
    centroid[1] /= numPoints;
    centroid[2] /= numPoints;
}

void RegistrationThread::AlignBBoxToWorld(vtkPolyData* tempelateMesh,
                                          vtkPolyData* sourceMesh,
                                          vtkPolyData* outTemplate,
                                          vtkPolyData* outSource,
                                          vtkLandmarkTransform* invTrans) {
    vtkNew<vtkOBBTree> sourceOBBTree;
    sourceOBBTree->SetDataSet(sourceMesh);
    sourceOBBTree->SetMaxLevel(1);
    sourceOBBTree->BuildLocator();
    vtkNew<vtkPolyData> sourceLandmarks;
    sourceOBBTree->GenerateRepresentation(0, sourceLandmarks);

    vtkNew<vtkOBBTree> templateOBBTree;
    templateOBBTree->SetDataSet(tempelateMesh);
    templateOBBTree->SetMaxLevel(1);
    templateOBBTree->BuildLocator();
    vtkNew<vtkPolyData> templateLandmarks;
    templateOBBTree->GenerateRepresentation(0, templateLandmarks);

    // first align template bbox to world coordinates
    vtkNew<vtkCubeSource> cubeSource;
    cubeSource->SetCenter(0, 0, 0);
    cubeSource->Update();
    vtkNew<vtkOBBTree> boxOBBTree;
    boxOBBTree->SetDataSet(cubeSource->GetOutput());
    boxOBBTree->SetMaxLevel(1);
    boxOBBTree->BuildLocator();
    vtkNew<vtkPolyData> boxLandmarks;
    boxOBBTree->GenerateRepresentation(0, boxLandmarks);
    vtkNew<vtkLandmarkTransform> boxLmTransformer;
    boxLmTransformer->SetSourceLandmarks(templateLandmarks->GetPoints());
    boxLmTransformer->SetTargetLandmarks(boxLandmarks->GetPoints());
    boxLmTransformer->SetModeToSimilarity();
    boxLmTransformer->Update();

    invTrans->DeepCopy(boxLmTransformer);

    vtkNew<vtkTransformPolyDataFilter> templatePdt;
    templatePdt->SetInputData(tempelateMesh);
    templatePdt->SetTransform(boxLmTransformer);
    templatePdt->Update();
    outTemplate->Initialize();
    outTemplate->DeepCopy(templatePdt->GetOutput());

    vtkNew<vtkOBBTree> newTemplateOBBTree;
    newTemplateOBBTree->SetDataSet(outTemplate);
    newTemplateOBBTree->SetMaxLevel(1);
    newTemplateOBBTree->BuildLocator();
    vtkNew<vtkPolyData> newTemplateLandmarks;
    newTemplateOBBTree->GenerateRepresentation(0, newTemplateLandmarks);

    // then align target to this new template
    vtkNew<vtkLandmarkTransform> lmTransformer;
    lmTransformer->SetSourceLandmarks(sourceLandmarks->GetPoints());
    lmTransformer->SetTargetLandmarks(newTemplateLandmarks->GetPoints());
    lmTransformer->SetModeToSimilarity();
    lmTransformer->Update();

    vtkNew<vtkTransformPolyDataFilter> pdt;
    pdt->SetInputData(sourceMesh);
    pdt->SetTransform(lmTransformer);
    pdt->Update();

    outSource->Initialize();
    outSource->DeepCopy(pdt->GetOutput());
}

void RegistrationThread::BestBoundingBox(std::string const& axis,
                                         vtkPolyData* templ,
                                         vtkPolyData* source,
                                         vtkPolyData* templateResampled,
                                         vtkPolyData* sourceResampled) {
    double bestDistance = VTK_DOUBLE_MAX;

    /* std::vector<double> permutationDistance;
    std::vector<double> permutationAngle; */

    int angleResolution = 4;
    double delta = 360.0 / angleResolution;
    for (int i = 0; i < angleResolution; ++i) {
        double angle = delta * i;
        vtkNew<vtkOBBTree> sourceOBBTree;
        sourceOBBTree->SetDataSet(source);
        sourceOBBTree->SetMaxLevel(1);
        sourceOBBTree->BuildLocator();
        vtkNew<vtkPolyData> sourceBboxLandmarks;
        sourceOBBTree->GenerateRepresentation(0, sourceBboxLandmarks);
        double sourceCenter[3];
        sourceBboxLandmarks->GetCenter(sourceCenter);
        vtkNew<vtkTransform> bBoxTransformer;
        bBoxTransformer->Identity();
        bBoxTransformer->Translate(sourceCenter[0], sourceCenter[1],
                                   sourceCenter[2]);
        if (axis == "X") {
            bBoxTransformer->RotateX(angle);
        } else if (axis == "Y") {
            bBoxTransformer->RotateY(angle);
        } else {
            bBoxTransformer->RotateZ(angle);
        }
        bBoxTransformer->Translate(-sourceCenter[0], -sourceCenter[1],
                                   -sourceCenter[2]);

        bBoxTransformer->Modified();
        bBoxTransformer->Update();
        vtkNew<vtkTransformPolyDataFilter> bBoxTpd;
        bBoxTpd->SetTransform(bBoxTransformer);
        bBoxTpd->SetInputData(sourceBboxLandmarks);
        bBoxTpd->Update();
        auto rotatedBbox = bBoxTpd->GetOutput();
        vtkNew<vtkLandmarkTransform> bBoxLt;
        bBoxLt->SetModeToSimilarity();
        bBoxLt->SetTargetLandmarks(rotatedBbox->GetPoints());
        bBoxLt->SetSourceLandmarks(sourceBboxLandmarks->GetPoints());
        bBoxLt->Update();

        vtkNew<vtkTransformPolyDataFilter> meshTpd;
        meshTpd->SetTransform(bBoxLt);
        meshTpd->SetInputData(source);
        meshTpd->Update();

        vtkNew<vtkTransformPolyDataFilter> resampledPtsTpd;
        resampledPtsTpd->SetTransform(bBoxLt);
        resampledPtsTpd->SetInputData(sourceResampled);
        resampledPtsTpd->Update();

        double testDistance =
            HausdorffDistance(templateResampled, resampledPtsTpd->GetOutput());
        // double testDistance = HausdorffDistance(templ, meshTpd->GetOutput());

        /* std::cout<< "Angle"<<std::endl;
        std::cout<< angle<<std::endl;
        std::cout<< "Distance"<<std::endl;
        std::cout<< testDistance<<std::endl; */

        if (testDistance < bestDistance) {
            bestDistance = testDistance;
            source->DeepCopy(meshTpd->GetOutput());
            sourceResampled->DeepCopy(resampledPtsTpd->GetOutput());
        }
    }
}

void RegistrationThread::Resample(vtkPolyData* mesh, int resolution,
                                  vtkPoints* out) {
    std::vector<int>* idList = new std::vector<int>;
    std::vector<double>* probab = new std::vector<double>;
    std::vector<int>* shuffledIds =
        new std::vector<int>(mesh->GetNumberOfPoints());
    std::generate(shuffledIds->begin(), shuffledIds->end(),
                  [n = 0]() mutable { return n++; });
    auto rng = std::default_random_engine{};
    std::shuffle(std::begin(*shuffledIds), std::end(*shuffledIds), rng);
    vtkNew<vtkPoints> resampledPt;
    vtkNew<vtkPoints> tempPt;
    vtkNew<vtkPolyData> tempPoly;
    double totalArea = GetMeshCellArea(probab, mesh);

    if (mesh->GetNumberOfPoints() > 5000) {
        RandomChoice(mesh, 5000, probab, idList, resampledPt);
        vtkNew<vtkPolyData> resampledPtsPoly;
        resampledPtsPoly->SetPoints(resampledPt);
        resampledPtsPoly->Modified();
        delete m_BlueNoiseThread;
        m_BlueNoiseThread = new BlueNoiseThread(resampledPtsPoly, totalArea,
                                                resolution, tempPt, m_mutex);
        m_BlueNoiseThread->start();
        m_BlueNoiseThread->wait();

    } else {
        delete m_BlueNoiseThread;
        m_BlueNoiseThread =
            new BlueNoiseThread(mesh, totalArea, resolution, tempPt, m_mutex);
        m_BlueNoiseThread->start();
        m_BlueNoiseThread->wait();
    }

    out->Initialize();

    out->DeepCopy(tempPt);

    delete idList;
    delete probab;
    delete shuffledIds;
}

void RegistrationThread::RandomChoice(vtkPolyData* mesh, int outputSize,
                                      std::vector<double>* probab,
                                      std::vector<int>* idList,
                                      vtkPoints* points) {
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
        double X1 = mesh->GetCell(index)->GetPoints()->GetPoint(0)[0];
        double Y1 = mesh->GetCell(index)->GetPoints()->GetPoint(0)[1];
        double Z1 = mesh->GetCell(index)->GetPoints()->GetPoint(0)[2];

        double X2 = mesh->GetCell(index)->GetPoints()->GetPoint(1)[0];
        double Y2 = mesh->GetCell(index)->GetPoints()->GetPoint(1)[1];
        double Z2 = mesh->GetCell(index)->GetPoints()->GetPoint(1)[2];

        double X3 = mesh->GetCell(index)->GetPoints()->GetPoint(2)[0];
        double Y3 = mesh->GetCell(index)->GetPoints()->GetPoint(2)[1];
        double Z3 = mesh->GetCell(index)->GetPoints()->GetPoint(2)[2];

        double x = A * X1 + B * X2 + C * X3;
        double y = A * Y1 + B * Y2 + C * Y3;
        double z = A * Z1 + B * Z3 + C * Z3;
        points->InsertNextPoint(x, y, z);
        idList->push_back(index);
    }
}

double RegistrationThread::RandomFloat(double maximum) {
    return (double)rand() / (double)(RAND_MAX / maximum);
}

double RegistrationThread::GetMeshCellArea(std::vector<double>* probab,
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

double RegistrationThread::HausdorffDistance(vtkPolyData* inputA,
                                             vtkPolyData* inputB) {
    double RelativeDistance[2];
    RelativeDistance[0] = 0.0;
    RelativeDistance[1] = 0.0;
    double HausdorffDistance = 0;
    vtkNew<vtkKdTreePointLocator> pointLocatorA;
    vtkNew<vtkKdTreePointLocator> pointLocatorB;

    pointLocatorA->SetDataSet(inputA);
    pointLocatorA->BuildLocator();
    pointLocatorB->SetDataSet(inputB);
    pointLocatorB->BuildLocator();

    double dist;
    double currentPoint[3];
    double closestPoint[3];

    // Find the nearest neighbors to each point and add edges between them,
    // if they do not already exist and they are not self loops
    for (int i = 0; i < inputA->GetNumberOfPoints(); i++) {
        inputA->GetPoint(i, currentPoint);
        vtkIdType closestPointId =
            pointLocatorB->FindClosestPoint(currentPoint);
        inputB->GetPoint(closestPointId, closestPoint);
        dist = std::sqrt(std::pow(currentPoint[0] - closestPoint[0], 2) +
                         std::pow(currentPoint[1] - closestPoint[1], 2) +
                         std::pow(currentPoint[2] - closestPoint[2], 2));

        if (dist > RelativeDistance[0]) {
            RelativeDistance[0] = dist;
        }
    }
    for (int i = 0; i < inputB->GetNumberOfPoints(); i++) {
        inputB->GetPoint(i, currentPoint);
        vtkIdType closestPointId =
            pointLocatorA->FindClosestPoint(currentPoint);
        inputA->GetPoint(closestPointId, closestPoint);
        dist = std::sqrt(std::pow(currentPoint[0] - closestPoint[0], 2) +
                         std::pow(currentPoint[1] - closestPoint[1], 2) +
                         std::pow(currentPoint[2] - closestPoint[2], 2));

        if (dist > RelativeDistance[1]) {
            RelativeDistance[1] = dist;
        }
    }
    if (RelativeDistance[0] >= RelativeDistance[1]) {
        HausdorffDistance = RelativeDistance[0];
    } else {
        HausdorffDistance = RelativeDistance[1];
    }
    return HausdorffDistance;
}

RegistrationThread::~RegistrationThread() {
    delete m_BlueNoiseThread;
}

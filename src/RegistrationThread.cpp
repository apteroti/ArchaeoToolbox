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

#include "include/RegistrationThread.h"

RegistrationThread::RegistrationThread(vtkPolyData* templateMesh,
                                       vtkPolyData* sourceMesh,
                                       vtkPolyData* outMesh, int beta,
                                       int lambda, bool ignoreInside, int res,
                                       QMutex* mutex, bool preAlign)
    : m_outMesh(outMesh),
      m_ignoreInside(ignoreInside),
      m_flexibility(beta),
      m_smoothness(lambda),
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

    if (m_templateMesh->GetNumberOfPoints() <
            alignedMesh->GetNumberOfPoints() ||
        m_templateMesh->GetNumberOfPoints() <
            m_sourceMesh->GetNumberOfPoints()) {
        if (m_templateMesh->GetNumberOfPoints() < m_res) {
            resampleResolution = m_templateMesh->GetNumberOfPoints() / 1.0;
        }
    } else {
        if (!m_preAlign) {
            if (alignedMesh->GetNumberOfPoints() < m_res) {
                resampleResolution = alignedMesh->GetNumberOfPoints() / 1.0;
            }
        } else {
            if (m_sourceMesh->GetNumberOfPoints() < m_res) {
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

    // Beta (Kernel Width) determines the "flexibility" of the deformation
    // Lambda determines smoothness of deformation
    CPD::Nonrigid nonrigid;
    nonrigid.beta(m_flexibility);
    nonrigid.lambda(m_smoothness);
    auto deformedSourceMatrix =
        nonrigid.run(templateMatrix, sourceMatrix).points;

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

void RegistrationThread::PrealignMesh(vtkPolyData* sourceMesh,
                                      vtkPolyData* templateMesh,
                                      vtkPolyData* alignedOutput) {
    // Step 1: Compute centroids
    double sourceCentroid[3];
    double templateCentroid[3];
    ComputeCentroid(sourceMesh, sourceCentroid);
    ComputeCentroid(templateMesh, templateCentroid);

    // Step 2: Compute eigenvectors of covariance matrices
    Eigen::Matrix3d covSource =
        ComputeCovarianceMatrix(sourceMesh, sourceCentroid);
    Eigen::Matrix3d covTemplate =
        ComputeCovarianceMatrix(templateMesh, templateCentroid);

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
    transform->Translate(-sourceCentroid[0], -sourceCentroid[1],
                         -sourceCentroid[2]);

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

Eigen::Matrix3d RegistrationThread::ComputeCovarianceMatrix(
    vtkPolyData* mesh, const double centroid[3]) {
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

void RegistrationThread::ComputeCentroid(vtkPolyData* mesh,
                                         double centroid[3]) {
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

RegistrationThread::~RegistrationThread() { delete m_BlueNoiseThread; }

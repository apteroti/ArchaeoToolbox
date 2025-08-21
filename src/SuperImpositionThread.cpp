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

#include "include/SuperImpositionThread.h"
/**
 * @class SuperImpositionThread
 * @brief A class for performing Procrustes superimposition (Generalized Procrustes Analysis) on 3D landmark data
 * 
 * This class implements Procrustes superimposition using Kabsch algorithm to align 3D shapes by
 * removing translation, rotation, and optionally scaling differences between landmark configurations.
 * It also computes Procrustes residuals and distances after alignment.
 */

/**
 * @brief Constructor for SuperImpositionThread
 * @param dataBase Pointer to the database containing landmark data
 * @param nameList List of specimen names to process
 * @param lmIdList Pointer to vector of landmark IDs to use for alignment
 * @param method Alignment method: 0 for with scaling, 1 for without scaling
 * @param mutex QMutex for thread-safe operations
 * 
 * Initializes the superimposition thread and sets up template landmarks.
 * The template is used as the reference for all subsequent alignments.
 */
SuperImpositionThread::SuperImpositionThread(
    DataBase* dataBase, const std::vector<std::string>& nameList,
    std::vector<int>* lmIdList, int method, QMutex* mutex)
    : m_dataBase(dataBase),
      m_nameList(nameList),
      m_lmIdList(lmIdList),
      m_method(method), m_mutex(mutex) {

    GetAnchors("Template", m_lmIdList, m_template);
    m_numFixed = m_dataBase->GetTypeI("Template")->GetNumberOfPoints();
    m_numCurveSlider =
        m_dataBase->GetCurveSliders("Template")->GetNumberOfPoints();
    m_numSurfaceSlider =
        m_dataBase->GetSurfaceSliders("Template")->GetNumberOfPoints();
}

/**
 * @brief Extracts specified landmarks from a specimen and stores them in a matrix
 * @param name Name of the specimen
 * @param lmIdList Pointer to vector of landmark IDs to extract
 * @param[out] outLandmarks Output matrix (n x 3) where n is number of landmarks
 * 
 * Each row in the output matrix represents a landmark's 3D coordinates (x,y,z).
 */
void SuperImpositionThread::GetAnchors(std::string name,
                                       const std::vector<int>* lmIdList,
                                       Eigen::MatrixXd& outLandmarks) {
    outLandmarks.resize(0, 0);
    outLandmarks.resize(lmIdList->size(), 3);
    vtkPoints* tempPts = m_dataBase->GetTotalLandmarks(name)->GetPoints();
    for (int i = 0; i < lmIdList->size(); i++) {
        outLandmarks.operator()(i, 0) = tempPts->GetPoint(lmIdList->at(i))[0];
        outLandmarks.operator()(i, 1) = tempPts->GetPoint(lmIdList->at(i))[1];
        outLandmarks.operator()(i, 2) = tempPts->GetPoint(lmIdList->at(i))[2];
    }
}

/**
 * @brief Debug utility to print a matrix to console
 * @param matrix The Eigen matrix to print
 * 
 * Formats the matrix with 4 decimal places and adds separators for readability.
 */
void SuperImpositionThread::DebugPrintMatrix(Eigen::MatrixXd matrix) {
    Eigen::IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");
    std::string sep = "\n----------------------------------------\n";
    std::cout << matrix.format(CleanFmt) << sep;
}

/**
 * @brief Computes the Kabsch transformation to align specimen to template
 * @param tmplate Reference template landmarks (n x 3 matrix)
 * @param specimen Specimen landmarks to align (n x 3 matrix)
 * @param[out] outTrans Output VTK transform containing the computed transformation
 * @param scale Whether to include scaling in the transformation (true for full Procrustes)
 * 
 * Uses Umeyama's algorithm to compute the optimal rigid (or similarity) transformation.
 * The transformation minimizes the least-squares error between corresponding points:
 * 
 * min ‖s·R·X + t - Y‖²
 * 
 * where:
 * - R is the rotation matrix
 * - t is the translation vector
 * - s is the scaling factor (if scale=true)
 * - X is the specimen matrix
 * - Y is the template matrix
 */
void SuperImpositionThread::GetKabschTransformation(
    const Eigen::MatrixXd& tmplate, Eigen::MatrixXd& specimen,
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

/**
 * @brief Main thread execution function that performs the superimposition
 * 
 * For each specimen in the name list:
 * 1. Extracts landmarks
 * 2. Computes Kabsch transformation
 * 3. Applies transformation to all relevant data (landmarks, mesh)
 * 4. Computes Procrustes residuals and distances
 * 5. Updates the database with transformed data
 * 
 * Emits signals to notify GUI of progress and completion.
 */
void SuperImpositionThread::run() {
    for (std::string name : m_nameList) {
        Eigen::MatrixXd specimen;
        GetAnchors(name, m_lmIdList, specimen);
        vtkNew<vtkTransform> transMatrix;
        bool scaleOrNot = 1;
        if (m_method == 0) {
            scaleOrNot = 1;
        } else if (m_method == 1) {
            scaleOrNot = 0;
        }
       
        try {
            GetKabschTransformation(m_template, specimen, transMatrix,
                                    scaleOrNot);
            transMatrix->Modified();
        } catch (std::bad_alloc e) {
            std::cerr << "Sample " + name + " is throwing exception"
                      << std::endl;
            emit CoordinateNotChanged(name);
            break;
        }
        m_mutex->lock();
        vtkPolyData* specimenTotalLms = m_dataBase->GetTotalLandmarks(name);
        vtkPolyData* specimenMesh = m_dataBase->GetPolyNode(name);
        m_mutex->unlock();
        vtkNew<vtkTransformPolyDataFilter> totalLmTransformFilter;
        totalLmTransformFilter->SetInputData(specimenTotalLms);
        totalLmTransformFilter->SetTransform(transMatrix);
        totalLmTransformFilter->Update();

        vtkNew<vtkTransformPolyDataFilter> meshTransformFilter;
        meshTransformFilter->SetInputData(specimenMesh);
        meshTransformFilter->SetTransform(transMatrix);
        meshTransformFilter->Update();

        vtkPolyData* transSpecimenTotalLms =
            totalLmTransformFilter->GetOutput();
        vtkPolyData* transSpecimenMesh = meshTransformFilter->GetOutput();

        if (m_numFixed > 0) {
            vtkNew<vtkPolyData> specimenFixedLms;
            m_mutex->lock();
            specimenFixedLms->SetPoints(m_dataBase->GetTypeI(name));
            specimenFixedLms->Modified();
            m_mutex->unlock();
            vtkNew<vtkTransformPolyDataFilter> fixedLmTransformFilter;
            fixedLmTransformFilter->SetInputData(specimenFixedLms);
            fixedLmTransformFilter->SetTransform(transMatrix);
            fixedLmTransformFilter->Update();
            vtkPolyData* transFixedLms = fixedLmTransformFilter->GetOutput();
            m_mutex->lock();
            m_dataBase->InsertTypeI(name, transFixedLms->GetPoints());
            m_mutex->unlock();
        }
        if (m_numCurveSlider > 0) {
            vtkNew<vtkPolyData> specimenCurveLms;
            m_mutex->lock();
            specimenCurveLms->SetPoints(m_dataBase->GetCurveSliders(name));
            specimenCurveLms->Modified();
            m_mutex->unlock();
            vtkNew<vtkTransformPolyDataFilter> curveSliderLmTransformFilter;
            curveSliderLmTransformFilter->SetInputData(specimenCurveLms);
            curveSliderLmTransformFilter->SetTransform(transMatrix);
            curveSliderLmTransformFilter->Update();
            vtkPolyData* transCurveLms =
                curveSliderLmTransformFilter->GetOutput();
            m_mutex->lock();
            m_dataBase->InsertCurveSliders(name, transCurveLms->GetPoints());
            m_mutex->unlock();
        }

        if (m_numSurfaceSlider > 0) {
            vtkNew<vtkPolyData> specimenSurfaceLms;
            m_mutex->lock();
            specimenSurfaceLms->SetPoints(m_dataBase->GetSurfaceSliders(name));
            specimenSurfaceLms->Modified();
            m_mutex->unlock();
            vtkNew<vtkTransformPolyDataFilter> surfaceSliderLmTransformFilter;
            surfaceSliderLmTransformFilter->SetInputData(specimenSurfaceLms);
            surfaceSliderLmTransformFilter->SetTransform(transMatrix);
            surfaceSliderLmTransformFilter->Update();
            vtkPolyData* transSurfaceLms =
                surfaceSliderLmTransformFilter->GetOutput();
            m_mutex->lock();
            m_dataBase->InsertSurfaceSliders(name, transSurfaceLms->GetPoints());
            m_mutex->unlock();
        }
        m_mutex->lock();
        vtkNew<vtkThinPlateSplineTransform> lmWarpTrans;
        lmWarpTrans->SetTargetLandmarks(
            m_dataBase->GetTotalLandmarks("Template")->GetPoints());
        lmWarpTrans->SetSourceLandmarks(transSpecimenTotalLms->GetPoints());
        lmWarpTrans->SetBasisToR();
        lmWarpTrans->Update();
        m_mutex->unlock();

        vtkNew<vtkTransformPolyDataFilter> polyWarpTransformFilter;
        polyWarpTransformFilter->SetInputData(transSpecimenMesh);
        polyWarpTransformFilter->SetTransform(lmWarpTrans);
        polyWarpTransformFilter->Update();
        vtkPolyData* warpedSpeciMesh = polyWarpTransformFilter->GetOutput();

        vtkNew<vtkDoubleArray> wMagArray;
        wMagArray->SetName("ProcrustesResidualMagnitude");
        wMagArray->SetNumberOfComponents(1);
        wMagArray->SetNumberOfTuples(warpedSpeciMesh->GetNumberOfPoints());

        vtkNew<vtkDoubleArray> wVecArray;
        wVecArray->SetName("ProcrustesResidualVector");
        wVecArray->SetNumberOfComponents(3);
        wVecArray->SetNumberOfTuples(warpedSpeciMesh->GetNumberOfPoints());

        for (int i = 0; i < warpedSpeciMesh->GetNumberOfPoints(); i++) {
            double deltaX = warpedSpeciMesh->GetPoint(i)[0] -
                            transSpecimenMesh->GetPoint(i)[0];
            double deltaY = warpedSpeciMesh->GetPoint(i)[1] -
                            transSpecimenMesh->GetPoint(i)[1];
            double deltaZ = warpedSpeciMesh->GetPoint(i)[2] -
                            transSpecimenMesh->GetPoint(i)[2];
            double magn =
                sqrt((deltaX * deltaX) + (deltaY * deltaY) + (deltaZ * deltaZ));
            wMagArray->SetTuple1(i, magn);
            wVecArray->SetTuple3(i, deltaX, deltaY, deltaZ);
        }
        wMagArray->Modified();
        wVecArray->Modified();
        transSpecimenMesh->GetPointData()->SetScalars(wMagArray);
        transSpecimenMesh->GetPointData()->SetVectors(wVecArray);

        vtkNew<vtkDoubleArray> ProcDistArray;
        ProcDistArray->SetName("ProcrustesDistance");
        ProcDistArray->SetNumberOfComponents(1);
        ProcDistArray->SetNumberOfTuples(
            transSpecimenTotalLms->GetNumberOfPoints());

        vtkNew<vtkDoubleArray> ProcVecArray;
        ProcVecArray->SetName("ProcrustesVector");
        ProcVecArray->SetNumberOfComponents(3);
        ProcVecArray->SetNumberOfTuples(
            transSpecimenTotalLms->GetNumberOfPoints());
        m_mutex->lock();
        vtkPoints* templatePts =
            m_dataBase->GetTotalLandmarks("Template")->GetPoints();
        for (int i = 0; i < transSpecimenTotalLms->GetNumberOfPoints(); i++) {
            double deltaX = templatePts->GetPoint(i)[0] -
                            transSpecimenTotalLms->GetPoint(i)[0];
            double deltaY = templatePts->GetPoint(i)[1] -
                            transSpecimenTotalLms->GetPoint(i)[1];
            double deltaZ = templatePts->GetPoint(i)[2] -
                            transSpecimenTotalLms->GetPoint(i)[2];
            double magn =
                sqrt((deltaX * deltaX) + (deltaY * deltaY) + (deltaZ * deltaZ));
            ProcDistArray->SetTuple1(i, magn);
            ProcVecArray->SetTuple3(i, deltaX, deltaY, deltaZ);
        }
        ProcDistArray->Modified();
        ProcVecArray->Modified();
        m_mutex->unlock();

        transSpecimenTotalLms->GetPointData()->SetScalars(ProcDistArray);
        transSpecimenTotalLms->GetPointData()->SetVectors(ProcVecArray);
        

        m_mutex->lock();

        m_dataBase->ChangePoly(name, transSpecimenMesh);
        m_dataBase->SetLandMarks(name, transSpecimenTotalLms);
        m_dataBase->SetProcDistance(name, ProcDistArray);
        
        
        m_mutex->unlock();
        emit CoordinateChanged(name);
    }
}

/**
 * @brief Destructor for SuperImpositionThread
 */
SuperImpositionThread::~SuperImpositionThread() {
}
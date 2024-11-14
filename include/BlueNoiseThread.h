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

#ifndef BLUENOISETHREAD_H
#define BLUENOISETHREAD_H

#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkAppendArcLength.h>
#include <vtkArrowSource.h>
#include <vtkAutoInit.h>
#include <vtkBooleanOperationPolyDataFilter.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkCellIterator.h>
#include <vtkCellLocator.h>
#include <vtkCellPicker.h>
#include <vtkCenterOfMass.h>
#include <vtkCleanPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkColorSeries.h>
#include <vtkColorTransferFunction.h>
#include <vtkCompositeDataGeometryFilter.h>
#include <vtkCompositePolyDataMapper2.h>
#include <vtkConnectivityFilter.h>
#include <vtkContourFilter.h>
#include <vtkContourTriangulator.h>
#include <vtkDataObjectTreeIterator.h>
#include <vtkDataSetMapper.h>
#include <vtkDecimatePro.h>
#include <vtkDelaunay3D.h>
#include <vtkDijkstraGraphGeodesicPath.h>
#include <vtkDoubleArray.h>
#include <vtkExtractEdges.h>
#include <vtkExtractGeometry.h>
#include <vtkExtractPolyDataGeometry.h>
#include <vtkFeatureEdges.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkGenericRenderWindowInteractor.h>
#include <vtkGeometryFilter.h>
#include <vtkGlyph3D.h>
#include <vtkGlyph3DMapper.h>
#include <vtkImplicitPolyDataDistance.h>
#include <vtkImplicitSelectionLoop.h>
#include <vtkInformation.h>
#include <vtkIntArray.h>
#include <vtkInteractorStyleImage.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkIntersectionPolyDataFilter.h>
#include <vtkLabeledDataMapper.h>
#include <vtkLight.h>
#include <vtkLine.h>
#include <vtkLineSource.h>
#include <vtkLookupTable.h>
#include <vtkMaskPoints.h>
#include <vtkMassProperties.h>
#include <vtkMeshQuality.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOBJWriter.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricSpline.h>
#include <vtkPlaneSource.h>
#include <vtkPointData.h>
#include <vtkPointGaussianMapper.h>
#include <vtkPointLocator.h>
#include <vtkPointPicker.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyDataPointSampler.h>
#include <vtkPolyLine.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSCurveSpline.h>
#include <vtkSelectEnclosedPoints.h>
#include <vtkSelectPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkSortDataArray.h>
#include <vtkSphereSource.h>
#include <vtkSplineFilter.h>
#include <vtkStaticPointLocator.h>
#include <vtkStripper.h>
#include <vtkSurfaceReconstructionFilter.h>
#include <vtkTable.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkThinPlateSplineTransform.h>
#include <vtkThreshold.h>
#include <vtkThresholdPoints.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTubeFilter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkXMLMultiBlockDataWriter.h>
#include <vtkXMLPolyDataWriter.h>

#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QThread>
#include <QWidget>
#include <omp.h>
#include <chrono>

// Cem Yuskel 2015, with modifications

class BlueNoiseThread : public QThread{
    Q_OBJECT
   private:
    vtkPolyData* m_meshData;
    vtkSmartPointer<vtkStaticPointLocator> m_kdTree;
    vtkPoints* m_output = nullptr;
    vtkPoints* m_fixedLm = nullptr;
    vtkPoints* m_curveLm = nullptr;
    double m_area = 0.0;
    int m_resolution = 0;
    int m_initRes = 0;
    double m_gamma = 1.5;
    double m_beta = 0.65;
    double m_rMax = 0.0;
    double m_rMin = 0.0;
    double m_lmBuffer = 0.0;
    QMutex* m_mutex;
    void IntegrateLandmarks(vtkPoints* fixedLm, vtkPoints* curveSliders);
   public:
    BlueNoiseThread(vtkPolyData* inputPoly, double area, int resol, vtkPoints* output, QMutex* mutex);
    BlueNoiseThread(vtkPolyData* inputPoly, vtkPoints* fixedLm, vtkPoints* curveLm, double area, int resol, vtkPoints* output, QMutex* mutex);
    int FindIndexOfLargest(vtkPolyData* poly, std::string arrName);
    void PerformCalculations(vtkPolyData *poly, vtkStaticPointLocator* tree, std::string arrName, int targetId, double rMin, double rMax);
    void CalculateWeight(vtkPolyData* inputPoly, vtkStaticPointLocator* tree, double rMax, double rMin, int res);
    double EucDist(double Ax, double Ay, double Az, double Bx, double By, double Bz);
    void run()override;
    ~BlueNoiseThread();
   signals:
    void SamplingIsDone();
};



#endif
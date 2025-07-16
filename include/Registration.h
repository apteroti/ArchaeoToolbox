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

#ifndef REGISTRATION_H
#define REGISTRATION_H

#include <QVTKOpenGLWidget.h>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkAxesActor.h>
#include <vtkCamera.h>
#include <vtkCellLocator.h>
#include <vtkCellPicker.h>
#include <vtkCubeSource.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkGenericCell.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkGenericRenderWindowInteractor.h>
#include <vtkGlyph3DMapper.h>
#include <vtkImplicitPolyDataDistance.h>
#include <vtkInteractorStyleImage.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkIterativeClosestPointTransform.h>
#include <vtkKdTreePointLocator.h>
#include <vtkLabeledDataMapper.h>
#include <vtkLandmarkTransform.h>
#include <vtkLight.h>
#include <vtkMaskPoints.h>
#include <vtkMassProperties.h>
#include <vtkMatrix4x4.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOBBTree.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPointData.h>
#include <vtkPointGaussianMapper.h>
#include <vtkPointLocator.h>
#include <vtkPointPicker.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkThinPlateSplineTransform.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkXMLPolyDataWriter.h>

#include <Eigen/Dense>
#include <Eigen/Eigen>
#include <Eigen/Eigenvalues>
#include <Eigen/Sparse>
#include <QtGui/QCloseEvent>
#include <QtGui/QSurfaceFormat>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGraphicsDropShadowEffect>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolBar>

#include <cpd/gauss_transform.hpp>
#include <iostream>

#include "SpecimenDigitiser.fwd.h"
//#include "ProSetMenu.fwd.h"
#include "Morphometrics.h"
#include "RegistrationThread.h"
#include "SlidingThread.h"
#include "StatusReporterThread.h"

enum METHOD { Auto = 0, Semi = 1, Manual = 2 };
enum ALIGNEDSTATUS { NotAligned = 0, Aligned = 1};

class Registration : public QMainWindow {
   private:
    // Data
    QMutex* m_mutex;
    bool m_ignoreInside = true; 
    METHOD m_regMethod = METHOD::Manual;
    ALIGNEDSTATUS m_status = ALIGNEDSTATUS::NotAligned;
    SpecimenDigitiser* m_parent;
    vtkSmartPointer<vtkPoints> m_templateHighlightPoints;
    vtkSmartPointer<vtkIntArray> m_templateOldIds;
    vtkSmartPointer<vtkPoints> m_targetHighlightPoints;
    vtkSmartPointer<vtkIntArray> m_targetOldIds;
    vtkSmartPointer<vtkPolyData> m_overlaidMesh;
    vtkSmartPointer<vtkPolyData> m_morphedMesh;
    vtkSmartPointer<vtkPoints> m_preSliderHighlightPoints;
    vtkPolyData* m_meshData;
    vtkPolyData* m_templateMesh;
    vtkPoints* m_templateSliders;

    bool m_anchorStat = 0;
    vtkSmartPointer<vtkPoints> m_templateAnchor;
    vtkSmartPointer<vtkPoints> m_sourceAnchor;

    /* vtkPoints* m_templateTypeII;
    vtkSmartPointer<vtkPoints> m_templateCurveSliders;

    vtkPoints* m_typeII;
    vtkSmartPointer<vtkPoints> m_curveSliders; */
    
    int m_resampledRes = 300;
    
    StatusReporterThread* m_morphingStatThread = nullptr;
    RegistrationThread* m_regThread = nullptr;
    // Toolbar
    QToolBar* mainToolbar;
    QComboBox* registerTypeComboBox;
    QLineEdit* progressLineEdit;
    // Buttons
    QPushButton* morphButton;
    QPushButton* sliderButton;
    QPushButton* resetButton;
    //QPushButton* refineButton;

    // Labels
    QLabel* overlayLabel;
    QLabel* statusLabel;
    QLabel* progressLabel;
    // Rendering
    QVTKOpenGLWidget* m_templateRenderWidget;
    QVTKOpenGLWidget* m_overlayRenderWidget;
    QVTKOpenGLWidget* m_targetRenderWidget;
    // template
    vtkSmartPointer<vtkRenderer> m_templateRenderer;
    vtkSmartPointer<vtkRenderWindow> m_templateRenWin;
    vtkSmartPointer<vtkGenericRenderWindowInteractor> m_templateIren;
    vtkSmartPointer<vtkInteractorStyleTrackballCamera>
        m_templatePointPickerStyle;
    vtkSmartPointer<vtkInteractorStyleTrackballCamera>
        m_templatePointMoverStyle;
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> m_templateStyle;
    vtkSmartPointer<vtkInteractorStyleImage> m_template2Dstyle;
    vtkSmartPointer<vtkActor> m_templateMeshActor;
    vtkSmartPointer<vtkActor> m_templatePointActor;
    vtkSmartPointer<vtkVertexGlyphFilter> m_templateVertexFilter;
    vtkSmartPointer<vtkPolyData> m_templatePointsPoly;
    vtkSmartPointer<vtkPointPicker> m_templateGrabPicker;
    // target
    vtkSmartPointer<vtkRenderer> m_targetRenderer;
    vtkSmartPointer<vtkRenderWindow> m_targetRenWin;
    vtkSmartPointer<vtkGenericRenderWindowInteractor> m_targetIren;
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> m_targetPointPickerStyle;
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> m_targetPointMoverStyle;
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> m_targetStyle;
    vtkSmartPointer<vtkInteractorStyleImage> m_target2Dstyle;
    vtkSmartPointer<vtkDataSetMapper> m_targetMapper;
    vtkSmartPointer<vtkActor> m_targetMeshActor;
    vtkSmartPointer<vtkActor> m_targetPointActor;
    vtkSmartPointer<vtkVertexGlyphFilter> m_targetVertexFilter;
    vtkSmartPointer<vtkPolyData> m_targetPointsPoly;
    vtkSmartPointer<vtkPointPicker> m_targetGrabPicker;

    vtkSmartPointer<vtkActor> m_sliderTempPointActor;
    vtkSmartPointer<vtkVertexGlyphFilter> m_sliderTempVertexFilter;

    // overlay
    vtkSmartPointer<vtkRenderer> m_overlayRenderer;
    vtkSmartPointer<vtkRenderWindow> m_overlayRenWin;
    vtkSmartPointer<vtkDataSetMapper> m_overlayMapper;
    vtkSmartPointer<vtkActor> m_overlayMeshActor;
    vtkSmartPointer<vtkActor> m_overlayTemplateActor;
    vtkSmartPointer<vtkActor> m_sliderPointActor;
    vtkSmartPointer<vtkVertexGlyphFilter> m_sliderVertexFilter;
    // vtkSmartPointer<vtkPolyData> m_sliderPointsPoly;

   public:
    // Registration();
    Registration(vtkPolyData* data, vtkPolyData* templateMesh,
                 vtkPoints* sliders, SpecimenDigitiser* parent);
    void Register();
    void SetTemplateScene();
    void SetTargetScene();
    void SetOverlayScene();
    // Interaction
    void TemplatePickFunc(vtkObject* caller, long unsigned int eventId,
                          void* callData);
    void TemplateResetLeftClck(vtkObject* caller, long unsigned int eventId,
                               void* callData);
    void TemplateMoveFunc(vtkObject* caller, long unsigned int eventId,
                          void* callData);
    void TemplateResetMouseMove(vtkObject* caller, long unsigned int eventId,
                                void* callData);
    void TemplateCoordinateFunc(vtkObject* caller, long unsigned int eventId,
                                void* callData);

    void TargetPickFunc(vtkObject* caller, long unsigned int eventId,
                        void* callData);
    void TargetResetLeftClck(vtkObject* caller, long unsigned int eventId,
                             void* callData);
    void TargetMoveFunc(vtkObject* caller, long unsigned int eventId,
                        void* callData);
    void TargetResetMouseMove(vtkObject* caller, long unsigned int eventId,
                              void* callData);
    void TargetCoordinateFunc(vtkObject* caller, long unsigned int eventId,
                              void* callData);
    void DebugPrintMatrix(Eigen::MatrixXd matrix);
    void ChangeRegisterMode(int index);
    void MorphTool();
    void SliderTool();
    void Warp();
    void SI();  // SuperImposition
    void GetKabschTransformation(const Eigen::MatrixXd& tmplate,
                                 Eigen::MatrixXd& specimen,
                                 vtkTransform* outTrans, bool scale);
    void ResetOverlay();
    void ResetTool();
    //void Refine();
    //void Morph();
    void FinalizeDigitization(Eigen::MatrixXd& Lndmrks, bool sendOffData);
    void PCA(Eigen::MatrixXd& data, Eigen::MatrixXd& out);
    ~Registration();
   public Q_SLOTS:
    void OnCoordinateChanged(Eigen::MatrixXd sendOffData);
    void OnCoordinateNotChanged(Eigen::MatrixXd sendOffData);
    void OnMeshMorphed();
    void RegistrationStatus();
    void OnRegisterStatusChanged(int status);
    bool IsRunning();
    void SetAnchors(vtkPoints* sourceAnchor, vtkPoints* templateAnchor);
   protected:
    void closeEvent(QCloseEvent* event) override;
};

#endif
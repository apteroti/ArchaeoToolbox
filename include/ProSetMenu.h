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

#ifndef PROSETMENU_H
#define PROSETMENU_H

#include <QtWidgets/QLabel>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QAction>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <vtkPoints.h>
#include <vtkOBJReader.h>
#include <vtkPolyData.h>
#include <vtkPolygon.h>
#include <vtkSmartPointer.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkPLYReader.h>
#include <vtkTriangleFilter.h>


#include "MainWindow.fwd.h"
#include "TemplateDigitiser.h"
#include <iostream>
#include <string>


class ProSetMenu:public QWidget
{
private:
    bool m_ignoreInside = true;
    TemplateDigitiser* m_templatePlot;
    int typeINOL = 0;
    int surfaceNOS = 0;
    //int surfacePatchNOS = 0;
    int surfacePatchUNOS = 0;
    int surfacePatchVNOS = 0;
    int surfacePatchNOP = 1;
    int curveNOS = 0;
    int curveNOC = 1;
    int fixedLandmarkLimit = 3;
    int surfaceSliderLimit = 25;
    int surfaceResolutionLimit = 5;
    int curveSliderLimit = 10;
    std::string m_geometryType = "";
    QGridLayout *layout;
    QLineEdit *typeILineEdit;
    QLineEdit *surfaceLineEditNOS;
    QLineEdit *surfaceLineEditPatchUNOS;
    QLineEdit *surfaceLineEditPatchVNOS;
    QLineEdit *surfaceLineEditPatchNOP;
    QLineEdit *curveLineEditNOS;
    QLineEdit *curveLineEditNOC;
    QLineEdit *maskLineEdit;
    QLineEdit *registerLineEdit;
    QGroupBox *resolutionGroup;
    
    QComboBox *surfaceComboBox;
    MainWindow* m_parent;
    QLabel *surfacePatchLabel;
    QLabel *surfaceNOSLabel;
    QLabel *surfaceResolutionLabel;
    vtkSmartPointer<vtkPoints> m_templateTypeI;
    vtkSmartPointer<vtkPoints> m_templateSurfaceSliders;
    vtkSmartPointer<vtkMultiBlockDataSet> m_templateCurveSliders;
    vtkSmartPointer<vtkMultiBlockDataSet> m_templateCurvePtsPoly;
    vtkSmartPointer<vtkMultiBlockDataSet> m_templatePatchSurfaceSliders;
    vtkSmartPointer<vtkMultiBlockDataSet> m_templatePatchSurfaceCurve;
    vtkSmartPointer<vtkPolyData> m_templatePoly;
    //Buttons
    QPushButton *loadTemplateButton;
    QPushButton *resetButton;
    QPushButton *saveButton;
    QPushButton *importButton;
    QPushButton *registerButton;
    //Layout
    QGridLayout *surfaceLayout;
    QGridLayout *resolutionLayout;
public:
    ProSetMenu(MainWindow* parent);
    void SetTypeINOL();
    int GetTypeINOL();
    void Reset();
    void Register();
    void SaveTemplate();
    void ImportTemplate();
    void ChangeSurfaceMode(int index);
    void LoadTemplate();
    void SetSurfaceNOS();
    int GetSurfaceNOS();
    void SetTemplateTypeI(vtkPoints* fixedPts);
    void SetTemplateCurveSliders(vtkMultiBlockDataSet* sliderPtsPolyBlock, vtkMultiBlockDataSet* curvePtsPolyBlock);
    void SetTemplateSurfaceSliders(vtkPoints* points);
    void SetTemplatePatchSurfaceSliders(vtkMultiBlockDataSet* surfaceptsPolyBlock, vtkMultiBlockDataSet* surfacePatchCurve);
    void SetSurfacePatchUNOS();
    void SetSurfacePatchVNOS();
    std::tuple<int, int> GetSurfacePatchResolution();
    void SetSurfacePatchNOP();
    int GetSurfacePatchNOP();
    void SetCurveNOS();
    int GetCurveNOS();
    void SetCurveNOC();
    int GetCurveNOC();
    void Refresh(bool condition);
    void SetIgnorInternals(bool option);
    bool GetIgnorInternals();
    ~ProSetMenu();

protected:

};






#endif
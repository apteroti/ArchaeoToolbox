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

#include "../include/ExportDialogue.h"
#include "../include/MainWindow.h"

ExportDialogue::ExportDialogue(MainWindow* parent) : m_parent(parent) {
    this->setWindowTitle("Attention!");
    this->resize(400, 150);
    
    QGridLayout *mainLayout = new QGridLayout;
    QLabel *label = new QLabel();
    okButton = new QPushButton(tr("&OK"));
    connect(okButton, &QPushButton::clicked, this,
            &ExportDialogue::Finilise);
    label->setText(tr("What do you want to Export?"));
    
    exportSelectComboBox = new QComboBox();
    exportSelectComboBox->addItem("Raw Landmarks");
    exportSelectComboBox->addItem("SuperImposed Landmarks");
    exportSelectComboBox->addItem("Procrustes Residual Vectors");
    exportSelectComboBox->addItem("Procrestus Residual Magnitude");
    void (QComboBox ::*exportSelectFp)(int) = &QComboBox ::currentIndexChanged;
    connect(exportSelectComboBox, exportSelectFp, this,
            &ExportDialogue::ChangeExport);

    

    mainLayout->addWidget(label, 0, 0, 1, 1);
    mainLayout->addWidget(exportSelectComboBox, 0, 2, 0, 5);
    mainLayout->addWidget(okButton, 5, 2, 4, 4);

    this->setLayout(mainLayout);

    exportSelectComboBox->setCurrentIndex(0);
}

void ExportDialogue::ChangeExport(int index){
    if(index == 0){
        lm = 1;
        si = 0;
        pv = 0;
        pm = 0;
    }
    else if(index == 1){
        lm = 0;
        si = 1;
        pv = 0;
        pm = 0;
    }
    else if(index == 2){
        lm = 0;
        si = 0;
        pv = 1;
        pm = 0;
    }
    else if(index == 3){
        lm = 0;
        si = 0;
        pv = 0;
        pm = 1;
    }
    else{
        std::cout<< "Error! Debug"<<std::endl;
    }
}

void ExportDialogue::Finilise(){
    m_parent->SetToExport(lm, si, pv, pm);
    m_parent->DoExport();
    this->close();
}
ExportDialogue::~ExportDialogue() {}
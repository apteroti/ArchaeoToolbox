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

#include "../include/ProSetMenu.h"
#include "../include/MainWindow.h"

#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
#define ENDL Qt::endl
#else
#define ENDL endl
#endif

ProSetMenu::ProSetMenu(MainWindow* parent) :m_parent(parent) {
    m_templateSurfaceSliders = vtkSmartPointer<vtkPoints>::New();
    m_templateTypeI = vtkSmartPointer<vtkPoints>::New();
    m_templatePoly = vtkSmartPointer<vtkPolyData>::New();
    m_templateCurveSliders = vtkSmartPointer<vtkMultiBlockDataSet>::New();
    m_templatePatchSurfaceSliders = vtkSmartPointer<vtkMultiBlockDataSet>::New();
    m_templatePatchSurfaceCurve = vtkSmartPointer<vtkMultiBlockDataSet>::New();
    m_templateCurvePtsPoly = vtkSmartPointer<vtkMultiBlockDataSet>::New();
    m_templatePlot = new TemplateDigitiser(this);
    m_templatePlot->hide();
    layout = new QGridLayout();
    QString style = "QGroupBox {"
        "font: bold;"
        "border: 1px solid silver;"
        "border-radius: 5px;"
        "margin-top: 1ex;"
        "}" "QGroupBox::title {"
        "subcontrol-origin: margin;"
        "left: 7px;"
        "padding: 0px 5px 0px 5px;"
        "}";
    //---------------------------------------------------------------------
    QGroupBox* typeIGroup = new QGroupBox(tr("Fixed (Type I-II)"));
    typeIGroup->setStyleSheet(style);
    QLabel* typeILabel = new QLabel(tr("Number of Landmarks:"));
    typeILineEdit = new QLineEdit;
    std::string typeIQuery("Number >=" + std::to_string(fixedLandmarkLimit));
    typeILineEdit->setPlaceholderText(QString::fromStdString(typeIQuery));
    typeILineEdit->setValidator(new QIntValidator(0, 10000, this));
    //----------------------------------------------------------------------
    QGroupBox* surfaceGroup = new QGroupBox(tr("Surface"));
    resolutionGroup = new QGroupBox();
    surfaceGroup->setStyleSheet(style);
    QLabel* surfaceLabel = new QLabel(tr("Surface Type"));
    surfacePatchLabel = new QLabel(tr("Number of Patches"));
    surfaceNOSLabel = new QLabel(tr("Number of Sliders:"));
    surfaceResolutionLabel = new QLabel(tr("Resolution(UV):"));
    surfaceComboBox = new QComboBox();
    surfaceComboBox->addItem(tr("Whole geometry"));
    surfaceComboBox->addItem(tr("Patches"));
    surfaceLineEditNOS = new QLineEdit();
    surfaceLineEditPatchUNOS = new QLineEdit();
    surfaceLineEditPatchVNOS = new QLineEdit();
    std::string surfaceQuery("Number >=" + std::to_string(surfaceSliderLimit));
    std::string surfacePatchQuery("Number >=" + std::to_string(surfaceResolutionLimit));
    surfaceLineEditNOS->setPlaceholderText(QString::fromStdString(surfaceQuery));
    surfaceLineEditNOS->setValidator(new QIntValidator(0, 10000, this));
    surfaceLineEditPatchUNOS->setPlaceholderText(QString::fromStdString(surfacePatchQuery));
    surfaceLineEditPatchUNOS->setValidator(new QIntValidator(0, 10000, this));
    surfaceLineEditPatchVNOS->setPlaceholderText(QString::fromStdString(surfacePatchQuery));
    surfaceLineEditPatchVNOS->setValidator(new QIntValidator(0, 10000, this));
    surfaceLineEditPatchNOP = new QLineEdit();
    surfaceLineEditPatchNOP->setPlaceholderText("1");
    surfaceLineEditPatchNOP->setValidator(new QIntValidator(1, 10000, this));
    //--------------------------------------------------------------------
    QGroupBox* curveGroup = new QGroupBox(tr("Curve"));
    curveGroup->setStyleSheet(style);
    QLabel* curveLabel = new QLabel(tr("Number of Semi-Landmarks:"));
    curveLineEditNOS = new QLineEdit;
    std::string curveQuery("Number >=" + std::to_string(curveSliderLimit));
    curveLineEditNOS->setPlaceholderText(QString::fromStdString(curveQuery));
    curveLineEditNOS->setValidator(new QIntValidator(0, 10000, this));
    QLabel* curveNumberLabel = new QLabel(tr("Number of Curves:"));
    curveLineEditNOC = new QLineEdit;
    curveLineEditNOC->setPlaceholderText("1");
    curveLineEditNOC->setValidator(new QIntValidator(1, 10000, this));
    //--------------------------------------------------------------------
    QGroupBox* maskGroup = new QGroupBox(tr("Mask"));
    maskGroup->setStyleSheet(style);
    QLabel* maskLabel = new QLabel(tr("Number of Masks:"));
    maskLineEdit = new QLineEdit;
    maskLineEdit->setPlaceholderText("Not implemented");
    maskLineEdit->setReadOnly(1);
    //--------------------------------------------------------------------
    QGroupBox* registerGroup = new QGroupBox();
    registerGroup->setStyleSheet(style);
    loadTemplateButton = new QPushButton("Make Template");
    loadTemplateButton->setEnabled(true);
    registerButton = new QPushButton("Register");
    registerButton->setEnabled(false);
    saveButton = new QPushButton("Save Template");
    saveButton->setEnabled(false);
    importButton = new QPushButton("Import Template");
    resetButton = new QPushButton("Reset");
    //--------------------------------------------------------------------

    QGridLayout* typeILayout = new QGridLayout();
    typeILayout->addWidget(typeILabel, 0, 0);
    typeILayout->addWidget(typeILineEdit, 0, 1);
    typeIGroup->setLayout(typeILayout);

    surfaceLayout = new QGridLayout();
    surfaceLayout->addWidget(surfaceLabel, 0, 0);
    surfaceLayout->addWidget(surfaceComboBox, 0, 1);
    surfaceLayout->addWidget(surfaceNOSLabel, 1, 0);
    surfaceLayout->addWidget(surfaceLineEditNOS, 1, 1);
    surfaceGroup->setLayout(surfaceLayout);

    resolutionLayout = new QGridLayout();
    resolutionLayout->addWidget(surfaceLineEditPatchUNOS, 0, 0);
    resolutionLayout->addWidget(surfaceLineEditPatchVNOS, 0, 1);
    resolutionGroup->setLayout(resolutionLayout);

    QGridLayout* curveLayout = new QGridLayout();
    curveLayout->addWidget(curveLabel, 0, 0);
    curveLayout->addWidget(curveLineEditNOS, 0, 1);
    curveLayout->addWidget(curveNumberLabel, 1, 0);
    curveLayout->addWidget(curveLineEditNOC, 1, 1);
    curveGroup->setLayout(curveLayout);

    QGridLayout* maskLayout = new QGridLayout();
    maskLayout->addWidget(maskLabel, 0, 0);
    maskLayout->addWidget(maskLineEdit, 0, 1);
    maskGroup->setLayout(maskLayout);

    QGridLayout* registerLayout = new QGridLayout();
    registerLayout->addWidget(loadTemplateButton, 0, 0);
    registerLayout->addWidget(registerButton, 0, 1);
    registerLayout->addWidget(resetButton, 1, 0, 1, 2);
    //registerLayout->addWidget(resetButton, 1, 0);
    registerLayout->addWidget(saveButton, 2, 0);
    registerLayout->addWidget(importButton, 2, 1);
    registerGroup->setLayout(registerLayout);


    connect(typeILineEdit, &QLineEdit::textChanged, this, &ProSetMenu::SetTypeINOL);
    connect(loadTemplateButton, &QPushButton::clicked, this, &ProSetMenu::LoadTemplate);
    connect(surfaceLineEditNOS, &QLineEdit::textChanged, this, &ProSetMenu::SetSurfaceNOS);
    connect(surfaceLineEditPatchUNOS, &QLineEdit::textChanged, this, &ProSetMenu::SetSurfacePatchUNOS);
    connect(surfaceLineEditPatchVNOS, &QLineEdit::textChanged, this, &ProSetMenu::SetSurfacePatchVNOS);
    connect(surfaceLineEditPatchNOP, &QLineEdit::textChanged, this, &ProSetMenu::SetSurfacePatchNOP);
    connect(curveLineEditNOS, &QLineEdit::textChanged, this, &ProSetMenu::SetCurveNOS);
    connect(curveLineEditNOC, &QLineEdit::textChanged, this, &ProSetMenu::SetCurveNOC);
    connect(resetButton, &QPushButton::clicked, this, &ProSetMenu::Reset);
    connect(registerButton, &QPushButton::clicked, this, &ProSetMenu::Register);
    connect(saveButton, &QPushButton::clicked, this, &ProSetMenu::SaveTemplate);
    connect(importButton, &QPushButton::clicked, this, &ProSetMenu::ImportTemplate);
    void (QComboBox :: * fp) (int) = &QComboBox::currentIndexChanged;
    connect(surfaceComboBox, fp, this, &ProSetMenu::ChangeSurfaceMode);

    layout->addWidget(curveGroup, 0, 0);
    layout->addWidget(surfaceGroup, 0, 1);
    layout->addWidget(typeIGroup, 1, 0);
    layout->addWidget(maskGroup, 1, 1);
    layout->addWidget(registerGroup, 2, 0, 1, 2);

    this->setWindowTitle("Project Setting");
    this->setLayout(layout);
}

void ProSetMenu::SetTypeINOL() {
    auto textNum = typeILineEdit->text().toStdString();
    int num = -1;
    try
    {
        num = stoi(textNum);
    }
    catch (const std::exception& e)
    {
        num = -1;
        typeINOL = 0;
    }
    if (num >= 0) {
        typeINOL = num;
    }
    else {
        typeILineEdit->clear();
        typeINOL = 0;
    }
}

int ProSetMenu::GetTypeINOL() {
    return typeINOL;
}

void ProSetMenu::SetTemplateTypeI(vtkPoints* fixedPts) {
    m_templateTypeI = fixedPts;
}

void ProSetMenu::Reset() {
    if (QMessageBox::Yes == QMessageBox::question(this, "Reset Confirmation",
        "This will erase all of the landmarks in your project! \n Do you want to continue?",
        QMessageBox::Yes | QMessageBox::No)) {
        surfaceLineEditNOS->clear();
        surfaceLineEditNOS->setReadOnly(false);
        surfaceNOS = 0;
        surfaceLineEditPatchUNOS->clear();
        surfaceLineEditPatchUNOS->setReadOnly(false);
        surfaceLineEditPatchVNOS->clear();
        surfaceLineEditPatchVNOS->setReadOnly(false);
        surfacePatchUNOS = 0;
        surfacePatchVNOS = 0;
        surfaceLineEditPatchNOP->clear();
        surfaceLineEditPatchNOP->setReadOnly(false);
        surfacePatchNOP = 1;
        //----------
        typeILineEdit->clear();
        typeILineEdit->setReadOnly(false);
        typeINOL = 0;

        //----------
        curveLineEditNOS->clear();
        curveLineEditNOC->clear();
        curveLineEditNOS->setReadOnly(false);
        curveLineEditNOC->setReadOnly(false);
        curveNOS = 0;
        curveNOC = 1;
        //----------
        registerButton->setEnabled(false);
        saveButton->setEnabled(false);
        loadTemplateButton->setEnabled(true);
        importButton->setEnabled(true);
        surfaceComboBox->setEnabled(1);
        surfaceComboBox->setCurrentIndex(0);

        m_templateSurfaceSliders->Initialize();
        m_templateTypeI->Initialize();
        m_templateCurveSliders->Initialize();
        m_templateCurvePtsPoly->Initialize();
        m_templatePatchSurfaceSliders->Initialize();
        m_templatePatchSurfaceCurve->Initialize();
        m_templatePoly->Initialize();

        delete m_templatePlot;
        m_templatePlot = new TemplateDigitiser(this);
        m_parent->TemplateStatus(0);
    }
}

void ProSetMenu::Register() {
    m_parent->SetTemplateMesh(m_templatePoly);
    m_parent->SetTemplateTypeI(m_templateTypeI);
    m_parent->SetTemplateCurveSliders(m_templateCurveSliders, m_templateCurvePtsPoly, curveNOS);
    m_parent->SetTemplateSurfaceSliders(m_templateSurfaceSliders);
    m_parent->SetTemplatePatchSurfaceSliders(m_templatePatchSurfaceSliders, m_templatePatchSurfaceCurve, surfacePatchUNOS, surfacePatchVNOS);
    m_parent->TemplateStatus(1);
    resetButton->setEnabled(true);
    registerButton->setEnabled(false);
    loadTemplateButton->setEnabled(false);
    importButton->setEnabled(false);
}

void ProSetMenu::SaveTemplate() {
    vtkNew<vtkPoints> allPts;
    for (int i = 0; i < typeINOL; i++) {
        allPts->InsertNextPoint(m_templateTypeI->GetPoint(i));
    }
    vtkNew<vtkDataObjectTreeIterator> itercurveSliders;
    itercurveSliders->SetDataSet(m_templateCurveSliders);
    itercurveSliders->SkipEmptyNodesOn();
    itercurveSliders->VisitOnlyLeavesOn();
    for (itercurveSliders->InitTraversal(); !itercurveSliders->IsDoneWithTraversal(); itercurveSliders->GoToNextItem()) {
        vtkDataObject* dso = itercurveSliders->GetCurrentDataObject();
        vtkPolyData* pd = dynamic_cast<vtkPolyData*>(dso);
        for (int i = 0; i < pd->GetNumberOfPoints(); i++) {
            allPts->InsertNextPoint(pd->GetPoint(i));
        }
    }
    for (int i = 0; i < surfaceNOS; i++) {
        allPts->InsertNextPoint(m_templateSurfaceSliders->GetPoint(i));
    }
    vtkNew<vtkDataObjectTreeIterator> itersurfaceSliders;
    itersurfaceSliders->SetDataSet(m_templatePatchSurfaceSliders);
    itersurfaceSliders->SkipEmptyNodesOn();
    itersurfaceSliders->VisitOnlyLeavesOn();
    for (itersurfaceSliders->InitTraversal(); !itersurfaceSliders->IsDoneWithTraversal(); itersurfaceSliders->GoToNextItem()) {
        vtkDataObject* dso = itersurfaceSliders->GetCurrentDataObject();
        vtkPolyData* pd = dynamic_cast<vtkPolyData*>(dso);
        for (int i = 0; i < pd->GetNumberOfPoints(); i++) {
            allPts->InsertNextPoint(pd->GetPoint(i));
        }
    }
    auto filter = "att(*.att)";
    QString filename = QFileDialog::getSaveFileName(this, "Save file", "", filter);
    QFileInfo fi(filename);
    QString ext = fi.completeSuffix();
    if (filename.isEmpty()) {
        return;
    }
    if (ext != "att") {
        filename += ".att";
    }
    QFile f(filename);
    if (f.open(QFile::WriteOnly | QFile::Truncate)) {
        QStringList textItem;
        QTextStream data(&f);

        textItem += QString("Do Not Modify The Content of This File");
        data << textItem.join("") << ENDL;
        textItem.clear();

        textItem += QString("Number of Fixed Landmarks") + ",";
        textItem += QString::number(typeINOL);
        data << textItem.join("") << ENDL;
        textItem.clear();

        textItem += QString("Number of Curves") + ",";
        textItem += QString::number(curveNOC);
        data << textItem.join("") << ENDL;
        textItem.clear();

        textItem += QString("Number of Curve Semi-Landmarks") + ",";
        textItem += QString::number(curveNOS);
        data << textItem.join("") << ENDL;
        textItem.clear();

        textItem += QString("Number of Surface Sliders") + ",";
        textItem += QString::number(surfaceNOS);
        data << textItem.join("") << ENDL;
        textItem.clear();

        textItem += QString("Ignore Islands") + ",";
        textItem += QString::number(m_ignoreInside);
        data << textItem.join("") << ENDL;
        textItem.clear();

        textItem += QString("Number of Surface Patches") + ",";
        textItem += QString::number(surfacePatchNOP);
        data << textItem.join("") << ENDL;
        textItem.clear();

        textItem += QString("Surface Patch U") + ",";
        textItem += QString::number(surfacePatchUNOS);
        data << textItem.join("") << ENDL;
        textItem.clear();

        textItem += QString("Surface Patch V") + ",";
        textItem += QString::number(surfacePatchVNOS);
        data << textItem.join("") << ENDL;
        textItem.clear();

        textItem += QString("Landmarks") + ",";
        data << textItem.join("") << ENDL;
        textItem.clear();

        for (int i = 0; i < allPts->GetNumberOfPoints(); i++) {
            for (int j = 0; j < 3; j++) {
                textItem += QString::number(allPts->GetPoint(i)[j]) + ",";
            }
            data << textItem.join("") << ENDL;
            textItem.clear();
        }

        textItem += QString("Curve Direction Glyph") + ",";
        textItem += QString::number(curveNOC) + ",";
        textItem += QString::number(curveNOS);
        data << textItem.join("") << ENDL;
        textItem.clear();
        vtkNew<vtkDataObjectTreeIterator> itercurvepts;
        itercurvepts->SetDataSet(m_templateCurvePtsPoly);
        itercurvepts->SkipEmptyNodesOn();
        itercurvepts->VisitOnlyLeavesOn();
        for (itercurvepts->InitTraversal(); !itercurvepts->IsDoneWithTraversal(); itercurvepts->GoToNextItem()) {
            vtkDataObject* dso = itercurvepts->GetCurrentDataObject();
            vtkPolyData* pd = dynamic_cast<vtkPolyData*>(dso);
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    textItem += QString::number(pd->GetPoint(i)[j]) + ",";
                }
                data << textItem.join("") << ENDL;
                textItem.clear();
            }
        }

        textItem += QString("Surface Patch Direction Glyph") + ",";
        textItem += QString::number(surfacePatchNOP) + ",";
        textItem += QString::number(surfacePatchUNOS * surfacePatchVNOS);
        data << textItem.join("") << ENDL;
        textItem.clear();
        vtkNew<vtkDataObjectTreeIterator> iterSurfaceCurvepts;
        iterSurfaceCurvepts->SetDataSet(m_templatePatchSurfaceCurve);
        iterSurfaceCurvepts->SkipEmptyNodesOn();
        iterSurfaceCurvepts->VisitOnlyLeavesOn();
        for (iterSurfaceCurvepts->InitTraversal(); !iterSurfaceCurvepts->IsDoneWithTraversal(); iterSurfaceCurvepts->GoToNextItem()) {
            vtkDataObject* dso = iterSurfaceCurvepts->GetCurrentDataObject();
            vtkPolyData* pd = dynamic_cast<vtkPolyData*>(dso);
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    textItem += QString::number(pd->GetPoint(i)[j]) + ",";
                }
                data << textItem.join("") << ENDL;
                textItem.clear();
            }
        }

        textItem += QString("3D Mesh Points") + ",";
        textItem += QString::number(m_templatePoly->GetNumberOfPoints());
        data << textItem.join("") << ENDL;
        textItem.clear();
        for (int i = 0; i < m_templatePoly->GetNumberOfPoints(); i++) {
            for (int j = 0; j < 3; j++) {
                textItem += QString::number(m_templatePoly->GetPoint(i)[j]) + ",";
            }
            data << textItem.join("") << ENDL;
            textItem.clear();
        }
        textItem += QString("Mesh Connectivity") + ",";
        textItem += QString::number(m_templatePoly->GetNumberOfCells());
        data << textItem.join("") << ENDL;
        textItem.clear();
        for (int i = 0; i < m_templatePoly->GetNumberOfCells(); i++) {
            int numPts = m_templatePoly->GetCell(i)->GetNumberOfPoints();
            for (int j = 0; j < numPts; j++) {
                textItem += QString::number(m_templatePoly->GetCell(i)->GetPointId(j)) + ",";
            }
            data << textItem.join("") << ENDL;
            textItem.clear();
        }

        textItem += QString("Mask Array") + ",";
        textItem += QString::number(m_templatePoly->GetNumberOfCells());
        data << textItem.join("") << ENDL;
        textItem.clear();

        // Get the Masked array
        vtkIntArray* maskArray = vtkIntArray::SafeDownCast(
        m_templatePoly->GetCellData()->GetArray("Masked"));
        for (vtkIdType i = 0; i < m_templatePoly->GetNumberOfCells(); ++i) {
            int maskValue = maskArray->GetValue(i);
            textItem += QString::number(maskValue) + ",";
            data << textItem.join("") << ENDL;
            textItem.clear();
        }

        f.close();
    }
}

void ProSetMenu::ImportTemplate() {
    QString fileName = QFileDialog::getOpenFileName(this, "ToolBox Template Files", QDir::homePath()
        , "ATT Files (*.att);;All Files (*)");
    if (fileName.isEmpty()) {}
    else {
        if (fileName.endsWith(".att")) {
            loadTemplateButton->setEnabled(0);
            surfaceLineEditNOS->setReadOnly(true);
            surfaceLineEditPatchUNOS->setReadOnly(true);
            surfaceLineEditPatchVNOS->setReadOnly(true);
            surfaceLineEditPatchNOP->setReadOnly(true);
            typeILineEdit->setReadOnly(true);
            curveLineEditNOS->setReadOnly(true);
            curveLineEditNOC->setReadOnly(true);
            surfaceComboBox->setEnabled(false);
            vtkNew<vtkPoints> tempImportedPts;
            vtkNew<vtkCellArray> tempImportedPolyCells;
            std::string fname = fileName.toStdString();
            std::vector<std::vector<string>> content;
            std::vector<std::string> row;
            std::string line, word;
            fstream file(fname, ios::in);
            if (file.is_open()) {
                while (getline(file, line)) {
                    row.clear();
                    std::stringstream str(line);
                    while (getline(str, word, ',')) {
                        row.push_back(word);
                    }
                    content.push_back(row);
                }
            }
            else {
                std::cout << "Could not open the file\n";
            }

            for (int i = 0; i < content.size(); i++) {
                if (content[i][0] == "Number of Fixed Landmarks") {
                    typeILineEdit->setText(QString::fromUtf8(content[i][1].c_str()));
                    typeILineEdit->isModified();
                }
                if (content[i][0] == "Number of Curves") {
                    curveLineEditNOC->setText(QString::fromUtf8(content[i][1].c_str()));
                    curveLineEditNOC->isModified();
                }
                if (content[i][0] == "Number of Curve Semi-Landmarks") {
                    curveLineEditNOS->setText(QString::fromUtf8(content[i][1].c_str()));
                    curveLineEditNOS->isModified();
                }
                if (content[i][0] == "Number of Surface Sliders") {
                    surfaceLineEditNOS->setText(QString::fromUtf8(content[i][1].c_str()));
                    surfaceLineEditNOS->isModified();
                }
                if (content[i][0] == "Ignore Islands") {
                    m_ignoreInside = std::stoi(content[i][1]);
                }
                if (surfaceNOS == 0) {
                    if (content[i][0] == "Number of Surface Patches") {
                        surfaceLineEditPatchNOP->setText(QString::fromUtf8(content[i][1].c_str()));
                        surfaceLineEditPatchNOP->isModified();
                    }
                    if (content[i][0] == "Surface Patch U") {
                        surfaceLineEditPatchUNOS->setText(QString::fromUtf8(content[i][1].c_str()));
                        surfaceLineEditPatchUNOS->isModified();
                    }
                    if (content[i][0] == "Surface Patch V") {
                        surfaceLineEditPatchVNOS->setText(QString::fromUtf8(content[i][1].c_str()));
                        surfaceLineEditPatchVNOS->isModified();
                    }
                    if (surfacePatchUNOS != 0 && surfacePatchVNOS != 0) {
                        surfaceComboBox->setCurrentIndex(1);
                        surfaceLineEditPatchUNOS->setReadOnly(true);
                        surfaceLineEditPatchVNOS->setReadOnly(true);
                        surfaceLineEditPatchNOP->setReadOnly(true);
                    }
                }
                if (content[i][0] == "3D Mesh Points") {
                    tempImportedPts->SetNumberOfPoints(std::stoi(content[i][1]));
                    int startPt = i + 1;
                    for (int j = 0; j < std::stoi(content[i][1]); j++) {
                        tempImportedPts->SetPoint(j, std::stod(content[startPt + j][0]), std::stod(content[startPt + j][1]), std::stod(content[startPt + j][2]));
                    }
                }
                if (content[i][0] == "Mesh Connectivity") {
                    int startCell = i + 1;
                    for (int j = 0; j < std::stoi(content[i][1]); j++) {
                        int dim = content[startCell + j].size();
                        vtkNew<vtkPolygon> tempCell;
                        tempCell->GetPointIds()->SetNumberOfIds(dim);
                        for (int k = 0; k < dim; k++) {
                            tempCell->GetPointIds()->SetId(k, std::stoi(content[startCell + j][k]));
                        }
                        tempImportedPolyCells->InsertNextCell(tempCell);
                    }
                }
                m_templatePoly->SetPoints(tempImportedPts);
                m_templatePoly->SetPolys(tempImportedPolyCells);
                m_templatePoly->Modified();

                if (content[i][0] == "Mask Array") {
                    int numCells = std::stoi(content[i][1]);  // Number of cell values
                    int startIdx = i + 1;

                    vtkNew<vtkIntArray> maskArray;
                    maskArray->SetName("Masked");
                    maskArray->SetNumberOfComponents(1);
                    maskArray->SetNumberOfTuples(numCells);

                    for (int j = 0; j < numCells; ++j) {
                        int val = std::stoi(content[startIdx + j][0]);
                        maskArray->SetTuple1(j, val);
                    }

                    m_templatePoly->GetCellData()->AddArray(maskArray);
                    m_templatePoly->GetCellData()->SetActiveScalars("Masked");
                    break;
                }

                if (content[i][0] == "Curve Direction Glyph") {
                    if (std::stoi(content[i][2]) != 0) {
                        int startCell = i + 1;
                        for (int j = 0; j < std::stoi(content[i][1]); j++) {
                            vtkNew<vtkPoints> tempArrowPts;
                            vtkNew<vtkPolyData> tempArrowPoly;
                            for (int k = 0; k < 3; k++) {
                                int ptID = startCell + (j * 3) + k;
                                tempArrowPts->InsertNextPoint(std::stod(content[ptID][0]),
                                    std::stod(content[ptID][1]), std::stod(content[ptID][2]));
                            }
                            tempArrowPoly->SetPoints(tempArrowPts);
                            tempArrowPoly->Modified();
                            m_templateCurvePtsPoly->SetBlock(j, tempArrowPoly);
                            m_templateCurvePtsPoly->Modified();
                        }
                    }
                }
                if (content[i][0] == "Surface Patch Direction Glyph") {
                    if (std::stoi(content[i][2]) != 0) {
                        int startCell = i + 1;
                        for (int j = 0; j < std::stoi(content[i][1]); j++) {
                            vtkNew<vtkPoints> tempArrowPts;
                            vtkNew<vtkPolyData> tempArrowPoly;
                            for (int k = 0; k < 3; k++) {
                                int ptID = startCell + (j * 3) + k;
                                tempArrowPts->InsertNextPoint(std::stod(content[ptID][0]),
                                    std::stod(content[ptID][1]), std::stod(content[ptID][2]));
                            }
                            tempArrowPoly->SetPoints(tempArrowPts);
                            tempArrowPoly->Modified();
                            m_templatePatchSurfaceCurve->SetBlock(j, tempArrowPoly);
                            m_templatePatchSurfaceCurve->Modified();
                        }
                    }
                }
                if (content[i][0] == "Landmarks") {
                    int startCell = i + 1;
                    int numCurveSliders = 0;
                    int numSurfaceSliders = 0;
                    m_templateTypeI->Initialize();
                    m_templateSurfaceSliders->Initialize();
                    for (int j = 0; j < typeINOL; j++) {
                        m_templateTypeI->InsertNextPoint(std::stod(content[startCell + j][0]),
                            std::stod(content[startCell + j][1]), std::stod(content[startCell + j][2]));
                    }
                    m_templateTypeI->Modified();
                    if (curveNOS != 0) {
                        numCurveSliders = curveNOS * curveNOC;
                        int start = startCell + typeINOL;
                        for (int j = 0; j < curveNOC; j++) {
                            vtkNew<vtkPoints> sliderPts;
                            vtkNew<vtkPolyData> sliderPtsPoly;
                            for (int k = 0; k < curveNOS; k++) {
                                int curvePtID = start + (j * curveNOS) + k;
                                sliderPts->InsertNextPoint(std::stod(content[curvePtID][0]),
                                    std::stod(content[curvePtID][1]), std::stod(content[curvePtID][2]));
                            }
                            sliderPtsPoly->SetPoints(sliderPts);
                            m_templateCurveSliders->SetBlock(j, sliderPtsPoly);
                            m_templateCurveSliders->Modified();
                        }
                    }
                    if (surfacePatchUNOS == 0 && surfacePatchVNOS == 0 && surfaceNOS != 0) {
                        int start = startCell + typeINOL + numCurveSliders;
                        for (int j = 0; j < surfaceNOS; j++) {
                            m_templateSurfaceSliders->InsertNextPoint(std::stod(content[start + j][0]),
                                std::stod(content[start + j][1]), std::stod(content[start + j][2]));
                        }
                    }
                    if (surfacePatchUNOS != 0 && surfacePatchVNOS != 0 && surfaceNOS == 0) {
                        int start = startCell + typeINOL + numCurveSliders;
                        for (int j = 0; j < surfacePatchNOP; j++) {
                            vtkNew<vtkPoints> sliderPts;
                            vtkNew<vtkPolyData> sliderPtsPoly;
                            for (int k = 0; k < (surfacePatchUNOS * surfacePatchVNOS); k++) {
                                int surfacePtID = start + (j * (surfacePatchUNOS * surfacePatchVNOS)) + k;
                                sliderPts->InsertNextPoint(std::stod(content[surfacePtID][0]),
                                    std::stod(content[surfacePtID][1]), std::stod(content[surfacePtID][2]));
                            }
                            sliderPtsPoly->SetPoints(sliderPts);
                            m_templatePatchSurfaceSliders->SetBlock(j, sliderPtsPoly);
                            m_templatePatchSurfaceSliders->Modified();
                        }
                    }
                }
            }
            Refresh(1);
        }
        if (fileName.endsWith(".att") == 0) {
            auto errorDialogue = QMessageBox();
            errorDialogue.setIcon(QMessageBox::Critical);
            errorDialogue.setWindowTitle("Error");
            errorDialogue.setText("No Suitable file was Selected");
            errorDialogue.exec();
        }
    }
}

void ProSetMenu::ChangeSurfaceMode(int index) {
    switch (index)
    {
    case 0:
        surfaceLayout->removeWidget(surfacePatchLabel);
        surfaceLayout->removeWidget(surfaceLineEditPatchNOP);
        surfaceLayout->removeWidget(resolutionGroup);
        surfaceLayout->removeWidget(surfaceResolutionLabel);
        surfaceResolutionLabel->setVisible(false);
        surfacePatchLabel->setVisible(false);
        surfaceLineEditPatchNOP->setVisible(false);
        resolutionGroup->setVisible(false);
        surfaceNOSLabel->setVisible(true);
        surfaceLineEditNOS->setVisible(true);
        surfaceLayout->addWidget(surfaceNOSLabel, 1, 0);
        surfaceLayout->addWidget(surfaceLineEditNOS, 1, 1);
        surfaceLineEditNOS->setVisible(true);
        surfaceLineEditPatchNOP->clear();
        surfaceLineEditPatchUNOS->clear();
        surfaceLineEditPatchVNOS->clear();
        surfacePatchNOP = 1;
        surfacePatchUNOS = 0;
        surfacePatchVNOS = 0;
        break;
    case 1:
        surfaceLayout->removeWidget(surfaceLineEditNOS);
        surfaceLayout->removeWidget(surfaceNOSLabel);
        surfaceNOSLabel->setVisible(false);
        surfaceLineEditNOS->setVisible(false);
        surfaceResolutionLabel->setVisible(true);
        surfacePatchLabel->setVisible(true);
        surfaceLineEditPatchNOP->setVisible(true);
        resolutionGroup->setVisible(true);
        surfaceLayout->addWidget(surfacePatchLabel, 2, 0);
        surfaceLayout->addWidget(surfaceLineEditPatchNOP, 2, 1);
        surfaceLayout->addWidget(surfaceResolutionLabel, 1, 0);
        surfaceLayout->addWidget(resolutionGroup, 1, 1);
        surfaceLineEditNOS->clear();
        surfaceNOS = 0;
        break;
    }
}

void ProSetMenu::LoadTemplate() {
    if (surfaceNOS == 0 && typeINOL == 0 && curveNOS == 0 && surfacePatchUNOS == 0 && surfacePatchVNOS == 0) {
        auto errorDialogue = QMessageBox();
        errorDialogue.setIcon(QMessageBox::Critical);
        errorDialogue.setWindowTitle("Error");
        errorDialogue.setText("The project setting is empty");
        errorDialogue.exec();
    }
    else if (surfaceNOS != 0 && surfaceNOS < surfaceSliderLimit) {
        //surfaceLineEditNOS->clear();
        auto errorDialogue = QMessageBox();
        errorDialogue.setIcon(QMessageBox::Critical);
        errorDialogue.setWindowTitle("Error");
        errorDialogue.setText("Number of the surface sliders is less than the limit. Try again");
        errorDialogue.exec();
    }
    else if (surfacePatchUNOS != 0 && (surfacePatchUNOS < surfaceResolutionLimit || surfacePatchVNOS < surfaceResolutionLimit)) {
        auto errorDialogue = QMessageBox();
        errorDialogue.setIcon(QMessageBox::Critical);
        errorDialogue.setWindowTitle("Error");
        errorDialogue.setText("The resolution is less than the limit. Try again");
        errorDialogue.exec();
    }
    else if (surfacePatchVNOS != 0 && surfacePatchVNOS < surfaceResolutionLimit) {
        auto errorDialogue = QMessageBox();
        errorDialogue.setIcon(QMessageBox::Critical);
        errorDialogue.setWindowTitle("Error");
        errorDialogue.setText("The resolution is less than the limit. Try again");
        errorDialogue.exec();
    }
    else if (typeINOL != 0 && typeINOL < fixedLandmarkLimit) {
        //typeILineEdit->clear();
        auto errorDialogue = QMessageBox();
        errorDialogue.setIcon(QMessageBox::Critical);
        errorDialogue.setWindowTitle("Error");
        errorDialogue.setText("Number of the fixed landmarks is less than the limit. Try again");
        errorDialogue.exec();
    }
    else if (curveNOS != 0 && curveNOS < curveSliderLimit) {
        //curveLineEditNOS->clear();
        auto errorDialogue = QMessageBox();
        errorDialogue.setIcon(QMessageBox::Critical);
        errorDialogue.setWindowTitle("Error");
        errorDialogue.setText("Number of the curve sliders is less than the limit. Try again");
        errorDialogue.exec();
    }
    else {
        QString fileName = QFileDialog::getOpenFileName(this, "OBJ Files", QDir::homePath()
            , "OBJ Files (*.obj);;All Files (*)");
        if (fileName.isEmpty()) {}
        else {
            if (fileName.endsWith(".obj")) {
                vtkSmartPointer<vtkOBJReader> objReader
                    = vtkSmartPointer<vtkOBJReader>::New();
                objReader->SetFileName(fileName.toLocal8Bit().data());

                objReader->Update();
                vtkSmartPointer<vtkCleanPolyData> cleanFilter =
                    vtkSmartPointer<vtkCleanPolyData>::New();
                cleanFilter->SetInputData(objReader->GetOutput());
                cleanFilter->Update();
                m_templatePoly = cleanFilter->GetOutput();
                loadTemplateButton->setEnabled(false);
                importButton->setEnabled(false);
                registerButton->setEnabled(false);
                saveButton->setEnabled(false);
                resetButton->setEnabled(false);
                surfaceComboBox->setEnabled(false);
                surfaceLineEditNOS->setReadOnly(true);
                surfaceLineEditPatchUNOS->setReadOnly(true);
                surfaceLineEditPatchVNOS->setReadOnly(true);
                surfaceLineEditPatchNOP->setReadOnly(true);
                typeILineEdit->setReadOnly(true);
                curveLineEditNOS->setReadOnly(true);
                curveLineEditNOC->setReadOnly(true);
                m_templatePlot->SetPoly(m_templatePoly);
                m_templatePlot->show();
            }
            if (fileName.endsWith(".obj") == 0) {
                auto errorDialogue = QMessageBox();
                errorDialogue.setIcon(QMessageBox::Critical);
                errorDialogue.setWindowTitle("Error");
                errorDialogue.setText("No Suitable file was Selected");
                errorDialogue.exec();
            }
        }
    }
}

void ProSetMenu::SetSurfaceNOS() {
    auto textNum = surfaceLineEditNOS->text().toStdString();
    int num = -1;
    try
    {
        num = stoi(textNum);
    }
    catch (const std::exception& e)
    {
        num = -1;
        surfaceNOS = 0;
        surfaceLineEditPatchUNOS->setReadOnly(false);
        surfaceLineEditPatchVNOS->setReadOnly(false);
        surfaceLineEditPatchNOP->setReadOnly(false);
    }
    if (num >= 0) {
        surfaceNOS = num;
        surfaceLineEditPatchUNOS->setReadOnly(true);
        surfaceLineEditPatchVNOS->setReadOnly(true);
        surfaceLineEditPatchNOP->setReadOnly(true);
    }
    else {
        surfaceLineEditNOS->clear();
        surfaceNOS = 0;
        surfaceLineEditPatchUNOS->setReadOnly(false);
        surfaceLineEditPatchVNOS->setReadOnly(false);
        surfaceLineEditPatchNOP->setReadOnly(false);
    }
}

int ProSetMenu::GetSurfaceNOS() {
    return surfaceNOS;
}

void ProSetMenu::SetSurfacePatchUNOS() {
    auto textNum = surfaceLineEditPatchUNOS->text().toStdString();
    int num = -1;
    try
    {
        num = stoi(textNum);
    }
    catch (const std::exception& e)
    {
        num = -1;
        surfacePatchUNOS = 0;
        surfaceLineEditNOS->setReadOnly(false);
    }
    if (num >= 0) {
        surfacePatchUNOS = num;
        surfaceLineEditNOS->setReadOnly(true);
    }
    else {
        surfaceLineEditPatchUNOS->clear();
        surfacePatchUNOS = 0;
        surfaceLineEditNOS->setReadOnly(false);
    }
}

void ProSetMenu::SetSurfacePatchVNOS() {
    auto textNum = surfaceLineEditPatchVNOS->text().toStdString();
    int num = -1;
    try
    {
        num = stoi(textNum);
    }
    catch (const std::exception& e)
    {
        num = -1;
        surfacePatchVNOS = 0;
        surfaceLineEditNOS->setReadOnly(false);
    }
    if (num >= 0) {
        surfacePatchVNOS = num;
        surfaceLineEditNOS->setReadOnly(true);
    }
    else {
        surfaceLineEditPatchVNOS->clear();
        surfacePatchVNOS = 0;
        surfaceLineEditNOS->setReadOnly(false);
    }
}

std::tuple<int, int> ProSetMenu::GetSurfacePatchResolution() {
    return std::make_tuple(surfacePatchUNOS, surfacePatchVNOS);
}

void ProSetMenu::SetSurfacePatchNOP() {
    auto textNum = surfaceLineEditPatchNOP->text().toStdString();
    int num = -1;
    try
    {
        num = stoi(textNum);
    }
    catch (const std::exception& e)
    {
        num = -1;
        surfacePatchNOP = 1;
        surfaceLineEditNOS->setReadOnly(false);
    }
    if (num >= 0) {
        surfacePatchNOP = num;
        surfaceLineEditNOS->setReadOnly(true);
    }
    else {
        surfaceLineEditPatchNOP->clear();
        surfacePatchNOP = 1;
        surfaceLineEditNOS->setReadOnly(false);
    }
}

int ProSetMenu::GetSurfacePatchNOP() {
    return surfacePatchNOP;
}

void ProSetMenu::SetCurveNOS() {
    std::string textNum = curveLineEditNOS->text().toStdString();
    int num = -1;
    try
    {
        num = stoi(textNum);
    }
    catch (const std::exception& e)
    {
        num = -1;
        curveNOS = 0;
    }
    if (num >= 0) {
        curveNOS = num;
    }
    else {
        curveLineEditNOS->clear();
        curveNOS = 0;
    }
}

void ProSetMenu::SetCurveNOC() {
    std::string textNum = curveLineEditNOC->text().toStdString();
    int num = -1;
    try
    {
        num = stoi(textNum);
    }
    catch (const std::exception& e)
    {
        num = -1;
        curveNOC = 1;
    }
    if (num > 0) {
        curveNOC = num;
    }
    else {
        curveLineEditNOC->clear();
        curveNOC = 1;
    }
}

int ProSetMenu::GetCurveNOS() {
    return curveNOS;
}

int ProSetMenu::GetCurveNOC() {
    return curveNOC;
}

void ProSetMenu::SetTemplateCurveSliders(vtkMultiBlockDataSet* sliderPtsPolyBlock, vtkMultiBlockDataSet* curvePtsPolyBlock) {
    m_templateCurveSliders = sliderPtsPolyBlock;
    m_templateCurvePtsPoly = curvePtsPolyBlock;
}

void ProSetMenu::SetTemplateSurfaceSliders(vtkPoints* points) {
    m_templateSurfaceSliders = points;
}

void ProSetMenu::SetTemplatePatchSurfaceSliders(vtkMultiBlockDataSet* surfaceptsPolyBlock, vtkMultiBlockDataSet* surfacePatchCurve) {
    m_templatePatchSurfaceSliders = surfaceptsPolyBlock;
    m_templatePatchSurfaceCurve = surfacePatchCurve;
}

void ProSetMenu::Refresh(bool condition) {
    if (condition == 1) {
        registerButton->setEnabled(true);
        saveButton->setEnabled(true);
    }
    resetButton->setEnabled(true);
    //loadTemplateButton->setEnabled(true);
}

void ProSetMenu::SetIgnorInternals(bool option) {
    m_ignoreInside = option;
}

bool ProSetMenu::GetIgnorInternals() {
    return m_ignoreInside;
}

ProSetMenu::~ProSetMenu() {
    delete layout;
    delete m_templatePlot;
}
cmake_minimum_required(VERSION 3.16)

project("Archaeotoolbox")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Debug or Release
set(CMAKE_BUILD_TYPE Debug)

# Common source files
set(SOURCE_FILES
    main.cpp
    src/MainWindow.cpp src/SpecimenDigitiser.cpp src/DataBase.cpp src/ProSetMenu.cpp
    src/TemplateDigitiser.cpp src/Morphometrics.cpp src/Registration.cpp
    src/TemplateViewer.cpp src/SuperImpositionThread.cpp src/SuperImposition.cpp
    src/SlidingThread.cpp src/StatusReporterThread.cpp src/PCAWindow.cpp
    src/RecoverLMThread.cpp src/ExportDialogue.cpp src/RegistrationThread.cpp
    src/BlueNoiseThread.cpp src/HelpBrowser.cpp src/ImportThread.cpp src/ExclusionPaint.cpp

    include/MainWindow.h include/SpecimenDigitiser.h include/DataBase.h
    include/MainWindow.fwd.h include/ProSetMenu.h include/ProSetMenu.fwd.h
    include/TemplateDigitiser.h include/Morphometrics.h include/Registration.h
    include/TemplateViewer.h include/SpecimenDigitiser.fwd.h
    include/SuperImpositionThread.h include/SuperImposition.h include/SlidingThread.h
    include/StatusReporterThread.h include/PCAWindow.h include/RecoverLMThread.h
    include/ExportDialogue.h include/RegistrationThread.h include/BlueNoiseThread.h
    include/HelpBrowser.h include/ImportThread.h include/ExclusionPaint.h

    Resources/resources.qrc
)

# Windows-specific
if(WIN32)
    message(STATUS "Configuring for Windows")
    set(WIN32_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Resources/icon.rc)
    list(APPEND SOURCE_FILES ${WIN32_RESOURCES})

    set(OpenCASCADE_DIR "C:/Program Files/OCCT/cmake")
    set(Cpd_DIR "C:/Repo/cpd/install/lib/cmake/cpd")

    include_directories(
        ./include
        "C:/Repo/eigen3"
        "C:/Program Files/VTK8/include/vtk-8.2"
        "C:/Program Files/OCCT/inc"
        "C:/Qt/5.15.2/msvc2019_64/include"
        "C:/Program Files/cpd/include"
    )
endif()

# Linux-specific
if(UNIX)
    message(STATUS "Configuring for Linux")
    set(OpenCASCADE_DIR /usr/local/lib/cmake/opencascade)

    include_directories(
        ./include
        ./include/vcglib
        /usr/include/eigen3
        /usr/local/include/vtk-8.2
        /usr/local/include/opencascade
    )
endif()

# Find packages (shared across both platforms)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui Charts Test Help)
find_package(Cpd REQUIRED)
find_package(OpenCASCADE REQUIRED)
find_package(VTK REQUIRED COMPONENTS
    vtkCommonColor
    vtkCommonCore
    vtkGUISupportQt
    vtkCommonDataModel
    vtkCommonMisc
    vtkIOImage
    vtkIOGeometry
    vtkFiltersCore
    vtkFiltersPoints
    vtkFiltersModeling
    vtkFiltersSources
    vtkFiltersVerdict
    vtkFiltersGeneral
    vtkCommonComputationalGeometry
    vtkIOLegacy
    vtkIOPLY
    vtkIOXML
    vtkInteractionStyle
    vtkInteractionWidgets
    vtkRenderingContextOpenGL2
    vtkRenderingCore
    vtkRenderingFreeType
    vtkRenderingGL2PSOpenGL2
    vtkRenderingOpenGL2
    vtkRenderingLabel
    vtkImagingHybrid
    vtkViewsInfovis
    vtkViewsContext2D
    vtkRenderingContext2D
    vtkRenderingVolumeOpenGL2
)

# Build target
add_executable("${PROJECT_NAME}" ${SOURCE_FILES})

# Link libraries
target_link_libraries("${PROJECT_NAME}"
    ${VTK_LIBRARIES}
    ${PCL_LIBRARIES}
    Qt5::Widgets Qt5::Gui Qt5::Core Qt5::Charts Qt5::Help Qt5::Test
    Cpd::Library-C++ TKIVtk TKIGES TKSTEP TKFeat TKOffset 
    TKShHealing
)
cmake_minimum_required(VERSION 3.16)

project("Archaeotoolbox")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Debug or Release
set(CMAKE_BUILD_TYPE Debug)

# Source files
set(SOURCE_FILES
    main.cpp
    src/MainWindow.cpp src/SpecimenDigitiser.cpp src/DataBase.cpp src/ProjectSetMenu.cpp
    src/TemplateDigitiser.cpp src/Registration.cpp
    src/TemplateViewer.cpp src/SuperImpositionThread.cpp src/SuperImposition.cpp
    src/SlidingThread.cpp src/StatusReporterThread.cpp src/PCAWindow.cpp
    src/RecoverLMThread.cpp src/ExportDialogue.cpp src/RegistrationThread.cpp
    src/BlueNoiseThread.cpp src/HelpBrowser.cpp src/ImportThread.cpp src/ExclusionPaint.cpp 
    src/CPD3D.cpp src/DecimationDialog.cpp src/CuttingThread.cpp src/HarmonicParameteriser.cpp
    src/SurfaceParameterisationThread.cpp src/LoadMeshThread.cpp src/UpdateContributionThread.cpp

    include/MainWindow.h include/SpecimenDigitiser.h include/DataBase.h
    include/MainWindow.fwd.h include/ProjectSetMenu.h include/ProjectSetMenu.fwd.h
    include/TemplateDigitiser.h include/Registration.h
    include/TemplateViewer.h include/SpecimenDigitiser.fwd.h
    include/SuperImpositionThread.h include/SuperImposition.h include/SlidingThread.h
    include/StatusReporterThread.h include/PCAWindow.h include/RecoverLMThread.h
    include/ExportDialogue.h include/RegistrationThread.h include/BlueNoiseThread.h
    include/HelpBrowser.h include/ImportThread.h include/ExclusionPaint.h include/CPD3D.h
    include/DecimationDialog.h include/VCGDefinitions.h include/DecimateThread.h
    include/SpinnerDialog.h include/CuttingThread.h include/HarmonicParameteriser.h
    include/SurfaceParameterisationThread.h include/LoadMeshThread.h
    include/UpdateContributionThread.h

    Resources/resources.qrc
)

# Windows-specific
if(WIN32)
    message(STATUS "Configuring for Windows")

    set(WIN32_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Resources/icon.rc)
    list(APPEND SOURCE_FILES ${WIN32_RESOURCES})

    include_directories(
        ./include
        ./include/vcglib
        ./include/eigen3
        "C:/Program Files/VTK8/include/vtk-8.2"
        "C:/Qt/5.15.2/msvc2019_64/include"
    )
endif()

# Linux-specific
if(UNIX)
    message(STATUS "Configuring for Linux")
    
    include_directories(
        ./include
        ./include/vcglib
        ./include/eigen3
        /usr/local/include/vtk-8.2
    )
endif()

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui Charts Test Help)
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
    vtkIOExport
    vtkIOExportOpenGL2
)

# Add executable
if(WIN32 AND NOT WITH_CONSOLE)
    add_executable(${PROJECT_NAME} WIN32 ${SOURCE_FILES})
else()
    add_executable(${PROJECT_NAME} ${SOURCE_FILES})
endif()

# Find and enable OpenMP
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    message(STATUS "OpenMP found and enabled")
    target_compile_options(${PROJECT_NAME} PUBLIC
        $<$<CXX_COMPILER_ID:MSVC>:/openmp>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fopenmp>)
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX)
else()
    message(WARNING "OpenMP not found. Parallelization disabled")
endif()

# Link libraries
target_link_libraries(${PROJECT_NAME} PUBLIC
    ${VTK_LIBRARIES}
    Qt5::Widgets Qt5::Gui Qt5::Core Qt5::Charts Qt5::Help Qt5::Test
)

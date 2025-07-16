cmake_minimum_required(VERSION 3.16)
project("Archaeotoolbox")

set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 17)
set(OPENCASCADE_DIR /usr/local/lib/cmake/opencascade)

set(SOURCE_FILES main.cpp src/MainWindow.cpp src/SpecimenDigitiser.cpp src/DataBase.cpp src/ProSetMenu.cpp src/TemplateDigitiser.cpp src/Morphometrics.cpp 
src/Registration.cpp src/TemplateViewer.cpp src/SuperImpositionThread.cpp src/SuperImposition.cpp src/SlidingThread.cpp
src/StatusReporterThread.cpp src/PCAWindow.cpp  src/RecoverLMThread.cpp src/ExportDialogue.cpp src/RegistrationThread.cpp
src/BlueNoiseThread.cpp src/HelpBrowser.cpp src/ImportThread.cpp src/ExclusionPaint.cpp
include/MainWindow.h include/SpecimenDigitiser.h include/DataBase.h include/MainWindow.fwd.h include/ProSetMenu.h include/ProSetMenu.fwd.h
include/TemplateDigitiser.h include/Morphometrics.h include/Registration.h include/TemplateViewer.h include/SpecimenDigitiser.fwd.h
include/SuperImpositionThread.h include/SuperImposition.h include/SlidingThread.h include/StatusReporterThread.h include/PCAWindow.h 
include/RecoverLMThread.h include/ExportDialogue.h include/RegistrationThread.h include/BlueNoiseThread.h include/HelpBrowser.h
include/ImportThread.h include/ExclusionPaint.h
Resources/resources.qrc)

#find_package(OpenMP REQUIRED)

#set(Qt5_DIR /home/kaveh/Software/QT5/lib/cmake/Qt5)
#set(Qt5_INCLUDE_DIR /home/kaveh/Software/QT5/include)
#set(Qt5_LIB_DIR /home/kaveh/Software/QT5/lib)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui Charts Test Help)
find_package(OpenCASCADE REQUIRED)
find_package(Cpd REQUIRED)
find_package(VTK REQUIRED COMPONENTS vtkCommonColor
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
  vtkIOGeometry
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
  vtkRenderingVolumeOpenGL2)


include_directories(./include)
include_directories(./include/cyCode)
#include_directories(./include/CPD)
include_directories(./include/vcglib)
#include_directories(./include/cpd)
#include_directories(./include/cppoptlib)
#include_directories(/usr/local/include/pcl-1.11)
include_directories(/usr/include/eigen3)
#include_directories(/usr/local/include/vtk-9.1)
include_directories(/usr/local/include/vtk-8.2)
#include_directories(/usr/include/vtk)
include_directories(/usr/local/include/opencascade)
#include_directories(/home/kaveh/Software/QT5/include)
#include_directories(/home/kaveh/Software/libigl2.4/include)
include_directories(/usr/local/include/cpd)


#link_directories(/home/kaveh/Software/libigl2.4/build)



add_executable("${PROJECT_NAME}" "${SOURCE_FILES}")


# Set compiler flags for OpenMP support
#if(OPENMP_FOUND)
    #target_compile_options(${PROJECT_NAME} PRIVATE ${OpenMP_CXX_FLAGS})
#endif()


target_link_libraries("${PROJECT_NAME}" "${PCL_LIBRARIES}" "${VTK_LIBRARIES}"
Qt5::Widgets Qt5::Gui Qt5::Core Qt5::Charts  Qt5::Test  Qt5::Help TKIVtk TKIGES TKSTEP TKFeat TKOffset 
TKShHealing Cpd::Library-C++)

#target_link_libraries("${PROJECT_NAME}" PRIVATE OpenMP::OpenMP_CXX
    #"${PCL_LIBRARIES}" "${VTK_LIBRARIES}"
    #Qt5::Widgets Qt5::Gui Qt5::Core Qt5::Charts Qt5::Help Qt5::Test  TKIVtk TKIGES TKSTEP TKFeat TKOffset 
    #TKShHealing Cpd::Library-C++)

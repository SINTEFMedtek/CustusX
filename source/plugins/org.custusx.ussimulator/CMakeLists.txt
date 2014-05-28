project(org_custusx_ussimulator)

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

option(CX_BUILD_US_SIMULATOR "Build the US simulator (US from CT/MR)" OFF)

include_directories(
    #${CX_RESOURCE_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_WIDGETS_INCLUDE_DIRECTORIES}
    ${CustusX3_SOURCE_DIR}/source/modules/GUIExtender
    ${CustusX3_SOURCE_DIR}/source/service/video
    ${CustusX3_SOURCE_DIR}/source/resource/videoServer
    ${CustusX3_SOURCE_DIR}/source/resource/OpenIGTLinkUtilities
    ${CustusX3_SOURCE_DIR}/source/gui/dataAdapters
    ${CustusX3_SOURCE_DIR}/source/service/tracking
    ${CustusX3_SOURCE_DIR}/source/logic
    )

set(PLUGIN_SRCS
  cxUSSimulatorPluginActivator.cpp
  cxUSSImulatorGUIExtenderService.cpp
  cxSimulateUSWidget.cpp
  cxSimulatedImageStreamerInterface.cpp
  cxSimulatedImageStreamerInterface.h
  cxSimulatedImageStreamer.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
  cxUSSimulatorPluginActivator.h
  cxUSSImulatorGUIExtenderService.h
  cxSimulateUSWidget.h
  cxSimulatedImageStreamer.h
)

# Build all files from the UltrasoundSimulation lib
IF(CX_BUILD_US_SIMULATOR)
  set(SIMULATOR_SPECKLE_PATH ${CustusX3_SOURCE_DIR}/../../UltrasoundSimulation/UltrasoundSimulation)

  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/simConfig.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/simConfig.h"
  )
  set(PLUGIN_SRCS
    ${PLUGIN_SRCS}
    ${CustusX3_SOURCE_DIR}/../../UltrasoundSimulation/UltrasoundSimulation/ImageSimulator.h
    ${CustusX3_SOURCE_DIR}/../../UltrasoundSimulation/UltrasoundSimulation/ImageSimulator.cpp
    ${CustusX3_SOURCE_DIR}/../../UltrasoundSimulation/UltrasoundSimulation/ImageArray.h
    ${CustusX3_SOURCE_DIR}/../../UltrasoundSimulation/UltrasoundSimulation/ImageArray.cpp
)
ENDIF(CX_BUILD_US_SIMULATOR)

# Qt Designer files which should be processed by Qts uic
set(PLUGIN_UI_FORMS
)

# QRC Files which should be compiled into the plugin
set(PLUGIN_resources
)

#Compute the plugin dependencies
ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)
set(PLUGIN_target_libraries 
    ${PLUGIN_target_libraries}
    #${CX_RESOURCE_LIBRARIES}
    cxGUIExtenderService
    vtkRenderingOpenGL
    vtkRenderingFreeTypeOpenGL
    vtkInteractionStyle
    vtkImagingGeneral
    vtkFiltersCore
    vtkIOImage
    vtkImagingMorphological
    vtkFiltersParallel
    vtkFiltersGeneral
    cxResource
    cxResourceWidgets
    cxVideoService
    cxLogicManager
    cxGui
    #cxGrabber #Rename to cxVideoServer?
)

ctkMacroBuildPlugin(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS}
  MOC_SRCS ${PLUGIN_MOC_SRCS}
  UI_FORMS ${PLUGIN_UI_FORMS}
  RESOURCES ${PLUGIN_resources}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
)

add_subdirectory(testing)


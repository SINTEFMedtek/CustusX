###########################################################
##               US Reconstruction Plugin
##
##               Plugin
##
##  Dependencies: Service Layer, 
##                Resource Layer. 
##
###########################################################

SET( MOC_HEADER_FILES
   logic/cxReconstructionManager.h
   logic/cxReconstructThreads.h
   logic/cxReconstructParams.h
   gui/cxReconstructionWidget.h
   gui/cxReconstructOutputValueParamsInterfaces.h
   gui/cxUsReconstructionPlugin.h
    )

QT4_WRAP_CPP( MOC_HEADER_FILES ${MOC_HEADER_FILES} )

# library
SET( SOURCE_FILES
  ${MOC_HEADER_FILES}
  logic/cxReconstructionManager.h
  logic/cxReconstructionManager.cpp
  logic/cxReconstructThreads.h
  logic/cxReconstructThreads.cpp
  logic/cxReconstructParams.h
  logic/cxReconstructParams.cpp
  gui/cxReconstructionWidget.cpp
  gui/cxReconstructOutputValueParamsInterfaces.cpp
  gui/cxReconstructOutputValueParamsInterfaces.h
  gui/cxUsReconstructionPlugin.h
  gui/cxUsReconstructionPlugin.cpp
  )

# =========================================================

include_directories(${CX_RESOURCE_INCLUDE_DIRECTORIES})
include_directories(
    ${CX_RESOURCE_WIDGETS_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_USRECONSTRUCTIONTYPES_INCLUDE_DIRECTORIES}
    .
    ./logic
    ./logic/3DReconstruction
    ./gui
	${CMAKE_CURRENT_BINARY_DIR}/logic/3DReconstruction
    ${CustusX3_SOURCE_DIR}/source/service/patient
    ${CustusX3_SOURCE_DIR}/source/service/tracking
    ${CustusX3_SOURCE_DIR}/source/service/visualization
	${CustusX3_SOURCE_DIR}/source/logic
	${CustusX3_SOURCE_DIR}/source/modules/GUIExtender
    ${CustusX3_SOURCE_DIR}/source/modules/Algorithm/gui
    ${CustusX3_SOURCE_DIR}/source/gui/utilities
    ${CustusX3_SOURCE_DIR}/source/resource/pluginUtilities
    )

SET( cxPluginUSReconstruction_LIB_FILES
    cxResource
    cxResourceWidgets
    sscUS3DReconstruction
    cxResource
    cxPatientService
    cxTrackingService
    cxVisualizationService
    cxPluginAlgorithm
    )

# =========================================================
# create the target
add_library( cxPluginUSReconstruction ${SOURCE_FILES} )
target_link_libraries( cxPluginUSReconstruction ${cxPluginUSReconstruction_LIB_FILES})
CX_COTIRE_ADD_LIBRARY( cxPluginUSReconstruction "${cxPluginUSReconstruction_LIB_FILES}")

add_subdirectory(testing)
add_subdirectory(logic/3DReconstruction)

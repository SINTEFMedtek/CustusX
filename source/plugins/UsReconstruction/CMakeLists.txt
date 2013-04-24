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
  logic/sscReconstructManager.h
  logic/sscReconstructThreads.h
  logic/sscReconstructParams.h
   gui/sscReconstructionWidget.h
   gui/sscReconstructOutputValueParamsInterfaces.h
   gui/cxUsReconstructionPlugin.h
    )

QT4_WRAP_CPP( MOC_HEADER_FILES ${MOC_HEADER_FILES} )

# library
SET( SOURCE_FILES
  ${MOC_HEADER_FILES}
  logic/sscReconstructManager.h
  logic/sscReconstructManager.cpp
  logic/sscReconstructThreads.h
  logic/sscReconstructThreads.cpp
  logic/sscReconstructParams.h
  logic/sscReconstructParams.cpp
  gui/sscReconstructionWidget.cpp
  gui/sscReconstructOutputValueParamsInterfaces.cpp
  gui/sscReconstructOutputValueParamsInterfaces.h
  gui/cxUsReconstructionPlugin.h
  gui/cxUsReconstructionPlugin.cpp
  )

# =========================================================

include_directories(
    .
    ./logic
    ./logic/3DReconstruction
    ./gui
    ${CustusX3_SOURCE_DIR}/source/resource
    ${CustusX3_SOURCE_DIR}/source/service/patient
    ${CustusX3_SOURCE_DIR}/source/service/tracking
    ${CustusX3_SOURCE_DIR}/source/service/visualization
    ${CustusX3_SOURCE_DIR}/source/resource/settings
    ${CustusX3_SOURCE_DIR}/source/resource/utilities
    ${CustusX3_SOURCE_DIR}/source/resource/algorithms
    ${CustusX3_SOURCE_DIR}/source/plugins/PluginBase
    ${CustusX3_SOURCE_DIR}/source/plugins/Algorithm/gui
    ${CustusX3_SOURCE_DIR}/source/gui/utilities
    )

SET( cxPluginUSReconstruction_LIB_FILES
    SSC
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

#add_subdirectory(logic)
#add_subdirectory(gui)
add_subdirectory(testing)
add_subdirectory(logic/3DReconstruction)

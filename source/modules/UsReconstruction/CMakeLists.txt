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
   logic/cxReconstructionExecuter.h
   logic/cxReconstructionService.h
   gui/cxReconstructionWidget.h
   gui/cxReconstructOutputValueParamsInterfaces.h
   gui/cxUsReconstructionPlugin.h
    )

cx_remove_duplicate_include_directories()
QT5_WRAP_CPP( MOC_HEADER_FILES ${MOC_HEADER_FILES} )

# library
SET( SOURCE_FILES
    ${MOC_HEADER_FILES}
    logic/cxReconstructionManager.h
    logic/cxReconstructionManager.cpp
    logic/cxReconstructThreads.h
    logic/cxReconstructThreads.cpp
    logic/cxReconstructParams.h
    logic/cxReconstructParams.cpp
    logic/cxReconstructionExecuter.cpp
    logic/cxReconstructedOutputVolumeParams.h
    logic/cxReconstructedOutputVolumeParams.cpp
    logic/cxReconstructCore.cpp
    logic/cxReconstructCore.h
    logic/cxReconstructPreprocessor.cpp
    logic/cxReconstructPreprocessor.h
    logic/cxSyntheticVolume.h
    logic/cxSyntheticVolume.cpp
    logic/cxSimpleSyntheticVolume.h
    logic/cxSimpleSyntheticVolume.cpp
    gui/cxReconstructionWidget.cpp
    gui/cxReconstructOutputValueParamsInterfaces.cpp
    gui/cxReconstructOutputValueParamsInterfaces.h
    gui/cxUsReconstructionPlugin.h
    gui/cxUsReconstructionPlugin.cpp
  )
  

# =========================================================
if (CX_USE_OPENCL_UTILITY)
	include_directories(
		${OPENCL_INCLUDE_DIRS}
		)
endif (CX_USE_OPENCL_UTILITY)

include_directories(
    ${CX_RESOURCE_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_WIDGETS_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_USRECONSTRUCTIONTYPES_INCLUDE_DIRECTORIES}
    .
    ./logic
    ./logic/3DReconstruction
    ./gui
    ${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/logic
    ${CustusX_SOURCE_DIR}/source/service/patient
    ${CustusX_SOURCE_DIR}/source/service/tracking
    ${CustusX_SOURCE_DIR}/source/service/visualization
	${CustusX_SOURCE_DIR}/source/logic
	${CustusX_SOURCE_DIR}/source/modules/GUIExtender
    ${CustusX_SOURCE_DIR}/source/modules/Algorithm/gui
    ${CustusX_SOURCE_DIR}/source/gui/utilities
    ${CustusX_SOURCE_DIR}/source/resource/pluginUtilities
    ${CustusX_BINARY_DIR}/source/resource/usReconstructionTypes
    ${CustusX_BINARY_DIR}/source/logic
    ${CustusX_BINARY_DIR}/source/modules/GUIExtender
    ${CustusX_BINARY_DIR}/source/service/patient
    ${CustusX_BINARY_DIR}/source/service/visualization
    ${CustusX_BINARY_DIR}/source/resource/widgets
    ${CustusX_BINARY_DIR}/source/modules/Algorithm
    )

SET( cxPluginUSReconstruction_LIB_FILES
    cxResource
    cxResourceWidgets
    cxResource
    cxPatientService
    cxTrackingService
    cxVisualizationService
    cxPluginAlgorithm
    cxResourceUsReconstructionTypes
    cxLogicManager
    )
    
if( CX_USE_OPENCL_UTILITY )
    set(cxPluginUSReconstruction_LIB_FILES
        ${cxPluginUSReconstruction_LIB_FILES}
        ${OPENCL_LIBRARIES})
endif()

# =========================================================
cx_create_export_header("cxPluginUSReconstruction")

# create the target
add_library( cxPluginUSReconstruction ${SOURCE_FILES} )
target_link_libraries( cxPluginUSReconstruction ${cxPluginUSReconstruction_LIB_FILES})

add_subdirectory(testing)
#add_subdirectory(logic/3DReconstruction)

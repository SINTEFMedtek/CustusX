###########################################################
##               US Reconstruction Plugin
##
##               Plugin
##
##  Dependencies: Service Layer, 
##                Resource Layer. 
##
###########################################################

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)


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
# create the target
add_library( cxPluginUSReconstruction ${SOURCE_FILES} )
target_link_libraries( cxPluginUSReconstruction ${cxPluginUSReconstruction_LIB_FILES})
CX_COTIRE_ADD_LIBRARY( cxPluginUSReconstruction "${cxPluginUSReconstruction_LIB_FILES}")

add_subdirectory(testing)
#add_subdirectory(logic/3DReconstruction)

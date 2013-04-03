###########################################################
##               Algorithm Plugin
##
##               Plugin
##
##  Description: A collection of all algorithms and associated widgets not in
##               other plugins. 
##
##
##  Dependencies: resource layer, service layer
##                 
##
###########################################################

SET( MOC_HEADER_FILES
	logic/cxFilter.h
	logic/cxFilterImpl.h
	logic/cxFilterTimedAlgorithm.h
	logic/cxDummyFilter.h
	logic/cxPipeline.h
    logic/cxBinaryThresholdImageFilter.h
    logic/cxConnectedThresholdImageFilter.h
	logic/cxContourFilter.h
	logic/cxBinaryThinningImageFilter3DFilter.h
	logic/cxSmoothingImageFilter.h
	logic/cxResampleImageFilter.h
	logic/cxTubeSegmentationFilter.h
	logic/cxTSFPresets.h
    gui/cxAlgorithmPlugin.h
    gui/cxConnectedThresholdImageFilterWidget.h
    gui/cxTimedAlgorithmProgressBar.h
	gui/cxFilterWidget.h
	gui/cxPipelineWidget.h
	gui/cxFilterPresetWidget.h
	)

QT4_WRAP_CPP( MOC_HEADER_FILES ${MOC_HEADER_FILES} )

# library
SET( SOURCE_FILES
    ${MOC_HEADER_FILES}
	logic/cxFilter.h
	logic/cxFilter.cpp
	logic/cxFilterImpl.h
	logic/cxFilterImpl.cpp
	logic/cxFilterTimedAlgorithm.h
	logic/cxFilterTimedAlgorithm.cpp
	logic/cxDummyFilter.h
	logic/cxDummyFilter.cpp
	logic/cxTubeSegmentationFilter.h
	logic/cxTubeSegmentationFilter.cpp
	logic/cxPipeline.h
	logic/cxPipeline.cpp
	logic/cxTrackingDataToVolume.h
    logic/cxTrackingDataToVolume.cpp
    logic/cxBinaryThresholdImageFilter.h
    logic/cxBinaryThresholdImageFilter.cpp
    logic/cxConnectedThresholdImageFilter.h
    logic/cxConnectedThresholdImageFilter.cpp
	logic/cxContourFilter.h
	logic/cxContourFilter.cpp
	logic/cxBinaryThinningImageFilter3DFilter.h
	logic/cxBinaryThinningImageFilter3DFilter.cpp
	logic/cxSmoothingImageFilter.h
	logic/cxSmoothingImageFilter.cpp
	logic/cxResampleImageFilter.h
	logic/cxResampleImageFilter.cpp
	logic/cxTSFPresets.h
	logic/cxTSFPresets.cpp
    gui/cxAlgorithmPlugin.h
    gui/cxAlgorithmPlugin.cpp
    gui/cxConnectedThresholdImageFilterWidget.h
    gui/cxConnectedThresholdImageFilterWidget.cpp
    gui/cxTimedAlgorithmProgressBar.h
    gui/cxTimedAlgorithmProgressBar.cpp
	gui/cxFilterWidget.h
	gui/cxFilterWidget.cpp
	gui/cxPipelineWidget.h
	gui/cxPipelineWidget.cpp
	gui/cxFilterPresetWidget.h
	gui/cxFilterPresetWidget.cpp
  )

# =========================================================

include_directories(
    .
    ./logic
    ./gui
    ${CustusX3_SOURCE_DIR}/source/resource
    ${CustusX3_SOURCE_DIR}/source/resource/settings
    ${CustusX3_SOURCE_DIR}/source/resource/utilities
    ${CustusX3_SOURCE_DIR}/source/resource/algorithms

    ${CustusX3_SOURCE_DIR}/source/service/patient
    ${CustusX3_SOURCE_DIR}/source/service/tracking
    ${CustusX3_SOURCE_DIR}/source/service/visualization

    ${CustusX3_SOURCE_DIR}/source/plugins/PluginBase
    ${CustusX3_SOURCE_DIR}/source/gui/utilities
    ${CustusX3_SOURCE_DIR}/source/gui/dataAdapters
	${CustusX3_SOURCE_DIR}/source/gui/dataWidgets
	)

set(CX_ALGORITHM_LIBRARIES cxPatientService cxTrackingService cxResource cxGui)
message(STATUS "CX_USE_TSF: "${CX_USE_TSF})
if (CX_USE_TSF)
    set(CX_ALGORITHM_LIBRARIES ${CX_ALGORITHM_LIBRARIES} ${OPENCL_LIBRARIES} tubeSegmentationLib OCL-Utilities)
endif(CX_USE_TSF)

# =========================================================
# create the target
add_library( cxPluginAlgorithm ${SOURCE_FILES} )
target_link_libraries( cxPluginAlgorithm ${CX_ALGORITHM_LIBRARIES})

#------------------------------------------------------------------------------
# Cotire - pch and unity builds
#------------------------------------------------------------------------------
if(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)
    cotire(cxPluginAlgorithm)
    if(COTIRE_ADD_UNITY_BUILDS)
        target_link_libraries(cxPluginAlgorithm_unity
                        ${CX_ALGORITHM_LIBRARIES}
                        )
    endif(COTIRE_ADD_UNITY_BUILDS)
endif(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)
# =========================================================

# add/remove to suit your own needs.
#add_subdirectory(logic)
#add_subdirectory(gui)
add_subdirectory(testing)

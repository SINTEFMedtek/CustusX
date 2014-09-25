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
#	logic/cxFilterTimedAlgorithm.h
	logic/cxDummyFilter.h
	logic/cxPipeline.h
    logic/cxBinaryThresholdImageFilter.h
    logic/cxConnectedThresholdImageFilter.h
	logic/cxContourFilter.h
	logic/cxBinaryThinningImageFilter3DFilter.h
	logic/cxSmoothingImageFilter.h
	logic/cxResampleImageFilter.h
	logic/cxDilationFilter.h
    gui/cxAlgorithmPlugin.h
    gui/cxConnectedThresholdImageFilterWidget.h
    gui/cxTimedAlgorithmProgressBar.h
	gui/cxFilterWidget.h
	gui/cxPipelineWidget.h
	gui/cxAllFiltersWidget.h
	gui/cxCompactFilterSetupWidget.h
	)

QT4_WRAP_CPP( MOC_HEADER_FILES ${MOC_HEADER_FILES} )

# library
SET( SOURCE_FILES
	${MOC_HEADER_FILES}
#	logic/cxFilterTimedAlgorithm.h
#	logic/cxFilterTimedAlgorithm.cpp
	logic/cxDummyFilter.h
	logic/cxDummyFilter.cpp
	logic/cxDilationFilter.h
	logic/cxDilationFilter.cpp
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
	gui/cxAllFiltersWidget.h
	gui/cxAllFiltersWidget.cpp
	gui/cxCompactFilterSetupWidget.h
	gui/cxCompactFilterSetupWidget.cpp
  )

# =========================================================

include_directories(${CX_RESOURCE_INCLUDE_DIRECTORIES})

include_directories(
    ${CX_RESOURCE_WIDGETS_INCLUDE_DIRECTORIES}
    .
    ./logic
    ./gui
	${CustusX_SOURCE_DIR}/source/resource/pluginUtilities
	${CustusX_SOURCE_DIR}/source/service/patient
    ${CustusX_SOURCE_DIR}/source/service/tracking
    ${CustusX_SOURCE_DIR}/source/service/visualization
	${CustusX_SOURCE_DIR}/source/logic

    ${CustusX_SOURCE_DIR}/source/modules/GUIExtender
    ${CustusX_SOURCE_DIR}/source/gui/utilities
    ${CustusX_SOURCE_DIR}/source/gui/dataAdapters
	${CustusX_SOURCE_DIR}/source/gui/dataWidgets
	${CustusX_SOURCE_DIR}/source/resource/filter
	)

set(CX_ALGORITHM_LIBRARIES cxPatientService cxTrackingService cxResource cxGui cxGUIExtenderService cxResourceFilter)




# =========================================================
# create the target
add_library( cxPluginAlgorithm ${SOURCE_FILES} )
target_link_libraries( cxPluginAlgorithm ${CX_ALGORITHM_LIBRARIES})

add_subdirectory(testing)

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
    logic/cxCenterline.h
    logic/cxResample.h
    logic/cxBinaryThresholdImageFilter.h
    logic/cxContour.h
    logic/cxConnectedThresholdImageFilter.h
   gui/cxAlgorithmPlugin.h
    gui/cxBinaryThresholdImageFilterWidget.h
    gui/cxConnectedThresholdImageFilterWidget.h
    gui/cxSurfaceWidget.h
    gui/cxCenterlineWidget.h
    gui/cxResampleWidget.h
    gui/cxTimedAlgorithmProgressBar.h
	gui/cxFilterWidget.h
	)

QT4_WRAP_CPP( MOC_HEADER_FILES ${MOC_HEADER_FILES} )

# library
SET( SOURCE_FILES
    ${MOC_HEADER_FILES}
    logic/cxTrackingDataToVolume.h
    logic/cxTrackingDataToVolume.cpp
    logic/cxCenterline.h
    logic/cxCenterline.cpp
    logic/cxResample.h
    logic/cxResample.cpp
    logic/cxBinaryThresholdImageFilter.h
    logic/cxBinaryThresholdImageFilter.cpp
    logic/cxContour.h
    logic/cxContour.cpp
    logic/cxConnectedThresholdImageFilter.h
    logic/cxConnectedThresholdImageFilter.cpp
   gui/cxAlgorithmPlugin.h
   gui/cxAlgorithmPlugin.cpp
    gui/cxBinaryThresholdImageFilterWidget.h
    gui/cxBinaryThresholdImageFilterWidget.cpp
    gui/cxConnectedThresholdImageFilterWidget.h
    gui/cxConnectedThresholdImageFilterWidget.cpp
    gui/cxSurfaceWidget.cpp
    gui/cxSurfaceWidget.h
    gui/cxCenterlineWidget.cpp
    gui/cxCenterlineWidget.h
    gui/cxResampleWidget.cpp
    gui/cxResampleWidget.h
    gui/cxVisualizationMethodsWidget.h
    gui/cxVisualizationMethodsWidget.cpp
    gui/cxSegmentationMethodsWidget.h
    gui/cxSegmentationMethodsWidget.cpp
    gui/cxTimedAlgorithmProgressBar.h
    gui/cxTimedAlgorithmProgressBar.cpp
	gui/cxFilterWidget.h
	gui/cxFilterWidget.cpp
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
    )

set(CX_ALGORITHM_LIBRARIES cxPatientService cxTrackingService cxResource cxGui)

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

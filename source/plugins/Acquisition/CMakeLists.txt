###########################################################
##               Acquisition Plugin
##
##               Plugin
##
##  Description: Acquisition of tracking and image data, in particular ultrasound recordings.
##
##
##  Dependencies: USReconstruction Plugin
##                Service Layer  
##                Resource Layer 
##
###########################################################

SET( MOC_HEADER_FILES
    logic/cxAcquisitionData.h
    logic/cxUSAcquisition.h
	logic/cxUSSavingRecorder.h
    gui/cxRecordSessionWidget.h
    gui/cxRecordBaseWidget.h
    gui/cxTrackedCenterlineWidget.h
    gui/cxUSAcqusitionWidget.h
    gui/cxSoundSpeedConversionWidget.h
    gui/cxSelectRecordSessionStringDataAdapter.h
    gui/cxAcquisitionPlugin.h
    )

QT4_WRAP_CPP( MOC_HEADER_FILES ${MOC_HEADER_FILES} )

# library
SET( SOURCE_FILES
    ${MOC_HEADER_FILES}
    logic/cxAcquisitionData.h
    logic/cxAcquisitionData.cpp
    logic/cxRecordSession.h
    logic/cxRecordSession.cpp
    logic/cxUSAcquisition.h
    logic/cxUSAcquisition.cpp
	logic/cxUSSavingRecorder.h
	logic/cxUSSavingRecorder.cpp
    gui/cxAcquisitionPlugin.h
    gui/cxAcquisitionPlugin.cpp
    gui/cxRecordSessionWidget.h
    gui/cxRecordSessionWidget.cpp
    gui/cxRecordBaseWidget.h
    gui/cxRecordBaseWidget.cpp
    gui/cxTrackedCenterlineWidget.h
    gui/cxTrackedCenterlineWidget.cpp
    gui/cxUSAcqusitionWidget.h
    gui/cxUSAcqusitionWidget.cpp
    gui/cxSoundSpeedConversionWidget.h
    gui/cxSoundSpeedConversionWidget.cpp
    gui/cxSelectRecordSessionStringDataAdapter.h
    gui/cxSelectRecordSessionStringDataAdapter.cpp
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
    ${CustusX3_SOURCE_DIR}/source/service/video
    ${CustusX3_SOURCE_DIR}/source/service/visualization

    ${CustusX3_SOURCE_DIR}/source/plugins/UsReconstruction/logic
    ${CustusX3_SOURCE_DIR}/source/plugins/UsReconstruction/logic/3DReconstruction
    ${CustusX3_SOURCE_DIR}/source/plugins/Algorithm/logic
    ${CustusX3_SOURCE_DIR}/source/plugins/Algorithm/gui
    ${CustusX3_SOURCE_DIR}/source/plugins/PluginBase

    ${CustusX3_SOURCE_DIR}/source/algorithms
    ${CustusX3_SOURCE_DIR}/source/gui/utilities
    ${CustusX3_SOURCE_DIR}/source/gui/dataAdapters
    ${CustusX3_SOURCE_DIR}/source/gui/toolWidgets
    )

set(CX_ACQUISITION_LIBRARIES
                        cxPluginAlgorithm
                        cxPluginUSReconstruction
                        cxGui)
# =========================================================
# create the target
add_library( cxPluginAcquisition ${SOURCE_FILES} )
target_link_libraries( cxPluginAcquisition ${CX_ACQUISITION_LIBRARIES} )

#------------------------------------------------------------------------------
# Cotire - pch and unity builds
#------------------------------------------------------------------------------
if(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)
    cotire(cxPluginAcquisition)
    if(COTIRE_ADD_UNITY_BUILDS)
        target_link_libraries(cxPluginAcquisition_unity ${CX_ACQUISITION_LIBRARIES})
    endif(COTIRE_ADD_UNITY_BUILDS)
endif(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)

# =========================================================


# add/remove to suit your own needs.
#add_subdirectory(logic)
#add_subdirectory(gui)
add_subdirectory(testing)

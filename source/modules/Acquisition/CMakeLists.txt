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
cx_remove_duplicate_include_directories()
QT5_WRAP_CPP( MOC_HEADER_FILES ${MOC_HEADER_FILES} )

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
    ${CX_RESOURCE_VISUALIZATION_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_WIDGETS_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_USRECONSTRUCTIONTYPES_INCLUDE_DIRECTORIES}
    .
    ./logic
    ./gui
    ${CustusX_SOURCE_DIR}/source/service/video
    ${CustusX_SOURCE_DIR}/source/service/visualization
    ${CustusX_SOURCE_DIR}/source/logic

    ${CustusX_SOURCE_DIR}/source/modules/Algorithm/logic
    ${CustusX_SOURCE_DIR}/source/modules/Algorithm/gui
    ${CustusX_SOURCE_DIR}/source/modules/GUIExtender

    ${CustusX_SOURCE_DIR}/source/gui/utilities
    ${CustusX_SOURCE_DIR}/source/gui/dataAdapters
    ${CustusX_SOURCE_DIR}/source/gui/toolWidgets
    
    ${CustusX_BINARY_DIR}/source/service/video
    ${CustusX_BINARY_DIR}/source/service/visualization
    ${CustusX_BINARY_DIR}/source/modules/Algorithm
    ${CustusX_BINARY_DIR}/source/modules/GUIExtender
    ${CustusX_BINARY_DIR}/source/gui
    ${CustusX_BINARY_DIR}/source/logic

    ${CustusX_SOURCE_DIR}/source/plugins/org.custusx.usreconstruction
    ${CustusX_BINARY_DIR}/source/plugins/org.custusx.usreconstruction
    )

set(CX_ACQUISITION_LIBRARIES
    cxPluginAlgorithm
    org_custusx_usreconstruction
    cxGui)
# =========================================================
cx_remove_duplicate_include_directories()
cx_create_export_header("cxPluginAcquisition")
# create the target
add_library( cxPluginAcquisition ${SOURCE_FILES} )
target_link_libraries( cxPluginAcquisition ${CX_ACQUISITION_LIBRARIES} )

# add/remove to suit your own needs.
#add_subdirectory(logic)
#add_subdirectory(gui)
add_subdirectory(testing)

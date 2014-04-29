###########################################################
##               Calibration Plugin
##
##               Plugin
##
##  Description: Calibration methods and widgets
##
##
##  Dependencies: AcquisitionPlugin,  
##                 
##
###########################################################


SET( MOC_HEADER_FILES
   gui/cxCalibrationPlugin.h
   gui/cxToolTipSampleWidget.h
   gui/cxToolManualCalibrationWidget.h
   gui/cxTemporalCalibrationWidget.h
   gui/cxToolTipCalibrationWidget.h
   gui/cxLapFrameToolCalibrationWidget.h
    )

QT4_WRAP_CPP( MOC_HEADER_FILES ${MOC_HEADER_FILES} )

# library
SET( SOURCE_FILES
    ${MOC_HEADER_FILES}
  logic/cxTemporalCalibration.h
  logic/cxTemporalCalibration.cpp

   gui/cxCalibrationPlugin.h
   gui/cxCalibrationPlugin.cpp
   gui/cxToolTipSampleWidget.h
   gui/cxToolTipSampleWidget.cpp
   gui/cxToolManualCalibrationWidget.h
   gui/cxToolManualCalibrationWidget.cpp
   gui/cxTemporalCalibrationWidget.h
   gui/cxTemporalCalibrationWidget.cpp
   gui/cxCalibrationMethodsWidget.h
   gui/cxCalibrationMethodsWidget.cpp
   gui/cxToolTipCalibrationWidget.h
   gui/cxToolTipCalibrationWidget.cpp
   gui/cxLapFrameToolCalibrationWidget.h
   gui/cxLapFrameToolCalibrationWidget.cpp
  )

# =========================================================


include_directories(${CX_RESOURCE_INCLUDE_DIRECTORIES})
include_directories(
    ${CX_RESOURCE_WIDGETS_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_USRECONSTRUCTIONTYPES_INCLUDE_DIRECTORIES}
    .
    ./logic
    ./gui
    ${CustusX3_SOURCE_DIR}/source/service/patient
    ${CustusX3_SOURCE_DIR}/source/service/tracking
	${CustusX3_SOURCE_DIR}/source/logic

    ${CustusX3_SOURCE_DIR}/source/modules/PluginBase
    ${CustusX3_SOURCE_DIR}/source/modules/Acquisition/gui
    ${CustusX3_SOURCE_DIR}/source/modules/Acquisition/logic
    ${CustusX3_SOURCE_DIR}/source/gui/utilities
    ${CustusX3_SOURCE_DIR}/source/gui/dataAdapters
    ${CustusX3_SOURCE_DIR}/source/gui/toolWidgets
    )

# =========================================================
# create the target
add_library( cxPluginCalibration ${SOURCE_FILES} )
set(LINK_LIBRARIES
    cxResource
    cxResourceWidgets
    cxPluginAcquisition
      )
target_link_libraries( cxPluginCalibration ${LINK_LIBRARIES} )
CX_COTIRE_ADD_LIBRARY(cxPluginCalibration "${LINK_LIBRARIES}")

add_subdirectory(testing)

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

qt5_wrap_cpp( MOC_HEADER_FILES ${MOC_HEADER_FILES} )

set( SOURCE_FILES
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
cx_create_export_header("cxPluginCalibration")
add_library( cxPluginCalibration ${SOURCE_FILES} )
set(LINK_LIBRARIES
    cxResource
    cxResourceWidgets
	cxGui
	cxLogicManager
	cxGUIExtenderService
	org_custusx_acquisition
)
target_link_libraries( cxPluginCalibration ${LINK_LIBRARIES} )
target_include_directories(cxPluginCalibration
    PUBLIC
    .
    ./gui
    ./logic
    ${CMAKE_CURRENT_BINARY_DIR}
)

add_subdirectory(testing)

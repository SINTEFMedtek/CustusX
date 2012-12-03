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


include_directories(
    .
    ./logic
    ./gui
    ${CustusX3_SOURCE_DIR}/source/resource
    ${CustusX3_SOURCE_DIR}/source/service/patient
    ${CustusX3_SOURCE_DIR}/source/service/tracking
    ${CustusX3_SOURCE_DIR}/source/resource/settings
    ${CustusX3_SOURCE_DIR}/source/resource/utilities
    ${CustusX3_SOURCE_DIR}/source/plugins/PluginBase
    ${CustusX3_SOURCE_DIR}/source/plugins/Acquisition/gui
    ${CustusX3_SOURCE_DIR}/source/plugins/Acquisition/logic
    ${CustusX3_SOURCE_DIR}/source/gui/utilities
    ${CustusX3_SOURCE_DIR}/source/gui/dataAdapters
    ${CustusX3_SOURCE_DIR}/source/gui/toolWidgets
    )

# =========================================================
# create the target
add_library( cxPluginCalibration ${SOURCE_FILES} )
target_link_libraries( cxPluginCalibration
    cxResource
    cxPluginAcquisition
      )

#------------------------------------------------------------------------------
# Cotire - pch and unity builds
#------------------------------------------------------------------------------
if(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)
    cotire(cxPluginCalibration)
    if(COTIRE_ADD_UNITY_BUILDS)
        target_link_libraries(cxPluginCalibration_unity
                cxResource
                cxPluginAcquisition
                        )
    endif(COTIRE_ADD_UNITY_BUILDS)
endif(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)
# =========================================================


# add/remove to suit your own needs.
#add_subdirectory(logic)
#add_subdirectory(gui)

add_subdirectory(testing)

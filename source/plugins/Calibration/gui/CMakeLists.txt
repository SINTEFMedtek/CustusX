###########################################################
##               Calibration Plugin GUI
##
##               GUI Part
##
###########################################################



SET( MOC_HEADER_FILES
   cxCalibrationPlugin.h
   cxToolTipSampleWidget.h
   cxToolManualCalibrationWidget.h
   cxTemporalCalibrationWidget.h  
   cxToolTipCalibrationWidget.h
   cxLapFrameToolCalibrationWidget.h
    )

QT4_WRAP_CPP( MOC_HEADER_FILES ${MOC_HEADER_FILES} )

# library
SET( SOURCE_FILES
  ${MOC_HEADER_FILES}
   cxCalibrationPlugin.h
   cxCalibrationPlugin.cpp
   
   cxToolTipSampleWidget.h
   cxToolTipSampleWidget.cpp
   cxToolManualCalibrationWidget.h
   cxToolManualCalibrationWidget.cpp
   cxTemporalCalibrationWidget.h
   cxTemporalCalibrationWidget.cpp
   cxCalibrationMethodsWidget.h
   cxCalibrationMethodsWidget.cpp
   cxToolTipCalibrationWidget.h
   cxToolTipCalibrationWidget.cpp
   cxLapFrameToolCalibrationWidget.h
   cxLapFrameToolCalibrationWidget.cpp
  )

# =========================================================

include_directories(
    .
    ${CustusX3_SOURCE_DIR}/source/resource
    ${CustusX3_SOURCE_DIR}/source/service/patient
    ${CustusX3_SOURCE_DIR}/source/service/tracking
    ${CustusX3_SOURCE_DIR}/source/resource/settings
    ${CustusX3_SOURCE_DIR}/source/resource/utilities
    ${CustusX3_SOURCE_DIR}/source/plugins/PluginBase

    ${CustusX3_SOURCE_DIR}/source/plugins/Acquisition/gui
    ${CustusX3_SOURCE_DIR}/source/plugins/Acquisition/logic

    ../logic

    ${CustusX3_SOURCE_DIR}/source/gui/utilities
    ${CustusX3_SOURCE_DIR}/source/gui/dataAdapters
    ${CustusX3_SOURCE_DIR}/source/gui/toolWidgets
    )


# =========================================================
# create the target
add_library( cxPluginCalibrationGUI ${SOURCE_FILES} )
target_link_libraries( cxPluginCalibrationGUI cxPluginCalibrationLogic cxPluginAcquisitionGUI)

#------------------------------------------------------------------------------
# Cotire - pch and unity builds
#------------------------------------------------------------------------------
if(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)
    cotire(cxPluginCalibrationGUI)
    if(COTIRE_ADD_UNITY_BUILDS)
        target_link_libraries(cxPluginCalibrationGUI_unity 
        		cxPluginCalibrationLogic 
        		cxPluginAcquisitionGUI
        		)
    endif(COTIRE_ADD_UNITY_BUILDS)
endif(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)
# =========================================================
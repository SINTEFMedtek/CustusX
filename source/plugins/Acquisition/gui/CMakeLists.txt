###########################################################
##               Acquisition Plugin GUI
##
##               GUI Part
##
###########################################################


SET( MOC_HEADER_FILES
    cxRecordSessionWidget.h
    cxRecordBaseWidget.h
    cxTrackedCenterlineWidget.h
    cxUSAcqusitionWidget.h
    cxSoundSpeedConversionWidget.h
    cxSelectRecordSessionStringDataAdapter.h
    cxAcquisitionPlugin.h
    )

QT4_WRAP_CPP( MOC_HEADER_FILES ${MOC_HEADER_FILES} )

# library
SET( SOURCE_FILES
  ${MOC_HEADER_FILES}
    cxAcquisitionPlugin.h
    cxAcquisitionPlugin.cpp
    cxRecordSessionWidget.h
    cxRecordSessionWidget.cpp
    cxRecordBaseWidget.h
    cxRecordBaseWidget.cpp
    cxTrackedCenterlineWidget.h
    cxTrackedCenterlineWidget.cpp
    cxUSAcqusitionWidget.h
    cxUSAcqusitionWidget.cpp
    cxSoundSpeedConversionWidget.h
    cxSoundSpeedConversionWidget.cpp  
    cxSelectRecordSessionStringDataAdapter.h
    cxSelectRecordSessionStringDataAdapter.cpp
  )

# =========================================================

include_directories(
    .
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
    ../logic
    )


# =========================================================
# create the target
add_library( cxPluginAcquisitionGUI ${SOURCE_FILES} )
target_link_libraries( cxPluginAcquisitionGUI cxPluginAcquisitionLogic cxPluginAlgorithmGUI cxGui )

#------------------------------------------------------------------------------
# Cotire - pch and unity builds
#------------------------------------------------------------------------------
if(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)
    cotire(cxPluginAcquisitionGUI)
    if(COTIRE_ADD_UNITY_BUILDS)
        target_link_libraries(cxPluginAcquisitionGUI_unity 
        		cxPluginAcquisitionLogic 
        		cxPluginAlgorithmGUI 
        		cxGui
        		)
    endif(COTIRE_ADD_UNITY_BUILDS)
endif(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)

# =========================================================



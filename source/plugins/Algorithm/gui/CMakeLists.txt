###########################################################
##               Algorithm Plugin GUI
##
##               GUI Part
##
###########################################################


SET( MOC_HEADER_FILES
   cxAlgorithmPlugin.h
    cxBinaryThresholdImageFilterWidget.h
    cxConnectedThresholdImageFilterWidget.h
    cxSurfaceWidget.h
    cxCenterlineWidget.h
    cxResampleWidget.h   
    cxTimedAlgorithmProgressBar.h
    )

QT4_WRAP_CPP( MOC_HEADER_FILES ${MOC_HEADER_FILES} )

# library
SET( SOURCE_FILES
  ${MOC_HEADER_FILES}
   cxAlgorithmPlugin.h
   cxAlgorithmPlugin.cpp
    cxBinaryThresholdImageFilterWidget.h
    cxBinaryThresholdImageFilterWidget.cpp
    cxConnectedThresholdImageFilterWidget.h
    cxConnectedThresholdImageFilterWidget.cpp
    cxSurfaceWidget.cpp
    cxSurfaceWidget.h
    cxCenterlineWidget.cpp
    cxCenterlineWidget.h
    cxResampleWidget.cpp
    cxResampleWidget.h
    cxVisualizationMethodsWidget.h
    cxVisualizationMethodsWidget.cpp
    cxSegmentationMethodsWidget.h
    cxSegmentationMethodsWidget.cpp   
    cxTimedAlgorithmProgressBar.h
    cxTimedAlgorithmProgressBar.cpp
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

    ${CustusX3_SOURCE_DIR}/source/plugins/PluginBase
    ../logic
    ${CustusX3_SOURCE_DIR}/source/gui/utilities
    ${CustusX3_SOURCE_DIR}/source/gui/dataAdapters
    )


# =========================================================
# create the target
add_library( cxPluginAlgorithmGUI ${SOURCE_FILES} )
target_link_libraries( cxPluginAlgorithmGUI cxPluginAlgorithmLogic cxGui)

#------------------------------------------------------------------------------
# Cotire - pch and unity builds
#------------------------------------------------------------------------------
if(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)
    cotire(cxPluginAlgorithmGUI)
    if(COTIRE_ADD_UNITY_BUILDS)
        target_link_libraries(cxPluginAlgorithmGUI_unity 
        		cxPluginAcquisitionLogic 
        		cxGui
        		)
    endif(COTIRE_ADD_UNITY_BUILDS)
endif(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)
# =========================================================
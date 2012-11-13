###########################################################
##               Registration Plugin GUI
##
##               GUI Part
##
###########################################################



SET( MOC_HEADER_FILES
   cxRegistrationPlugin.h
   #cxRegistrationBaseWidget.h
    cxLandmarkImageRegistrationWidget.h
    cxLandmarkImage2ImageRegistrationWidget.h
    cxLandmarkRegistrationWidget.h
    cxLandmarkPatientRegistrationWidget.h
    cxRegistrationHistoryWidget.h
    cxManualRegistrationWidget.h
    cxFastImageRegistrationWidget.h
    #cxFastPatientRegistrationWidget.h
    cxFastOrientationRegistrationWidget.h
    cxImageSegmentationAndCenterlineWidget.h
    cxPlateRegistrationWidget.h
    cxSeansVesselRegistrationWidget.h
    cxRegisterI2IWidget.h
    cxRegistrationMethodsWidget.h
    cxRegistrationDataAdapters.h
    cxElastixWidget.h
    cxElastixSyntaxHighlighter.h
    cxWirePhantomWidget.h
    cxPatientOrientationWidget.h
    )

QT4_WRAP_CPP( MOC_HEADER_FILES ${MOC_HEADER_FILES} )

# library
SET( SOURCE_FILES
  ${MOC_HEADER_FILES}
   cxRegistrationPlugin.h
   cxRegistrationPlugin.cpp
   cxRegistrationBaseWidget.h
   cxRegistrationBaseWidget.cpp
    cxLandmarkImageRegistrationWidget.h
    cxLandmarkImageRegistrationWidget.cpp
    cxLandmarkRegistrationWidget.h
    cxLandmarkRegistrationWidget.cpp
    cxLandmarkPatientRegistrationWidget.h
    cxLandmarkPatientRegistrationWidget.cpp
    cxLandmarkImage2ImageRegistrationWidget.h    
    cxLandmarkImage2ImageRegistrationWidget.cpp
    cxRegistrationHistoryWidget.h
    cxRegistrationHistoryWidget.cpp
    cxManualRegistrationWidget.h
    cxManualRegistrationWidget.cpp
    cxFastImageRegistrationWidget.h
    cxFastImageRegistrationWidget.cpp
    cxFastPatientRegistrationWidget.h
    cxFastPatientRegistrationWidget.cpp
    cxFastOrientationRegistrationWidget.h
    cxFastOrientationRegistrationWidget.cpp
    cxImageSegmentationAndCenterlineWidget.h
    cxImageSegmentationAndCenterlineWidget.cpp
    cxPlateRegistrationWidget.h
    cxPlateRegistrationWidget.cpp
    cxSeansVesselRegistrationWidget.h
    cxSeansVesselRegistrationWidget.cpp
    cxRegisterI2IWidget.h
    cxRegisterI2IWidget.cpp    
    cxRegistrationMethodsWidget.h
    cxRegistrationMethodsWidget.cpp  
    cxRegistrationDataAdapters.h
    cxRegistrationDataAdapters.cpp
    cxElastixWidget.h
    cxElastixWidget.cpp
    cxElastixSyntaxHighlighter.h
    cxElastixSyntaxHighlighter.cpp
    cxWirePhantomWidget.h
    cxWirePhantomWidget.cpp
    cxPatientOrientationWidget.h
    cxPatientOrientationWidget.cpp
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
    ${CustusX3_SOURCE_DIR}/source/service/visualization

    ${CustusX3_SOURCE_DIR}/source/plugins/PluginBase
    ${CustusX3_SOURCE_DIR}/source/plugins/Acquisition/gui
    ${CustusX3_SOURCE_DIR}/source/plugins/Acquisition/logic
    ${CustusX3_SOURCE_DIR}/source/plugins/Algorithm/gui
    ${CustusX3_SOURCE_DIR}/source/plugins/Algorithm/logic
    ${CustusX3_SOURCE_DIR}/source/plugins/UsReconstruction/logic
    ${CustusX3_SOURCE_DIR}/source/plugins/UsReconstruction/logic/3DReconstruction    

    ../logic

    ${CustusX3_SOURCE_DIR}/source/gui/utilities
    ${CustusX3_SOURCE_DIR}/source/gui/dataAdapters
    ${CustusX3_SOURCE_DIR}/source/gui/infoWidgets
    )


# =========================================================
# create the target
add_library( cxPluginRegistrationGUI ${SOURCE_FILES} )
target_link_libraries( cxPluginRegistrationGUI cxPluginAcquisitionGUI cxPluginRegistrationLogic cxPluginAlgorithmGUI)

#------------------------------------------------------------------------------
# Cotire - pch and unity builds
#------------------------------------------------------------------------------
if(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)
    cotire(cxPluginRegistrationGUI)
    if(COTIRE_ADD_UNITY_BUILDS)
        target_link_libraries(cxPluginRegistrationGUI_unity 
        		cxPluginAcquisitionGUI
        		cxPluginRegistrationLogic 
        		cxPluginAlgorithmGUI
        		)
    endif(COTIRE_ADD_UNITY_BUILDS)
endif(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)
# =========================================================
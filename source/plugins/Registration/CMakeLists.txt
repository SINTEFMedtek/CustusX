###########################################################
##               Registration Plugin
##
##               Plugin
##
##  Description: Registration methods and widgets
##
##
##  Dependencies: AcquisitionPlugin,  
##                 
##
###########################################################

SET( MOC_HEADER_FILES
   logic/cxRegistrationManager.h
   logic/cxElastixManager.h
   logic/cxElastixExecuter.h

   gui/cxRegistrationPlugin.h
    gui/cxLandmarkImageRegistrationWidget.h
    gui/cxLandmarkImage2ImageRegistrationWidget.h
    gui/cxLandmarkRegistrationWidget.h
    gui/cxLandmarkPatientRegistrationWidget.h
    gui/cxRegistrationHistoryWidget.h
    gui/cxManualRegistrationWidget.h
    gui/cxFastImageRegistrationWidget.h
    gui/cxFastOrientationRegistrationWidget.h
	gui/cxPrepareVesselsWidget.h
    gui/cxPlateRegistrationWidget.h
    gui/cxSeansVesselRegistrationWidget.h
    gui/cxRegisterI2IWidget.h
    gui/cxRegistrationMethodsWidget.h
    gui/cxRegistrationDataAdapters.h
    gui/cxElastixWidget.h
    gui/cxElastixSyntaxHighlighter.h
    gui/cxWirePhantomWidget.h
	gui/cxPatientOrientationWidget.h
    )

QT4_WRAP_CPP( MOC_HEADER_FILES ${MOC_HEADER_FILES} )

# library
SET( SOURCE_FILES
    ${MOC_HEADER_FILES}
   logic/cxRegistrationManager.h
   logic/cxRegistrationManager.cpp
   logic/cxLandmarkTranslationRegistration.cpp
   logic/cxElastixManager.h
   logic/cxElastixManager.cpp
   logic/cxElastixExecuter.h
   logic/cxElastixExecuter.cpp

   gui/cxRegistrationPlugin.h
   gui/cxRegistrationPlugin.cpp
   gui/cxRegistrationBaseWidget.h
   gui/cxRegistrationBaseWidget.cpp
	gui/cxPrepareVesselsWidget.h
	gui/cxPrepareVesselsWidget.cpp
	gui/cxLandmarkImageRegistrationWidget.h
    gui/cxLandmarkImageRegistrationWidget.cpp
    gui/cxLandmarkRegistrationWidget.h
    gui/cxLandmarkRegistrationWidget.cpp
    gui/cxLandmarkPatientRegistrationWidget.h
    gui/cxLandmarkPatientRegistrationWidget.cpp
    gui/cxLandmarkImage2ImageRegistrationWidget.h
    gui/cxLandmarkImage2ImageRegistrationWidget.cpp
    gui/cxRegistrationHistoryWidget.h
    gui/cxRegistrationHistoryWidget.cpp
    gui/cxManualRegistrationWidget.h
    gui/cxManualRegistrationWidget.cpp
    gui/cxFastImageRegistrationWidget.h
    gui/cxFastImageRegistrationWidget.cpp
    gui/cxFastPatientRegistrationWidget.h
    gui/cxFastPatientRegistrationWidget.cpp
    gui/cxFastOrientationRegistrationWidget.h
    gui/cxFastOrientationRegistrationWidget.cpp
    gui/cxPlateRegistrationWidget.h
    gui/cxPlateRegistrationWidget.cpp
    gui/cxSeansVesselRegistrationWidget.h
    gui/cxSeansVesselRegistrationWidget.cpp
    gui/cxRegisterI2IWidget.h
    gui/cxRegisterI2IWidget.cpp
    gui/cxRegistrationMethodsWidget.h
    gui/cxRegistrationMethodsWidget.cpp
    gui/cxRegistrationDataAdapters.h
    gui/cxRegistrationDataAdapters.cpp
    gui/cxElastixWidget.h
    gui/cxElastixWidget.cpp
    gui/cxElastixSyntaxHighlighter.h
    gui/cxElastixSyntaxHighlighter.cpp
    gui/cxWirePhantomWidget.h
    gui/cxWirePhantomWidget.cpp
	gui/cxPatientOrientationWidget.h
    gui/cxPatientOrientationWidget.cpp
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
    ${CustusX3_SOURCE_DIR}/source/plugins/Acquisition/gui
    ${CustusX3_SOURCE_DIR}/source/plugins/Acquisition/logic
    ${CustusX3_SOURCE_DIR}/source/plugins/Algorithm/gui
    ${CustusX3_SOURCE_DIR}/source/plugins/Algorithm/logic
    ${CustusX3_SOURCE_DIR}/source/plugins/UsReconstruction/logic
    ${CustusX3_SOURCE_DIR}/source/plugins/UsReconstruction/logic/3DReconstruction

    ${CustusX3_SOURCE_DIR}/source/gui/utilities
    ${CustusX3_SOURCE_DIR}/source/gui/dataAdapters
    ${CustusX3_SOURCE_DIR}/source/gui/infoWidgets
    )


# =========================================================
# create the target
add_library( cxPluginRegistration ${SOURCE_FILES} )
target_link_libraries( cxPluginRegistration cxPluginAcquisition cxPluginAlgorithm)

#------------------------------------------------------------------------------
# Cotire - pch and unity builds
#------------------------------------------------------------------------------
if(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)
    cotire(cxPluginRegistration)
    if(COTIRE_ADD_UNITY_BUILDS)
        target_link_libraries(cxPluginRegistration_unity
                        cxPluginAcquisition
                        cxPluginAlgorithm
                        )
    endif(COTIRE_ADD_UNITY_BUILDS)
endif(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)
# =========================================================


# add/remove to suit your own needs.
#add_subdirectory(logic)
#add_subdirectory(gui)
add_subdirectory(testing)

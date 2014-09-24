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
    logic/cxElastixParameters.h
    logic/cxElastixExecuter.h

    gui/cxRegistrationPlugin.h
    gui/cxLandmarkImageRegistrationWidget.h
    gui/cxLandmarkImage2ImageRegistrationWidget.h
    gui/cxLandmarkRegistrationWidget.h
    gui/cxLandmarkPatientRegistrationWidget.h
    gui/cxManualRegistrationWidget.h
    gui/cxFastImageRegistrationWidget.h
    gui/cxFastOrientationRegistrationWidget.h
    gui/cxPrepareVesselsWidget.h
    gui/cxPlateRegistrationWidget.h
    gui/cxSeansVesselRegistrationWidget.h
    gui/cxRegisterI2IWidget.h
    gui/cxRegistrationMethodsWidget.h
    gui/cxElastixWidget.h
    gui/cxWirePhantomWidget.h
    gui/cxPatientOrientationWidget.h
    gui/cxBronchoscopyRegistrationWidget.h
    )

QT4_WRAP_CPP( MOC_HEADER_FILES ${MOC_HEADER_FILES} )

# library
SET( SOURCE_FILES
    ${MOC_HEADER_FILES}
   logic/cxRegistrationManager.h
   logic/cxRegistrationManager.cpp
   logic/cxElastixManager.h
   logic/cxElastixManager.cpp
   logic/cxElastixExecuter.h
   logic/cxElastixExecuter.cpp
   logic/cxElastixParameters.h
   logic/cxElastixParameters.cpp
   logic/cxBronchoscopyRegistration.h
   logic/cxBronchoscopyRegistration.cpp
   logic/cxBranch.h
   logic/cxBranch.cpp
   logic/cxBranchList.h
   logic/cxBranchList.cpp
    
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
    gui/cxElastixWidget.h
    gui/cxElastixWidget.cpp
    gui/cxElastixSyntaxHighlighter.h
	gui/cxElastixSyntaxHighlighter.cpp
    gui/cxWirePhantomWidget.h
    gui/cxWirePhantomWidget.cpp
	gui/cxPatientOrientationWidget.h
    gui/cxPatientOrientationWidget.cpp
    gui/cxBronchoscopyRegistrationWidget.h
    gui/cxBronchoscopyRegistrationWidget.cpp
    )

# =========================================================

include_directories(
    ${CX_RESOURCE_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_VISUALIZATION_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_WIDGETS_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_USRECONSTRUCTIONTYPES_INCLUDE_DIRECTORIES}
    .
    ./logic
    ./gui

    ${CustusX_SOURCE_DIR}/source/service/patient
    ${CustusX_SOURCE_DIR}/source/service/tracking
    ${CustusX_SOURCE_DIR}/source/service/visualization
	${CustusX_SOURCE_DIR}/source/logic

    ${CustusX_SOURCE_DIR}/source/modules/GUIExtender
    ${CustusX_SOURCE_DIR}/source/modules/Acquisition/gui
    ${CustusX_SOURCE_DIR}/source/modules/Acquisition/logic
    ${CustusX_SOURCE_DIR}/source/modules/Algorithm/gui
    ${CustusX_SOURCE_DIR}/source/modules/Algorithm/logic
    ${CustusX_SOURCE_DIR}/source/modules/UsReconstruction/logic
    ${CustusX_SOURCE_DIR}/source/modules/UsReconstruction/logic/3DReconstruction

    ${CustusX_SOURCE_DIR}/source/gui/utilities
    ${CustusX_SOURCE_DIR}/source/gui/dataAdapters
    ${CustusX_SOURCE_DIR}/source/gui/infoWidgets
    ${CustusX_SOURCE_DIR}/source/gui/dataWidgets

    ${CustusX_SOURCE_DIR}/source/resource/pluginUtilities
    ${CustusX_SOURCE_DIR}/source/resource/core/registration
	${CustusX_SOURCE_DIR}/source/resource/filter
    )


# =========================================================
# create the target
add_library( cxPluginRegistration ${SOURCE_FILES} )
set(LINK_LIBRARIES cxPluginAcquisition cxPluginAlgorithm)
target_link_libraries( cxPluginRegistration ${LINK_LIBRARIES})

# add/remove to suit your own needs.
#add_subdirectory(logic)
#add_subdirectory(gui)
add_subdirectory(testing)

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

    gui/cxRegistrationPlugin.h
    gui/cxLandmarkImageRegistrationWidget.h
    gui/cxLandmarkImage2ImageRegistrationWidget.h
    gui/cxLandmarkRegistrationWidget.h
    gui/cxLandmarkPatientRegistrationWidget.h
    gui/cxManualRegistrationWidget.h
    gui/cxFastOrientationRegistrationWidget.h
    gui/cxPrepareVesselsWidget.h
    gui/cxSeansVesselRegistrationWidget.h
    gui/cxRegisterI2IWidget.h
    gui/cxRegistrationMethodsWidget.h
    gui/cxWirePhantomWidget.h
    gui/cxPatientOrientationWidget.h
    )

cx_remove_duplicate_include_directories()
QT5_WRAP_CPP( MOC_HEADER_FILES ${MOC_HEADER_FILES} )

# library
SET( SOURCE_FILES
    ${MOC_HEADER_FILES}
   logic/cxRegistrationManager.h
   logic/cxRegistrationManager.cpp
    
   gui/cxRegistrationPlugin.h
   gui/cxRegistrationPlugin.cpp
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
    gui/cxFastPatientRegistrationWidget.h
    gui/cxFastPatientRegistrationWidget.cpp
    gui/cxFastOrientationRegistrationWidget.h
    gui/cxFastOrientationRegistrationWidget.cpp
    gui/cxSeansVesselRegistrationWidget.h
    gui/cxSeansVesselRegistrationWidget.cpp
    gui/cxRegisterI2IWidget.h
    gui/cxRegisterI2IWidget.cpp
    gui/cxRegistrationMethodsWidget.h
    gui/cxRegistrationMethodsWidget.cpp
    gui/cxWirePhantomWidget.h
    gui/cxWirePhantomWidget.cpp
	gui/cxPatientOrientationWidget.h
    gui/cxPatientOrientationWidget.cpp
    )

# =========================================================

include_directories(
    ${CX_RESOURCE_VISUALIZATION_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_WIDGETS_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_USRECONSTRUCTIONTYPES_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_FILTER_INCLUDE_DIRECTORIES}
    .
    ./logic
    ./gui

    ${CustusX_SOURCE_DIR}/source/service/patient
    ${CustusX_SOURCE_DIR}/source/service/tracking
    ${CustusX_SOURCE_DIR}/source/service/visualization
	${CustusX_SOURCE_DIR}/source/logic
	${CustusX_BINARY_DIR}/source/logic
	${CustusX_BINARY_DIR}/source/service/patient
    ${CustusX_BINARY_DIR}/source/service/tracking
    ${CustusX_BINARY_DIR}/source/service/visualization

    ${CustusX_SOURCE_DIR}/source/modules/GUIExtender
    ${CustusX_BINARY_DIR}/source/modules/GUIExtender
    ${CustusX_SOURCE_DIR}/source/modules/Acquisition/gui
    ${CustusX_SOURCE_DIR}/source/modules/Acquisition/logic
    ${CustusX_BINARY_DIR}/source/modules/Acquisition
    ${CustusX_SOURCE_DIR}/source/modules/Algorithm/gui
    ${CustusX_SOURCE_DIR}/source/modules/Algorithm/logic
    ${CustusX_BINARY_DIR}/source/modules/Algorithm
    ${CustusX_SOURCE_DIR}/source/modules/UsReconstruction/logic
    ${CustusX_SOURCE_DIR}/source/modules/UsReconstruction/logic/3DReconstruction
    ${CustusX_BINARY_DIR}/source/modules/UsReconstruction

    ${CustusX_SOURCE_DIR}/source/gui/utilities
    ${CustusX_SOURCE_DIR}/source/gui/dataAdapters
    ${CustusX_SOURCE_DIR}/source/gui/infoWidgets
    ${CustusX_SOURCE_DIR}/source/gui/dataWidgets
    ${CustusX_BINARY_DIR}/source/gui
    )


# =========================================================
cx_remove_duplicate_include_directories()
cx_create_export_header("cxPluginRegistration")

# create the target
add_library( cxPluginRegistration ${SOURCE_FILES} )
set(LINK_LIBRARIES cxPluginAcquisition cxPluginAlgorithm)
target_link_libraries( cxPluginRegistration ${LINK_LIBRARIES})

# add/remove to suit your own needs.
#add_subdirectory(logic)
#add_subdirectory(gui)
add_subdirectory(testing)

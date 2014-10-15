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
#    gui/cxRegistrationMethodsWidget.h
    )

cx_remove_duplicate_include_directories()
QT5_WRAP_CPP( MOC_HEADER_FILES ${MOC_HEADER_FILES} )

# library
SET( SOURCE_FILES
    ${MOC_HEADER_FILES}
#    gui/cxRegistrationMethodsWidget.h
#    gui/cxRegistrationMethodsWidget.cpp
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

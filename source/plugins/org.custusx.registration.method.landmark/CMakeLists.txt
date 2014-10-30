project(org_custusx_registration_method_landmark)

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

include_directories(
    ${CX_RESOURCE_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_WIDGETS_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_VISUALIZATION_INCLUDE_DIRECTORIES}
    ${CustusX_SOURCE_DIR}/source/plugins/org.custusx.registration
	${CustusX_SOURCE_DIR}/source/modules/Acquisition/logic
	${CustusX_SOURCE_DIR}/source/service/tracking
	${CustusX_SOURCE_DIR}/source/service/visualization
	${CustusX_SOURCE_DIR}/source/gui/infoWidgets
	${CustusX_SOURCE_DIR}/source/logic

	${CustusX_BINARY_DIR}/source/plugins/org.custusx.registration
	${CustusX_BINARY_DIR}/source/modules/Acquisition
	${CustusX_BINARY_DIR}/source/service/tracking
	${CustusX_BINARY_DIR}/source/service/visualization
	${CustusX_BINARY_DIR}/source/gui
	${CustusX_BINARY_DIR}/source/logic
)

set(PLUGIN_SRCS
    cxRegistrationMethodLandmarkPluginActivator.cpp
    cxRegistrationMethodLandmarkService.cpp
    cxFastImageRegistrationWidget.cpp
    cxFastRegistrationsWidget.cpp
    cxFastRegistrationsWidget.h
    cxLandmarkRegistrationsWidget.cpp
    cxLandmarkImageRegistrationWidget.cpp
    cxLandmarkRegistrationWidget.cpp
    cxLandmarkPatientRegistrationWidget.cpp
    cxLandmarkImage2ImageRegistrationWidget.cpp
    cxFastPatientRegistrationWidget.cpp
    cxFastOrientationRegistrationWidget.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
    cxRegistrationMethodLandmarkPluginActivator.h
    cxRegistrationMethodLandmarkService.h
    cxLandmarkRegistrationsWidget.h
    cxLandmarkRegistrationWidget.h
    cxLandmarkImageRegistrationWidget.h
    cxLandmarkPatientRegistrationWidget.h
    cxLandmarkImage2ImageRegistrationWidget.h
    cxFastPatientRegistrationWidget.h
    cxFastImageRegistrationWidget.h
    cxFastOrientationRegistrationWidget.h
)


cx_create_export_header(${PROJECT_NAME})
cx_remove_duplicate_include_directories()

# Qt Designer files which should be processed by Qts uic
set(PLUGIN_UI_FORMS
)

# QRC Files which should be compiled into the plugin
set(PLUGIN_resources
)

#Compute the plugin dependencies
ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)
set(PLUGIN_target_libraries 
    ${PLUGIN_target_libraries}   
    cxResourceWidgets
	cxPluginAlgorithm
	cxTrackingService
	org_custusx_registration
    )

set(PLUGIN_OUTPUT_DIR "")
if(CX_WINDOWS)
    #on windows we want dlls to be placed with the executables
    set(PLUGIN_OUTPUT_DIR "../")
endif(CX_WINDOWS)

ctkMacroBuildPlugin(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS}
  MOC_SRCS ${PLUGIN_MOC_SRCS}
  UI_FORMS ${PLUGIN_UI_FORMS}
  RESOURCES ${PLUGIN_resources}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
  OUTPUT_DIR ${PLUGIN_OUTPUT_DIR}
)

#add_subdirectory(testing)


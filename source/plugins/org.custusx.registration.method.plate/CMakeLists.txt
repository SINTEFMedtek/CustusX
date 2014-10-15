project(org_custusx_registration_method_plate)

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

message(STATUS "===================================== parsing org_custusx_registration_method_plate")

include_directories(
    ${CX_RESOURCE_WIDGETS_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_VISUALIZATION_INCLUDE_DIRECTORIES}
    ${CustusX_SOURCE_DIR}/source/plugins/org.custusx.registration
    ${CustusX_SOURCE_DIR}/source/plugins/org.custusx.registration.method.landmark
    ${CustusX_SOURCE_DIR}/source/logic
	${CustusX_SOURCE_DIR}/source/modules/Algorithm/logic
	${CustusX_SOURCE_DIR}/source/service/tracking

	${CustusX_BINARY_DIR}/source/plugins/org.custusx.registration
	${CustusX_BINARY_DIR}/source/plugins/org.custusx.registration.method.landmark
	${CustusX_BINARY_DIR}/source/logic
	${CustusX_BINARY_DIR}/source/modules/Algorithm
	${CustusX_BINARY_DIR}/source/service/tracking
)

set(PLUGIN_SRCS
  cxRegistrationMethodPlatePluginActivator.cpp
  cxRegistrationMethodPlateService.cpp
  cxPlateImageRegistrationWidget.cpp
  cxPlateImageRegistrationWidget.h
  cxPlateRegistrationWidget.cpp
  cxImage2PlateRegistrationWidget.cpp
  cxImage2PlateRegistrationWidget.h
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
  cxRegistrationMethodPlatePluginActivator.h
  cxRegistrationMethodPlateService.h
  cxPlateRegistrationWidget.h
)

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
	org_custusx_registration_method_landmark
    )

ctkMacroBuildPlugin(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS}
  MOC_SRCS ${PLUGIN_MOC_SRCS}
  UI_FORMS ${PLUGIN_UI_FORMS}
  RESOURCES ${PLUGIN_resources}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
)

#add_subdirectory(testing)


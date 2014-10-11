project(org_custusx_registration_method_commandline)

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

include_directories(
    ${CX_RESOURCE_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_WIDGETS_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_VISUALIZATION_INCLUDE_DIRECTORIES}
    ${CustusX_SOURCE_DIR}/source/plugins/org.custusx.registration
    ${CustusX_SOURCE_DIR}/source/logic
	${CustusX_SOURCE_DIR}/source/modules/Registration/gui
	${CustusX_SOURCE_DIR}/source/modules/Algorithm/logic
	${CustusX_SOURCE_DIR}/source/modules/Algorithm/gui
	${CustusX_SOURCE_DIR}/source/service/tracking
	${CustusX_SOURCE_DIR}/source/service/patient
	${CustusX_BINARY_DIR}/source/service/patient

	${CustusX_BINARY_DIR}/source/logic
	${CustusX_BINARY_DIR}/source/modules/Registration
	${CustusX_BINARY_DIR}/source/modules/Algorithm
	${CustusX_BINARY_DIR}/source/service/tracking
		
    ${CustusX_SOURCE_DIR}/source/gui/dataAdapters
    ${CustusX_BINARY_DIR}/source/gui
	.
	gui
	logic
)

set(PLUGIN_SRCS
    cxRegistrationMethodCommandLinePluginActivator.cpp
    cxRegistrationMethodCommandLineService.cpp
    logic/cxElastixManager.h
    logic/cxElastixManager.cpp
    logic/cxElastixExecuter.h
    logic/cxElastixExecuter.cpp
    logic/cxElastixParameters.h
    logic/cxElastixParameters.cpp
    gui/cxElastixWidget.h
    gui/cxElastixWidget.cpp
    gui/cxElastixSyntaxHighlighter.h
	gui/cxElastixSyntaxHighlighter.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
    cxRegistrationMethodCommandLinePluginActivator.h
    cxRegistrationMethodCommandLineService.h
    logic/cxElastixManager.h
    logic/cxElastixParameters.h
    logic/cxElastixExecuter.h
    gui/cxElastixWidget.h
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
    cxPluginUtilities
	cxPluginRegistration
	cxPluginAlgorithm
	cxTrackingService
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

add_subdirectory(testing)


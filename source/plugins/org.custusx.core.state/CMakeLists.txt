###########################################################
##               State Service
##
###########################################################

project(org_custusx_core_state)

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

include_directories(${CX_RESOURCE_INCLUDE_DIRECTORIES})
include_directories(
    .
    ${CustusX_SOURCE_DIR}/source/service/visualization
    ${CustusX_BINARY_DIR}/source/service/visualization
    ${CustusX_SOURCE_DIR}/source/service/video
    ${CustusX_BINARY_DIR}/source/service/video
    ${CustusX_SOURCE_DIR}/source/resource/pluginUtilities
    ${CustusX_BINARY_DIR}/source/resource/pluginUtilities
    )

set(PLUGIN_SRCS
	cxStateServiceImpl.cpp
	cxWorkflowStateMachine.h
	cxWorkflowStateMachine.cpp
	cxWorkflowState.h
	cxWorkflowState.cpp
	cxApplicationStateMachine.cpp
	cxApplicationStateMachine.h
	cxApplicationState.h
	cxApplicationState.cpp
	cxRequestEnterStateTransition.h
	cxStateServiceBackend.cpp
	cxStateServiceBackend.h
	cxStatePluginActivator.cpp
	cxApplicationsParser.h
	cxApplicationsParser.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
	cxWorkflowStateMachine.h
	cxWorkflowState.h
	cxApplicationStateMachine.h
	cxApplicationState.h
	cxStateServiceImpl.h
	cxStatePluginActivator.h
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
    cxVisualizationService
    cxPluginUtilities
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


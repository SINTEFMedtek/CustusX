project(org_custusx_help)

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

include_directories(
    ${CustusX_SOURCE_DIR}/source/modules/GUIExtender
	${CustusX_BINARY_DIR}/source/modules/GUIExtender
	${CustusX_SOURCE_DIR}/source/resource/core/help
    ${CustusX_SOURCE_DIR}/source/resource/pluginUtilities
    ${CX_RESOURCE_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_WIDGETS_INCLUDE_DIRECTORIES}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/widgets
	${CMAKE_CURRENT_SOURCE_DIR}/core
	)

set(PLUGIN_SRCS
  cxHelpPluginActivator.cpp
  cxHelpGUIExtenderService.cpp
  widgets/cxHelpWidget.cpp
  widgets/cxHelpIndexWidget.cpp
  widgets/cxHelpSearchWidget.cpp
  widgets/cxHelpBrowser.cpp
  cxHelpEngine.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
  cxHelpPluginActivator.h
  cxHelpGUIExtenderService.h
  widgets/cxHelpWidget.h
  widgets/cxHelpIndexWidget.h
  widgets/cxHelpSearchWidget.h
  widgets/cxHelpBrowser.h
  cxHelpEngine.h
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
    cxGUIExtenderService
    cxResourceWidgets
    Qt5::Help
    cxPluginUtilities
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


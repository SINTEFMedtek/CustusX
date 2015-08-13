project(org_custusx_robot_ur5)

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

set(PLUGIN_SRCS
  cxUr5PluginActivator.cpp
  cxUr5GUIExtenderService.cpp
  cxUr5Widget.cpp
  cxUr5Connection.cpp
  cxUr5State.cpp
  cxUr5Transmit.cpp
  cxUr5Receive.cpp
  cxUr5Script.cpp
  cxUr5RTReceive.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
  cxUr5PluginActivator.h
  cxUr5GUIExtenderService.h
  cxUr5Widget.h
  cxUr5Connection.h
  cxUr5State.h
  cxUr5Transmit.h
  cxUr5Receive.h
  cxUr5Script.h
  cxUr5RTReceive.h
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
    cxResource
    cxGUIExtenderService
    #cxLogicManager
    cxPluginUtilities
	Qt5::Widgets
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

target_include_directories(org_custusx_robot_ur5
    PUBLIC
    .
    ${CMAKE_CURRENT_BINARY_DIR}
)


add_subdirectory(testing)


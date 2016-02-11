project(org_custusx_robot_ur5)

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

set(PLUGIN_SRCS
  cxUr5PluginActivator.cpp
  cxUr5Robot.cpp
  cxUr5Connection.cpp
  cxUr5State.cpp
  cxUr5MessageEncoder.cpp
  cxUr5MessageDecoder.cpp
  cxUr5ProgramEncoder.cpp
  cxUr5GUIExtenderService.cpp
  cxUr5Kinematics.cpp

  cxUr5Widget.cpp
  widgets/cxUr5Script.cpp
  widgets/cxUr5Initialize.cpp
  widgets/cxUr5ManualMove.cpp
  widgets/cxUr5PlannedMove.cpp
  widgets/cxUr5Settings.cpp
  widgets/cxUr5Misc.cpp

  trackingSystemRobot/cxRobotTrackingSystemService.cpp
  trackingSystemRobot/cxRobotTool.cpp

  applications/cxUr5LungSimulation.cpp
  applications/cxUr5LungSimulationTab.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
  cxUr5PluginActivator.h
  cxUr5Robot.h
  cxUr5Connection.h
  cxUr5State.h
  cxUr5MessageEncoder.h
  cxUr5MessageDecoder.h
  cxUr5ProgramEncoder.h
  cxUr5GUIExtenderService.h
  cxUr5Kinematics.h

  cxUr5Widget.h
  widgets/cxUr5Script.h
  widgets/cxUr5Initialize.h
  widgets/cxUr5ManualMove.h
  widgets/cxUr5PlannedMove.h
  widgets/cxUr5Settings.h
  widgets/cxUr5Misc.h

  trackingSystemRobot/cxRobotTrackingSystemService.h
  trackingSystemRobot/cxRobotTool.h

  applications/cxUr5LungSimulation.h
  applications/cxUr5LungSimulationTab.h
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
    cxResourceWidgets
    cxGUIExtenderService
    cxLogicManager
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

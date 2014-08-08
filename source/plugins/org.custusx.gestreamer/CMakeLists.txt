project(org_custusx_gestreamer)

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

include_directories(
    ${CustusX_SOURCE_DIR}/source/service/video
    ${CustusX_SOURCE_DIR}/source/resource/videoServer
    ${CustusX_SOURCE_DIR}/source/resource/OpenIGTLinkUtilities
    ${CustusX_SOURCE_DIR}/source/resource/core
    ${CustusX_SOURCE_DIR}/source/resource/core/dataAdapters
    ${CustusX_SOURCE_DIR}/source/resource/core/utilities
    ${CustusX_SOURCE_DIR}/source/resource/core/Data
    ${CustusX_SOURCE_DIR}/source/resource/core/Math
    ${CustusX_SOURCE_DIR}/source/resource/core/Tool
    )

set(PLUGIN_SRCS
  cxGEStreamerPluginActivator.cpp
  cxGEStreamerService.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
  cxGEStreamerPluginActivator.h
  cxGEStreamerService.h
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
    cxVideoService
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


project(org_custusx_tordreconstruction)

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

include_directories(
    ${CustusX3_BINARY_DIR}/source/modules/UsReconstruction/logic
    ${CustusX3_SOURCE_DIR}/source/modules/UsReconstruction/logic
    ${CustusX3_SOURCE_DIR}/source/resource/core
    ${CustusX3_SOURCE_DIR}/source/resource/core/utilities
    ${CustusX3_SOURCE_DIR}/source/resource/core/Math
    ${CustusX3_SOURCE_DIR}/source/resource/core/dataAdapters
    ${CustusX3_SOURCE_DIR}/source/resource/core/Tool
    ${CustusX3_SOURCE_DIR}/source/resource/core/logger
    ${CustusX3_SOURCE_DIR}/source/resource/usReconstructionTypes
    ${CustusX3_SOURCE_DIR}/source/ThirdParty
    )

set(PLUGIN_SRCS
  cxTordPluginActivator.cpp
  cxTordReconstructionService.cpp
  cxTordReconstructionService.h
  cxTordAlgorithm.h
  cxTordAlgorithm.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
  cxTordPluginActivator.h
)

# Qt Designer files which should be processed by Qts uic
set(PLUGIN_UI_FORMS
)

# QRC Files which should be compiled into the plugin
set(PLUGIN_resources
)

SET(TORD_KERNEL_PATH ${CMAKE_CURRENT_SOURCE_DIR})
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/recConfig.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/recConfig.h"
  )
  
#Compute the plugin dependencies
ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)
set(PLUGIN_target_libraries 
    ${PLUGIN_target_libraries}
    cxPluginUSReconstruction
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


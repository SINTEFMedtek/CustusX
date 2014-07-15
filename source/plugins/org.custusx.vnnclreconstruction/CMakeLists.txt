project(org_custusx_vnnclreconstruction)

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

include_directories(
    ${CustusX_BINARY_DIR}/source/modules/UsReconstruction/logic
    ${CustusX_SOURCE_DIR}/source/modules/UsReconstruction/logic
    ${CustusX_SOURCE_DIR}/source/resource/core
    ${CustusX_SOURCE_DIR}/source/resource/core/utilities
    ${CustusX_SOURCE_DIR}/source/resource/core/Math
    ${CustusX_SOURCE_DIR}/source/resource/core/dataAdapters
    ${CustusX_SOURCE_DIR}/source/resource/core/Tool
    ${CustusX_SOURCE_DIR}/source/resource/core/logger
    ${CustusX_SOURCE_DIR}/source/resource/usReconstructionTypes
    ${CustusX_SOURCE_DIR}/source/ThirdParty
    )

set(PLUGIN_SRCS
  cxVNNclPluginActivator.cpp
  cxVNNclReconstructionService.cpp
  cxVNNclReconstructionService.h
  cxVNNclAlgorithm.h
  cxVNNclAlgorithm.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
  cxVNNclPluginActivator.h
)

# Qt Designer files which should be processed by Qts uic
set(PLUGIN_UI_FORMS
)

# QRC Files which should be compiled into the plugin
set(PLUGIN_resources
)

SET(VNNCL_KERNEL_PATH ${CMAKE_CURRENT_SOURCE_DIR})
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


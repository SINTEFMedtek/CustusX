project(org_custusx_pnnreconstruction)

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

include_directories(
    #${CustusX_BINARY_DIR}/source/modules/UsReconstruction/logic
    ${CustusX_SOURCE_DIR}/source/modules/UsReconstruction/logic
    ${CustusX_SOURCE_DIR}/source/resource/core
    ${CustusX_SOURCE_DIR}/source/resource/core/utilities
    ${CustusX_SOURCE_DIR}/source/resource/core/Math
    ${CustusX_SOURCE_DIR}/source/resource/core/dataAdapters
    ${CustusX_SOURCE_DIR}/source/resource/core/Tool
    ${CustusX_SOURCE_DIR}/source/resource/core/Data
    ${CustusX_SOURCE_DIR}/source/resource/core/logger
    ${CustusX_SOURCE_DIR}/source/resource/usReconstructionTypes
    ${CustusX_SOURCE_DIR}/source/ThirdParty
    ${CustusX_SOURCE_DIR}/source/logic
    )

set(PLUGIN_SRCS
  cxPNNReconstructionPluginActivator.cpp
  cxPNNReconstructionService.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
  cxPNNReconstructionPluginActivator.h
  cxPNNReconstructionService.h
)

set(PLUGIN_UI_FORMS
)

# QRC Files which should be compiled into the plugin
set(PLUGIN_resources
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


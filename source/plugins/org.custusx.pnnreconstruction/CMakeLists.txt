project(org_custusx_pnnreconstruction)

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

include_directories(
    ${CX_RESOURCE_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_USRECONSTRUCTIONTYPES_INCLUDE_DIRECTORIES}
#    ${CustusX_SOURCE_DIR}/source/modules/UsReconstruction/logic
#    ${CustusX_SOURCE_DIR}/source/resource/usReconstructionTypes
    ${CustusX_SOURCE_DIR}/source/ThirdParty
#    ${CustusX_BINARY_DIR}/source/modules/UsReconstruction
#    ${CustusX_BINARY_DIR}/source/resource/core
#    ${CustusX_BINARY_DIR}/source/resource/usReconstructionTypes
    ${CustusX_BINARY_DIR}/source/ThirdParty
    ${CustusX_SOURCE_DIR}/source/logic

    ${CustusX_SOURCE_DIR}/source/plugins/org.custusx.usreconstruction
    ${CustusX_BINARY_DIR}/source/plugins/org.custusx.usreconstruction
    )

set(PLUGIN_SRCS
  cxPNNReconstructionPluginActivator.cpp
  cxPNNReconstructionMethodService.cpp
  cxPNNReconstructionMethodService.h
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
  cxPNNReconstructionPluginActivator.h
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
#    cxPluginUSReconstruction
    cxPluginUtilities
    org_custusx_usreconstruction
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


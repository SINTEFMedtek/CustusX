project(org_custusx_vnnclreconstruction)

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

include_directories(
    ${CX_RESOURCE_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_USRECONSTRUCTIONTYPES_INCLUDE_DIRECTORIES}
    ${CustusX_BINARY_DIR}/source/ThirdParty
    ${CustusX_SOURCE_DIR}/source/logic

    ${CustusX_SOURCE_DIR}/source/plugins/org.custusx.usreconstruction
    ${CustusX_BINARY_DIR}/source/plugins/org.custusx.usreconstruction
    )

set(PLUGIN_SRCS
  cxVNNclPluginActivator.cpp
  cxVNNclReconstructionMethodService.cpp
  cxVNNclReconstructionMethodService.h
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
    cxPluginUtilities
    org_custusx_usreconstruction
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

add_subdirectory(testing)


project(org_custusx_usreconstruction_vnncl)

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")


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
  
# Install OpenCL kernels into bundle
install(FILES
	${CustusX_SOURCE_DIR}/source/plugins/org.custusx.usreconstruction.vnncl/kernels.cl
	${CustusX_SOURCE_DIR}/source/plugins/org.custusx.usreconstruction.vnncl/kernels.cl.h
	DESTINATION
        ${CX_INSTALL_CONFIG_DIR}/shaders/)

#Compute the plugin dependencies
ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)
set(PLUGIN_target_libraries 
    ${PLUGIN_target_libraries}
    ${OPENCL_LIBRARIES}
    cxPluginUtilities
    OpenCLUtilityLibrary
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
  ${CX_CTK_PLUGIN_NO_INSTALL}
)

target_include_directories(org_custusx_usreconstruction_vnncl
    PUBLIC
    .
    ${CMAKE_CURRENT_BINARY_DIR}
)

cx_doc_define_plugin_user_docs("${PROJECT_NAME}" "${CMAKE_CURRENT_SOURCE_DIR}/doc")
cx_add_non_source_file("doc/org.custusx.usreconstruction.vnncl.md")

add_subdirectory(testing)


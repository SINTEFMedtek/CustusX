project(org_custusx_filter_accusurf)

include(cxInstallUtilities)


set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")



set(PLUGIN_SRCS
    cxAccusurf.h
    cxAccusurf.cpp
    cxFilterAccusurfPluginActivator.cpp
    cxAccusurfFilterService.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
    cxFilterAccusurfPluginActivator.h
    cxAccusurfFilterService.h
)

set(PLUGIN_UI_FORMS
)

# QRC Files which should be compiled into the plugin
set(PLUGIN_resources
)

set(VTK_MODULES
  VTK::CommonComputationalGeometry
)

#Compute the plugin dependencies
ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)
set(PLUGIN_target_libraries
    PUBLIC
    ${PLUGIN_target_libraries}
    #cxPluginAlgorithm

    PRIVATE
    cxResource
    cxResourceFilter
    cxPluginUtilities
    cxResourceVisualization
    ${VTK_MODULES}
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

target_include_directories(org_custusx_filter_accusurf
    PUBLIC
    .
    ${CMAKE_CURRENT_BINARY_DIR}
)

vtk_module_autoinit(
  TARGETS org_custusx_filter_accusurf
  MODULES ${VTK_MODULES}
  )
cx_doc_define_plugin_user_docs("${PROJECT_NAME}" "${CMAKE_CURRENT_SOURCE_DIR}/doc")
cx_add_non_source_file("doc/org.custusx.filter.accusurf.md")

add_subdirectory(testing)



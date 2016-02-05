project(org_custusx_filter_tubesegmentation)

include(cxInstallUtilities)

###############################################################################
# Initialize Tube-Segmentation-Framework library
# Find the package and run the include USE file.
###############################################################################
macro(cx_initialize_TSF)
    find_package( Tube-Segmentation-Framework REQUIRED)
    include(${Tube-Segmentation-Framework_USE_FILE})

    cx_install_add_library_dirs(${Tube-Segmentation-Framework_LIBRARY_DIRS})
endmacro()

cx_initialize_TSF()

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

set(PLUGIN_SRCS
    cxFilterTubeSegmentationPluginActivator.cpp
    cxTSFPresets.cpp
    cxTubeSegmentationFilterService.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
    cxFilterTubeSegmentationPluginActivator.h
    cxTSFPresets.h
    cxTubeSegmentationFilterService.h
)

set(PLUGIN_UI_FORMS
)

# QRC Files which should be compiled into the plugin
set(PLUGIN_resources
)

install(FILES
        ${Tube-Segmentation-Framework_KERNELS_DIR}/kernels.cl
        ${Tube-Segmentation-Framework_KERNELS_DIR}/kernels_no_3d_write.cl
		DESTINATION ${CX_INSTALL_CONFIG_DIR}/tsf/)
install(DIRECTORY ${Tube-Segmentation-Framework_PARAMETERS_DIR}
		DESTINATION ${CX_INSTALL_CONFIG_DIR}/tsf/
        FILE_PERMISSIONS ${CX_FULL_PERMISSIONS}
        DIRECTORY_PERMISSIONS ${CX_FULL_PERMISSIONS})

#Compute the plugin dependencies
ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)
set(PLUGIN_target_libraries
    PUBLIC
    ${PLUGIN_target_libraries}

    PRIVATE
    tubeSegmentationLib
    SIPL
    cxResource
    cxResourceFilter
	cxResourceVisualization
    cxPluginUtilities
)

if(CX_USE_OPENCL_UTILITY)
    set(PLUGIN_target_libraries ${PLUGIN_target_libraries} OpenCLUtilityLibrary ${OPENCL_LIBRARIES})
    include_directories(${OpenCLUtilityLibrary_INCLUDE_DIRS})
endif(CX_USE_OPENCL_UTILITY)

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

target_include_directories(org_custusx_filter_tubesegmentation
    PUBLIC
    .
    ${CMAKE_CURRENT_BINARY_DIR}
)

cx_doc_define_plugin_user_docs("${PROJECT_NAME}" "${CMAKE_CURRENT_SOURCE_DIR}/doc")
cx_add_non_source_file("doc/org.custusx.filter.tubesegmentation.md")

add_subdirectory(testing)


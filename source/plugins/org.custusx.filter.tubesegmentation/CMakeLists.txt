project(org_custusx_filter_tubesegmentation)

include(cxInstallUtilities)

###############################################################################
# Initialize Tube-Segmentation-Framework library
# Find the package and run the include USE file.
###############################################################################
macro(cx_initialize_TSF)
    find_package( Tube-Segmentation-Framework REQUIRED)
    include(${Tube-Segmentation-Framework_USE_FILE})

    set(CX_ALL_LIBRARY_DIRS
        ${CX_ALL_LIBRARY_DIRS}
        ${Tube-Segmentation-Framework_LIBRARY_DIRS}
        CACHE INTERNAL "List of dirs to look for libraries to use in fixup_bundle")
endmacro()

cx_initialize_TSF()

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

include_directories(
    ${CX_RESOURCE_INCLUDE_DIRECTORIES}
    #${CustusX_SOURCE_DIR}/source/ThirdParty
    ${CustusX_SOURCE_DIR}/source/service/patient
    ${CustusX_SOURCE_DIR}/source/modules/Algorithm/logic
    ${CustusX_SOURCE_DIR}/source/gui/dataAdapters
	${CustusX_SOURCE_DIR}/source/logic
    )

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
		DESTINATION ${CX_INSTALL_ROOT_DIR}/config/tsf/)
install(DIRECTORY ${Tube-Segmentation-Framework_PARAMETERS_DIR}
		DESTINATION ${CX_INSTALL_ROOT_DIR}/config/tsf/
		FILE_PERMISSIONS ${CX_FULL_PERMISSIONS}
		DIRECTORY_PERMISSIONS ${CX_FULL_PERMISSIONS})

#Compute the plugin dependencies
ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)
set(PLUGIN_target_libraries
    LINK_PUBLIC 
    ${PLUGIN_target_libraries}   
    cxPluginAlgorithm
    LINK_PRIVATE tubeSegmentationLib
    )    

if (CX_USE_OPENCL_UTILITY)
    set(PLUGIN_target_libraries ${PLUGIN_target_libraries} OpenCLUtilityLibrary ${OPENCL_LIBRARIES})
    include_directories(${OpenCLUtilityLibrary_INCLUDE_DIRS})
endif(CX_USE_OPENCL_UTILITY)

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


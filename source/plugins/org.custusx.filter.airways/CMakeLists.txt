project(org_custusx_filter_airways)

include(cxInstallUtilities)

macro(cx_initialize_FAST)
    find_package( FAST REQUIRED)
    include(${FAST_USE_FILE})
    message("===========================")
    message("Initializing FAST in TSF plugin")
    message("===========================")
    cx_install_add_library_dirs(${FAST_LIBRARY_DIRS})
endmacro()

cx_initialize_FAST()

#### Enable C++11
# Note that Microsoft Visual C++ compiler enables C++11 by default
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR CMAKE_COMPILER_IS_GNUCXX)
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag(--std=c++11 SUPPORTS_STD_CXX11)
    check_cxx_compiler_flag(--std=c++0x SUPPORTS_STD_CXX01)
    if(SUPPORTS_STD_CXX11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --std=c++11")
    elseif(SUPPORTS_STD_CXX01)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++0x")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --std=c++0x")
    else()
        message(ERROR "Compiler does not support --std=c++11 or --std=c++0x.")
    endif()
    if(APPLE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -g3 -Wall -O0")
    endif()
endif()

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

set(PLUGIN_SRCS
    cxFilterAirwaysPluginActivator.cpp
    cxAirwaysFilterService.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
    cxFilterAirwaysPluginActivator.h
    cxAirwaysFilterService.h
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
    PUBLIC
    ${PLUGIN_target_libraries}

    PRIVATE
    #airwaysLib
    FAST
    cxResource
    cxResourceFilter
	cxResourceVisualization
    cxPluginUtilities
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

target_include_directories(org_custusx_filter_airways
    PUBLIC
    .
    ${CMAKE_CURRENT_BINARY_DIR}
)

cx_doc_define_plugin_user_docs("${PROJECT_NAME}" "${CMAKE_CURRENT_SOURCE_DIR}/doc")
cx_add_non_source_file("doc/org.custusx.filter.airways.md")

#add_subdirectory(testing)


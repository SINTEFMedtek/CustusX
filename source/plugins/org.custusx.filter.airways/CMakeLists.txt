project(org_custusx_filter_airways)

include(cxInstallUtilities)

macro(cx_initialize_FAST)
    find_package(FAST REQUIRED)
    include_directories("${FAST_DIR}/../FAST/source/")
    include_directories("${FAST_DIR}/")
    link_directories("${FAST_DIR}/lib/")
    set(FAST_SOURCE_DIR ${FAST_DIR}/../FAST/source/FAST/)
    add_definitions("-DFAST_SOURCE_DIR=\"${FAST_SOURCE_DIR}/\"")
    message("===========================")
    message("Initializing FAST in Airways plugin")
    message("-- ${FAST_DIR}/../FAST/source/")
    message("===========================")
    cx_install_add_library_dirs(${FAST_DIR}/lib/)
endmacro()

cx_initialize_FAST()

#### Enable C++11
set (CMAKE_CXX_STANDARD 11)

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

# Make sure OpenCL kernel files are copied to the installer
# Install OpenCL kernels
install(DIRECTORY ${FAST_SOURCE_DIR}
        DESTINATION ${CX_INSTALL_CONFIG_DIR}/FAST/
        FILES_MATCHING PATTERN "*.cl"
)

#Compute the plugin dependencies
ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)
# Z lib comes with many names, needed for FAST atm
if(WIN32)
    set(ZLIB zlib.lib)
elseif(APPLE)
    set(ZLIB libz.dylib)
else()
    set(ZLIB libz.so)
endif()
set(PLUGIN_target_libraries
    PUBLIC
    ${PLUGIN_target_libraries}
    FAST

    PRIVATE
    ${OPENCL_LIBRARIES}
    ${ZLIB}
    cxResource
    cxResourceFilter
    cxResourceVisualization
    cxPluginUtilities
    org_custusx_registration_method_bronchoscopy
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

target_include_directories(org_custusx_filter_airways
    PUBLIC
    .
    ${CMAKE_CURRENT_BINARY_DIR}
)

cx_doc_define_plugin_user_docs("${PROJECT_NAME}" "${CMAKE_CURRENT_SOURCE_DIR}/doc")
cx_add_non_source_file("doc/org.custusx.filter.airways.md")

add_subdirectory(testing)


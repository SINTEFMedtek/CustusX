project(org_custusx_core_tracking_system_ndi)

# ---- ndicapi (third-party copy, see ndicapi/README-CustusX.md) ----

set(NDICAPI_DIR "${CMAKE_CURRENT_LIST_DIR}/ndicapi")

# ndicapi_serial.cxx and ndicapi_socket.cxx include their platform specific
# *_unix/_apple/_win32.cxx files themselves.
add_library(cx_ndicapi STATIC
    ${NDICAPI_DIR}/ndicapi.cxx
    ${NDICAPI_DIR}/ndicapi_math.cxx
    ${NDICAPI_DIR}/ndicapi_serial.cxx
    ${NDICAPI_DIR}/ndicapi_socket.cxx
    ${NDICAPI_DIR}/ndicapi_thread.cxx
)
target_include_directories(cx_ndicapi PUBLIC ${NDICAPI_DIR})
set_target_properties(cx_ndicapi PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Third-party code: don't report its warnings
if(MSVC)
    target_compile_options(cx_ndicapi PRIVATE /W0)
else()
    target_compile_options(cx_ndicapi PRIVATE -w)
endif()

if(WIN32)
    # ndicapi includes <windows.h>: keep its min/max macros from breaking std::min/std::max
    target_compile_definitions(cx_ndicapi PUBLIC NOMINMAX)
    target_link_libraries(cx_ndicapi PRIVATE ws2_32)
elseif(UNIX)
    find_package(Threads REQUIRED)
    target_link_libraries(cx_ndicapi PRIVATE Threads::Threads)
endif()

# ---- Plugin ----

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

set(PLUGIN_SRCS
    cxNDITrackingPluginActivator.h
    cxNDITrackingPluginActivator.cpp
    trackingSystemNDI/cxNdiDevice.h
    trackingSystemNDI/cxNdiCapiDevice.h
    trackingSystemNDI/cxNdiCapiDevice.cpp
    trackingSystemNDI/cxNdiToolSetup.h
    trackingSystemNDI/cxNdiToolSetup.cpp
    trackingSystemNDI/cxNdiTool.h
    trackingSystemNDI/cxNdiTool.cpp
    trackingSystemNDI/cxNdiTrackerManager.h
    trackingSystemNDI/cxNdiTrackerManager.cpp
    trackingSystemNDI/cxNdiTransformUtils.h
    trackingSystemNDI/cxNdiTrackerThread.h
    trackingSystemNDI/cxNdiTrackerThread.cpp
    trackingSystemNDI/cxToolUsingNDI.h
    trackingSystemNDI/cxToolUsingNDI.cpp
    trackingSystemNDI/cxTrackingSystemNDIService.h
    trackingSystemNDI/cxTrackingSystemNDIService.cpp
)

set(PLUGIN_MOC_SRCS
    cxNDITrackingPluginActivator.h
    trackingSystemNDI/cxNdiTool.h
    trackingSystemNDI/cxNdiTrackerManager.h
    trackingSystemNDI/cxNdiTrackerThread.h
    trackingSystemNDI/cxToolUsingNDI.h
    trackingSystemNDI/cxTrackingSystemNDIService.h
)

set(PLUGIN_UI_FORMS
)

set(PLUGIN_resources
)

ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)
set(PLUGIN_target_libraries
    PUBLIC
    cxResource
    cxPluginUtilities
    ${PLUGIN_target_libraries}

    PRIVATE
    cx_ndicapi
)

set(PLUGIN_OUTPUT_DIR "")
if(CX_WINDOWS)
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

target_include_directories(${PROJECT_NAME}
    PUBLIC
    .
    ./trackingSystemNDI
    ${CMAKE_CURRENT_BINARY_DIR}
)

vtk_module_autoinit(
  TARGETS ${PROJECT_NAME}
  MODULES VTK::CommonCore
)

add_subdirectory(testing)

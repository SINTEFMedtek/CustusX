project(org_custusx_registration_method_centerline)

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

set(PLUGIN_SRCS
  cxRegistrationMethodCenterlinePluginActivator.cpp
  cxRegistrationMethodCenterlineService.h
  cxRegistrationMethodCenterlineService.cpp
  cxCenterlineRegistration.cpp
  cxCenterlineRegistration.h
  cxCenterlineRegistrationWidget.cpp
  cxCenterlinePointsWidget.h
  cxCenterlinePointsWidget.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
  cxRegistrationMethodCenterlinePluginActivator.h
  cxCenterlineRegistrationWidget.h
  cxCenterlinePointsWidget.h
)

# Qt Designer files which should be processed by Qts uic
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
    ${PLUGIN_target_libraries}   
    cxResourceWidgets
    cxResourceVisualization
    org_custusx_registration
    org_custusx_registration_method_landmark
    org_custusx_acquisition
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

target_include_directories(org_custusx_registration_method_centerline
    PUBLIC
    .
    ${CMAKE_CURRENT_BINARY_DIR}
)

vtk_module_autoinit(
  TARGETS ${PROJECT_NAME}
  MODULES ${VTK_MODULES}
)

cx_doc_define_plugin_user_docs("${PROJECT_NAME}" "${CMAKE_CURRENT_SOURCE_DIR}/doc")
cx_add_non_source_file("doc/org_custusx_registration_method_centerline.md")
cx_add_non_source_file("doc/org_custusx_registration_method_centerline.h")

add_subdirectory(testing)


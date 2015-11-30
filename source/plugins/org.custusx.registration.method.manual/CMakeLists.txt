project(org_custusx_registration_method_manual)

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

set(PLUGIN_SRCS
    cxRegistrationMethodManualPluginActivator.cpp
    cxRegistrationMethodManualService.cpp
    cxManualRegistrationWidget.cpp
    cxRegistrationMethodManualImageToImageService.cpp
    cxRegistrationMethodManualImageToPatientService.cpp
    cxRegistrationMethodManualImageTransformService.cpp
    cxRegistrationMethodManualImageToImageService.h
    cxRegistrationMethodManualImageToPatientService.h
    cxRegistrationMethodManualImageTransformService.h
    cxManualImage2ImageRegistrationWidget.cpp
    cxManualImage2ImageRegistrationWidget.h
    cxManualImageTransformRegistrationWidget.cpp
    cxManualImageTransformRegistrationWidget.h
    cxManualPatientRegistrationWidget.cpp
    cxRegistrationMethodManualPatientOrientationService.h
    cxRegistrationMethodManualPatientOrientationService.cpp
    cxPatientOrientationWidget.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
    cxRegistrationMethodManualPluginActivator.h
    cxRegistrationMethodManualService.h
    cxManualRegistrationWidget.h
    cxManualPatientRegistrationWidget.h
    cxPatientOrientationWidget.h
)

# Qt Designer files which should be processed by Qts uic
set(PLUGIN_UI_FORMS
)

# QRC Files which should be compiled into the plugin
set(PLUGIN_resources
)

#Compute the plugin dependencies
ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)
set(PLUGIN_target_libraries 
    ${PLUGIN_target_libraries}
    cxResourceWidgets
    org_custusx_registration
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

target_include_directories(org_custusx_registration_method_manual
    PUBLIC
    .
    ${CMAKE_CURRENT_BINARY_DIR}
)

cx_doc_define_plugin_user_docs("${PROJECT_NAME}" "${CMAKE_CURRENT_SOURCE_DIR}/doc")
cx_add_non_source_file("doc/org_custusx_registration_method_manual.md")
cx_add_non_source_file("doc/org_custusx_registration_method_manual.h")
cx_add_non_source_file("doc/org_custusx_registration_method_manual_image_to_patient.md")
cx_add_non_source_file("doc/org_custusx_registration_method_manual_image2image.md")
cx_add_non_source_file("doc/org_custusx_registration_method_manual_image_transform.md")
cx_add_non_source_file("doc/org_custusx_registration_method_manual_patient_orientation.md")

add_subdirectory(testing)


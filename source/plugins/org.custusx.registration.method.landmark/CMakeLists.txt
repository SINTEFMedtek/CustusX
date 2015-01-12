project(org_custusx_registration_method_landmark)

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

set(PLUGIN_SRCS
    cxRegistrationMethodLandmarkPluginActivator.cpp
    cxRegistrationMethodLandmarkService.h
    cxRegistrationMethodLandmarkService.cpp
    cxFastImageRegistrationWidget.cpp
    cxFastRegistrationsWidget.cpp
    cxFastRegistrationsWidget.h
    cxLandmarkRegistrationsWidget.h
    cxLandmarkRegistrationsWidget.cpp
    cxLandmarkImageRegistrationWidget.cpp
    cxLandmarkRegistrationWidget.cpp
    cxLandmarkPatientRegistrationWidget.cpp
    cxLandmarkImage2ImageRegistrationWidget.cpp
    cxFastPatientRegistrationWidget.h
    cxFastPatientRegistrationWidget.cpp
    cxFastOrientationRegistrationWidget.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
    cxRegistrationMethodLandmarkPluginActivator.h
    cxLandmarkRegistrationWidget.h
    cxLandmarkImageRegistrationWidget.h
    cxLandmarkPatientRegistrationWidget.h
    cxLandmarkImage2ImageRegistrationWidget.h
    cxFastImageRegistrationWidget.h
    cxFastOrientationRegistrationWidget.h
)


cx_create_export_header(${PROJECT_NAME})

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
	cxPluginAlgorithm
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
)

target_include_directories(org_custusx_registration_method_landmark
    PUBLIC
    .
    ${CMAKE_CURRENT_BINARY_DIR}
)

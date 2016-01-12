project(org_custusx_registration)

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

set(PLUGIN_SRCS
  cxRegistrationPluginActivator.cpp
  cxRegistrationImplService.cpp
  cxRegistrationImplService.h
  cxRegistrationApplicator.cpp
  cxRegistrationApplicator.h
  cxLandmarkTranslationRegistration.cpp
  cxLandmarkTranslationRegistration.h
  cxRegServices.cpp
  cxRegServices.h

  cxRegistrationMethodService.h

  cxRegistrationServiceNull.cpp
  cxRegistrationServiceNull.h
  cxRegistrationService.cpp
  cxRegistrationServiceProxy.cpp
  cxRegistrationProperties.cpp

  cxRegistrationBaseWidget.cpp
  cxRegistrationBaseWidget.h
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
  cxRegistrationPluginActivator.h

  cxRegistrationService.h
  cxRegistrationServiceProxy.h
  cxRegistrationMethodService.h
  cxRegistrationProperties.h
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
    cxResource
    cxResourceWidgets
    cxPluginUtilities
	cxResourceVisualization
    ${ITK_LIBRARIES}
#TODO: Remove these dependencies
    org_custusx_acquisition
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

target_include_directories(org_custusx_registration
    PUBLIC
    .
    ${CMAKE_CURRENT_BINARY_DIR}
)

add_subdirectory(testing)


project(org_custusx_registration)

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

message(STATUS "===================================== parsing example")

include_directories(
    ${CX_RESOURCE_WIDGETS_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_VISUALIZATION_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_USRECONSTRUCTIONTYPES_INCLUDE_DIRECTORIES}
    ${CustusX_SOURCE_DIR}/source/logic
    ${CustusX_SOURCE_DIR}/source/modules/Acquisition/logic
#    ${CustusX_SOURCE_DIR}/source/modules/UsReconstruction/logic
    ${CustusX_SOURCE_DIR}/source/modules/Algorithm
#    ${CustusX_SOURCE_DIR}/source/modules/Algorithm/gui
#    ${CustusX_SOURCE_DIR}/source/modules/Algorithm/logic
#    ${CustusX_SOURCE_DIR}/source/gui/dataAdapters
#    ${CustusX_SOURCE_DIR}/source/service/visualization

    ${CustusX_BINARY_DIR}/source/service/patient
    ${CustusX_BINARY_DIR}/source/logic
    ${CustusX_BINARY_DIR}/source/modules/Acquisition
#    ${CustusX_BINARY_DIR}/source/modules/UsReconstruction
#    ${CustusX_BINARY_DIR}/source/modules/Algorithm
#    ${CustusX_BINARY_DIR}/source/gui
#    ${CustusX_BINARY_DIR}/source/service/visualization
    )

set(PLUGIN_SRCS
  cxRegistrationPluginActivator.cpp
  cxRegistrationPluginActivator.h
  cxRegistrationImplService.cpp
  cxRegistrationImplService.h
  cxRegistrationApplicator.cpp
  cxRegistrationApplicator.h
  cxLandmarkTranslationRegistration.cpp
  cxLandmarkTranslationRegistration.h
  cxRegistrationMethodServices.cpp
  cxRegistrationMethodServices.h

  cxRegistrationServiceNull.cpp
  cxRegistrationServiceNull.h
  cxRegistrationService.cpp
  cxRegistrationServiceProxy.cpp
  cxRegistrationDataAdapters.cpp

  cxRegistrationBaseWidget.cpp
  cxRegistrationBaseWidget.h
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
  cxRegistrationPluginActivator.h

  cxRegistrationService.h
  cxRegistrationServiceProxy.h
  cxRegistrationMethodService.h
  cxRegistrationDataAdapters.h
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
    cxResourceUsReconstructionTypes
    cxPluginUtilities
#TODO: Remove these dependencies
    cxLogicManager
    cxPluginAlgorithm
#    cxGui
    cxVisualizationService
    )

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


project(org_custusx_registration_gui)

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

include_directories(
    ${CX_RESOURCE_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_WIDGETS_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_VISUALIZATION_INCLUDE_DIRECTORIES}
    ${CustusX_SOURCE_DIR}/source/plugins/org.custusx.registration
    ${CustusX_SOURCE_DIR}/source/modules/GUIExtender
    ${CustusX_SOURCE_DIR}/source/modules/Algorithm/gui
    ${CustusX_SOURCE_DIR}/source/modules/Algorithm/logic
    ${CustusX_SOURCE_DIR}/source/logic
#	${CustusX_SOURCE_DIR}/source/gui/infoWidgets

    ${CustusX_BINARY_DIR}/source/plugins/org.custusx.registration
    ${CustusX_BINARY_DIR}/source/modules/GUIExtender
    ${CustusX_BINARY_DIR}/source/modules/Algorithm/gui
    ${CustusX_BINARY_DIR}/source/modules/Algorithm/logic
    ${CustusX_BINARY_DIR}/source/modules/Algorithm
    ${CustusX_BINARY_DIR}/source/logic
#	${CustusX_BINARY_DIR}/source/gui
    ${CustusX_SOURCE_DIR}/source/gui/utilities
    ${CustusX_SOURCE_DIR}/source/gui/dataAdapters
	${CustusX_SOURCE_DIR}/source/gui/dataWidgets
	${CustusX_BINARY_DIR}/source/gui
	${CustusX_SOURCE_DIR}/source/resource/filter
    ${CustusX_SOURCE_DIR}/source/plugins/org.custusx.usreconstruction
    ${CustusX_BINARY_DIR}/source/plugins/org.custusx.usreconstruction
usReconstructionTypes
    ${CustusX_SOURCE_DIR}/source/resource/usReconstructionTypes
    ${CustusX_BINARY_DIR}/source/resource/usReconstructionTypes
)

set(PLUGIN_SRCS
  cxRegistrationGUIPluginActivator.cpp
  cxRegistrationGUIExtenderService.cpp
  cxRegistrationGUIExtenderService.h
  cxRegistrationWidget.cpp
  cxRegistrationHistoryGUIExtenderService.cpp
  cxRegistrationHistoryGUIExtenderService.h
  cxRegistrationHistoryWidget.cpp
  cxWirePhantomWidget.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
  cxRegistrationGUIPluginActivator.h
  cxRegistrationWidget.h
  cxRegistrationHistoryWidget.h
  cxWirePhantomWidget.h
)

# Qt Designer files which should be processed by Qts uic
set(PLUGIN_UI_FORMS
)

# QRC Files which should be compiled into the plugin
set(PLUGIN_resources
)

ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)

set(PLUGIN_target_libraries 
    ${PLUGIN_target_libraries}   
    cxGUIExtenderService
    cxResourceWidgets
    cxResourceVisualization
    cxPluginUtilities
    cxPluginAlgorithm
    cxGui
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
)


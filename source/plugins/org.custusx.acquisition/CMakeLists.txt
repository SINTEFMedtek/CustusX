###########################################################
##               Acquisition Plugin
##
##               Plugin
##
##  Description: Acquisition of tracking and image data, in particular ultrasound recordings.
##
##
###########################################################
project(org_custusx_acquisition)

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

set(PLUGIN_SRCS
    cxAcquisitionService.cpp
    cxAcquisitionServiceProxy.cpp
    cxAcquisitionServiceNull.h
    cxAcquisitionServiceNull.cpp
    cxAcquisitionImplService.h
    cxAcquisitionImplService.cpp
    cxAcquisitionServicePluginActivator.cpp
    cxRecordSession.h
    cxRecordSession.cpp

    logic/cxAcquisitionData.h
    logic/cxAcquisitionData.cpp
    logic/cxUSAcquisition.h
    logic/cxUSAcquisition.cpp
    logic/cxUSSavingRecorder.h
    logic/cxUSSavingRecorder.cpp
    gui/cxAcquisitionPlugin.h
    gui/cxAcquisitionPlugin.cpp
    gui/cxUSAcqusitionWidget.h
    gui/cxUSAcqusitionWidget.cpp
    gui/cxSoundSpeedConversionWidget.h
    gui/cxSoundSpeedConversionWidget.cpp
	gui/cxStringPropertySelectRecordSession.h
	gui/cxStringPropertySelectRecordSession.cpp
    gui/cxRecordSessionWidget.cpp
    gui/cxRecordTrackingWidget.cpp
	gui/cxRecordSessionSelector.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
    cxAcquisitionService.h
    cxAcquisitionServiceProxy.h
    cxAcquisitionServicePluginActivator.h

    logic/cxAcquisitionData.h
    logic/cxUSAcquisition.h
    logic/cxUSSavingRecorder.h
    gui/cxSoundSpeedConversionWidget.h
    gui/cxUSAcqusitionWidget.h
	gui/cxStringPropertySelectRecordSession.h
    gui/cxAcquisitionPlugin.h
    gui/cxRecordSessionWidget.h
    gui/cxRecordTrackingWidget.h
	gui/cxRecordSessionSelector.h
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
    org_custusx_usreconstruction
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

target_include_directories( org_custusx_acquisition
    PUBLIC
    .
    ./logic
    ./gui
    ${CMAKE_CURRENT_BINARY_DIR}
)

cx_doc_define_plugin_user_docs("${PROJECT_NAME}" "${CMAKE_CURRENT_SOURCE_DIR}/doc")
cx_add_non_source_file("doc/org.custusx.acquisition.md")

add_subdirectory(testing)

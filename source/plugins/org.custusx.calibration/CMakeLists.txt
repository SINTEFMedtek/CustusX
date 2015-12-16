project(org_custusx_calibration)

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

set(PLUGIN_SRCS
  cxCalibrationPluginActivator.cpp
  cxCalibrationGUIExtenderService.cpp
  cxCalibrationGUIExtenderService.h
  logic/cxTemporalCalibration.h
  logic/cxTemporalCalibration.cpp
   gui/cxToolTipSampleWidget.h
   gui/cxToolTipSampleWidget.cpp
   gui/cxToolManualCalibrationWidget.h
   gui/cxToolManualCalibrationWidget.cpp
   gui/cxTemporalCalibrationWidget.h
   gui/cxTemporalCalibrationWidget.cpp
   gui/cxCalibrationMethodsWidget.h
   gui/cxCalibrationMethodsWidget.cpp
   gui/cxToolTipCalibrationWidget.h
   gui/cxToolTipCalibrationWidget.cpp
   gui/cxLapFrameToolCalibrationWidget.h
   gui/cxLapFrameToolCalibrationWidget.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
  cxCalibrationPluginActivator.h

   gui/cxToolTipSampleWidget.h
   gui/cxToolManualCalibrationWidget.h
   gui/cxTemporalCalibrationWidget.h
   gui/cxToolTipCalibrationWidget.h
   gui/cxLapFrameToolCalibrationWidget.h

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
	cxLogicManager
	cxGUIExtenderService
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

target_include_directories(org_custusx_calibration
	PUBLIC
	.
	./gui
	./logic
	${CMAKE_CURRENT_BINARY_DIR}
)

cx_doc_define_plugin_user_docs("${PROJECT_NAME}" "${CMAKE_CURRENT_SOURCE_DIR}/doc")

cx_add_non_source_file("doc/org.custusx.calibration.md")

add_subdirectory(testing)


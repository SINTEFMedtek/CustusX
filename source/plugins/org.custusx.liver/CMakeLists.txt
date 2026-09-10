project(org_custusx_liver)

include(cxInstallUtilities)

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

set(PLUGIN_SRCS
	cxLiverPluginActivator.cpp
	cxLiverGUIExtenderService.cpp
	cxLiverSegmentationRunner.cpp
	cxLiverSegmentationWidget.cpp
	cxLiverVisibilityWidget.cpp
	cxLiverWidget.cpp
)

set(PLUGIN_MOC_SRCS
	cxLiverPluginActivator.h
	cxLiverGUIExtenderService.h
	cxLiverSegmentationRunner.h
	cxLiverSegmentationWidget.h
	cxLiverVisibilityWidget.h
	cxLiverWidget.h
)

set(PLUGIN_UI_FORMS
)

set(PLUGIN_resources
)

ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)
set(PLUGIN_target_libraries
	${PLUGIN_target_libraries}
	cxResource
	cxResourceWidgets
	cxResourceFilter
	cxResourceVisualization
	cxGUIExtenderService
	cxPluginUtilities
	Qt5::Widgets
	)

set(PLUGIN_OUTPUT_DIR "")
if(CX_WINDOWS)
	set(PLUGIN_OUTPUT_DIR "../")
endif(CX_WINDOWS)

ctkMacroBuildPlugin(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS} ${PLUGIN_MOC_SRCS}
  MOC_SRCS ${PLUGIN_MOC_SRCS}
  UI_FORMS ${PLUGIN_UI_FORMS}
  RESOURCES ${PLUGIN_resources}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
  OUTPUT_DIR ${PLUGIN_OUTPUT_DIR}
  ${CX_CTK_PLUGIN_NO_INSTALL}
)

target_include_directories(org_custusx_liver
	PUBLIC
	.
	${CMAKE_CURRENT_BINARY_DIR}
)

cx_doc_define_plugin_user_docs("${PROJECT_NAME}" "${CMAKE_CURRENT_SOURCE_DIR}/doc")
cx_add_non_source_file("doc/org.custusx.liver.md")

add_subdirectory(testing)

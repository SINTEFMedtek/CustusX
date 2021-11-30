project(org_custusx_virtualbronchoscopy)

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

set(PLUGIN_SRCS
  cxVBPluginActivator.cpp
  cxVBService.h
  cxVBService.cpp
  cxVBWidget.cpp
  cxVBcameraPath.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
  cxVBPluginActivator.h
  cxVBWidget.h
  cxVBcameraPath.h
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
    PUBLIC
    ${PLUGIN_target_libraries}

    PRIVATE
    cxGUIExtenderService
    cxPluginUtilities
    cxResource
    cxResourceVisualization
    cxResourceWidgets
    Qt5::Widgets
		org_custusx_filter_routetotarget
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

target_include_directories(org_custusx_virtualbronchoscopy
    PUBLIC
    .
    ${CMAKE_CURRENT_BINARY_DIR}
)

cx_doc_define_plugin_user_docs("${PROJECT_NAME}" "${CMAKE_CURRENT_SOURCE_DIR}/doc")
cx_add_non_source_file("doc/org.custusx.virtualbronchoscopy.md")
#cx_add_non_source_file("doc/org.custusx.virtualbronchoscopy.h")

#add_subdirectory(testing)
#add_subdirectory(testApp)



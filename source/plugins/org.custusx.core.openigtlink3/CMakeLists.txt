project(org_custusx_core_openigtlink3)

cx_initialize_CTK()
cx_initialize_OpenIGTLinkIO()

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

set(PLUGIN_SRCS
    cxOpenIGTLinkPluginActivator.cpp

    guiExtenderService/cxOpenIGTLinkGuiExtenderService.h
    guiExtenderService/cxOpenIGTLinkGuiExtenderService.cpp
    guiExtenderService/cxPlusConnectWidget.h
    guiExtenderService/cxPlusConnectWidget.cpp

    network/cxNetworkHandler.cpp
    network/cxProbeDefinitionFromStringMessages.h
    network/cxProbeDefinitionFromStringMessages.cpp

    streamerService/cxOpenIGTLinkStreamer.cpp
    streamerService/cxOpenIGTLinkStreamerService.h
    streamerService/cxOpenIGTLinkStreamerService.cpp

    trackingSystemService/cxOpenIGTLinkTrackingSystemService.cpp
    trackingSystemService/cxOpenIGTLinkTool.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
    cxOpenIGTLinkPluginActivator.h

    network/cxNetworkHandler.h

    streamerService/cxOpenIGTLinkStreamer.h

    trackingSystemService/cxOpenIGTLinkTrackingSystemService.h
    trackingSystemService/cxOpenIGTLinkTool.h
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
    cxGUIExtenderService
    cxResource
    cxResourceWidgets
    cxResourceVisualization
    cxPluginUtilities
    igtlioLogic
    igtlioDevices
    igtlioConverter
    igtlioGUI
    CTKVisualizationVTKCore
    cxGrabber
    )

set(PLUGIN_OUTPUT_DIR "")
if(CX_WINDOWS)
    #on windows we want dlls to be placed with the executables
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

target_include_directories(${PROJECT_NAME}
    PUBLIC
    .
    ${CMAKE_CURRENT_BINARY_DIR}
    network
    guiExtenderService
    streamerService
    trackingSystemService
)


add_subdirectory(testing)

cx_doc_define_plugin_user_docs("${PROJECT_NAME}" "${CMAKE_CURRENT_SOURCE_DIR}/doc")
cx_add_non_source_file("doc/org_custusx_core_openigtlink_datatransfer.md")
cx_add_non_source_file("doc/org_custusx_core_openigtlink_plus_install.md")



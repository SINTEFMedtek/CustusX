project(org_custusx_core_openigtlink)

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

set(PLUGIN_SRCS
    cxOpenIGTLinkPluginActivator.cpp

    guiExtenderService/cxOpenIGTLinkGuiExtenderService.h
	dataTransfer/cxOpenIGTLinkDataTransferWidget.cpp
	dataTransfer/cxOpenIGTLinkDataTransfer.cpp

    guiExtenderService/cxOpenIGTLinkGuiExtenderService.cpp
    guiExtenderService/cxOpenIGTLinkConnectionWidget.cpp
	guiExtenderService/cxNetworkConnectionsWidget.cpp

    trackingSystemService/cxOpenIGTLinkTrackingSystemService.cpp
    trackingSystemService/cxOpenIGTLinkTool.cpp

    streamerService/cxOpenIGTLinkStreamerService.h
    streamerService/cxOpenIGTLinkStreamerService.cpp
    streamerService/cxOpenIGTLinkStreamer.cpp

#    openigtlinkClient/cxSocket.cpp
    openigtlinkClient/cxOpenIGTLinkClient.cpp
    openigtlinkClient/cxDialect.cpp
    openigtlinkClient/cxPlusDialect.h
    openigtlinkClient/cxPlusDialect.cpp
    openigtlinkClient/cxCustusDialect.h
    openigtlinkClient/cxCustusDialect.cpp
	openigtlinkClient/cxRASDialect.h
	openigtlinkClient/cxRASDialect.cpp
	openigtlinkClient/cxNetworkConnectionManager.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
    cxOpenIGTLinkPluginActivator.h

	dataTransfer/cxOpenIGTLinkDataTransferWidget.h
	dataTransfer/cxOpenIGTLinkDataTransfer.h
	guiExtenderService/cxOpenIGTLinkConnectionWidget.h
	guiExtenderService/cxNetworkConnectionsWidget.h

    trackingSystemService/cxOpenIGTLinkTrackingSystemService.h
    trackingSystemService/cxOpenIGTLinkTool.h

    streamerService/cxOpenIGTLinkStreamer.h

#    openigtlinkClient/cxSocket.h
    openigtlinkClient/cxOpenIGTLinkClient.h
    openigtlinkClient/cxDialect.h
	openigtlinkClient/cxNetworkConnectionManager.h
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
	cxResourceVisualization
	cxResource
    cxPluginUtilities
    cxGrabber
    cxGUIExtenderService
    OpenIGTLink
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

target_include_directories(${PROJECT_NAME}
    PUBLIC
    .
    ${CMAKE_CURRENT_BINARY_DIR}
    openigtlinkClient
    trackingSystemService
    streamerService
    guiExtenderService
	dataTransfer
)


add_subdirectory(testing)


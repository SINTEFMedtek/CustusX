project(org_custusx_core_video)

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

include_directories(
    ${CX_RESOURCE_INCLUDE_DIRECTORIES}
    ${CustusX_SOURCE_DIR}/source/logic
    ${CustusX_BINARY_DIR}/source/logic

	${CX_RESOURCE_INCLUDE_DIRECTORIES}
	${CX_RESOURCE_USRECONSTRUCTIONTYPES_INCLUDE_DIRECTORIES}
	.
	${CustusX_SOURCE_DIR}/source/resource/videoServer
	${CustusX_SOURCE_DIR}/source/resource/OpenIGTLinkUtilities
	${CMAKE_CURRENT_BINARY_DIR}
	${CustusX_SOURCE_DIR}/source/resource/pluginUtilities
	${CustusX_BINARY_DIR}/source/resource/usReconstructionTypes
	${CustusX_BINARY_DIR}/source/resource/videoServer
	${CustusX_BINARY_DIR}/source/resource/OpenIGTLinkUtilities
	${CustusX_BINARY_DIR}/source/service/video
	${CustusX_SOURCE_DIR}/source/service/video
	)

set(PLUGIN_SRCS
  cxVideoPluginActivator.cpp
  cxVideoImplService.cpp
  cxVideoImplService.h

    cxBasicVideoSource.h
	cxBasicVideoSource.cpp
    cxVideoConnection.h
    cxVideoConnection.cpp
    cxPlaybackUSAcquisitionVideo.cpp

    cxImageReceiverThread.h
    cxImageReceiverThread.cpp
    
    cxVideoServiceBackend.h
    cxVideoServiceBackend.cpp
    cxStreamerServiceUtilities.h
    cxStreamerServiceUtilities.cpp
	cxIGTLinkStreamerService.cpp
	cxIGTLinkClientStreamer.cpp

	cxOpenCVStreamerService.h
	cxOpenCVStreamerService.cpp
	cxImageFileStreamerService.h
	cxImageFileStreamerService.cpp
	cxLocalServerStreamerServer.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
  cxVideoPluginActivator.h

    cxPlaybackUSAcquisitionVideo.h
    cxBasicVideoSource.h
    cxVideoConnection.h
    cxImageReceiverThread.h
	cxIGTLinkStreamerService.h
	cxIGTLinkClientStreamer.h
	cxOpenCVStreamerService.h
	cxLocalServerStreamerServer.h
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
    cxPluginUtilities
    OpenIGTLink
    cxGrabber
	cxResourceUsReconstructionTypes
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

add_subdirectory(testing)


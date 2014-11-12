project(org_custusx_core_tracking)

cx_initialize_IGSTK()

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

include_directories(
    ${CX_RESOURCE_INCLUDE_DIRECTORIES}
#    ${CustusX_SOURCE_DIR}/source/logic
#    ${CustusX_SOURCE_DIR}/source/service/patient

#    ${CustusX_BINARY_DIR}/source/logic
#    ${CustusX_BINARY_DIR}/source/service/patient

	${CX_RESOURCE_INCLUDE_DIRECTORIES}
	${IGSTK_LIBRARY_DIRS}
	./trackingSystemIGSTK
	./trackingSystemPlayback
	.
    )

set(PLUGIN_SRCS
  cxTrackingPluginActivator.cpp
  cxTrackingImplService.cpp
    cxProbeImpl.h
    cxProbeImpl.cpp
    cxManualToolAdapter.h
    cxManualToolAdapter.cpp
    cxToolConfigurationParser.h
    cxToolConfigurationParser.cpp
	cxTrackerConfigurationImpl.h
	cxTrackerConfigurationImpl.cpp
	cxTrackingSystemDummyService.cpp

	trackingSystemPlayback/cxPlaybackTool.h
    trackingSystemPlayback/cxPlaybackTool.cpp
	trackingSystemPlayback/cxTrackingSystemPlaybackService.cpp

    trackingSystemIGSTK/cxToolUsingIGSTK.h
    trackingSystemIGSTK/cxToolUsingIGSTK.cpp
    trackingSystemIGSTK/cxIgstkTracker.h
    trackingSystemIGSTK/cxIgstkTracker.cpp
    trackingSystemIGSTK/cxIgstkTool.h
    trackingSystemIGSTK/cxIgstkTool.cpp
    trackingSystemIGSTK/cxIgstkToolManager.h
    trackingSystemIGSTK/cxIgstkToolManager.cpp
    trackingSystemIGSTK/cxIgstkTrackerThread.h
    trackingSystemIGSTK/cxIgstkTrackerThread.cpp
	trackingSystemIGSTK/cxTrackingSystemIGSTKService.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
  cxTrackingPluginActivator.h
  cxTrackingImplService.h
	cxProbeImpl.h
	trackingSystemIGSTK/cxToolUsingIGSTK.h
    trackingSystemIGSTK/cxIgstkTracker.h
    cxManualToolAdapter.h
	cxTrackingSystemDummyService.h
    trackingSystemPlayback/cxPlaybackTool.h
	trackingSystemPlayback/cxTrackingSystemPlaybackService.h
    trackingSystemIGSTK/cxIgstkTool.h
    trackingSystemIGSTK/cxIgstkToolManager.h
    trackingSystemIGSTK/cxIgstkTrackerThread.h
	trackingSystemIGSTK/cxTrackingSystemIGSTKService.h
)

# Qt Designer files which should be processed by Qts uic
set(PLUGIN_UI_FORMS
)

# QRC Files which should be compiled into the plugin
set(PLUGIN_resources
)

cx_remove_duplicate_include_directories()

#Compute the plugin dependencies
ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)
set(PLUGIN_target_libraries 
    LINK_PUBLIC cxResource cxPluginUtilities 
    ${PLUGIN_target_libraries}   
    #cxResource
    #cxLogicManager
#    cxResource cxPluginUtilities 
#    IGSTK
    LINK_PRIVATE IGSTK
    )


set(PLUGIN_OUTPUT_DIR "")
if(CX_WINDOWS)
    #on windows we want dlls to be placed with the executables
    set(PLUGIN_OUTPUT_DIR "../")
endif(CX_WINDOWS)

#add_library( cxTrackingService ${cxTrackingService_SOURCE} ${MOC_HEADER_FILES})
#target_link_libraries(cxTrackingService LINK_PUBLIC cxResource LINK_PRIVATE IGSTK)

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


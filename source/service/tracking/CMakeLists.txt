###########################################################
##               Tracking Service
###########################################################

cx_initialize_IGSTK()

SET( CX_QT_MOC_HEADER_FILES
    cxToolManagerUsingIGSTK.h
    cxProbeImpl.h
    trackingSystemIGSTK/cxToolUsingIGSTK.h
    trackingSystemIGSTK/cxIgstkTracker.h
    cxManualToolAdapter.h
	cxTrackingSystemService.h
	cxTrackingSystemDummyService.h
    trackingSystemPlayback/cxPlaybackTool.h
	trackingSystemPlayback/cxTrackingSystemPlaybackService.h
    trackingSystemIGSTK/cxIgstkTool.h
    trackingSystemIGSTK/cxIgstkToolManager.h
    trackingSystemIGSTK/cxIgstkTrackerThread.h
	trackingSystemIGSTK/cxTrackingSystemIGSTKService.h
)

SET ( cxTrackingService_SOURCE
    ${CX_QT_MOC_HEADER_FILES}
    cxToolManagerUsingIGSTK.h
    cxToolManagerUsingIGSTK.cpp
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

include_directories(
	${CX_RESOURCE_INCLUDE_DIRECTORIES}
	${IGSTK_LIBRARY_DIRS}
	./trackingSystemIGSTK
	./trackingSystemPlayback
	.
	)

cx_remove_duplicate_include_directories()
QT5_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

cx_create_export_header("cxTrackingService")

add_library( cxTrackingService ${cxTrackingService_SOURCE} ${MOC_HEADER_FILES})
target_link_libraries(cxTrackingService LINK_PUBLIC cxResource LINK_PRIVATE IGSTK)

ADD_SUBDIRECTORY( testing )

###########################################################
##               Tracking Service
###########################################################

cx_initialize_IGSTK()

SET( CX_QT_MOC_HEADER_FILES
    cxToolManagerUsingIGSTK.h
    cxProbeImpl.h
    cxToolUsingIGSTK.h
    cxIgstkTracker.h
    cxManualToolAdapter.h
    cxIgstkTool.h
    cxIgstkToolManager.h
    cxIgstkTrackerThread.h
    cxDominantToolProxy.h
    cxPlaybackTool.h
	cxTrackingSystemIGSTKService.h
	cxTrackingSystemService.h
	cxTrackingSystemDummyService.h
	cxTrackingSystemPlaybackService.h
)

SET ( cxTrackingService_SOURCE
    ${CX_QT_MOC_HEADER_FILES}
    cxToolManagerUsingIGSTK.h
    cxToolManagerUsingIGSTK.cpp
    cxProbeImpl.h
    cxProbeImpl.cpp
    cxToolUsingIGSTK.h
    cxToolUsingIGSTK.cpp
    cxManualToolAdapter.h
    cxManualToolAdapter.cpp
    cxIgstkTracker.h
    cxIgstkTracker.cpp
    cxToolConfigurationParser.h
    cxToolConfigurationParser.cpp
    cxIgstkTool.h
    cxIgstkTool.cpp
    cxIgstkToolManager.h
    cxIgstkToolManager.cpp
    cxIgstkTrackerThread.h
    cxIgstkTrackerThread.cpp
    cxDominantToolProxy.h
    cxDominantToolProxy.cpp
    cxPlaybackTool.h
    cxPlaybackTool.cpp
	cxTrackerConfigurationImpl.h
	cxTrackerConfigurationImpl.cpp
	cxTrackingSystemIGSTKService.cpp
	cxTrackingSystemDummyService.cpp
	cxTrackingSystemPlaybackService.cpp
)

include_directories(${CX_RESOURCE_INCLUDE_DIRECTORIES} ${IGSTK_LIBRARY_DIRS})

cx_remove_duplicate_include_directories()
QT5_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

cx_create_export_header("cxTrackingService")

add_library( cxTrackingService ${cxTrackingService_SOURCE} ${MOC_HEADER_FILES})
target_link_libraries(cxTrackingService LINK_PUBLIC cxResource LINK_PRIVATE IGSTK)

ADD_SUBDIRECTORY( testing )

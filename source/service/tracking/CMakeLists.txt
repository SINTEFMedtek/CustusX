###########################################################
##               Tracking Service
###########################################################


SET( CX_QT_MOC_HEADER_FILES
    cxToolManager.h
    cxProbe.h
    cxTool.h
    cxIgstkTracker.h
    cxManualToolAdapter.h
    cxIgstkTool.h
    cxIgstkToolManager.h
    cxIgstkTrackerThread.h
    cxDominantToolProxy.h
    cxPlaybackTool.h
)

SET ( cxTrackingService_SOURCE
    ${CX_QT_MOC_HEADER_FILES}
    cxToolManager.h
    cxToolManager.cpp
    cxProbe.h
    cxProbe.cpp
    cxTool.h
    cxTool.cpp
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
)

include_directories(
    .
    ${CustusX3_SOURCE_DIR}/source/resource
    ${CustusX3_SOURCE_DIR}/source/resource/settings
    ${CustusX3_SOURCE_DIR}/source/resource/utilities
    )

QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

ADD_LIBRARY ( cxTrackingService ${cxTrackingService_SOURCE} ${MOC_HEADER_FILES})
target_link_libraries(cxTrackingService SSC IGSTK cxResource)

ADD_SUBDIRECTORY( testing )

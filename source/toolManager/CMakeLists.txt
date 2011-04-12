SET (LIB toolManager)

SET( CX_QT_MOC_HEADER_FILES
    cxToolManager.h
    cxProbe.h
    cxTool.h
    cxIgstkTracker.h
    cxManualToolAdapter.h
    cxIgstkTool.h
    cxIgstkToolManager.h
    cxIgstkTrackerThread.h
)

SET ( ${LIB}_toolManager
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
)

QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

ADD_LIBRARY ( ${LIB} ${${LIB}_toolManager} ${MOC_HEADER_FILES})
target_link_libraries(${LIB} SSC IGSTK cxCommon)

ADD_SUBDIRECTORY( testing )

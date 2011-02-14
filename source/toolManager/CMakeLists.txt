SET (LIB toolManager)

SET( CX_QT_MOC_HEADER_FILES
    cxToolManager.h
    cxProbe.h
    cxTool.h
    cxTracker.h
    cxManualToolAdapter.h
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
    cxTracker.h
    cxTracker.cpp
    cxToolConfigurationParser.h
    cxToolConfigurationParser.cpp
)

QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

ADD_LIBRARY ( ${LIB} ${${LIB}_toolManager} ${MOC_HEADER_FILES})
target_link_libraries(${LIB} SSC IGSTK cxCommon)

ADD_SUBDIRECTORY( testing )

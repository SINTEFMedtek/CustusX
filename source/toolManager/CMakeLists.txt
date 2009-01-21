SET (LIB toolManager)

SET( CX_QT_MOC_HEADER_FILES
    cxToolManager.h
    cxTool.h
    cxTracker.h
)

SET ( ${LIB}_toolManager
    ${CX_QT_MOC_HEADER_FILES}
    cxToolManager.cpp
    cxTool.cpp
    cxTracker.cpp
)

QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

ADD_LIBRARY ( ${LIB} STATIC ${${LIB}_toolManager} ${MOC_HEADER_FILES})

SET (LIB viewManager)

SET( CX_QT_MOC_HEADER_FILES
	cxView2D.h
	cxView3D.h
	cxViewManager.h
)

SET ( ${LIB}_viewManager
    ${CX_QT_MOC_HEADER_FILES}
    cxView2D.cpp
    cxView2D.h
    cxView3D.cpp
    cxView3D.h
    cxViewManager.cpp
    cxViewManager.h
)

QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

ADD_LIBRARY ( ${LIB} STATIC ${${LIB}_viewManager} ${MOC_HEADER_FILES})

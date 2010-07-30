SET (LIB viewManager)

SET( CX_QT_MOC_HEADER_FILES
	cxView2D.h
	cxView3D.h
  cxViewWrapper2D.h
  cxViewWrapper3D.h
	cxViewManager.h
	cxViewWrapper.h
	cxViewGroup.h
	#cxViewGroupInria.h
)

SET ( ${LIB}_viewManager
    ${CX_QT_MOC_HEADER_FILES}
    cxView2D.cpp
    cxView2D.h
    cxView3D.cpp
    cxView3D.h
    cxViewWrapper2D.cpp
    cxViewWrapper2D.h
    cxViewWrapper3D.cpp
    cxViewWrapper3D.h
    cxViewWrapper.h
    cxViewWrapper.cpp
    cxViewManager.cpp
    cxViewManager.h
    cxViewGroup.h
    cxViewGroup.cpp
    cxLayoutData.h
    cxLayoutData.cpp
    #cxViewGroupInria.h
    #cxViewGroupInria.cpp
    cxTreeModelItem.cpp
)

QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

ADD_LIBRARY ( ${LIB} ${${LIB}_viewManager} ${MOC_HEADER_FILES})
target_link_libraries(${LIB} SSC repManager cxCommon)


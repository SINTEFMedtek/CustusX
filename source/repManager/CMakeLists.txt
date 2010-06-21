SET (LIB repManager)

SET( CX_QT_MOC_HEADER_FILES
    cxLandmarkRep.h
    cxRepManager.h
)

SET ( ${LIB}_repManager
    cxLandmarkRep.h
    cxLandmarkRep.cpp
    cxRepManager.h
    cxRepManager.cpp
)

QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

ADD_LIBRARY ( ${LIB} STATIC ${${LIB}_repManager} ${MOC_HEADER_FILES})

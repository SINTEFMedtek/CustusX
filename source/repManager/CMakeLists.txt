SET (LIB repManager)

SET( CX_QT_MOC_HEADER_FILES
    cxInriaRep3D.h
    cxInriaRep2D.h
    cxLandmarkRep.h
    cxRepManager.h
)

SET ( ${LIB}_repManager
    cxInriaRep3D.cpp
    cxInriaRep2D.cpp
    cxLandmarkRep.cpp
    cxRepManager.cpp
)

QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

ADD_LIBRARY ( ${LIB} STATIC ${${LIB}_repManager} ${MOC_HEADER_FILES})

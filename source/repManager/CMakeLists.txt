SET (LIB repManager)

SET( CX_QT_MOC_HEADER_FILES
    cxLandmarkRep.h
    sscOrientationAnnotation3DRep.h
    cxRepManager.h
)

SET ( ${LIB}_repManager
    cxLandmarkRep.h
    cxLandmarkRep.cpp
    cxImageLandmarkRep.h
    cxImageLandmarkRep.cpp
    cxPatientLandmarkRep.h
    cxPatientLandmarkRep.cpp
    sscOrientationAnnotation3DRep.h
    sscOrientationAnnotation3DRep.cpp
    cxRepManager.h
    cxRepManager.cpp
)

QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

ADD_LIBRARY ( ${LIB} ${${LIB}_repManager} ${MOC_HEADER_FILES})
target_link_libraries(${LIB} SSC toolManager dataManager)

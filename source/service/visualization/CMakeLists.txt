###########################################################
##               Visualization Service
##
##  Dependencies:
##     - entire service layer, resource layer
##  TODO: find out if this is a service or part of the logic layer (because of the service layer dependencies)
##
###########################################################

SET( CX_QT_MOC_HEADER_FILES
    cxView2D.h
    cxView3D.h
    cxViewWrapper2D.h
    cxViewWrapper3D.h
    cxViewWrapperVideo.h
    cxViewManager.h
    cxViewWrapper.h
    cxViewGroup.h
    cxInteractiveClipper.h
    cxInteractiveCropper.h
    cxCameraControl.h
    cxCameraStyle.h
    Rep/cxLandmarkRep.h
    Rep/sscOrientationAnnotation3DRep.h
    Rep/sscOrientationAnnotation2DRep.h
    cxRepManager.h
)

SET ( cxVisualizationService_SOURCE
    ${CX_QT_MOC_HEADER_FILES}
    cxView2D.cpp
    cxView2D.h
    cxView3D.cpp
    cxView3D.h
    cxViewWrapper2D.cpp
    cxViewWrapper2D.h
    cxViewWrapperVideo.cpp
    cxViewWrapperVideo.h
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
    cxInteractiveClipper.h
    cxInteractiveClipper.cpp
    cxInteractiveCropper.h
    cxInteractiveCropper.cpp
    cxCameraControl.h
    cxCameraControl.cpp
    cxCameraStyle.h
    cxCameraStyle.cpp
    Rep/cxLandmarkRep.h
    Rep/cxLandmarkRep.cpp
#    Rep/cxImageLandmarkRep.h
#    Rep/cxImageLandmarkRep.cpp
#    Rep/cxPatientLandmarkRep.h
#    Rep/cxPatientLandmarkRep.cpp
    Rep/sscOrientationAnnotation3DRep.h
    Rep/sscOrientationAnnotation3DRep.cpp
    Rep/sscOrientationAnnotation2DRep.h
    Rep/sscOrientationAnnotation2DRep.cpp
    cxRepManager.h
    cxRepManager.cpp
    )

include_directories(
    .
    ${CustusX3_SOURCE_DIR}/source/resource
    ${CustusX3_SOURCE_DIR}/source/resource/settings
    ${CustusX3_SOURCE_DIR}/source/resource/utilities
    ${CustusX3_SOURCE_DIR}/source/resource/algorithms
    ${CustusX3_SOURCE_DIR}/source/service/patient
    ${CustusX3_SOURCE_DIR}/source/service/tracking
    ${CustusX3_SOURCE_DIR}/source/service/video
    )

QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

include_directories(Rep)

ADD_LIBRARY ( cxVisualizationService ${cxVisualizationService_SOURCE} ${MOC_HEADER_FILES})
target_link_libraries(cxVisualizationService SSC cxSettings cxVideoService cxPatientService cxTrackingService cxAlgorithms_Res)


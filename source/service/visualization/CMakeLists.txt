###########################################################
##               Visualization Service
##
##  Dependencies:
##     - entire service layer, resource layer
##  TODO: find out if this is a service or part of the logic layer (because of the service layer dependencies)
##
###########################################################

cx_add_class_qt_moc(cxVisualizationService_SOURCE
    cxViewWrapper2D
    cxViewWrapper3D
    cxViewWrapperVideo
    cxViewManager
    cxViewWrapper
    cxViewGroup
    cxInteractiveClipper
    cxInteractiveCropper
    cxCameraControl
    cxCameraStyle
    Rep/cxLandmarkRep
    Rep/sscOrientationAnnotation3DRep
    Rep/sscOrientationAnnotation2DRep
    Rep/sscImage2DRep3D
    cxRepManager
    cxThresholdPreview
    cxMultiVolume3DRepProducer
    cxViewGroupData
    cxAxisConnector
    )

cx_add_class(cxVisualizationService_SOURCE
    cxLayoutData
    )

include_directories(${CX_RESOURCE_INCLUDE_DIRECTORIES})
include_directories(
    .
    ${CustusX3_SOURCE_DIR}/source/service/patient
    ${CustusX3_SOURCE_DIR}/source/service/tracking
    ${CustusX3_SOURCE_DIR}/source/service/video
    )

include_directories(Rep)

ADD_LIBRARY ( cxVisualizationService ${cxVisualizationService_SOURCE} ${MOC_HEADER_FILES})
target_link_libraries(cxVisualizationService SSC cxResource cxVideoService cxPatientService cxTrackingService)

add_subdirectory(testing)

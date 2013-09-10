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
    Rep/cxMehdiGPURayCastMultiVolumeRep
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
    ${CustusX3_SOURCE_DIR}/source/ThirdParty/VTKMultiVolume
    )

include_directories(Rep)

set(cxVisualizationService_LIBS
    SSC
    cxResource
    cxVideoService
    cxPatientService
    cxTrackingService
    )

if(CX_BUILD_MEHDI_VTKMULTIVOLUME)
    set(cxVisualizationService_LIBS
        ${cxVisualizationService_LIBS}
        vtkGPUMultiVolumeRender
        )
endif()

ADD_LIBRARY ( cxVisualizationService ${cxVisualizationService_SOURCE} ${MOC_HEADER_FILES})
target_link_libraries(cxVisualizationService ${cxVisualizationService_LIBS})

add_subdirectory(testing)

###########################################################
##               Visualization Service
##
##  Dependencies:
##     - entire service layer, resource layer
##  TODO: find out if this is a service or part of the logic layer (because of the service layer dependencies)
##
###########################################################

include_directories(${CX_RESOURCE_VISUALIZATION_INCLUDE_DIRECTORIES})

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
	cxCameraStyleForView
    cxRepManager
    cxThresholdPreview
    cxMultiVolume3DRepProducer
    cxViewGroupData
    cxAxisConnector
#	cxLayoutWidget
	cxRenderLoop
	cx2DZoomHandler
	)

cx_add_class(cxVisualizationService_SOURCE
#    cxLayoutData
	cxLayoutRepository
	cxVisualizationServiceBackend
	cxNavigation
	cxDataRepContainer
	)

include_directories(
    ${CX_RESOURCE_VISUALIZATION_INCLUDE_DIRECTORIES}
    .
    ${CustusX_SOURCE_DIR}/source/service/patient
    ${CustusX_SOURCE_DIR}/source/service/tracking
    ${CustusX_SOURCE_DIR}/source/service/video
	${CustusX_SOURCE_DIR}/source/resource/visualization/ViewContainer
	${CustusX_SOURCE_DIR}/source/resource/visualization/View
	)

set(cxVisualizationService_LIBS
    cxResourceVisualization
    cxVideoService
    cxPatientService
    cxTrackingService
    )

ADD_LIBRARY ( cxVisualizationService ${cxVisualizationService_SOURCE} ${MOC_HEADER_FILES})
target_link_libraries(cxVisualizationService ${cxVisualizationService_LIBS})

add_subdirectory(testing)

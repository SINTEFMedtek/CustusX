###########################################################
##               Resource Visualization Library
###########################################################

find_package(GLEW REQUIRED)

cx_add_class_qt_moc(CX_RESOURCE_VISUALIZATION_FILES
    View/cxView
    View/cxViewCollectionWidget

    View/internal/cxViewRepCollection
    View/internal/ViewMixed/cxViewCollectionWidgetMixed
    View/internal/ViewWidget/cxViewWidget
    View/internal/ViewWidget/cxViewCollectionWidgetUsingViewWidgets
    View/internal/ViewContainer/cxViewContainerItem
    View/internal/ViewContainer/cxViewContainer
    View/internal/ViewContainer/cxViewCollectionWidgetUsingViewContainer

    Rep/cxRep.h
    Rep/cxRepImpl
    Rep/cxDataMetricRep

    Rep3D/cxToolRep3D
    Rep3D/cxPickerRep
    Rep3D/cxVolumetricRep
    Rep3D/cxTexture3DSlicerRep
    Rep3D/cxSlices3DRep
    Rep3D/cxSlicePlanes3DRep
    Rep3D/cxGeometricRep
    Rep3D/cxFiberBundleRep
    Rep3D/cxPointMetricRep
    Rep3D/cxDistanceMetricRep
    Rep3D/cxAngleMetricRep
    Rep3D/cxPlaneMetricRep
    Rep3D/cxFrameMetricRep
    Rep3D/cxToolMetricRep
    Rep3D/cxLandmarkRep
    Rep3D/cxOrientationAnnotation3DRep
    Rep3D/cxImage2DRep3D
    Rep3D/cxSphereMetricRep
    Rep3D/cxDonutMetricRep
    Rep3D/cxCustomMetricRep
    Rep3D/cxMetricNamesRep
    Rep3D/cxStreamRep3D
    Rep3D/cxStream2DRep3D
	Rep3D/cxRegionOfInterestMetricRep

    Rep2D/cxOrientationAnnotationRep
    Rep2D/cxSlicerRepSW
    Rep2D/cxToolRep2D
    Rep2D/cxCrossHairRep2D
    Rep2D/cxGeometricRep2D
    Rep2D/cxPointMetricRep2D
    Rep2D/cxSphereMetricRep2D
    Rep2D/cxGuideRep2D
    Rep2D/cxOrientationAnnotation2DRep

    Primitives/cxImageMapperMonitor
    Primitives/cxTexture3DSlicerProxy
    Primitives/cxSlicePlaneClipper
    Primitives/cxToolTracer
    Primitives/cxVolumeProperty
    Primitives/cxVideoSourceGraphics
    Primitives/cxViewFollower
	Primitives/cxGraphicalBox

    RepVideo/cxVideoRep

    cxViewService
    cxViewGroupData
    cxLayoutRepository
    cxInteractiveClipper
    cxInteractiveCropper
    cxCameraControl
    cxCameraStyleInteractor
    cxRenderWindowFactory
)

if(CX_BUILD_MEHDI_VTKMULTIVOLUME)
    cx_add_class_qt_moc(CX_RESOURCE_VISUALIZATION_FILES
        Rep3D/cxMehdiGPURayCastMultiVolumeRep
        )
endif()

if (CX_APPLE)
  set(CX_RESOURCE_VISUALIZATION_FILES
    ${CX_RESOURCE_VISUALIZATION_FILES}
    View/internal/cxOSXHelper.mm
    )
endif()

cx_add_class( CX_RESOURCE_VISUALIZATION_FILES
    View/cxLayoutData
	View/cxViewCollectionVideoSource
    View/internal/cxViewUtilities
	View/internal/ViewWidget/cxViewCache
	View/internal/ViewWidget/cxMultiViewCache
	View/internal/ViewWidget/cxViewLinkingViewWidget
    View/internal/ViewContainer/cxViewLinkingViewContainerItem
	View/cxViewCollectionImageWriter
	View/cxScreenShotImageWriter
	cxViewServiceNull
    cxViewServiceProxy
    cxVisServices
    cxNavigation
    Primitives/cxDepthPeeling
    Primitives/cxGraphicalTorus3D
    Primitives/cxVtkHelperClasses
#    Primitives/cxTextureSlicePainter
    Primitives/cxUltrasoundSectorSource
    Rep3D/cxAxesRep
    Rep/cxDisplayTextRep
    Primitives/cxVideoGraphics
    Primitives/cxGraphicalPrimitives
    Primitives/cxGraphicalAxes3D
    Primitives/cxImageEnveloper
    Primitives/cxGraphicalDisk
    Primitives/cxGraphicalObjectWithDirection
	Primitives/cxSliceAutoViewportCalculator
	Primitives/cxShaderCallback
	Primitives/cxOpenGLShaders
	cxRepContainer
	cxSharedOpenGLContext
	cxSharedContextCreatedCallback
)

configure_file(Rep/Shaders/glsl/upscale.frag Rep COPYONLY)
configure_file(Rep/Shaders/glsl/upscale.vert Rep COPYONLY)
configure_file(Rep/Shaders/glsl/raycasting_shader.frag Rep COPYONLY)
configure_file(Rep/Shaders/glsl/raycasting_shader.vert Rep COPYONLY)

cx_create_export_header("cxResourceVisualization")
qt5_wrap_cpp( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

set(CX_RESOURCE_VISUALIZATION_LIBRARIES
    cxResource
    cxPluginUtilities
    ${OPENGL_LIBRARIES}
    ${Boost_LIBRARIES}
    ${ITK_LIBRARIES}
)

if(CX_BUILD_MEHDI_VTKMULTIVOLUME)
    set(CX_RESOURCE_VISUALIZATION_LIBRARIES
        ${CX_RESOURCE_VISUALIZATION_LIBRARIES}
        vtkGPUMultiVolumeRender
    )
endif()


add_library(cxResourceVisualization ${CX_RESOURCE_VISUALIZATION_FILES})
target_include_directories(cxResourceVisualization
    PUBLIC
    .
    ./View
    ./Rep
    ./Rep2D
    ./Rep3D
    ./RepVideo
    ./Primitives
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CX_RESOURCE_INCLUDE_DIRECTORIES}

    ./View/internal
    ./View/internal/ViewContainer
    ./View/internal/ViewMixed
    ./View/internal/ViewWidget
    ${GLEW_INCLUDE_DIRS}
)
target_link_libraries(cxResourceVisualization
    PRIVATE
    ${CX_RESOURCE_VISUALIZATION_LIBRARIES}
#    ${GLEW_LIBRARY}
)

add_subdirectory(testing)

###########################################################
##               Resource Visualization Library
###########################################################

include_directories( ${CX_RESOURCE_INCLUDE_DIRECTORIES} )

cx_add_class_qt_moc(CX_RESOURCE_VISUALIZATION_FILES
	View/cxView
	View/cxLayoutWidget

	View/internal/cxViewUtilities
	View/internal/cxViewRepCollection
	View/internal/ViewMixed/cxLayoutWidgetMixed
	View/internal/ViewWidget/cxViewLinkingViewWidget
	View/internal/ViewWidget/cxViewWidget
	View/internal/ViewWidget/cxViewWidgetLayout
	View/internal/ViewContainer/cxViewContainerItem
	View/internal/ViewContainer/cxViewContainer	
	View/internal/ViewContainer/cxViewLinkingViewContainerItem
	View/internal/ViewContainer/cxViewCollectionLayout	
    
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
	Rep3D/cxMetricNamesRep

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

	RepVideo/cxVideoRep
        )

if(CX_BUILD_MEHDI_VTKMULTIVOLUME)
    cx_add_class_qt_moc(CX_RESOURCE_VISUALIZATION_FILES
        Rep3D/cxMehdiGPURayCastMultiVolumeRep
        )
endif()

cx_add_class( CX_RESOURCE_VISUALIZATION_FILES
	View/cxLayoutData
	View/internal/ViewWidget/cxViewCache.h
	Primitives/cxDepthPeeling
	Primitives/cxGraphicalTorus3D
	Primitives/cxVtkHelperClasses
	Primitives/cxTextureSlicePainter
	Primitives/cxUltrasoundSectorSource
    Rep3D/cxAxesRep
    Rep/cxDisplayTextRep
    Primitives/cxVideoGraphics
    Primitives/cxGraphicalPrimitives
    Primitives/cxGraphicalAxes3D
    Primitives/cxImageEnveloper
	Primitives/cxGraphicalDisk
    )

set( CX_RESOURCE_VISUALIZATION_LIBRARIES ${CX_RESOURCE_VISUALIZATION_LIBRARIES}
	vtkRenderingVolumeOpenGL
	vtkGUISupportQt
	vtkRenderingAnnotation
	vtkFiltersModeling
	vtkInteractionWidgets
	vtkFiltersTexture
#    vtkIO
#    vtkRendering
#    vtkImaging
#   vtkCommon
#    vtkHybrid
#    vtkWidgets
#    vtkFiltering
#    vtkVolumeRendering
    )

# =========================================================
#if(USE_GLX_SHARED_CONTEXT)
#    SET( CX_RESOURCE_VISUALIZATION_LIBRARIES
#        ${CX_RESOURCE_VISUALIZATION_LIBRARIES}
#        GLU
#        )
#endif(USE_GLX_SHARED_CONTEXT)



configure_file(Rep/Shaders/glsl/upscale.frag Rep COPYONLY)
configure_file(Rep/Shaders/glsl/upscale.vert Rep COPYONLY)
configure_file(Rep/Shaders/glsl/raycasting_shader.frag Rep COPYONLY)
configure_file(Rep/Shaders/glsl/raycasting_shader.vert Rep COPYONLY)
#configure_file(Rep/Shaders/glsl/Texture3DOverlay.frag Rep COPYONLY)

#set(SHADER_FROM_PATH ${CustusX_SOURCE_DIR}/source/resource/ssc/Code/Rep/Shaders/glsl)
#set(SHADER_TO_PATH ${CustusX_SOURCE_DIR}/config/shaders)
#configure_file(${SHADER_FROM_PATH}/upscale.frag ${SHADER_TO_PATH} COPYONLY)
#configure_file(${SHADER_FROM_PATH}/upscale.vert ${SHADER_TO_PATH} COPYONLY)
#configure_file(${SHADER_FROM_PATH}/raycasting_shader.frag ${SHADER_TO_PATH} COPYONLY)
#configure_file(${SHADER_FROM_PATH}/raycasting_shader.vert ${SHADER_TO_PATH} COPYONLY)



unset(CX_RESOURCE_VISUALIZATION_INCLUDE_DIRECTORIES)
set(CX_RESOURCE_VISUALIZATION_INCLUDE_DIRECTORIES
    ${CX_RESOURCE_INCLUDE_DIRECTORIES}
    ${CustusX_SOURCE_DIR}/source/resource/visualization
    ${CustusX_SOURCE_DIR}/source/resource/visualization/Rep
    ${CustusX_SOURCE_DIR}/source/resource/visualization/Rep2D
    ${CustusX_SOURCE_DIR}/source/resource/visualization/Rep3D
    ${CustusX_SOURCE_DIR}/source/resource/visualization/RepVideo
    ${CustusX_SOURCE_DIR}/source/resource/visualization/Primitives
	${CustusX_SOURCE_DIR}/source/resource/visualization/View
	CACHE INTERNAL "Include directories for resource visualization library")

include_directories(${CX_RESOURCE_VISUALIZATION_INCLUDE_DIRECTORIES}
    ${CustusX_SOURCE_DIR}/source/ThirdParty/VTKMultiVolume
	View/internal
	View/internal/ViewWidget
	View/internal/ViewContainer
	View/internal/ViewMixed
    )

QT5_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

set(CX_RESOURCE_VISUALIZATION_LIBRARIES
    ${CX_RESOURCE_VISUALIZATION_LIBRARIES}
    cxResource
	#QVTK
    ${SSC_GCOV_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${Boost_LIBRARIES}
    ${ITK_LIBRARIES})

if(CX_BUILD_MEHDI_VTKMULTIVOLUME)
    set(CX_RESOURCE_VISUALIZATION_LIBRARIES
        ${CX_RESOURCE_VISUALIZATION_LIBRARIES}
        vtkGPUMultiVolumeRender
        )
endif()

ADD_LIBRARY ( cxResourceVisualization ${CX_RESOURCE_VISUALIZATION_FILES})
target_link_libraries(cxResourceVisualization ${CX_RESOURCE_VISUALIZATION_LIBRARIES} )

add_subdirectory(testing)

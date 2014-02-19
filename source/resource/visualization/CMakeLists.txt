###########################################################
##               Resource Visualization Library
###########################################################

include_directories( ${CX_RESOURCE_INCLUDE_DIRECTORIES} )

cx_add_class_qt_moc(CX_RESOURCE_VISUALIZATION_FILES
	sscView
	sscViewContainer
    
	Rep/sscRep.h
	Rep/sscRepImpl
	Rep/sscDataMetricRep

	Rep3D/sscToolRep3D
	Rep3D/sscPickerRep
	Rep3D/sscVolumetricRep
	Rep3D/sscTexture3DSlicerRep
	Rep3D/sscSlices3DRep
	Rep3D/sscSlicePlanes3DRep
	Rep3D/sscGeometricRep
	Rep3D/sscFiberBundleRep
	Rep3D/sscPointMetricRep
	Rep3D/sscDistanceMetricRep
	Rep3D/sscAngleMetricRep
	Rep3D/sscPlaneMetricRep
	Rep3D/cxFrameMetricRep
	Rep3D/cxToolMetricRep
    Rep3D/cxLandmarkRep
    Rep3D/sscOrientationAnnotation3DRep
    Rep3D/sscImage2DRep3D
	Rep3D/cxSphereMetricRep
	Rep3D/cxDonutMetricRep
	Rep3D/cxMetricNamesRep

	Rep2D/sscOrientationAnnotationRep
	Rep2D/sscSlicerRepSW
	Rep2D/sscToolRep2D
	Rep2D/sscCrossHairRep2D
	Rep2D/sscGeometricRep2D
	Rep2D/sscPointMetricRep2D
	Rep2D/sscGuideRep2D
    Rep2D/sscOrientationAnnotation2DRep

    Primitives/cxImageMapperMonitor
	Primitives/sscTexture3DSlicerProxy
	Primitives/sscSlicePlaneClipper
	Primitives/sscToolTracer
    Primitives/cxVolumeProperty
	Primitives/sscVideoSourceGraphics
	Primitives/cxViewFollower

	RepVideo/sscVideoRep
        )

if(CX_BUILD_MEHDI_VTKMULTIVOLUME)
    cx_add_class_qt_moc(CX_RESOURCE_VISUALIZATION_FILES
        Rep3D/cxMehdiGPURayCastMultiVolumeRep
        )
endif()

cx_add_class( CX_RESOURCE_VISUALIZATION_FILES
	Primitives/cxDepthPeeling
	Primitives/cxGraphicalTorus3D
	Primitives/sscVtkHelperClasses
	Primitives/sscTextureSlicePainter
	Primitives/sscUltrasoundSectorSource
    Rep3D/sscAxesRep
    Rep/sscDisplayTextRep
    Primitives/sscVideoGraphics
    Primitives/sscGraphicalPrimitives
    Primitives/cxGraphicalAxes3D
    Primitives/cxImageEnveloper
    )

set( CX_RESOURCE_VISUALIZATION_LIBRARIES ${CX_RESOURCE_VISUALIZATION_LIBRARIES}
    vtkIO
    vtkRendering
    vtkImaging
    vtkCommon
    vtkHybrid
    vtkWidgets
    vtkFiltering
    vtkVolumeRendering
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

#set(SHADER_FROM_PATH ${CustusX3_SOURCE_DIR}/source/resource/ssc/Code/Rep/Shaders/glsl)
#set(SHADER_TO_PATH ${CustusX3_SOURCE_DIR}/config/shaders)
#configure_file(${SHADER_FROM_PATH}/upscale.frag ${SHADER_TO_PATH} COPYONLY)
#configure_file(${SHADER_FROM_PATH}/upscale.vert ${SHADER_TO_PATH} COPYONLY)
#configure_file(${SHADER_FROM_PATH}/raycasting_shader.frag ${SHADER_TO_PATH} COPYONLY)
#configure_file(${SHADER_FROM_PATH}/raycasting_shader.vert ${SHADER_TO_PATH} COPYONLY)



unset(CX_RESOURCE_VISUALIZATION_INCLUDE_DIRECTORIES)
set(CX_RESOURCE_VISUALIZATION_INCLUDE_DIRECTORIES
    ${CX_RESOURCE_INCLUDE_DIRECTORIES}
    ${CustusX3_SOURCE_DIR}/source/resource/visualization
    ${CustusX3_SOURCE_DIR}/source/resource/visualization/Rep
    ${CustusX3_SOURCE_DIR}/source/resource/visualization/Rep2D
    ${CustusX3_SOURCE_DIR}/source/resource/visualization/Rep3D
    ${CustusX3_SOURCE_DIR}/source/resource/visualization/RepVideo
    ${CustusX3_SOURCE_DIR}/source/resource/visualization/Primitives
    CACHE INTERNAL "Include directories for resource visualization library")

include_directories(${CX_RESOURCE_VISUALIZATION_INCLUDE_DIRECTORIES}
    ${CustusX3_SOURCE_DIR}/source/ThirdParty/VTKMultiVolume
    )

QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

set(CX_RESOURCE_VISUALIZATION_LIBRARIES
    ${CX_RESOURCE_VISUALIZATION_LIBRARIES}
    cxResource
    QVTK
    ${SSC_GCOV_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${QT_LIBRARIES}
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
CX_COTIRE_ADD_LIBRARY( cxResourceVisualization "cxResource")

add_subdirectory(testing)

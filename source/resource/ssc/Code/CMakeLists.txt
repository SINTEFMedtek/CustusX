# =========================================================
# Sonowand Sintef Cooperation C++ Library 
# =========================================================
find_package(OpenGL)
find_package(X11)

# =========================================================
# Define variables for paths and include files.
# =========================================================

if(X11_FOUND)
    if(APPLE)
        option(USE_GLX_SHARED_CONTEXT "use shared glx contexts" OFF)
    else(APPLE)
        option(USE_GLX_SHARED_CONTEXT "use shared glx contexts" ON)
    endif(APPLE)
    include_directories( ${X11_INCLUDE_DIR} ) # needed by Mac 10.8+
else(X11_FOUND)
    option(USE_GLX_SHARED_CONTEXT "use shared glx contexts" OFF)
endif(X11_FOUND)

cx_add_class_qt_moc(SSC_SOURCE_FILES
	sscView
	sscViewContainer
	Utilities/sscAcceptanceBoxWidget
	Data/sscData
	Data/sscImage
	Data/sscImageTF3D
	Data/sscImageLUT2D
	Data/sscImageTFData
	Data/sscDataManager
	Data/sscNavigatedVideoImage
	Data/sscRegistrationTransform
	Data/sscDataManagerImpl
	Data/sscMesh
	Data/sscVtkFileMesh
	Data/sscPresets
	Data/sscTransferFunctions3DPresets
	Data/sscPointMetric
	Data/sscDistanceMetric
	Data/sscPlaneMetric
	Data/sscAngleMetric
	Data/sscUnsignedDerivedImage
	Data/cxFrameMetric
        Data/cxToolMetric

	Video/sscVideoSource.h
	Video/sscVideoRecorder
	Video/sscVideoSourceSHM
	Video/sscTestVideoSource
	Video/sscImageImportVideoSource
	Video/sscVideoSourceGraphics

	Video/sscImageImportVideoSource
        
	Tool/sscToolManager
	Tool/sscTool.h
	Tool/sscProbe.h
	Tool/sscDummyTool
	Tool/sscDummyToolManager
	Tool/sscManualTool
	Tool/sscProbeAdapterRTSource
    
	Rep/sscSliceProxy
	Rep/sscRep.h
	Rep/sscSlicerRepSW
	Rep/sscToolRep3D
	Rep/sscPickerRep
	Rep/sscImageBlenderProxy
	Rep/sscVolumetricRep
	Rep/sscSlicePlaneRep
	Rep/sscProgressiveLODVolumetricRep
	Rep/sscBlendedSliceRep
	Rep/sscSlicedImageProxy
	Rep/sscSurfaceRep
	Rep/sscToolRep2D
	Rep/sscCrossHairRep2D
	Rep/sscTexture3DSlicerProxy
	Rep/sscTexture3DSlicerRep
	Rep/sscGPURayCastVolumeRep
	Rep/sscSlices3DRep
	Rep/sscSlicePlanes3DRep
	Rep/sscGeometricRep
	Rep/sscGeometricRep2D
	Rep/sscSlicePlaneClipper
	Rep/sscOrientationAnnotationRep
	Rep/sscVideoRep
	Rep/sscToolTracer
	Rep/sscFiberBundleRep
	Rep/sscDataMetricRep
	Rep/sscPointMetricRep
	Rep/sscPointMetricRep2D
	Rep/sscDistanceMetricRep
	Rep/sscAngleMetricRep
	Rep/sscPlaneMetricRep
	Rep/sscGuideRep2D
	Rep/cxFrameMetricRep
        Rep/cxToolMetricRep

	Widgets/sscOptimizedUpdateWidget
	Widgets/sscDataAdapter
	Widgets/sscDoubleDataAdapter.h
	Widgets/sscStringDataAdapter.h
	Widgets/sscDoubleDataAdapterXml
	Widgets/sscStringDataAdapterXml
	Widgets/sscBoolDataAdapter.h
	Widgets/sscBoolDataAdapterXml
	Widgets/sscConsoleWidget
	Widgets/sscDoubleWidgets
	Widgets/sscLabeledComboBoxWidget
	Widgets/sscLabeledLineEditWidget
	Widgets/sscCheckBoxWidget
	Widgets/sscHelperWidgets
	Widgets/sscFileSelectWidget
	Widgets/sscVector3DDataAdapter.h
	Widgets/sscVector3DDataAdapterXml
	Widgets/sscMousePadWidget
	Widgets/sscColorDataAdapter.h
	Widgets/sscColorDataAdapterXml
	Widgets/sscColorSelectWidget
	Widgets/cxColorSelectButton

	Utilities/sscApplication
	Utilities/sscMessageManager
	Utilities/sscSharedMemory
	)

# library
cx_add_class( SSC_SOURCE_FILES
    sscForwardDeclarations.h
    vtkForwardDeclarations.h

        Video/sscVideoGraphics

        vtkMods/sscViewRenderWindow
        vtkMods/sscViewQVTKWidget

        Tool/sscProbeSector
        Tool/sscProbeData

        Widgets/sscXmlOptionItem

        Rep/sscVtkHelperClasses
        Rep/sscTextureSlicePainter
        Rep/sscGPURayCastVolumePainter
        Rep/sscRepImpl
        Rep/sscUltrasoundSectorSource
        Rep/sscAxesRep
        Rep/sscDisplayTextRep

        Data/sscDataMetric
        Data/sscLandmark
        Data/sscDataReaderWriter
        Data/sscGPUImageBuffer

        Math/sscBoundingBox3D
        Math/sscVector3D
        Math/sscTransform3D
        Math/sscFrame3D

        Utilities/sscGraphicalPrimitives
        Utilities/cxGraphicalAxes3D
        Utilities/sscGLHelpers
        Utilities/sscTime
        Utilities/sscUtilHelpers
        Utilities/sscLogger
        Utilities/sscTypeConversions
        Utilities/sscSliceComputer
        Utilities/sscDefinitions.h
        Utilities/sscDefinitionStrings
        Utilities/sscCustomMetaImage
        Utilities/sscMD5Check
        Utilities/sscSnwInifile
        Utilities/sscIndent
        Utilities/sscCoordinateSystemHelpers
        Utilities/sscViewportListener
        Utilities/sscVolumeHelpers
        Utilities/sscPositionStorageFile
        Utilities/sscTimeKeeper
        Utilities/sscMeshHelpers

    vesselReg/SeansVesselReg.cxx
    vesselReg/SeansVesselReg.hxx
    vesselReg/HackTPSTransform.hxx
    )

set( SSC_INCLUDE_DIRS
    ${SSC_SOURCE_DIR}/Code/  
    ${SSC_SOURCE_DIR}/Code/Data/
    ${SSC_SOURCE_DIR}/Code/Video/
    ${SSC_SOURCE_DIR}/Code/Rep/
    ${SSC_SOURCE_DIR}/Code/Tool/
    ${SSC_SOURCE_DIR}/Code/Utilities/
    ${SSC_SOURCE_DIR}/Code/Math/
    ${SSC_SOURCE_DIR}/Code/Widgets/
    ${SSC_SOURCE_DIR}/Code/3rdParty/
    ${SSC_SOURCE_DIR}/Code/vtkMods/
    ${SSC_BINARY_DIR}
    ${SSC_SOURCE_DIR}/DICOMLib
    ${SSC_BINARY_DIR}/DICOMLib
    )


#set(SSC_LIBRARY_DIRS
#    )

# =========================================================
# linking
# =========================================================

set( SSC_LIBRARIES ${SSC_LIBRARIES}
    QVTK
    ${SSC_GCOV_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${QT_LIBRARIES}
     ${Boost_LIBRARIES})

unset(CX_SSC_INCLUDE_DIRECTORIES)
set(CX_SSC_INCLUDE_DIRECTORIES ${SSC_INCLUDE_DIRS}
        CACHE INTERNAL "Include directories for resource/ssc library")


if(WIN32)
    set( SSC_LIBRARIES ${SSC_LIBRARIES} )
# needed on apple
elseif(APPLE)
    set( SSC_LIBRARIES ${SSC_LIBRARIES} crypto )
# needed on linux
else(WIN32)
    set( SSC_LIBRARIES ${SSC_LIBRARIES} GLU crypto ssl)
endif(WIN32)

set( SSC_LIBRARIES ${SSC_LIBRARIES}
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
if(USE_GLX_SHARED_CONTEXT)
    SET( SSC_LIBRARIES 
        ${SSC_LIBRARIES}
        GLU
        )

    #add moc versions of out modified QVTKWidget
    if (VTK_MAJOR_VERSION EQUAL 5)
        if (VTK_MINOR_VERSION EQUAL 4)
			cx_add_class_qt_moc(SSC_SOURCE_FILES vtkMods/sscSNWQVTKWidget5_4.h)
            SET(SUPPORTED_VTK_VERSION_FOUND "5.4")
        endif (VTK_MINOR_VERSION EQUAL 4)
        if (VTK_MINOR_VERSION EQUAL 5)
			cx_add_class_qt_moc(SSC_SOURCE_FILES vtkMods/sscSNWQVTKWidget5_5.h)
            SET(SUPPORTED_VTK_VERSION_FOUND "5.5")
        endif (VTK_MINOR_VERSION EQUAL 5)
        if (VTK_MINOR_VERSION EQUAL 6)
			cx_add_class_qt_moc(SSC_SOURCE_FILES vtkMods/sscSNWQVTKWidget5_5.h)
            SET(SUPPORTED_VTK_VERSION_FOUND "5.6")
        endif (VTK_MINOR_VERSION EQUAL 6)
        if (VTK_MINOR_VERSION EQUAL 8)
        # no customized qvtkwidget for 5.8+
 #           SET(SSC_QT_MOC_HEADER_FILES ${SSC_QT_MOC_HEADER_FILES} vtkMods/sscSNWQVTKWidget5_5.h)
            SET(SUPPORTED_VTK_VERSION_FOUND "5.8")
        endif (VTK_MINOR_VERSION EQUAL 8)
    endif (VTK_MAJOR_VERSION EQUAL 5)

endif(USE_GLX_SHARED_CONTEXT)


# =========================================================
# stuff
# =========================================================
# Configure the default SSC_DATA_ROOT for the location of SSCData.
unset(SSC_DATA_ROOT)
set (SSC_DATA_ROOT ${CustusX3_SOURCE_DIR}/data CACHE INTERNAL "path to ssc data root")
  
#set(SSC_THUNDER_KERNEL_PATH ${SSC_SOURCE_DIR}/../../source/UsReconstruction/Thunder)

configure_file(
  "${SSC_SOURCE_DIR}/Code/Utilities/sscConfig.h.in"
  "${SSC_BINARY_DIR}/sscConfig.h"
  )

# =========================================================
# create output variables
# =========================================================

# =========================================================

include_directories( ${SSC_INCLUDE_DIRS} ${SSC_BINARY_DIR} ${QT_INCLUDES} ${Boost_INCLUDE_DIRS} )
link_directories( ${SSC_LIBRARY_DIRS})

# =========================================================
# create the target
add_library( SSC ${SSC_SOURCE_FILES} )
target_link_libraries( SSC ${SSC_LIBRARIES})

CX_COTIRE_ADD_LIBRARY(SSC "${SSC_LIBRARIES}")

# =========================================================
# 
# Configure the file describing how to use SSC

#set (SSC_DIR ${SSC_BINARY_DIR} CACHE PATH "where to find the SSC cmake")

#configure_file (
#    "${PROJECT_SOURCE_DIR}/CMake/SSCConfig.cmake.in"
#    "${PROJECT_BINARY_DIR}/SSCConfig.cmake"
#    )
configure_file(Rep/Shaders/glsl/upscale.frag Rep COPYONLY)
configure_file(Rep/Shaders/glsl/upscale.vert Rep COPYONLY)
configure_file(Rep/Shaders/glsl/raycasting_shader.frag Rep COPYONLY)
configure_file(Rep/Shaders/glsl/raycasting_shader.vert Rep COPYONLY)
configure_file(Rep/Shaders/glsl/Texture3DOverlay.frag Rep COPYONLY)
  
# Export our build settings and library dependencies
#include (CMakeExportBuildSettings)
#cmake_export_build_settings (${SSC_BINARY_DIR}/SSCBuildSettings.cmake)
#export_library_dependencies (${SSC_BINARY_DIR}/SSCLibraryDepends.cmake)

#SET_TARGET_PROPERTIES( SSC PROPERTIES COMPILE_FLAGS "${SNW_COMPILE_FLAGS}" LINK_FLAGS "${SNW_LINK_FLAGS}" LINK_LIBS "${SNW_LINK_LIBS}")

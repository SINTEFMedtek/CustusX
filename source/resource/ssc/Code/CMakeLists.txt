cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# =========================================================
# Sonowand Sintef Cooperation C++ Library 
# =========================================================

# =========================================================
# Define variables for paths and include files.
# =========================================================

# Must be the same as the project name. Replaces CMAKE_PROJECT_NAME,
# which I cannot get to work properly
#set( SSC_PROJECT_NAME SSC ) 

#set( SSC_SOURCE_ROOT_DIR ${PROJECT_SOURCE_DIR}/.. )

find_package(X11)
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
#    ${SSC_SOURCE_DIR}/Code/Ultrasound/3DReconstruction
#    ${SSC_SOURCE_DIR}/Code/3rdParty/cml-1_0_2
    ${SSC_SOURCE_DIR}/Code/vtkMods/
    ${SSC_BINARY_DIR}
    ) 

set( SSC_DICOMLIB_INCLUDE_DIRS
  ${SSC_SOURCE_DIR}/DICOMLib
  )
#find_package( CML REQUIRED )
#find_package (CML PATHS "cml-1_0_2" REQUIRED)
#include (${CML_USE_FILE})
#include_directories(${SSC_SOURCE_DIR}/Code/3rdParty/cml-1_0_2)
#message(STATUS "cml dir : " ${SSC_SOURCE_DIR}/Code/cml-1_0_2)


set(SSC_LIBRARY_DIRS 
#    ${SSC_SOURCE_DIR}/Code/Ultrasound/3DReconstruction
    )    

set( SSC_LIBRARIES ${SSC_LIBRARIES} QVTK ${SSC_GCOV_LIBRARY})

if(WIN32)
    set( SSC_LIBRARIES ${SSC_LIBRARIES} )
# needed on apple
elseif(APPLE)
    set( SSC_LIBRARIES ${SSC_LIBRARIES} crypto )
# needed on linux
else(WIN32)
    set( SSC_LIBRARIES ${SSC_LIBRARIES} GLU crypto ssl)
endif(WIN32)


# library
cx_add_class( SSC_SOURCE_FILES
    sscForwardDeclarations.h
    vtkForwardDeclarations.h

	Video/sscVideoGraphics

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


# =========================================================
# =========================================================
#message( STATUS "processing project " ${SSC_PROJECT_NAME} "...")

# _USE_MATH_DEFINES Must be defined before math.h is included
if (WIN32)
    add_definitions(-D_USE_MATH_DEFINES)
endif(WIN32)

# boost
find_package( Boost REQUIRED )


# =========================================================
# vtk
# =========================================================
find_package(VTK REQUIRED)

include(${VTK_USE_FILE})

find_package(OpenGL)
    SET( SSC_LIBRARIES 
        ${SSC_LIBRARIES}
        ${OPENGL_LIBRARIES}
        )

    
if (USE_GLX_SHARED_CONTEXT)
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

endif (USE_GLX_SHARED_CONTEXT)  

set( SSC_VTK_LIBRARIES 
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
# qt
# =========================================================
#find_package( Qt4 REQUIRED )
#set(QT_USE_QTNETWORK 1)
#set(QT_USE_QTXML 1)
#include( ${QT_USE_FILE} )

# clear variables; Qt appends to these vars
#set( UI_HEADER_FILES )
#set( GENERATED_RESOURCES )

#QT4_WRAP_UI( UI_HEADER_FILES ${SSC_QT_UI_FILES} )
#QT4_ADD_RESOURCES( GENERATED_RESOURCES ${SSC_QT_RESOURCE_FILES} )

# =========================================================
# stuff
# =========================================================
# Configure the default SSC_DATA_ROOT for the location of SSCData.
unset(SSC_DATA_ROOT)
set (SSC_DATA_ROOT ${CustusX3_SOURCE_DIR}/data CACHE INTERNAL "path to ssc data root")
#FIND_PATH(SSC_DATA_ROOT sscData.readme 
#  ${CustusX3_SOURCE_DIR}/data)
#  $ENV{SSC_DATA_ROOT})
# message(status ${SSC_DATA_ROOT} "  --" ${CustusX3_SOURCE_DIR})
  
#set(SSC_THUNDER_KERNEL_PATH ${SSC_SOURCE_DIR}/../../source/UsReconstruction/Thunder)

configure_file(
  "${SSC_SOURCE_DIR}/Code/Utilities/sscConfig.h.in"
  "${SSC_BINARY_DIR}/sscConfig.h"
  )

# =========================================================
# create output variables
# =========================================================

set( SSC_GENERATED_SOURCE_FILES
	${SSC_SOURCE_FILES} )
#    ${UI_HEADER_FILES}
#    ${MOC_HEADER_FILES}
#    ${GENERATED_RESOURCES} )

set( SSC_GENERATED_LIBRARIES
    ${QT_LIBRARIES} 
    ${SSC_VTK_LIBRARIES}
    ${SSC_LIBRARIES} )
# =========================================================

include_directories( ${SSC_INCLUDE_DIRS} ${SSC_BINARY_DIR} ${QT_INCLUDES} ${Boost_INCLUDE_DIRS} )
link_directories( ${SSC_LIBRARY_DIRS})

set (BUILD_SHARED_LIBS OFF CACHE BOOL "build shared or static libs")

# =========================================================
# create the target
add_library( SSC ${SSC_GENERATED_SOURCE_FILES} )
target_link_libraries( SSC ${SSC_GENERATED_LIBRARIES})
#message(STATUS SSC Libs:\n ${SSC_GENERATED_LIBRARIES})
# =========================================================

CX_COTIRE_ADD_LIBRARY(SSC "${SSC_GENERATED_LIBRARIES}")

# =========================================================
# 
# Configure the file describing how to use SSC

set (SSC_DIR ${SSC_BINARY_DIR} CACHE PATH "where to find the SSC cmake")

configure_file (
    "${PROJECT_SOURCE_DIR}/CMake/SSCConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/SSCConfig.cmake"
    )
configure_file(Rep/Shaders/glsl/upscale.frag Rep COPYONLY)
configure_file(Rep/Shaders/glsl/upscale.vert Rep COPYONLY)
configure_file(Rep/Shaders/glsl/raycasting_shader.frag Rep COPYONLY)
configure_file(Rep/Shaders/glsl/raycasting_shader.vert Rep COPYONLY)
configure_file(Rep/Shaders/glsl/Texture3DOverlay.frag Rep COPYONLY)
  
# Export our build settings and library dependencies
include (CMakeExportBuildSettings)
cmake_export_build_settings (${SSC_BINARY_DIR}/SSCBuildSettings.cmake)
export_library_dependencies (${SSC_BINARY_DIR}/SSCLibraryDepends.cmake)

SET_TARGET_PROPERTIES( SSC PROPERTIES COMPILE_FLAGS "${SNW_COMPILE_FLAGS}" LINK_FLAGS "${SNW_LINK_FLAGS}" LINK_LIBS "${SNW_LINK_LIBS}")

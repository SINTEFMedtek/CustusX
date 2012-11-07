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

SET( SSC_QT_MOC_HEADER_FILES
    sscView.h
    sscViewContainer.h
#    vtkMods/sscViewQVTKWidget.h
#    vtkMods/sscModified_QVTKWidget_5_8.h
    Utilities/sscAcceptanceBoxWidget.h
    #Data/sscLandmark.h
    Data/sscData.h
    Data/sscImage.h
    Data/sscImageTF3D.h
    Data/sscImageLUT2D.h
    Data/sscImageTFData.h
    Data/sscDataManager.h
    Data/sscNavigatedVideoImage.h
    Data/sscRegistrationTransform.h
    Data/sscDataManagerImpl.h
    Data/sscMesh.h
    Data/sscVtkFileMesh.h
    Data/sscPresetTransferFunctions3D.h
    Data/sscPointMetric.h
    Data/sscDistanceMetric.h
    Data/sscPlaneMetric.h
    Data/sscAngleMetric.h
       
    Video/sscVideoSource.h
    Video/sscVideoRecorder.h
    Video/sscVideoSourceSHM.h
    Video/sscTestVideoSource.h
    Video/sscImageImportVideoSource.h

    Video/sscImageImportVideoSource.h
        
    Tool/sscToolManager.h
    Tool/sscTool.h 
    Tool/sscDummyTool.h
    Tool/sscDummyToolManager.h
    Tool/sscManualTool.h
    Tool/sscProbeAdapterRTSource.h
    
    Rep/sscSliceProxy.h
    Rep/sscRep.h
    Rep/sscSlicerRepSW.h
    Rep/sscToolRep3D.h
    Rep/sscPickerRep.h
    Rep/sscImageBlenderProxy.h
    Rep/sscVolumetricRep.h    
    Rep/sscSlicePlaneRep.h
    Rep/sscProgressiveLODVolumetricRep.h
    Rep/sscBlendedSliceRep.h
    Rep/sscSlicedImageProxy.h
    Rep/sscSurfaceRep.h
    Rep/sscToolRep2D.h
    Rep/sscCrossHairRep2D.h
    Rep/sscTexture3DSlicerProxy.h
    Rep/sscTexture3DSlicerRep.h
    Rep/sscGPURayCastVolumeRep.h
    Rep/sscSlices3DRep.h
    Rep/sscSlicePlanes3DRep.h
    Rep/sscGeometricRep.h
    Rep/sscGeometricRep2D.h
    Rep/sscSlicePlaneClipper.h
    Rep/sscOrientationAnnotationRep.h
    Rep/sscVideoRep.h
    Rep/sscToolTracer.h
    Rep/sscFiberBundleRep.h
    Rep/sscDataMetricRep.h
    Rep/sscPointMetricRep.h
    Rep/sscPointMetricRep2D.h
    Rep/sscDistanceMetricRep.h
    Rep/sscAngleMetricRep.h
    Rep/sscPlaneMetricRep.h
    Rep/sscGuideRep2D.h
     
    Widgets/sscDataAdapter.h
    Widgets/sscDoubleDataAdapter.h
    Widgets/sscStringDataAdapter.h
    Widgets/sscDoubleDataAdapterXml.h
    Widgets/sscStringDataAdapterXml.h
    Widgets/sscBoolDataAdapter.h
    Widgets/sscBoolDataAdapterXml.h
    Widgets/sscConsoleWidget.h
    Widgets/sscDoubleWidgets.h
    Widgets/sscLabeledComboBoxWidget.h
    Widgets/sscLabeledLineEditWidget.h
    Widgets/sscCheckBoxWidget.h
    Widgets/sscHelperWidgets.h
    Widgets/sscFileSelectWidget.h
    Widgets/sscVector3DDataAdapter.h
    Widgets/sscVector3DDataAdapterXml.h
    Widgets/sscMousePadWidget.h
    Utilities/sscMessageManager.h
    Utilities/sscSharedMemory.h
    )

SET( SSC_HEADER_FILES
    ${SSC_QT_MOC_HEADER_FILES}
    sscRep.h 
    sscRepImpl.h 
    sscVolumeRep.h
    )
#if (USE_GLX_SHARED_CONTEXT)
#    SET(SSC_HEADER_FILES ${SSC_HEADER_FILES} vtkMods/sscSNWQVTKWidget.h)
#endif (USE_GLX_SHARED_CONTEXT)

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
    ${SSC_SOURCE_DIR}/Code/Ultrasound/3DReconstruction
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
    ${SSC_SOURCE_DIR}/Code/Ultrasound/3DReconstruction
    )    

set( SSC_LIBRARIES QVTK )

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
SET( SSC_SOURCE_FILES
    sscForwardDeclarations.h
    vtkForwardDeclarations.h

    sscView.cpp
    sscView.h
    sscViewContainer.cpp
    sscViewContainer.h
    vtkMods/sscViewRenderWindow.cpp
    vtkMods/sscViewRenderWindow.h
    vtkMods/sscViewQVTKWidget.cpp
    vtkMods/sscViewQVTKWidget.h

    Math/sscBoundingBox3D.cpp
    Math/sscBoundingBox3D.h
    Math/sscVector3D.cpp
    Math/sscVector3D.h
    Math/sscTransform3D.cpp 
    Math/sscTransform3D.h
    Math/sscFrame3D.cpp
    Math/sscFrame3D.h

    Utilities/sscAudio.h
    Utilities/sscCoordinateSystemHelpers.h
    Utilities/sscCoordinateSystemHelpers.cpp
    Utilities/sscMeshHelpers.cpp
    Utilities/sscMeshHelpers.h
    Utilities/sscSliceComputer.cpp 
    Utilities/sscSliceComputer.h
    Utilities/sscIndent.cpp
    Utilities/sscIndent.h
    Utilities/sscAcceptanceBoxWidget.cpp
    Utilities/sscAcceptanceBoxWidget.h
    Utilities/sscGraphicalPrimitives.cpp
    Utilities/sscGraphicalPrimitives.h
    Utilities/sscUtilHelpers.cpp
    Utilities/sscUtilHelpers.h
    Utilities/sscPositionStorageFile.cpp
    Utilities/sscPositionStorageFile.h
    Utilities/sscTypeConversions.cpp
    Utilities/sscTypeConversions.h
    Utilities/sscMD5Check.cpp
    Utilities/sscMD5Check.h
    Utilities/sscSnwInifile.cpp
    Utilities/sscSnwInifile.h
    Utilities/sscGLHelpers.cpp
    Utilities/sscGLHelpers.h
    Utilities/sscEnumConverter.h
    Utilities/sscDefinitions.h
    Utilities/sscDefinitions.cpp
    Utilities/sscDefinitionStrings.cpp
    Utilities/sscTime.h
    Utilities/sscTime.cpp
    Utilities/sscMessageManager.h
    Utilities/sscMessageManager.cpp
    Utilities/sscVolumeHelpers.h
    Utilities/sscVolumeHelpers.cpp
    Utilities/sscCustomMetaImage.h
    Utilities/sscCustomMetaImage.cpp
    Utilities/sscViewportListener.h
    Utilities/sscViewportListener.cpp
    Utilities/sscSharedMemory.cpp
    Utilities/sscLogger.h
    Utilities/sscLogger.cpp
    Utilities/sscTimeKeeper.h
    Utilities/sscTimeKeeper.cpp

    Rep/sscRep.h
    Rep/sscRepImpl.h
    Rep/sscRepImpl.cpp
    Rep/sscRepImpl.h
    Rep/sscAxesRep.cpp
    Rep/sscAxesRep.h
    Rep/sscSliceRep.cpp
    Rep/sscSliceRep.h
    Rep/sscVolumetricRep.h
    Rep/sscVolumetricRep.cpp
    Rep/sscProgressiveLODVolumetricRep.cpp
    Rep/sscProgressiveLODVolumetricRep.h
    Rep/sscGeometricRep.cpp
    Rep/sscGeometricRep.h
    Rep/sscGeometricRep2D.cpp
    Rep/sscGeometricRep2D.h
    Rep/sscToolRep3D.cpp
    Rep/sscToolRep3D.h
    Rep/sscPickerRep.cpp
    Rep/sscPickerRep.h
    Rep/sscSlicerRepSW.cpp
    Rep/sscSlicerRepSW.h
    Rep/sscSliceProxy.cpp
    Rep/sscSliceProxy.h
    Rep/sscSlicePlaneRep.cpp
    Rep/sscSlicePlaneRep.h
    Rep/sscSlicedImageProxy.cpp
    Rep/sscSlicedImageProxy.h
    Rep/sscBlendedSliceRep.cpp
    Rep/sscBlendedSliceRep.h
    Rep/sscSurfaceRep.cpp
    Rep/sscSurfaceRep.h
    Rep/sscImageBlenderProxy.cpp
    Rep/sscImageBlenderProxy.h
    Rep/sscTexture3DSlicerProxy.h
    Rep/sscTexture3DSlicerProxy.cpp
    Rep/sscTexture3DSlicerRep.cpp
    Rep/sscTexture3DSlicerRep.h
    Rep/sscGPURayCastVolumeRep.cpp
    Rep/sscGPURayCastVolumeRep.h
    Rep/sscGPURayCastVolumePainter.cpp
    Rep/sscGPURayCastVolumePainter.h
    Rep/sscSlices3DRep.h
    Rep/sscSlices3DRep.cpp
    Rep/sscTextureSlicePainter.cpp
    Rep/sscTextureSlicePainter.h
    Rep/sscOrientationAnnotationRep.cpp
    Rep/sscOrientationAnnotationRep.h
    Rep/sscToolRep2D.cpp
    Rep/sscToolRep2D.h
    Rep/sscCrossHairRep2D.cpp
    Rep/sscCrossHairRep2D.h
    Rep/sscDisplayTextRep.cpp
    Rep/sscDisplayTextRep.h
    Rep/sscVtkHelperClasses.cpp
    Rep/sscVtkHelperClasses.h
    Rep/sscSlicePlanes3DRep.cpp
    Rep/sscSlicePlanes3DRep.h
    Rep/sscSlicePlaneClipper.h
    Rep/sscSlicePlaneClipper.cpp
    Rep/sscVideoRep.h
    Rep/sscVideoRep.cpp
    Rep/sscUltrasoundSectorSource.h
    Rep/sscUltrasoundSectorSource.cpp
    Rep/sscToolTracer.h
    Rep/sscToolTracer.cpp
    Rep/sscFiberBundleRep.h
    Rep/sscFiberBundleRep.cpp
    Rep/sscDataMetricRep.h
    Rep/sscDataMetricRep.cpp
    Rep/sscPointMetricRep.h
    Rep/sscPointMetricRep.cpp
    Rep/sscPointMetricRep2D.h
    Rep/sscPointMetricRep2D.cpp
    Rep/sscDistanceMetricRep.h
    Rep/sscDistanceMetricRep.cpp
    Rep/sscAngleMetricRep.h
    Rep/sscAngleMetricRep.cpp
    Rep/sscPlaneMetricRep.h
    Rep/sscPlaneMetricRep.cpp
    Rep/sscGuideRep2D.h
    Rep/sscGuideRep2D.cpp
    
    Data/sscLandmark.h
    Data/sscLandmark.cpp
    Data/sscData.cpp
    Data/sscData.h
    Data/sscImage.h
    Data/sscImage.cpp
    Data/sscImageTF3D.h
    Data/sscImageTF3D.cpp
    Data/sscImageLUT2D.cpp
    Data/sscImageLUT2D.h
    Data/sscImageTFData.cpp
    Data/sscImageTFData.h
    Data/sscNavigatedVideoImage.cpp
    Data/sscNavigatedVideoImage.h
    Data/sscGPUImageBuffer.cpp
    Data/sscGPUImageBuffer.h
    Data/sscMesh.h
    Data/sscMesh.cpp
    Data/sscVtkFileMesh.h
    Data/sscVtkFileMesh.cpp
    Data/sscDataManager.cpp
    Data/sscDataManager.h
    Data/sscDataManagerImpl.cpp
    Data/sscDataManagerImpl.h
    Data/sscSNW2Volume.cpp
    Data/sscSNW2Volume.h
    Data/sscRegistrationTransform.cpp
    Data/sscRegistrationTransform.h
    Data/sscPresetTransferFunctions3D.h
    Data/sscPresetTransferFunctions3D.cpp
    Data/sscPointMetric.h
    Data/sscPointMetric.cpp
    Data/sscDistanceMetric.h
    Data/sscDistanceMetric.cpp
    Data/sscPlaneMetric.h
    Data/sscPlaneMetric.cpp
    Data/sscAngleMetric.h
    Data/sscAngleMetric.cpp
    Data/sscDataMetric.h
    Data/sscDataMetric.cpp
    
    Video/sscVideoSource.h
    Video/sscVideoRecorder.h
    Video/sscVideoRecorder.cpp
    Video/sscVideoSourceSHM.h
    Video/sscVideoSourceSHM.cpp
    Video/sscTestVideoSource.h
    Video/sscTestVideoSource.cpp    
    Video/sscImageImportVideoSource.h
    Video/sscImageImportVideoSource.cpp

    Tool/sscToolManager.cpp
    Tool/sscToolManager.h
    Tool/sscTool.h 
    Tool/sscDummyToolManager.cpp
    Tool/sscDummyToolManager.h
    Tool/sscDummyTool.cpp
    Tool/sscDummyTool.h
    Tool/sscManualTool.cpp
    Tool/sscManualTool.h
    Tool/sscProbeData.cpp
    Tool/sscProbeData.h
    Tool/sscProbeSector.h 
    Tool/sscProbeSector.cpp 
    Tool/sscProbeAdapterRTSource.h
    Tool/sscProbeAdapterRTSource.cpp
    
    Widgets/sscDoubleRange.h
    Widgets/sscHelperWidgets.cpp
    Widgets/sscHelperWidgets.h
    Widgets/sscDoubleDataAdapter.h
    Widgets/sscBoolDataAdapter.h
    Widgets/sscDoubleWidgets.h    
    Widgets/sscDoubleWidgets.cpp    
    Widgets/sscStringDataAdapter.h
    Widgets/sscDoubleDataAdapterXml.h
    Widgets/sscDoubleDataAdapterXml.cpp
    Widgets/sscBoolDataAdapterXml.h
    Widgets/sscBoolDataAdapterXml.cpp
    Widgets/sscStringDataAdapterXml.h
    Widgets/sscStringDataAdapterXml.cpp
    Widgets/sscLabeledComboBoxWidget.h    
    Widgets/sscLabeledComboBoxWidget.cpp  
    Widgets/sscLabeledLineEditWidget.h
    Widgets/sscLabeledLineEditWidget.cpp  
    Widgets/sscCheckBoxWidget.h
    Widgets/sscCheckBoxWidget.cpp
    Widgets/sscXmlOptionItem.h    
    Widgets/sscXmlOptionItem.cpp
    Widgets/sscConsoleWidget.h
    Widgets/sscConsoleWidget.cpp
    Widgets/sscFileSelectWidget.h
    Widgets/sscFileSelectWidget.cpp
    Widgets/sscVector3DDataAdapter.h
    Widgets/sscVector3DDataAdapterXml.h
    Widgets/sscVector3DDataAdapterXml.cpp
    Widgets/sscMousePadWidget.h
    Widgets/sscMousePadWidget.cpp

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
            SET(SSC_QT_MOC_HEADER_FILES ${SSC_QT_MOC_HEADER_FILES} vtkMods/sscSNWQVTKWidget5_4.h)
            SET(SUPPORTED_VTK_VERSION_FOUND "5.4")
        endif (VTK_MINOR_VERSION EQUAL 4)
        if (VTK_MINOR_VERSION EQUAL 5)
            SET(SSC_QT_MOC_HEADER_FILES ${SSC_QT_MOC_HEADER_FILES} vtkMods/sscSNWQVTKWidget5_5.h)
            SET(SUPPORTED_VTK_VERSION_FOUND "5.5")
        endif (VTK_MINOR_VERSION EQUAL 5)
        if (VTK_MINOR_VERSION EQUAL 6)
            SET(SSC_QT_MOC_HEADER_FILES ${SSC_QT_MOC_HEADER_FILES} vtkMods/sscSNWQVTKWidget5_5.h)
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
find_package( Qt4 REQUIRED )
set(QT_USE_QTNETWORK 1)
set(QT_USE_QTXML 1)
include( ${QT_USE_FILE} )

# clear variables; Qt appends to these vars
set( UI_HEADER_FILES )
set( MOC_HEADER_FILES )
set( GENERATED_RESOURCES )

QT4_WRAP_UI( UI_HEADER_FILES ${SSC_QT_UI_FILES} )
QT4_WRAP_CPP( MOC_HEADER_FILES ${SSC_QT_MOC_HEADER_FILES} )
QT4_ADD_RESOURCES( GENERATED_RESOURCES ${SSC_QT_RESOURCE_FILES} )

# =========================================================
# stuff
# =========================================================
# Configure the default SSC_DATA_ROOT for the location of SSCData.
FIND_PATH(SSC_DATA_ROOT sscData.readme 
  ${SSC_SOURCE_DIR}/Data
  $ENV{SSC_DATA_ROOT})
 #message(status ${SSC_DATA_ROOT} "  --" ${SSC_SOURCE_DIR})
  
#set(SSC_THUNDER_KERNEL_PATH ${SSC_SOURCE_DIR}/../../source/UsReconstruction/Thunder)

configure_file(
  "${SSC_SOURCE_DIR}/Code/Utilities/sscConfig.h.in"
  "${SSC_BINARY_DIR}/sscConfig.h"
  )

# =========================================================
# create output variables
# =========================================================

set( SSC_GENERATED_SOURCE_FILES
    ${SSC_SOURCE_FILES} 
    ${UI_HEADER_FILES} 
    ${MOC_HEADER_FILES} 
    ${GENERATED_RESOURCES} )

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

#------------------------------------------------------------------------------
# Cotire - pch and unity builds
#------------------------------------------------------------------------------
if(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)
    cotire(SSC)
    if(COTIRE_ADD_UNITY_BUILDS)
        target_link_libraries(SSC_unity 
                ${SSC_GENERATED_LIBRARIES}
                )
    endif(COTIRE_ADD_UNITY_BUILDS)
endif(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)


# =========================================================
# 
# Configure the file describing how to use SSC
#get_target_property (SSC_LIBRARY SSC LOCATION)
#set (SSC_INCLUDE ${PROJECT_BINARY_DIR}/include)

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


# =========================================================
# quick fix for removing install from CustusX (which uses fixup_bundle())
if (NOT CustusX3_SOURCE_DIR)
    # install lib into lib64 folder
    install( TARGETS SSC DESTINATION lib64 )
endif ()
# =========================================================

SET_TARGET_PROPERTIES( SSC PROPERTIES COMPILE_FLAGS "${SNW_COMPILE_FLAGS}" LINK_FLAGS "${SNW_LINK_FLAGS}" LINK_LIBS "${SNW_LINK_LIBS}")

add_subdirectory(Ultrasound/3DReconstruction)

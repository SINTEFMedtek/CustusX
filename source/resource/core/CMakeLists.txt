###########################################################
##               Resource Core library
###########################################################

cx_add_class_qt_moc(cxResource_SOURCE
	Data/cxData
	Data/cxImage
	Data/cxImageTF3D
	Data/cxImageLUT2D
	Data/cxImageTFData
	Data/cxDataManager
	Data/cxNavigatedVideoImage
	Data/cxRegistrationTransform
	Data/cxDataManagerImpl
	Data/cxMesh
#	Data/sscVtkFileMesh
	Data/cxPresets
	Data/cxTransferFunctions3DPresets
	Data/cxPointMetric
	Data/cxDistanceMetric
	Data/cxPlaneMetric
	Data/cxAngleMetric
	Data/cxUnsignedDerivedImage
	Data/cxFrameMetric
	Data/cxToolMetric
	Data/cxFrameMetricBase
	Data/cxShapedMetric
	Data/cxSphereMetric
	Data/cxMetricReferenceArgumentList
	Data/cxLandmark
	Data/cxDataFactory
	Data/cxPatientModelService

	Video/cxVideoSource.h
	Video/cxVideoRecorder
	Video/cxVideoSourceSHM
	Video/cxTestVideoSource
        
	Tool/cxToolManager
	Tool/cxTool.h
	Tool/cxProbe.h
	Tool/cxDummyTool
	Tool/cxDummyToolManager
	Tool/cxManualTool
	Tool/cxProbeAdapterRTSource
	Tool/cxSliceProxy
	Tool/cxSlicedImageProxy
	Tool/cxToolImpl

	dataAdapters/cxDataAdapter
	dataAdapters/cxDoubleDataAdapter.h
	dataAdapters/cxStringDataAdapter.h
	dataAdapters/cxDoubleDataAdapterXml
	dataAdapters/cxStringDataAdapterXml
	dataAdapters/cxBoolDataAdapter.h
	dataAdapters/cxBoolDataAdapterXml
	dataAdapters/cxVector3DDataAdapter.h
	dataAdapters/cxVector3DDataAdapterXml
	dataAdapters/cxColorDataAdapter.h
	dataAdapters/cxColorDataAdapterXml
	dataAdapters/cxVector3DComponentDataAdapter

	logger/cxReporter
	logger/cxMessageListener

	settings/cxSettings.h

	algorithms/cxTimedAlgorithm.h
    algorithms/cxThreadedTimedAlgorithm.h
    algorithms/cxCompositeTimedAlgorithm.h

    utilities/cxPlaybackTime.h
    utilities/cxProcessWrapper.h
	utilities/cxProcessReporter
	utilities/cxVLCRecorder
	utilities/cxSpaceProvider
	utilities/cxSpaceProviderImpl
	utilities/cxSpaceListener
	utilities/cxSpaceListenerImpl
	)

     
# library
cx_add_class(cxResource_SOURCE
	cxForwardDeclarations.h
	vtkForwardDeclarations.h
	
	Tool/cxProbeSector
	Tool/cxProbeData
	Tool/ProbeXmlConfigParser.h
	Tool/ProbeXmlConfigParserImpl
	Tool/ProbeXmlConfigParserMock
	Tool/cxCreateProbeDataFromConfiguration
	Tool/cxTrackingPositionFilter
	Tool/cxTrackerConfiguration.h

	Data/cxDataMetric
	Data/cxDataReaderWriter
	Data/cxGPUImageBuffer
	Data/cxImageDefaultTFGenerator
	Data/cxImageParameters

	Math/cxBoundingBox3D
	Math/cxVector3D
	Math/cxTransform3D
	Math/cxFrame3D
	Math/cxMathBase.h
	
	utilities/cxXmlOptionItem
	utilities/cxDoubleRange.h
	utilities/cxCyclicActionLogger
	utilities/cxTransformFile
	utilities/cxPlaybackTime
	utilities/cxProcessWrapper
	utilities/cxFileHelpers
	utilities/cxStringHelpers
	utilities/cxGLHelpers
	utilities/cxTime
	utilities/cxUtilHelpers
	utilities/cxTypeConversions
	utilities/cxSliceComputer
	utilities/cxDefinitions.h
	utilities/cxDefinitionStrings
	utilities/cxCustomMetaImage
	utilities/cxIndent
	utilities/cxCoordinateSystemHelpers
	utilities/cxViewportListener
	utilities/cxVolumeHelpers
	utilities/cxPositionStorageFile
	utilities/cxTimeKeeper
	utilities/cxMeshHelpers
	utilities/cxApplication
	utilities/cxSharedMemory
	utilities/cxImageDataContainer
	utilities/cxOptionalValue
	utilities/cxXMLNodeWrapper
	utilities/cxPlaneTypeCollection
	utilities/cxSharedPointerChecker

	logger/cxAudio.h
	logger/cxLogger
		
	algorithms/ItkVtkGlue/itkImageToVTKImageFilter.h
	algorithms/ItkVtkGlue/itkImageToVTKImageFilter.txx
	algorithms/ItkVtkGlue/itkVTKImageToImageFilter.h
	algorithms/ItkVtkGlue/itkVTKImageToImageFilter.txx
	algorithms/itkBinaryThinningImageFilter3D.h
	algorithms/itkBinaryThinningImageFilter3D.txx
	algorithms/cxImageAlgorithms
	algorithms/cxTimedAlgorithm
	algorithms/cxThreadedTimedAlgorithm
	algorithms/cxCompositeTimedAlgorithm
	algorithms/cxAlgorithmHelpers
	
	settings/cxDataLocations
	settings/cxSettings
			
	vesselReg/SeansVesselReg.cxx
	vesselReg/SeansVesselReg.hxx
	vesselReg/HackTPSTransform.hxx
	
	${CustusX_SOURCE_DIR}/source/ThirdParty/iir1/iir/Butterworth.cpp
	${CustusX_SOURCE_DIR}/source/ThirdParty/iir1/iir/Cascade.cpp
	${CustusX_SOURCE_DIR}/source/ThirdParty/iir1/iir/PoleFilter.cpp
	${CustusX_SOURCE_DIR}/source/ThirdParty/iir1/iir/Biquad.cpp
    )
    

if(WIN32)
    set( CX_RESOURCE_LIBRARIES ${CX_RESOURCE_LIBRARIES} )
# needed on apple
elseif(APPLE)
    set( CX_RESOURCE_LIBRARIES ${CX_RESOURCE_LIBRARIES} crypto )
# needed on linux
else(WIN32)
    set( CX_RESOURCE_LIBRARIES ${CX_RESOURCE_LIBRARIES} GLU crypto ssl)
endif(WIN32)

set( CX_RESOURCE_LIBRARIES 
    ${CX_RESOURCE_LIBRARIES}
vtkCommonCore
vtkCommonDataModel
vtkCommonExecutionModel
vtkIOImage
vtkIOGeometry
vtkIOLegacy
vtkIOVideo
vtkImagingStatistics
vtkImagingColor
vtkImagingMath
vtkImagingGeneral
#vtkImagingMorphological
vtkFiltersCore
vtkFiltersGeneral
vtkFiltersSources
#vtkFiltersParallel
vtkRenderingFreeTypeOpenGL
vtkRenderingFreeType
vtkRenderingCore
vtkRenderingVolume
vtkInteractionStyle
    )


# Configure the default SSC_DATA_ROOT for the location of SSCData.
unset(SSC_DATA_ROOT)
set (SSC_DATA_ROOT ${CustusX_SOURCE_DIR}/data CACHE INTERNAL "path to ssc data root")

# Configure the USE_GLX_SHARED_CONTEXT - used in sscConfig.h
#if(X11_FOUND)
#    if(APPLE)
#        option(USE_GLX_SHARED_CONTEXT "use shared glx contexts" OFF)
#    else(APPLE)
#        option(USE_GLX_SHARED_CONTEXT "use shared glx contexts" ON)
#    endif(APPLE)
#    include_directories( ${X11_INCLUDE_DIR} ) # needed by Mac 10.8+
#else(X11_FOUND)
#    option(USE_GLX_SHARED_CONTEXT "use shared glx contexts" OFF)
#endif(X11_FOUND)

configure_file(
  "${CustusX_SOURCE_DIR}/source/resource/core/utilities/sscConfig.h.in"
  "${CustusX_BINARY_DIR}/source/resource/core/utilities/sscConfig.h"
  )

# extra definitions:
add_subdirectory(settings)

unset(CX_RESOURCE_INCLUDE_DIRECTORIES)

set(CX_RESOURCE_INCLUDE_DIRECTORIES
    ${CustusX_SOURCE_DIR}/source/resource/core
    ${CustusX_SOURCE_DIR}/source/resource/core/logger
    ${CustusX_SOURCE_DIR}/source/resource/core/Math
    ${CustusX_SOURCE_DIR}/source/resource/core/Data
    ${CustusX_SOURCE_DIR}/source/resource/core/Video
    ${CustusX_SOURCE_DIR}/source/resource/core/Tool
    ${CustusX_SOURCE_DIR}/source/resource/core/dataAdapters
    ${CustusX_SOURCE_DIR}/source/resource/core/algorithms
    ${CustusX_SOURCE_DIR}/source/resource/core/settings
    ${CustusX_SOURCE_DIR}/source/resource/core/utilities
    ${CustusX_SOURCE_DIR}/source/ThirdParty
    ${CustusX_SOURCE_DIR}/source/ThirdParty/iir1
    
    ${CustusX_BINARY_DIR}/source/resource/core
    ${CustusX_BINARY_DIR}/source/resource/core/utilities
    ${CustusX_BINARY_DIR}/source/resource/core/settings
    CACHE INTERNAL "Include directories for resource libraries")



set (CX_RESOURCE_LIBRARIES
    ${CX_RESOURCE_LIBRARIES}
	#QVTK
    ${SSC_GCOV_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${QT_LIBRARIES}
    ${Boost_LIBRARIES}
    ${ITK_LIBRARIES}
    )
    
if (CX_USE_OPENCL_UTILITY)
    cx_add_class(cxResource_SOURCE
    	utilities/cxOpenCLPrinter
    )

    set (CX_RESOURCE_LIBRARIES
        ${CX_RESOURCE_LIBRARIES}
        OpenCLUtilityLibrary
        ${OPENCL_LIBRARIES}
    )    
endif ()

include_directories(${CX_RESOURCE_INCLUDE_DIRECTORIES})

add_library (cxResource ${cxResource_SOURCE})
target_link_libraries(cxResource ${CX_RESOURCE_LIBRARIES} )
CX_COTIRE_ADD_LIBRARY( cxResource "${CX_RESOURCE_LIBRARIES}")

add_subdirectory(testing)


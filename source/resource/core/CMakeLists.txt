###########################################################
##               Resource Core library
###########################################################

cx_add_class_qt_moc(cxResource_SOURCE
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
#	Data/sscVtkFileMesh
	Data/sscPresets
	Data/sscTransferFunctions3DPresets
	Data/sscPointMetric
	Data/sscDistanceMetric
	Data/sscPlaneMetric
	Data/sscAngleMetric
	Data/sscUnsignedDerivedImage
	Data/cxFrameMetric
	Data/cxToolMetric
	Data/sscCoordinateSystemListener
	Data/cxFrameMetricBase
	Data/cxImageParameters

	Video/sscVideoSource.h
	Video/sscVideoRecorder
	Video/sscVideoSourceSHM
	Video/sscTestVideoSource
	Video/sscImageImportVideoSource
        
	Tool/sscToolManager
	Tool/sscTool.h
	Tool/sscProbe.h
	Tool/sscDummyTool
	Tool/sscDummyToolManager
	Tool/sscManualTool
	Tool/sscProbeAdapterRTSource
	Tool/sscSliceProxy
	Tool/sscSlicedImageProxy

	dataAdapters/sscDataAdapter
	dataAdapters/sscDoubleDataAdapter.h
	dataAdapters/sscStringDataAdapter.h
	dataAdapters/sscDoubleDataAdapterXml
	dataAdapters/sscStringDataAdapterXml
	dataAdapters/sscBoolDataAdapter.h
	dataAdapters/sscBoolDataAdapterXml
	dataAdapters/sscVector3DDataAdapter.h
	dataAdapters/sscVector3DDataAdapterXml
	dataAdapters/sscColorDataAdapter.h
	dataAdapters/sscColorDataAdapterXml
	dataAdapters/cxVector3DComponentDataAdapter

	logger/sscMessageManager

	settings/cxSettings.h

	algorithms/cxTimedAlgorithm.h
    algorithms/cxThreadedTimedAlgorithm.h
    algorithms/cxCompositeTimedAlgorithm.h

	utilities/sscApplication
	utilities/sscSharedMemory
	utilities/sscAcceptanceBoxWidget
    utilities/cxPlaybackTime.h
    utilities/cxProcessWrapper.h
	utilities/cxProcessReporter
	utilities/cxImageDataContainer
	utilities/cxVLCRecorder
	)

     
# library
cx_add_class(cxResource_SOURCE
	sscForwardDeclarations.h
	cxForwardDeclarations.h
	vtkForwardDeclarations.h
	
	Tool/sscProbeSector
	Tool/sscProbeData
	Tool/ProbeXmlConfigParser.h
	Tool/ProbeXmlConfigParserImpl
	Tool/ProbeXmlConfigParserMock
	Tool/cxCreateProbeDataFromConfiguration

	Data/sscDataMetric
	Data/sscLandmark
	Data/sscDataReaderWriter
	Data/sscGPUImageBuffer
	Data/cxImageDefaultTFGenerator
	
	Math/sscBoundingBox3D
	Math/sscVector3D
	Math/sscTransform3D
	Math/sscFrame3D
	
	utilities/sscXmlOptionItem
	utilities/sscDoubleRange.h
	utilities/cxCyclicActionLogger
	utilities/cxTransformFile
	utilities/cxPlaybackTime
	utilities/cxProcessWrapper
	utilities/cxFileHelpers
	utilities/cxStringHelpers
	utilities/sscGLHelpers
	utilities/sscTime
	utilities/sscUtilHelpers
	utilities/sscTypeConversions
	utilities/sscSliceComputer
	utilities/sscDefinitions.h
	utilities/sscDefinitionStrings
	utilities/sscCustomMetaImage
	utilities/sscMD5Check
	utilities/sscIndent
	utilities/sscCoordinateSystemHelpers
	utilities/sscViewportListener
	utilities/sscVolumeHelpers
	utilities/sscPositionStorageFile
	utilities/sscTimeKeeper
	utilities/sscMeshHelpers
	utilities/cxOpenCLUtilities
	utilities/cxOpenCLPrinter
	
	logger/sscAudio.h
	logger/sscLogger
		
	algorithms/ItkVtkGlue/itkImageToVTKImageFilter.h
	algorithms/ItkVtkGlue/itkImageToVTKImageFilter.txx
	algorithms/ItkVtkGlue/itkVTKImageToImageFilter.h
	algorithms/ItkVtkGlue/itkVTKImageToImageFilter.txx
	algorithms/itkBinaryThinningImageFilter3D.h
	algorithms/itkBinaryThinningImageFilter3D.txx
	algorithms/sscImageAlgorithms
	algorithms/cxTimedAlgorithm
	algorithms/cxThreadedTimedAlgorithm
	algorithms/cxCompositeTimedAlgorithm
	algorithms/cxAlgorithmHelpers
	
	settings/cxDataLocations
	settings/cxSettings
			
	vesselReg/SeansVesselReg.cxx
	vesselReg/SeansVesselReg.hxx
	vesselReg/HackTPSTransform.hxx
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

set( CX_RESOURCE_LIBRARIES ${CX_RESOURCE_LIBRARIES}
    vtkIO
    vtkRendering
    vtkImaging
    vtkCommon
    vtkHybrid
    vtkWidgets
    vtkFiltering
    vtkVolumeRendering
    )


# Configure the default SSC_DATA_ROOT for the location of SSCData.
unset(SSC_DATA_ROOT)
set (SSC_DATA_ROOT ${CustusX3_SOURCE_DIR}/data CACHE INTERNAL "path to ssc data root")

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
  "${CustusX3_SOURCE_DIR}/source/resource/core/utilities/sscConfig.h.in"
  "${CustusX3_BINARY_DIR}/source/resource/core/utilities/sscConfig.h"
  )

# extra definitions:
add_subdirectory(settings)

unset(CX_RESOURCE_INCLUDE_DIRECTORIES)

set(CX_RESOURCE_INCLUDE_DIRECTORIES
    ${CustusX3_SOURCE_DIR}/source/resource/core
    ${CustusX3_SOURCE_DIR}/source/resource/core/logger
    ${CustusX3_SOURCE_DIR}/source/resource/core/Math
    ${CustusX3_SOURCE_DIR}/source/resource/core/Data
    ${CustusX3_SOURCE_DIR}/source/resource/core/Video
    ${CustusX3_SOURCE_DIR}/source/resource/core/Tool
    ${CustusX3_SOURCE_DIR}/source/resource/core/dataAdapters
    ${CustusX3_SOURCE_DIR}/source/resource/core/algorithms
    ${CustusX3_SOURCE_DIR}/source/resource/core/settings
    ${CustusX3_SOURCE_DIR}/source/resource/core/utilities
    ${CustusX3_SOURCE_DIR}/source/ThirdParty
    
    ${CustusX3_BINARY_DIR}/source/resource/core
    ${CustusX3_BINARY_DIR}/source/resource/core/utilities
    ${CustusX3_BINARY_DIR}/source/resource/core/settings
    CACHE INTERNAL "Include directories for resource libraries")



set (CX_RESOURCE_LIBRARIES
    ${CX_RESOURCE_LIBRARIES}
    QVTK
    ${SSC_GCOV_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${QT_LIBRARIES}
    ${Boost_LIBRARIES}
    ${ITK_LIBRARIES})
    
if (SSC_USE_OpenCL)
    set (CX_RESOURCE_LIBRARIES
        ${CX_RESOURCE_LIBRARIES}
        ${OPENCL_LIBRARIES}
    )    
endif ()

if (CX_USE_OPENCL_UTILITY)
    set (CX_RESOURCE_LIBRARIES
        ${CX_RESOURCE_LIBRARIES}
        ${OpenCLUtilityLibrary_LIBRARIES}
    )    
endif ()

include_directories(${CX_RESOURCE_INCLUDE_DIRECTORIES})

ADD_LIBRARY (cxResource ${cxResource_SOURCE})
target_link_libraries(cxResource ${CX_RESOURCE_LIBRARIES} )
CX_COTIRE_ADD_LIBRARY( cxResource "${CX_RESOURCE_LIBRARIES}")

add_subdirectory(testing)


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
	Data/sscCoordinateSystemListener
	Data/cxFrameMetricBase

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

	logger/sscMessageManager

	settings/cxSettings.h

	algorithms/cxTimedAlgorithm.h
    algorithms/cxThreadedTimedAlgorithm.h
    algorithms/cxCompositeTimedAlgorithm.h

	utilities/sscApplication
	utilities/sscSharedMemory
	utilities/sscAcceptanceBoxWidget
	utilities/cxDisplayTimerWidget.h
    utilities/cxPlaybackTime.h
    utilities/cxTimelineWidget.h
    utilities/cxProcessWrapper.h
	utilities/cxSavingVideoRecorder.h
	utilities/cxImageDataContainer.h	
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
	
	Widgets/sscXmlOptionItem
	Widgets/sscDoubleRange.h
	
	Data/sscDataMetric
	Data/sscLandmark
	Data/sscDataReaderWriter
	Data/sscGPUImageBuffer
	
	Math/sscBoundingBox3D
	Math/sscVector3D
	Math/sscTransform3D
	Math/sscFrame3D
	
	utilities/cxRenderTimer
	utilities/cxUsReconstructionFileMaker
	utilities/cxUsReconstructionFileReader
	utilities/cxDisplayTimerWidget
	utilities/cxTransformFile
	utilities/cxPlaybackTime
	utilities/cxTimelineWidget
	utilities/cxDepthPeeling
	utilities/cxProcessWrapper
	utilities/sscUSFrameData
	utilities/cxSavingVideoRecorder
	utilities/cxImageDataContainer
	utilities/cxFileHelpers
	utilities/cxUSReconstructInputData
	utilities/cxUSReconstructInputDataAlgoritms
	utilities/cxStringHelpers
	utilities/cxUSReconstructInputDataAlgoritms.cpp	
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
  
configure_file(
  "${CustusX3_SOURCE_DIR}/source/resource/core/utilities/sscConfig.h.in"
  "${CustusX3_SOURCE_DIR}/source/resource/core/utilities/sscConfig.h"
  )

# extra definitions:
add_subdirectory(settings)

unset(CX_RESOURCE_INCLUDE_DIRECTORIES)

set(CX_RESOURCE_INCLUDE_DIRECTORIES
    ${CustusX3_SOURCE_DIR}/source/resource/core/Data
    ${CustusX3_SOURCE_DIR}/source/resource/core/Video
    ${CustusX3_SOURCE_DIR}/source/resource/core/logger
    ${CustusX3_SOURCE_DIR}/source/resource/core/Tool
    ${CustusX3_SOURCE_DIR}/source/resource/core/Math
    ${CustusX3_SOURCE_DIR}/source/resource/core/Widgets
    ${CustusX3_SOURCE_DIR}/source/ThirdParty
    ${CustusX3_SOURCE_DIR}/source/resource/core
    ${CustusX3_SOURCE_DIR}/source/resource/core/algorithms
    ${CustusX3_SOURCE_DIR}/source/resource/core/settings
    ${CustusX3_SOURCE_DIR}/source/resource/core/utilities
    ${CustusX3_BINARY_DIR}/source/resource/core
    ${CustusX3_BINARY_DIR}/source/resource/core/utilities
    ${CustusX3_BINARY_DIR}/source/resource/core/settings
    CACHE INTERNAL "Include directories for resource libraries")

#include_directories( ${SSC_INCLUDE_DIRS} ${SSC_BINARY_DIR} ${QT_INCLUDES} ${Boost_INCLUDE_DIRS} )
include_directories(${CX_RESOURCE_INCLUDE_DIRECTORIES})

set (CX_RESOURCE_LIBRARIES
    ${CX_RESOURCE_LIBRARIES}
    QVTK
    ${SSC_GCOV_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${QT_LIBRARIES}
    ${Boost_LIBRARIES}
    ${ITK_LIBRARIES})

ADD_LIBRARY (cxResource ${cxResource_SOURCE})
target_link_libraries(cxResource ${CX_RESOURCE_LIBRARIES} )
CX_COTIRE_ADD_LIBRARY( cxResource "${CX_RESOURCE_LIBRARIES}")

add_subdirectory(testing)


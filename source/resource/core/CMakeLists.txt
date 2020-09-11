###########################################################
##               Resource Core library
###########################################################

cx_add_class_qt_moc(cxResource_SOURCE
  Data/cxData
  Data/cxImage
  Data/cxImageTF3D
  Data/cxImageLUT2D
  Data/cxImageTFData
  Data/cxNavigatedVideoImage
  Data/cxRegistrationTransform
  Data/cxMesh
  Data/cxMeshPropertyData
  Data/cxMeshTextureData
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
  Data/cxCustomMetric
  Data/cxSphereMetric
  Data/cxRegionOfInterestMetric
  Data/cxMetricReferenceArgumentList
  Data/cxLandmark
  Data/cxActiveImageProxy
  Data/cxTrackedStream
  Data/cxActiveData
  Data/cxFileReaderWriterService
  Data/cxFileReaderWriterServiceProxy
  Data/cxFileReaderWriterServiceNull
  Data/cxFileManagerService
  Data/cxFileManagerServiceProxy
  Data/cxFileManagerServiceNull
  Data/cxFileManagerServiceBase

  Video/cxVideoSource.h
  Video/cxVideoRecorder
  Video/cxVideoSourceSHM
  Video/cxTestVideoSource
  Video/cxVideoService
  Video/cxStreamerService
  Video/cxBasicVideoSource
  Video/cxStringPropertyActiveVideoSource

  Tool/cxTool
  Tool/cxToolProxy
  Tool/cxProbe.h
  Tool/cxProbeImpl.h
  Tool/cxDummyTool
  Tool/cxDummyToolManager
  Tool/cxManualTool
  Tool/cxProbeAdapterRTSource
  Tool/cxSliceProxy
  Tool/cxSlicedImageProxy
  Tool/cxToolImpl
  Tool/cxTrackingService
  Tool/cxActiveToolProxy
  Tool/cxTrackingSystemService
  Tool/cxTracker
  Tool/cxManualToolAdapter
  Tool/cxPlaybackTool

  properties/cxProperty
  properties/cxPropertyNull.h
  properties/cxDoublePropertyBase.h
  properties/cxStringPropertyBase.h
  properties/cxDoubleProperty
  properties/cxStringProperty
  properties/cxBoolPropertyBase.h
  properties/cxBoolProperty
  properties/cxVector3DPropertyBase.h
  properties/cxVector3DProperty
  properties/cxColorPropertyBase.h
  properties/cxColorProperty
  properties/cxVector3DComponentProperty
  properties/cxDoublePairPropertyBase.h
  properties/cxDoublePairProperty
  properties/cxSelectDataStringPropertyBase
  properties/cxSelectDataStringProperty
  properties/cxSpacePropertyBase
  properties/cxSpaceProperty
  properties/cxStringPropertySelectPointMetric
  properties/cxToolProperty
  properties/cxDoublePropertyTemporalCalibration
  properties/cxStringPropertySelectTool
  properties/cxStringPropertySelectCoordinateSystem
  properties/cxFilePathProperty
  properties/cxStringListProperty
  properties/cxFilePathPropertyBase
  properties/cxFilePreviewProperty

  logger/cxReporter
  logger/cxMessageListener
  logger/cxLog
  logger/cxReporter
  logger/cxMessageListener
  logger/cxLogFileWatcher
  logger/internal/cxLogThread
  logger/internal/cxReporterThread
  logger/internal/cxReporterMessageRepository
  logger/internal/cxLogFileWatcherThread

  settings/cxSettings.h
  settings/cxStateService
  settings/cxStateServiceNull
  settings/cxStateServiceProxy
  settings/cxSessionStorageService
  settings/cxProfile
  settings/cxPatientStorage
  settings/cxWorkflowStateMachine
  settings/cxWorkflowState

  algorithms/cxTimedAlgorithm.h
  algorithms/cxThreadedTimedAlgorithm.h
  algorithms/cxCompositeTimedAlgorithm.h

  utilities/cxPlaybackTime.h
  utilities/cxProcessWrapper.h
  utilities/cxProcessReporter
  utilities/cxVLCRecorder
  utilities/cxSpaceListener
  utilities/cxSpaceListenerImpl
  utilities/cxSyncedValue
  utilities/cxSpaceProvider
  utilities/cxSocket
  utilities/cxSocketConnection

  patientModel/cxPatientModelService

  usReconstructionTypes/cxSavingVideoRecorder

  network/cxNetworkService
  )

cx_add_class(cxResource_SOURCE
  cxForwardDeclarations.h
  vtkForwardDeclarations.h
  cxCoreServices

  Tool/cxProbeSector
  Tool/cxProbeDefinition
  Tool/ProbeXmlConfigParser.h
  Tool/ProbeXmlConfigParserImpl
  Tool/ProbeXmlConfigParserMock
  Tool/cxCreateProbeDefinitionFromConfiguration
  Tool/cxTrackingPositionFilter
  Tool/cxTrackerConfiguration
  Tool/cxToolNull
  Tool/cxProbeImpl
  Tool/cxToolFileParser
  Tool/cxToolConfigurationParser
  Tool/cxTrackerConfigurationImpl

  Data/cxDataMetric
  Data/cxErrorObserver
  Data/cxGPUImageBuffer
  Data/cxImageDefaultTFGenerator
  Data/cxImageParameters
  Data/cxFrameForest
  Data/cxDataFactory
  Data/cxErrorObserver

  Math/cxBoundingBox3D
  Math/cxVector3D
  Math/cxTransform3D
  Math/cxFrame3D
  Math/cxMathBase.h
  Math/cxMathUtils

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
  utilities/cxDefinitions
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
  utilities/cxNullDeleter.h
  utilities/cxSpaceProviderImpl
  utilities/cxStreamedTimestampSynchronizer
  utilities/cxEnumConverter.h
  utilities/cxEnumConversion.h
  utilities/cxSpaceProviderNull
  utilities/cxOrderedQDomDocument.h
  utilities/cxXmlFileHandler


  logger/cxAudio.h
  logger/cxLogger
  logger/cxLogMessage
  logger/cxLogMessageFilter
  logger/internal/cxLogQDebugRedirecter
  logger/internal/cxLogIOStreamRedirecter
  logger/internal/cxLogFile

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
  settings/cxSessionStorageServiceProxy
  settings/cxRequestEnterStateTransition.h

  vesselReg/SeansVesselReg.cxx
  vesselReg/SeansVesselReg.hxx
  vesselReg/HackTPSTransform.hxx

  patientModel/cxPatientModelServiceNull
  patientModel/cxPatientModelServiceProxy

  Video/cxVideoServiceNull
  Video/cxVideoServiceProxy
  Video/cxStreamerServiceProxy
  Video/cxStreamerServiceNull

  Tool/cxTrackingServiceNull
  Tool/cxTrackingServiceProxy

  usReconstructionTypes/cxUsReconstructionFileMaker
  usReconstructionTypes/cxUsReconstructionFileReader
  usReconstructionTypes/cxUSFrameData
  usReconstructionTypes/cxUSReconstructInputData
  usReconstructionTypes/cxUSReconstructInputDataAlgoritms

  ${CustusX_SOURCE_DIR}/source/ThirdParty/iir1/iir/Butterworth.cpp
  ${CustusX_SOURCE_DIR}/source/ThirdParty/iir1/iir/Cascade.cpp
  ${CustusX_SOURCE_DIR}/source/ThirdParty/iir1/iir/PoleFilter.cpp
  ${CustusX_SOURCE_DIR}/source/ThirdParty/iir1/iir/Biquad.cpp
  )

# Configure the default SSC_DATA_ROOT for the location of SSCData.
unset(SSC_DATA_ROOT)
set(SSC_DATA_ROOT ${CustusX_SOURCE_DIR}/data CACHE INTERNAL "path to ssc data root")


configure_file(
  "${CustusX_SOURCE_DIR}/source/resource/core/utilities/sscConfig.h.in"
  "${CustusX_BINARY_DIR}/source/resource/core/utilities/sscConfig.h"
)

# extra definitions:
add_subdirectory(settings)

set(CX_PUBLIC_RESOURCE_LIBRARIES
  Qt5::Core
  Qt5::Widgets
  Qt5::Xml
  Qt5::Multimedia
  Qt5::Sql
  Qt5::Concurrent
  CTKPluginFramework
  cxPluginUtilities
  ${VTK_LIBRARIES}
  ${OPENGL_LIBRARIES}
  )

set(CX_PRIVATE_RESOURCE_LIBRARIES
  ${CX_PRIVATE_RESOURCE_LIBRARIES}
  ${Boost_LIBRARIES}
  ${ITK_LIBRARIES}
  )
if(CX_USE_OPENCL_UTILITY)
  cx_add_class(cxResource_SOURCE
    utilities/cxOpenCLPrinter
    )

  set(CX_PRIVATE_RESOURCE_LIBRARIES
    ${CX_PRIVATE_RESOURCE_LIBRARIES}
    OpenCLUtilityLibrary
    ${OPENCL_LIBRARIES}
    )
endif()

cx_create_export_header("cxResource")

add_msvc_precompiled_header("cxPrecompiledHeader.h" "cxPrecompiledHeader.cpp" cxResource_SOURCE)

add_library(cxResource ${cxResource_SOURCE})
target_include_directories(cxResource
  PUBLIC
  .
  ./logger
  ./Math
  ./Data
  ./Video
  ./Tool
  ./properties
  ./algorithms
  ./algorithms/ItkVtkGlue
  ./settings
  ./utilities
  ./registration
  ./patientModel
  ./network
  ./usReconstructionTypes
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}/settings
  ${CMAKE_CURRENT_BINARY_DIR}/utilities
  ${PROJECT_SOURCE_DIR}/source/ThirdParty/iir1
  )
target_link_libraries(cxResource
	PUBLIC
	${CX_PUBLIC_RESOURCE_LIBRARIES}
	PRIVATE
	${CX_PRIVATE_RESOURCE_LIBRARIES}
	PUBLIC
	${SSC_GCOV_LIBRARY} # add gcov LAST in the link sequence! (http://stackoverflow.com/questions/566472/where-are-the-gcov-symbols)
	)

cx_doc_define_user_docs("${CMAKE_CURRENT_SOURCE_DIR}/usReconstructionTypes/doc")
cx_doc_define_user_docs("${CMAKE_CURRENT_SOURCE_DIR}/Data/doc")
cx_add_non_source_file("usReconstructionTypes/doc/org.custusx.resource.core.usacquisitionfileformat.md")
cx_add_non_source_file("Data/doc/org.custusx.resource.core.metrics.md")

add_subdirectory(testing)
add_subdirectory(usReconstructionTypes/testing)


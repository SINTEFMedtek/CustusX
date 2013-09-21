###########################################################
##               Resource Core library
###########################################################

cx_add_class_qt_moc(SSC_SOURCE_FILES
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
	Data/sscCoordinateSystemListener
	Data/cxFrameMetricBase

	Video/sscVideoSource.h
	Video/sscVideoRecorder
	Video/sscVideoSourceSHM
	Video/sscTestVideoSource
	Video/sscImageImportVideoSource

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

	Utilities/sscApplication
	Utilities/sscMessageManager
	Utilities/sscSharedMemory
	)

     
# library
cx_add_class( SSC_SOURCE_FILES
    sscForwardDeclarations.h
    vtkForwardDeclarations.h


        Tool/sscProbeSector
        Tool/sscProbeData

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
		Utilities/sscAudio.h

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


# =========================================================
# stuff
# =========================================================
# Configure the default SSC_DATA_ROOT for the location of SSCData.
unset(SSC_DATA_ROOT)
set (SSC_DATA_ROOT ${CustusX3_SOURCE_DIR}/data CACHE INTERNAL "path to ssc data root")
  
configure_file(
  "${CustusX3_SOURCE_DIR}/source/resource/core/utilities/sscConfig.h.in"
  "${CustusX3_SOURCE_DIR}/source/resource/core/utilities/sscConfig.h"
  )

# =========================================================

include_directories( ${SSC_INCLUDE_DIRS} ${SSC_BINARY_DIR} ${QT_INCLUDES} ${Boost_INCLUDE_DIRS} )
link_directories( ${SSC_LIBRARY_DIRS})

# ============================= beginning of resources file ===========================

# extra definitions:
add_subdirectory(settings)

SET( CX_QT_MOC_HEADER_FILES
    algorithms/cxTimedAlgorithm.h
    algorithms/cxThreadedTimedAlgorithm.h
    algorithms/cxCompositeTimedAlgorithm.h
    settings/cxSettings.h
    utilities/cxDisplayTimerWidget.h
    utilities/cxPlaybackTime.h
    utilities/cxTimelineWidget.h
    utilities/cxProcessWrapper.h
	utilities/cxSavingVideoRecorder.h
	utilities/cxImageDataContainer.h
)

SET ( cxResource_SOURCE
	cxForwardDeclarations.h
	${SSC_SOURCE_FILES}

	algorithms/ItkVtkGlue/itkImageToVTKImageFilter.h
    algorithms/ItkVtkGlue/itkImageToVTKImageFilter.txx
    algorithms/ItkVtkGlue/itkVTKImageToImageFilter.h
    algorithms/ItkVtkGlue/itkVTKImageToImageFilter.txx
    algorithms/itkBinaryThinningImageFilter3D.h
    algorithms/itkBinaryThinningImageFilter3D.txx
    algorithms/sscImageAlgorithms.h
    algorithms/sscImageAlgorithms.cpp
    algorithms/cxTimedAlgorithm.h
    algorithms/cxTimedAlgorithm.cpp
    algorithms/cxThreadedTimedAlgorithm.h
    algorithms/cxThreadedTimedAlgorithm.cpp
    algorithms/cxCompositeTimedAlgorithm.h
    algorithms/cxCompositeTimedAlgorithm.cpp
    algorithms/cxAlgorithmHelpers.h
    algorithms/cxAlgorithmHelpers.cpp

    settings/cxDataLocations.cpp
    settings/cxDataLocations.h
    settings/cxSettings.h
    settings/cxSettings.cpp

    utilities/ProbeXmlConfigParser.h
    utilities/ProbeXmlConfigParserImpl.h
    utilities/ProbeXmlConfigParserImpl.cpp
    utilities/ProbeXmlConfigParserMock.h
    utilities/ProbeXmlConfigParserMock.cpp
    utilities/cxCreateProbeDataFromConfiguration.h
    utilities/cxCreateProbeDataFromConfiguration.cpp
    utilities/cxRenderTimer.h
    utilities/cxRenderTimer.cpp
    utilities/cxUsReconstructionFileMaker.h
    utilities/cxUsReconstructionFileMaker.cpp
    utilities/cxUsReconstructionFileReader.h
    utilities/cxUsReconstructionFileReader.cpp
    utilities/cxDisplayTimerWidget.h
    utilities/cxDisplayTimerWidget.cpp
    utilities/cxTransformFile.h
    utilities/cxTransformFile.cpp
    utilities/cxPlaybackTime.h
    utilities/cxPlaybackTime.cpp
    utilities/cxTimelineWidget.h
    utilities/cxTimelineWidget.cpp
    utilities/cxDepthPeeling.h
    utilities/cxDepthPeeling.cpp
    utilities/cxProcessWrapper.h
    utilities/cxProcessWrapper.cpp
    utilities/sscUSFrameData.h
    utilities/sscUSFrameData.cpp
    utilities/cxSavingVideoRecorder.h
    utilities/cxSavingVideoRecorder.cpp
    utilities/cxImageDataContainer.h
    utilities/cxImageDataContainer.cpp
    utilities/cxFileHelpers.h
    utilities/cxFileHelpers.cpp
    utilities/cxUSReconstructInputData.h
    utilities/cxUSReconstructInputData.cpp
    utilities/cxUSReconstructInputDataAlgoritms.h
    utilities/cxUSReconstructInputDataAlgoritms.cpp
    utilities/cxStringHelpers.h
    utilities/cxStringHelpers.cpp
    utilities/cxUSReconstructInputDataAlgoritms.cpp	
)

unset(CX_RESOURCE_INCLUDE_DIRECTORIES)

set(CX_RESOURCE_INCLUDE_DIRECTORIES
    ${CustusX3_SOURCE_DIR}/source/resource/core/Data
    ${CustusX3_SOURCE_DIR}/source/resource/core/Video
#    ${CustusX3_SOURCE_DIR}/source/resource/core/Rep
    ${CustusX3_SOURCE_DIR}/source/resource/core/Tool
    ${CustusX3_SOURCE_DIR}/source/resource/core/Math
    ${CustusX3_SOURCE_DIR}/source/resource/core/Widgets
#    ${CustusX3_SOURCE_DIR}/source/resource/core/vtkMods
    ${CustusX3_SOURCE_DIR}/source/ThirdParty
    ${CustusX3_SOURCE_DIR}/source/resource/core
    ${CustusX3_SOURCE_DIR}/source/resource/core/algorithms
    ${CustusX3_SOURCE_DIR}/source/resource/core/settings
    ${CustusX3_SOURCE_DIR}/source/resource/core/utilities
    ${CustusX3_BINARY_DIR}/source/resource/core
    ${CustusX3_BINARY_DIR}/source/resource/core/utilities
    ${CustusX3_BINARY_DIR}/source/resource/core/settings
    CACHE INTERNAL "Include directories for resource libraries")


include_directories(${CX_RESOURCE_INCLUDE_DIRECTORIES})

QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

set (CX_RESOURCE_LIBRARIES
    ${CX_RESOURCE_LIBRARIES}
    QVTK
    ${SSC_GCOV_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${QT_LIBRARIES}
    ${Boost_LIBRARIES}
    ${ITK_LIBRARIES})

ADD_LIBRARY ( cxResource ${cxResource_SOURCE} ${MOC_HEADER_FILES})
target_link_libraries(cxResource ${CX_RESOURCE_LIBRARIES} )
CX_COTIRE_ADD_LIBRARY( cxResource "${CX_RESOURCE_LIBRARIES}")

add_subdirectory(testing)


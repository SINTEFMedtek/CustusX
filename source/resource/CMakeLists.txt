###########################################################
##               Resource Layer
###########################################################

add_subdirectory(videoServer)
add_subdirectory(OpenIGTLinkUtilities)

#add_subdirectory(utilities)

# extra definitions:
add_subdirectory(settings)

#add_subdirectory(algorithms)
#add_subdirectory(testing)

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

    utilities/probeXmlConfigParser.h
    utilities/probeXmlConfigParser.cpp
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
	
#	OpenIGTLinkUtilities/cxIGTLinkImageMessage.h
#	OpenIGTLinkUtilities/cxIGTLinkImageMessage.cpp    
#    OpenIGTLinkUtilities/cxIGTLinkUSStatusMessage.h
#    OpenIGTLinkUtilities/cxIGTLinkUSStatusMessage.cpp
#    OpenIGTLinkUtilities/igtl_us_status.h
#    OpenIGTLinkUtilities/igtl_us_status.c	
)

include_directories(
    .
    ${CustusX3_SOURCE_DIR}/source/resource
    ${CustusX3_SOURCE_DIR}/source/resource/algorithms
    ${CustusX3_SOURCE_DIR}/source/resource/settings
    ${CustusX3_SOURCE_DIR}/source/resource/utilities
    ${CustusX3_BINARY_DIR}/source/resource/settings
    )

QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

set (CX_RESOURCE_LIBRARIES
    SSC
    ${ITK_LIBRARIES})

ADD_LIBRARY ( cxResource ${cxResource_SOURCE} ${MOC_HEADER_FILES})
target_link_libraries(cxResource ${CX_RESOURCE_LIBRARIES} )


#------------------------------------------------------------------------------
# Cotire - pch and unity builds
#------------------------------------------------------------------------------
if(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)
    cotire(cxResource)
    if(COTIRE_ADD_UNITY_BUILDS)
        target_link_libraries(cxResource_unity ${CX_RESOURCE_LIBRARIES})
    endif(COTIRE_ADD_UNITY_BUILDS)
endif(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)

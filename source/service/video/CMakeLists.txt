###########################################################
##               Video Service
###########################################################

SET( CX_QT_MOC_HEADER_FILES
	cxBasicVideoSource.h
	cxVideoService.h
    cxVideoConnectionManager.h
    cxVideoConnection.h
    cxPlaybackUSAcquisitionVideo.h

    cxImageReceiverThread.h
    cxDirectlyLinkedImageReceiverThread.h
	cxIGTLinkedImageReceiverThread.h
	
	cxSimulatedImageStreamer.h
)

SET (cxVideoService_SOURCE
	cxBasicVideoSource.h
	cxBasicVideoSource.cpp
	cxVideoService.h
	cxVideoService.cpp
    cxVideoConnectionManager.cpp
    cxVideoConnectionManager.h
    cxVideoConnection.h
    cxVideoConnection.cpp
    cxPlaybackUSAcquisitionVideo.h
    cxPlaybackUSAcquisitionVideo.cpp

    cxImageReceiverThread.h
    cxImageReceiverThread.cpp
	cxIGTLinkedImageReceiverThread.h
	cxIGTLinkedImageReceiverThread.cpp
    cxDirectlyLinkedImageReceiverThread.h
    cxDirectlyLinkedImageReceiverThread.cpp
    
    cxSimulatedImageStreamer.h
    cxSimulatedImageStreamer.cpp
)

include_directories(
    .
    ${CustusX3_SOURCE_DIR}/source/resource
    ${CustusX3_SOURCE_DIR}/source/resource/OpenIGTLinkUtilities
    ${CustusX3_SOURCE_DIR}/source/resource/videoServer
    ${CustusX3_SOURCE_DIR}/source/resource/settings
    ${CustusX3_SOURCE_DIR}/source/resource/utilities
    ${CustusX3_SOURCE_DIR}/source/service/tracking
    )


QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

ADD_LIBRARY ( cxVideoService ${cxVideoService_SOURCE} ${MOC_HEADER_FILES} )
target_link_libraries(cxVideoService SSC cxResource OpenIGTLink cxTrackingService cxGrabber)

#add_subdirectory(testing)


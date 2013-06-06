# This file is part of CustusX, an Image Guided Therapy Application.
#
# Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
#
# CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
# code and binaries can only be used by SMT and those with explicit permission
# from SMT. CustusX shall not be distributed to anyone else.
#
# CustusX is a research tool. It is NOT intended for use or certified for use
# in a normal clinical setting. SMT does not take responsibility for its use
# in any way.
#
# See CustusX_License.txt for more information.

###########################################################
#               Video Service
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

add_subdirectory(testing)


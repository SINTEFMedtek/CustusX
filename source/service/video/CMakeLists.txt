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
        cxVideoServiceOld.h
    cxVideoConnectionManager.h
    cxVideoConnection.h
    cxPlaybackUSAcquisitionVideo.h
    cxImageReceiverThread.h
    cxDirectlyLinkedImageReceiverThread.h
    cxIGTLinkedImageReceiverThread.h
    cxStreamerService.h
)

SET (cxVideoServiceOld_SOURCE
    cxBasicVideoSource.h
	cxBasicVideoSource.cpp
	cxVideoServiceOld.cpp
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
    
    cxVideoServiceBackend.h
    cxVideoServiceBackend.cpp
    cxStreamerService.cpp
    cxStreamerServiceUtilities.h
    cxStreamerServiceUtilities.cpp
    cxCommandlineImageStreamerInterface.h
    cxCommandlineImageStreamerInterface.cpp
    cxImageStreamerFactory.h
    cxImageStreamerFactory.cpp
)

include_directories(
    ${CX_RESOURCE_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_USRECONSTRUCTIONTYPES_INCLUDE_DIRECTORIES}
    .
    ${CustusX_SOURCE_DIR}/source/resource/videoServer
    ${CustusX_SOURCE_DIR}/source/resource/OpenIGTLinkUtilities
    ${CustusX_SOURCE_DIR}/source/service/tracking
    ${CMAKE_CURRENT_BINARY_DIR}
    #${CustusX_SOURCE_DIR}/source/resource/widgets
    #${CustusX_SOURCE_DIR}/source/logic
    ${CustusX_SOURCE_DIR}/source/resource/pluginUtilities

#cxResourceWidgets
    )


QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

ADD_LIBRARY ( cxVideoServiceOld ${cxVideoServiceOld_SOURCE} ${MOC_HEADER_FILES} )
target_link_libraries(cxVideoServiceOld cxResource cxResourceUsReconstructionTypes OpenIGTLink cxTrackingService cxGrabber CTKPluginFramework)

add_subdirectory(testing)


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

#option(CX_BUILD_US_SIMULATOR "Build the US from CT/MR simulator" OFF)

SET( CX_QT_MOC_HEADER_FILES
    cxBasicVideoSource.h
    cxVideoService.h
    cxVideoConnectionManager.h
    cxVideoConnection.h
    cxPlaybackUSAcquisitionVideo.h
    cxImageReceiverThread.h
    cxDirectlyLinkedImageReceiverThread.h
    cxIGTLinkedImageReceiverThread.h
    #cxSimulatedImageStreamer.h
    cxStreamerService.h
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
    
    #cxSimulatedImageStreamer.h
    #cxSimulatedImageStreamer.cpp
    cxVideoServiceBackend.h
    cxVideoServiceBackend.cpp
    cxStreamerService.cpp
    #cxSimulatedImageStreamerInterface.h
    #cxSimulatedImageStreamerInterface.cpp
    cxCommandlineImageStreamerInterface.h
    cxCommandlineImageStreamerInterface.cpp
    cxImageStreamerFactory.h
    cxImageStreamerFactory.cpp
)

# Build all files from the UltrasoundSimulation lib
#IF(CX_BUILD_US_SIMULATOR)
#   SET(SIMULATOR_SPECKLE_PATH ${CustusX3_SOURCE_DIR}/../../UltrasoundSimulation/UltrasoundSimulation)

#    configure_file(
#        "${CMAKE_CURRENT_SOURCE_DIR}/simConfig.h.in"
#        "${CMAKE_CURRENT_BINARY_DIR}/simConfig.h"
#    )

#    SET(cxVideoService_SOURCE
#        ${cxVideoService_SOURCE}
#        ${CustusX3_SOURCE_DIR}/../../UltrasoundSimulation/UltrasoundSimulation/ImageSimulator.h
#        ${CustusX3_SOURCE_DIR}/../../UltrasoundSimulation/UltrasoundSimulation/ImageSimulator.cpp
#        ${CustusX3_SOURCE_DIR}/../../UltrasoundSimulation/UltrasoundSimulation/ImageArray.h
#        ${CustusX3_SOURCE_DIR}/../../UltrasoundSimulation/UltrasoundSimulation/ImageArray.cpp
#    )
#ENDIF(CX_BUILD_US_SIMULATOR)

include_directories(
    ${CX_RESOURCE_INCLUDE_DIRECTORIES}
    ${CX_RESOURCE_USRECONSTRUCTIONTYPES_INCLUDE_DIRECTORIES}
    .
    ${CustusX3_SOURCE_DIR}/source/resource/videoServer
    ${CustusX3_SOURCE_DIR}/source/resource/OpenIGTLinkUtilities
    ${CustusX3_SOURCE_DIR}/source/service/tracking
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CustusX3_SOURCE_DIR}/source/resource/widgets
#cxResourceWidgets
    )


QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

ADD_LIBRARY ( cxVideoService ${cxVideoService_SOURCE} ${MOC_HEADER_FILES} )
target_link_libraries(cxVideoService cxResource cxResourceUsReconstructionTypes OpenIGTLink cxTrackingService cxGrabber)

add_subdirectory(testing)


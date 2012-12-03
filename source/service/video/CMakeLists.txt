###########################################################
##               Video Service
###########################################################

SET( CX_QT_MOC_HEADER_FILES
	cxVideoService.h
    cxVideoConnection.h
    cxOpenIGTLinkClient.h
    cxOpenIGTLinkRTSource.h
    cxPlaybackUSAcquisitionVideo.h
#    cxOpenIGTLinkDirectLinkRTSource.h
    cxGrabberDirectLinkThread.h
    cxIGTLinkClientBase.h
)

SET (cxVideoService_SOURCE
	cxVideoService.h
	cxVideoService.cpp
    cxVideoConnection.h
    cxVideoConnection.cpp
    cxOpenIGTLinkClient.h
    cxOpenIGTLinkClient.cpp
    cxOpenIGTLinkRTSource.h
    cxOpenIGTLinkRTSource.cpp
    cxPlaybackUSAcquisitionVideo.h
    cxPlaybackUSAcquisitionVideo.cpp
#    cxOpenIGTLinkDirectLinkRTSource.cpp
#    cxOpenIGTLinkDirectLinkRTSource.h
    cxGrabberDirectLinkThread.h
    cxGrabberDirectLinkThread.cpp
    cxIGTLinkClientBase.h
    cxIGTLinkClientBase.cpp
    ../../../modules/grabberCommon/cxIGTLinkUSStatusMessage.h
    ../../../modules/grabberCommon/cxIGTLinkUSStatusMessage.cpp
    ../../../modules/grabberCommon/igtl_us_status.h
    ../../../modules/grabberCommon/igtl_us_status.c
)

include_directories(
    .
    ${CustusX3_SOURCE_DIR}/source/resource
    ${CustusX3_SOURCE_DIR}/modules/OpenIGTLinkServer
    ${CustusX3_SOURCE_DIR}/source/resource/settings
    ${CustusX3_SOURCE_DIR}/source/resource/utilities
    ${CustusX3_SOURCE_DIR}/source/service/tracking
    )


QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

ADD_LIBRARY ( cxVideoService ${cxVideoService_SOURCE} ${MOC_HEADER_FILES} )
target_link_libraries(cxVideoService SSC cxResource OpenIGTLink cxTrackingService cxGrabber)



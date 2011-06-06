###########################################################
##               Video Service
###########################################################

SET( CX_QT_MOC_HEADER_FILES
	cxVideoService.h
	
    cxVideoConnection.h
    cxOpenIGTLinkClient.h
    cxOpenIGTLinkRTSource.h
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
)

include_directories(
    .
    ${CustusX3_SOURCE_DIR}/source/resource
#    ${CustusX3_SOURCE_DIR}/source/stateMachineManager # REMOVE    
#    ${CustusX3_SOURCE_DIR}/source/registrationManager # REMOVE    
#    ${CustusX3_SOURCE_DIR}/source/gui/dataAdapters # REMOVE    
#    ${CustusX3_SOURCE_DIR}/source/service/tracking # REMOVE    
#    ${CustusX3_SOURCE_DIR}/source/service/visualization # REMOVE    
    ${CustusX3_SOURCE_DIR}/source/resource/settings
    ${CustusX3_SOURCE_DIR}/source/resource/utilities
    )


QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

ADD_LIBRARY ( cxVideoService ${cxVideoService_SOURCE} ${MOC_HEADER_FILES} )
target_link_libraries(cxVideoService SSC cxSettings cxUtilities OpenIGTLink)



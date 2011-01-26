SET (LIB statemachineManager)

SET( CX_QT_MOC_HEADER_FILES
	cxWorkflowStateMachine.h
	cxWorkflowState.h
	cxApplicationStateMachine.h
	cxApplicationState.h
	cxPatientData.h
	cxFileCopied.h
	cxStateMachineManager.h
    RTSource/cxOpenIGTLinkConnection.h
    RTSource/sscOpenIGTLinkClient.h
    RTSource/sscOpenIGTLinkRTSource.h
)

SET ( ${LIB}_statemachineManager
	cxStateMachineManager.cpp
	cxWorkflowStateMachine.h
	cxWorkflowStateMachine.cpp
	cxWorkflowState.h
	cxWorkflowState.cpp
	cxApplicationStateMachine.cpp
	cxApplicationStateMachine.h
	cxApplicationState.h
    cxApplicationState.cpp
	cxRequestEnterStateTransition.h
	cxPatientData.h
	cxPatientData.cpp
	cxFileCopied.h
	cxFileCopied.cpp
	cxRecordSession.h
    cxRecordSession.cpp
    RTSource/cxOpenIGTLinkConnection.h
    RTSource/cxOpenIGTLinkConnection.cpp
    RTSource/sscOpenIGTLinkClient.h
    RTSource/sscOpenIGTLinkClient.cpp
    RTSource/sscOpenIGTLinkRTSource.h
    RTSource/sscOpenIGTLinkRTSource.cpp    
)

QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

ADD_LIBRARY ( ${LIB} ${${LIB}_statemachineManager} ${MOC_HEADER_FILES})
target_link_libraries(${LIB} SSC cxCommon viewManager registrationManager)

SET (LIB statemachineManager)

# Make VERSION_NUMBER available for c++ files in stateMachineManger folder
ADD_DEFINITIONS(-DVERSION_NUMBER="${VERSION_NUMBER}")
ADD_DEFINITIONS(-DVERSION_NUMBER_VERBOSE="${VERSION_NUMBER_VERBOSE}")

SET( CX_QT_MOC_HEADER_FILES
	cxWorkflowStateMachine.h
	cxWorkflowState.h
	cxApplicationStateMachine.h
	cxApplicationState.h
	cxPatientData.h
	cxFileCopied.h
	cxStateMachineManager.h
#    RTSource/cxRTSourceManager.h
#    RTSource/sscOpenIGTLinkClient.h
#    RTSource/sscOpenIGTLinkRTSource.h
    cxUSAcquisition.h
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
#    RTSource/cxRTSourceManager.h
#    RTSource/cxRTSourceManager.cpp
#    RTSource/sscOpenIGTLinkClient.h
#    RTSource/sscOpenIGTLinkClient.cpp
#    RTSource/sscOpenIGTLinkRTSource.h
#    RTSource/sscOpenIGTLinkRTSource.cpp    
    cxUSAcquisition.h
    cxUSAcquisition.cpp
)

QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

ADD_LIBRARY ( ${LIB} ${${LIB}_statemachineManager} ${MOC_HEADER_FILES})
target_link_libraries(${LIB} SSC cxCommon viewManager registrationManager sscUsReconstructionLib cxVideoService)

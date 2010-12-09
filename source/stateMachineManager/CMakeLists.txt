SET (LIB statemachineManager)

SET( CX_QT_MOC_HEADER_FILES
	cxWorkflowStateMachine.h
	cxWorkflowState.h
	cxApplicationStateMachine.h
	cxApplicationState.h
	cxPatientData.h
	cxFileCopied.h
#	cxRecordSessionWidget.h
	cxStateMachineManager.h
)

SET ( ${LIB}_statemachineManager
	cxStateMachineManager.cpp
	cxWorkflowStateMachine.h
	cxWorkflowStateMachine.cpp
	cxWorkflowState.h
	cxApplicationStateMachine.cpp
	cxApplicationStateMachine.h
	cxApplicationState.h
    cxApplicationState.cpp
	cxRequestEnterStateTransition.h
	cxPatientData.h
	cxPatientData.cpp
	cxFileCopied.h
	cxFileCopied.cpp
#	cxRecordSessionWidget.cpp
)

QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

ADD_LIBRARY ( ${LIB} ${${LIB}_statemachineManager} ${MOC_HEADER_FILES})
target_link_libraries(${LIB} SSC cxCommon viewManager registrationManager)

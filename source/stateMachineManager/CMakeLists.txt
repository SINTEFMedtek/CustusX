SET (LIB statemachineManager)

SET( CX_QT_MOC_HEADER_FILES
	cxWorkflowStateMachine.h
	cxWorkflowState.h
)

SET ( ${LIB}_statemachineManager
	cxStateMachineManager.h
	cxStateMachineManager.cpp
	cxWorkflowStateMachine.h
	cxWorkflowStateMachine.cpp
	cxWorkflowState.h
	cxRequestEnterStateTransition.h
)

QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

ADD_LIBRARY ( ${LIB} ${${LIB}_statemachineManager} ${MOC_HEADER_FILES})
target_link_libraries(${LIB} SSC) 

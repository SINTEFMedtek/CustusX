###########################################################
##               State Service
##
##  Dependencies: cxVisualizationService
##
##
###########################################################

SET( CX_QT_MOC_HEADER_FILES
	cxWorkflowStateMachine.h
	cxWorkflowState.h
	cxApplicationStateMachine.h
	cxApplicationState.h
	cxStateService.h
	cxStateServiceBackend.h
)

SET ( cxStateService_SOURCE
	cxStateService.cpp
	cxWorkflowStateMachine.h
	cxWorkflowStateMachine.cpp
	cxWorkflowState.h
	cxWorkflowState.cpp
	cxApplicationStateMachine.cpp
	cxApplicationStateMachine.h
	cxApplicationState.h
	cxApplicationState.cpp
	cxRequestEnterStateTransition.h
	cxStateServiceBackend.cpp
)

include_directories(${CX_RESOURCE_INCLUDE_DIRECTORIES})
include_directories(
    .
    ${CustusX_SOURCE_DIR}/source/service/patient     
    ${CustusX_SOURCE_DIR}/source/service/tracking     
    ${CustusX_SOURCE_DIR}/source/service/visualization
    ${CustusX_SOURCE_DIR}/source/service/video
    ${CustusX_SOURCE_DIR}/source/resource/pluginUtilities
    )

QT5_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

ADD_LIBRARY ( cxStateService ${cxStateService_SOURCE} ${MOC_HEADER_FILES})
target_link_libraries(cxStateService cxVisualizationService cxVideoService)

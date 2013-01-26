###########################################################
##               Logic Layer
###########################################################

# definition of the cxLogicManager module:

SET( CX_QT_MOC_HEADER_FILES
	cxServiceController.h
	cxLogicManager.h
)

SET (cxLogicManager_SOURCE
    cxServiceController.h
    cxServiceController.cpp
    cxLogicManager.h
    cxLogicManager.cpp
)

include_directories(
    .
    ${CustusX3_SOURCE_DIR}/source/resource
    ${CustusX3_SOURCE_DIR}/source/resource/utilities
    ${CustusX3_SOURCE_DIR}/source/resource/settings

    ${CustusX3_SOURCE_DIR}/source/service/patient     
    ${CustusX3_SOURCE_DIR}/source/service/tracking     
    ${CustusX3_SOURCE_DIR}/source/service/visualization
    ${CustusX3_SOURCE_DIR}/source/service/video
    ${CustusX3_SOURCE_DIR}/source/service/state
    )


QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

ADD_LIBRARY ( cxLogicManager ${cxLogicManager_SOURCE} ${MOC_HEADER_FILES} )

target_link_libraries(cxLogicManager
    SSC
    cxPatientService
    cxTrackingService
    cxVisualizationService
    cxVideoService
	cxStateService
    )



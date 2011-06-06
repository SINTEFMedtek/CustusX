SET (LIB registrationManager)

SET( CX_QT_MOC_HEADER_FILES
	cxRegistrationManager.h
)

SET ( ${LIB}_registrationManager
	cxRegistrationManager.h
	cxRegistrationManager.cpp
	cxFrameForest.h
    cxFrameForest.cpp
    cxLandmarkTranslationRegistration.cpp
)

include_directories(
    .
    ${CustusX3_SOURCE_DIR}/source/resource
#    ${CustusX3_SOURCE_DIR}/source/stateMachineManager    
#    ${CustusX3_SOURCE_DIR}/source/registrationManager    
#    ${CustusX3_SOURCE_DIR}/source/gui/dataAdapters     
    ${CustusX3_SOURCE_DIR}/source/service/patient     
    ${CustusX3_SOURCE_DIR}/source/service/tracking     
    ${CustusX3_SOURCE_DIR}/source/service/visualization
    ${CustusX3_SOURCE_DIR}/source/service/video
    ${CustusX3_SOURCE_DIR}/source/algorithms
    ${CustusX3_SOURCE_DIR}/modules/cxCommon
    )
    
QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

ADD_LIBRARY ( ${LIB} ${${LIB}_registrationManager} ${MOC_HEADER_FILES})
target_link_libraries(${LIB} cxTrackingService cxPatientService cxAlgorithms SSC) 

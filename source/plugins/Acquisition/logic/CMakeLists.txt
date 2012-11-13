###########################################################
##               Acquisition Plugin Logic
##
##               Logic Part
##
###########################################################


SET( CX_QT_MOC_HEADER_FILES
    cxAcquisitionData.h
    cxUSAcquisition.h
)

SET ( cxPluginAcquisitionLogic_SOURCE
    cxAcquisitionData.h
    cxAcquisitionData.cpp
    cxRecordSession.h
    cxRecordSession.cpp
    cxUSAcquisition.h
    cxUSAcquisition.cpp
)

include_directories(
    .
    ${CustusX3_SOURCE_DIR}/source/resource
    ${CustusX3_SOURCE_DIR}/source/resource/settings
    ${CustusX3_SOURCE_DIR}/source/resource/utilities
    ${CustusX3_SOURCE_DIR}/source/resource/algorithms
    ${CustusX3_SOURCE_DIR}/source/service/patient     
    ${CustusX3_SOURCE_DIR}/source/service/tracking     
    ${CustusX3_SOURCE_DIR}/source/service/visualization
    ${CustusX3_SOURCE_DIR}/source/service/video
    
    ${CustusX3_SOURCE_DIR}/source/registrationManager    
    ${CustusX3_SOURCE_DIR}/source/algorithms

    ${CustusX3_SOURCE_DIR}/source/plugins/UsReconstruction/logic
    ${CustusX3_SOURCE_DIR}/source/plugins/UsReconstruction/logic/3DReconstruction    
    )

QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

ADD_LIBRARY ( cxPluginAcquisitionLogic ${cxPluginAcquisitionLogic_SOURCE} ${MOC_HEADER_FILES})
target_link_libraries(cxPluginAcquisitionLogic SSC cxVisualizationService cxPluginUSReconstructionLogic cxVideoService)

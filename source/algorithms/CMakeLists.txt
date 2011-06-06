SET (LIB cxAlgorithms)

SET( CX_QT_MOC_HEADER_FILES
    cxCenterline.h
    cxResample.h
    cxBinaryThresholdImageFilter.h
    cxContour.h
    cxConnectedThresholdImageFilter.h
)

SET ( ${LIB}_algorithms
    cxTrackingDataToVolume.h
    cxTrackingDataToVolume.cpp
    cxCenterline.h
    cxCenterline.cpp
    cxResample.h
    cxResample.cpp
    cxBinaryThresholdImageFilter.h
    cxBinaryThresholdImageFilter.cpp
    cxContour.h
    cxContour.cpp
    cxConnectedThresholdImageFilter.h
    cxConnectedThresholdImageFilter.cpp
    cxTemporalCalibration.h
    cxTemporalCalibration.cpp
)

include_directories(
    .
    ${CustusX3_SOURCE_DIR}/source/resource
    #${CustusX3_SOURCE_DIR}/source/service/patient
    #${CustusX3_SOURCE_DIR}/source/service/tracking
    ${CustusX3_SOURCE_DIR}/source/logic/stateMachineManager
#    ${CustusX3_SOURCE_DIR}/source/UsReconstruction
    ${CustusX3_SOURCE_DIR}/source/resource/settings
    ${CustusX3_SOURCE_DIR}/source/resource/utilities
    ${CustusX3_SOURCE_DIR}/source/resource/algorithms
    ${CustusX3_SOURCE_DIR}/source/plugins/UsReconstruction/logic
    )

QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

ADD_LIBRARY ( ${LIB} ${${LIB}_algorithms} ${MOC_HEADER_FILES})
target_link_libraries(${LIB} cxPatientService cxAlgorithms_Res cxPluginUSReconstructionLogic) 

add_subdirectory( testing )

###########################################################
##               Patient Service
###########################################################


SET( CX_QT_MOC_HEADER_FILES
	cxDataManager.h
)

SET ( cxPatientService_SOURCE
	cxDataManager.h
	cxDataManager.cpp
	cxPresetTransferFunctions3D.h
	cxPresetTransferFunctions3D.cpp
#	../cxForwardDeclarations.h
)

include_directories(
    .
    ${CustusX3_SOURCE_DIR}/modules/cxCommon
    )

QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

ADD_LIBRARY ( cxPatientService ${cxPatientService_SOURCE} ${MOC_HEADER_FILES})
target_link_libraries(cxPatientService SSC cxCommon)



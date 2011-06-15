###########################################################
##               Patient Service
###########################################################


SET( CX_QT_MOC_HEADER_FILES
    cxFileCopied.h
    cxPatientData.h
    cxPatientService.h
	cxDataManager.h
)

SET ( cxPatientService_SOURCE
	cxDataManager.h
	cxDataManager.cpp
	cxPresetTransferFunctions3D.h
	cxPresetTransferFunctions3D.cpp
	cxPatientData.h
    cxPatientData.cpp
    cxFileCopied.h
    cxFileCopied.cpp
    cxPatientService.h
    cxPatientService.cpp
#	../cxForwardDeclarations.h
)

include_directories(
    .
    ${CustusX3_SOURCE_DIR}/source/resource/settings
    )

QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

ADD_LIBRARY ( cxPatientService ${cxPatientService_SOURCE} ${MOC_HEADER_FILES})
target_link_libraries(cxPatientService SSC cxSettings)



###########################################################
##               Patient Service
###########################################################

SET( CX_QT_MOC_HEADER_FILES
    cxPatientData.h
    cxPatientService.h
    cxActiveImageProxy.h
)

SET ( cxPatientService_SOURCE
	cxPatientData.h
    cxPatientData.cpp
    cxPatientService.h
    cxPatientService.cpp
    cxActiveImageProxy.h
    cxActiveImageProxy.cpp
    
#	../cxForwardDeclarations.h
)

include_directories(
    ${CX_RESOURCE_INCLUDE_DIRECTORIES}
    )

cx_remove_duplicate_include_directories()
QT5_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

cx_create_export_header("cxPatientService")

add_library( cxPatientService ${cxPatientService_SOURCE} ${MOC_HEADER_FILES})
target_link_libraries(cxPatientService cxResource)

add_subdirectory(testing)



###########################################################
##               Patient Service
###########################################################

SET( CX_QT_MOC_HEADER_FILES
    cxPatientData.h
    cxPatientService.h
    cxActiveImageProxy.h
	cxActiveImageProxyOld.h
)

SET ( cxPatientService_SOURCE
	cxPatientData.h
    cxPatientData.cpp
    cxPatientService.h
    cxPatientService.cpp
    cxActiveImageProxy.h
    cxActiveImageProxy.cpp
	cxActiveImageProxyOld.cpp
    
#	../cxForwardDeclarations.h
)

include_directories(
    ${CX_RESOURCE_INCLUDE_DIRECTORIES}
    )

QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

ADD_LIBRARY ( cxPatientService ${cxPatientService_SOURCE} ${MOC_HEADER_FILES})
target_link_libraries(cxPatientService cxResource)

add_subdirectory(testing)



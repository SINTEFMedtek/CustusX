SET (LIB registrationManager)

SET( CX_QT_MOC_HEADER_FILES
	cxRegistrationManager.h
)

SET ( ${LIB}_registrationManager
	cxRegistrationManager.h
	cxRegistrationManager.cpp
)

QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

ADD_LIBRARY ( ${LIB} ${${LIB}_registrationManager} ${MOC_HEADER_FILES})
target_link_libraries(${LIB} toolManager dataManager SSC) 

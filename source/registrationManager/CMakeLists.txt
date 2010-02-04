SET (LIB registrationManager)

SET( CX_QT_MOC_HEADER_FILES
	cxRegistrationManager.h
)

SET ( ${LIB}_registrationManager
	cxRegistrationManager.h
	cxRegistrationManager.cpp
)

QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

ADD_LIBRARY ( ${LIB} STATIC ${${LIB}_registrationManager} ${MOC_HEADER_FILES})

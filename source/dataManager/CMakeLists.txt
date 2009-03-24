SET (LIB dataManager)

SET( CX_QT_MOC_HEADER_FILES
	cxDataManager.h
)

SET ( ${LIB}_dataManager
	cxDataManager.cpp
)

QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

ADD_LIBRARY ( ${LIB} STATIC ${${LIB}_dataManager} ${MOC_HEADER_FILES})

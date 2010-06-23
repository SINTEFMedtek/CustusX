SET (LIB dataManager)

SET( CX_QT_MOC_HEADER_FILES
	cxDataManager.h
)

SET ( ${LIB}_dataManager
	cxDataManager.h
	cxDataManager.cpp
	cxPresetTransferFunctions3D.h
	cxPresetTransferFunctions3D.cpp
	../cxForwardDeclarations.h
)

QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

ADD_LIBRARY ( ${LIB} STATIC ${${LIB}_dataManager} ${MOC_HEADER_FILES})



SET (LIB dataManager)

SET( CX_QT_MOC_HEADER_FILES
	cxDataManager.h
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

SET ( ${LIB}_dataManager
    cxDataLocations.cpp
    cxDataLocations.h
	cxDataManager.h
	cxDataManager.cpp
	cxPresetTransferFunctions3D.h
	cxPresetTransferFunctions3D.cpp
	../cxForwardDeclarations.h
)

QT4_WRAP_CPP( MOC_HEADER_FILES ${CX_QT_MOC_HEADER_FILES} )

ADD_LIBRARY ( ${LIB} STATIC ${${LIB}_dataManager} ${MOC_HEADER_FILES})

#message(status "custus dir local: " ${CustusX3_SOURCE_DIR})

set (CX_CONFIG_ROOT ${CustusX3_SOURCE_DIR}/config)
set (CX_DATA_ROOT ${CustusX3_SOURCE_DIR}/data)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cxConfig.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cxConfig.h"
  )

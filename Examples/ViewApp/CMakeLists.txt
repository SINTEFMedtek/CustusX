cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# =========================================================
# Sonowand Sintef Cooperation C++ Test App 
# =========================================================

find_package(SSC REQUIRED)
include (${SSC_USE_FILE})

SET( SSCTestApp_QT_MOC_HEADER_FILES
    sscTestInterface.h
    )

# library
SET( SSCTestApp_SOURCE_FILES
    sscTestInterface.cpp
    main.cpp
	) 
	
include_directories( ${SNW_INCLUDE_DIRS} ${SSC_BINARY_DIR} )	
	
# clear variables; Qt appends to these vars
set( MOC_HEADER_FILES )
QT4_WRAP_CPP( MOC_HEADER_FILES ${SSCTestApp_QT_MOC_HEADER_FILES} )
		
# =========================================================
# create the target
add_executable( SSCTestApp MACOSX_BUNDLE ${MOC_HEADER_FILES} ${SSCTestApp_SOURCE_FILES} )
target_link_libraries( SSCTestApp SSC )
# =========================================================




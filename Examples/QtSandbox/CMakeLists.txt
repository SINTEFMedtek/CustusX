cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# =========================================================
# SSC C++ Test App for Qt fun stuff 
# =========================================================

find_package(SSC REQUIRED)
include (${SSC_USE_FILE})

SET( MOC_HEADER_FILES
    sscMainWindow.h
    )

# library
SET( SOURCE_FILES
    sscMainWindow.cpp
    main.cpp
	) 
	
include_directories( ${SNW_INCLUDE_DIRS} ${SSC_BINARY_DIR} )	
	
QT4_WRAP_CPP( MOC_HEADER_FILES ${MOC_HEADER_FILES} )
	
QT4_ADD_RESOURCES( QT_RESOURCE_FILES resource.qrc )
		
# =========================================================
# create the target
add_executable( sscQtSandbox MACOSX_BUNDLE 
    ${QT_RESOURCE_FILES} 
    ${MOC_HEADER_FILES} 
    ${SOURCE_FILES} )
target_link_libraries( sscQtSandbox SSC )
# =========================================================




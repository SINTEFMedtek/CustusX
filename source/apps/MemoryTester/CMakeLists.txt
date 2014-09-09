
# =========================================================
# CX C++ Test App for Qt fun stuff 
# =========================================================

SET( MOC_HEADER_FILES
    cxMemoryTester.h
    )

# library
SET( SOURCE_FILES
    cxMemoryTester.cpp
    cxMemoryTesterMain.cpp
	) 
	
include_directories(${CX_RESOURCE_INCLUDE_DIRECTORIES})


QT5_WRAP_CPP( MOC_HEADER_FILES ${MOC_HEADER_FILES} )
	
QT5_ADD_RESOURCES( QT_RESOURCE_FILES resource.qrc )
		
# =========================================================
# create the target
add_executable( cxMemoryTester MACOSX_BUNDLE 
    ${QT_RESOURCE_FILES} 
    ${MOC_HEADER_FILES} 
    ${SOURCE_FILES} )
target_link_libraries( cxMemoryTester cxResource )
# =========================================================



cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

find_package(SSC REQUIRED)
include (${SSC_USE_FILE})

# library
SET( SOURCE_FILES
    main.cpp
    sscMainWindow.cpp
	) 
	
include_directories( ${SNW_INCLUDE_DIRS} ${SSC_BINARY_DIR} )	

QT4_ADD_RESOURCES(SOURCE_FILES ../../gui/cxResources.qrc)

QT4_WRAP_CPP( SOURCE_FILES sscMainWindow.h )
	
# =========================================================
# create the target
add_executable( sscUsReconstruction MACOSX_BUNDLE ${SOURCE_FILES} )
target_link_libraries( sscUsReconstruction sscUsReconstructionLib )
# =========================================================




# =========================================================
# Sonowand Sintef Cooperation C++ UsReconstruction App 
# =========================================================

cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

find_package(SSC REQUIRED)
include (${SSC_USE_FILE})

SET( QT_MOC_HEADER_FILES
    )

# library
SET( SOURCE_FILES
    main.cpp
	) 
	
include_directories( ${SNW_INCLUDE_DIRS} ${SSC_BINARY_DIR} )	

QT4_WRAP_CPP( MOC_HEADER_FILES ${QT_MOC_HEADER_FILES} )


include_directories( ${SSC_SOURCE_DIR}/Code/UsReconstruction )
link_directories( ${SSC_SOURCE_DIR}/Code/UsReconstruction )
		
# =========================================================
# create the target
add_executable( sscUsReconstruction MACOSX_BUNDLE ${MOC_HEADER_FILES} ${SOURCE_FILES} )
target_link_libraries( sscUsReconstruction sscUsReconstructionLib )
# =========================================================




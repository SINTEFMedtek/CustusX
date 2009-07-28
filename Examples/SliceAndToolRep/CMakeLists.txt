cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# =========================================================
# Example app
# =========================================================

find_package(SSC REQUIRED)
include (${SSC_USE_FILE} )
	
include_directories( ${SNW_INCLUDE_DIRS} ${SSC_BINARY_DIR} )
include_directories( ${SSC_SOURCE_DIR}/Examples/SliceAndToolRep )


QT4_WRAP_CPP( SSC_EX_SOURCE_FILES 
    sscExWindow.h)
set(SSC_EX_SOURCE_FILES
    sscExWindow.cpp 
    main.cpp
    ${SSC_EX_SOURCE_FILES}
    )

add_executable(exSliceAndToolRep 
    ${SSC_EX_SOURCE_FILES}
    )
target_link_libraries(exSliceAndToolRep SSC)
set_target_properties(exSliceAndToolRep PROPERTIES LINK_FLAGS "${SNW_LINK_FLAGS}" COMPILE_FLAGS "${SNW_COMPILE_FLAGS}")


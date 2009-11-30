cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# =========================================================
# app for reading sonowand recorder position files 
# =========================================================

find_package(SSC REQUIRED)
include (${SSC_USE_FILE} )
	
#include_directories( ${SNW_INCLUDE_DIRS} ${SSC_BINARY_DIR} )
#include_directories( ${SSC_SOURCE_DIR}/Examples/SliceAndToolRep )

add_executable(sscPositionFileReader main.cpp)
target_link_libraries(sscPositionFileReader SSC)
set_target_properties(sscPositionFileReader PROPERTIES LINK_FLAGS "${SNW_LINK_FLAGS}" COMPILE_FLAGS "${SNW_COMPILE_FLAGS}")


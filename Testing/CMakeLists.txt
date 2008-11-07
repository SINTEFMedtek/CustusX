cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# =========================================================
# Sonowand Sintef Cooperation C++ automated testing directory
# =========================================================

find_package(SSC REQUIRED)
include (${SSC_USE_FILE})
	
include_directories( ${SNW_INCLUDE_DIRS} ${SSC_BINARY_DIR} )

add_executable(TestDataManager sscTestDataManager.cpp)
target_link_libraries( TestDataManager SSC )
	
add_test(TestDataManager ${EXECUTABLE_OUTPUT_PATH}/TestDataManager)




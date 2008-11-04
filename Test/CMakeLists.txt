cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# =========================================================
# Sonowand Sintef Cooperation C++ Test App 
# =========================================================
project( SSCTest )

# =========================================================
# Define variables for paths and include files.
# =========================================================

# Must be the same as the project name. Replaces CMAKE_PROJECT_NAME,
# which I cannot get to work properly
set( SNW_PROJECT_NAME SSCTest ) 

set( SNW_SOURCE_ROOT_DIR ${PROJECT_SOURCE_DIR}/.. )
message(STATUS ${PROJECT_SOURCE_DIR}/..)

SET( SNW_QT_MOC_HEADER_FILES
    sscTestInterface.h
    )

SET( SNW_HEADER_FILES
    ${SNW_QT_MOC_HEADER_FILES}
    )

set( SNW_INCLUDE_DIRS
    ${SNW_SOURCE_ROOT_DIR}/Library/
    ${SNW_SOURCE_ROOT_DIR}/Library/Data/
    ${SNW_SOURCE_ROOT_DIR}/Library/Rep/
    ${SNW_SOURCE_ROOT_DIR}/Library/Tool/
    ${SNW_SOURCE_ROOT_DIR}/Library/Utilities/
    ) 
message(STATUS ${SNW_SOURCE_ROOT_DIR}/Data/)

set(SNW_LIBRARY_DIRS 
    ${SNW_SOURCE_ROOT_DIR}/
    )    

set( SNW_LIBRARIES 
    SSC    
     )

# library
SET( SNW_SOURCE_FILES
    sscTestInterface.cpp
    main.cpp
	) 
	
# =========================================================
# create an executable
include(${SNW_SOURCE_ROOT_DIR}/CMake/snwAddExecutable.cmake )
# =========================================================

add_executable(TestDataManager sscTestDataManager.cpp)
add_test(TestDataManager ${EXECUTABLE_OUTPUT_PATH}/TestDataManager)
#add_test(TestDataManager ${PROJECT_BINARY_DIR}/TestDataManager)




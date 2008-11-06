cmake_minimum_required( VERSION 2.4 )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# =========================================================
# Sonowand Sintef Cooperation C++ Test App 
# =========================================================
project( SSCTestApp )

# =========================================================
# Define variables for paths and include files.
# =========================================================

# Must be the same as the project name. Replaces CMAKE_PROJECT_NAME,
# which I cannot get to work properly
set( SNW_PROJECT_NAME SSCTestApp ) 

set( SNW_SOURCE_ROOT_DIR ${PROJECT_SOURCE_DIR}/../.. )
message(STATUS ${PROJECT_SOURCE_DIR}/..)

SET( SNW_QT_MOC_HEADER_FILES
    sscTestInterface.h
    )

SET( SNW_HEADER_FILES
    ${SNW_QT_MOC_HEADER_FILES}
    )

set( SNW_INCLUDE_DIRS
    ${SNW_SOURCE_ROOT_DIR}/Code/
    ${SNW_SOURCE_ROOT_DIR}/Code/Data/
    ${SNW_SOURCE_ROOT_DIR}/Code/Rep/
    ${SNW_SOURCE_ROOT_DIR}/Code/Tool/
    ${SNW_SOURCE_ROOT_DIR}/Code/Utilities/
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





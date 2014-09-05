###########################################################
##               CustusX project
###########################################################

project(CustusX)

cmake_minimum_required(VERSION 2.8.11)

# kill warning about mixing full and partial paths in libs:
# http://www.cmake.org/cmake/help/v2.8.10/cmake.html#policy%3aCMP0003
if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${PROJECT_SOURCE_DIR}/CMake)

include(cxUtilities)
include(cxInstallUtilities)
include(cxInitializeLibraries)
include(cxCatchUtilities)

#------------------------------------------------------------------------------
# Add some compiler flags (preprocessor definitions)
#------------------------------------------------------------------------------

cx_set_os_identifiers()

# _USE_MATH_DEFINES Must be defined before math.h is included
if (CX_WINDOWS)
    add_definitions(-D_USE_MATH_DEFINES)
endif(CX_WINDOWS)

if(SSC_PRINT_CALLER_INFO)
        add_definitions(-DSSC_PRINT_CALLER_INFO)
endif()

set(BUILD_SHARED_LIBS OFF CACHE BOOL "build shared or static libs")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")



message( STATUS "The " ${CMAKE_GENERATOR} " was used." )

#------------------------------------------------------------------------------
# Set CustusX version
#------------------------------------------------------------------------------

cx_read_version()

set(CMAKE_BUILD_TYPE:STRING=Debug)


cx_initialize_custusx_install()
cx_catch_initialize()
cx_initialize_coverage()

include(Dart)

#------------------------------------------------------------------------------
# Includes
#------------------------------------------------------------------------------
include_directories( /usr/local/include )
include_directories( /opt/local/include )
include_directories( /opt/X11/include )

#------------------------------------------------------------------------------
# Add subdirectories
#------------------------------------------------------------------------------
#add_subdirectory( source/resource/ssc )
add_subdirectory( doc )
add_subdirectory( source )

#------------------------------------------------------------------------------
# CTest
#------------------------------------------------------------------------------
enable_testing()

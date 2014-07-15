###########################################################
##               CustusX project
###########################################################

project(CustusX)

cmake_minimum_required(VERSION 2.8.4)

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

#if(SSC_USE_GCOV)
#	add_definitions(--coverage)
#endif()

# _USE_MATH_DEFINES Must be defined before math.h is included
if (CX_WINDOWS)
    add_definitions(-D_USE_MATH_DEFINES)
endif(CX_WINDOWS)

# TODO check this and try to remove.
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

#cmake_policy(SET CMP0012 NEW)

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

include(cxCotireMacros)
CX_COTIRE_INITIALIZE()

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
# External libraries
#------------------------------------------------------------------------------

#cx_initialize_cppunit()
#cx_initialize_opencv()

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

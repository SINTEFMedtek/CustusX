###########################################################
##               CustusX project
###########################################################

project(CustusX3)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${PROJECT_SOURCE_DIR}/CMake
    ${PROJECT_SOURCE_DIR}/source/resource/ssc/CMake)

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



message( STATUS "The " ${CMAKE_GENERATOR} " was used." )

#------------------------------------------------------------------------------
# Set CustusX version
#------------------------------------------------------------------------------

#set(CX_VERSION_TYPE ALPHA) # use when releasing an alpha
#set(CX_VERSION_TYPE BETA) # use when releasing a beta
#set(CX_VERSION_TYPE RELEASE) # use when releasing a normal release
#set(CX_VERSION_TYPE ".rc1") # set release candidate during release procedure
cx_read_version()
#cx_define_version(3 5 3 CX_VERSION_TYPE)
#message(STATUS "Version: ${CustusX3_VERSION_STRING}")

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

cx_initialize_cppunit()
cx_initialize_opencv()

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

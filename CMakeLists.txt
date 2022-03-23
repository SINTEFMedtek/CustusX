###########################################################
##               CustusX project
###########################################################

project(CustusX)

#### Enable C++11
set (CMAKE_CXX_STANDARD 11)

cmake_minimum_required(VERSION 2.8.11)

# kill warning about mixing full and partial paths in libs:
# http://www.cmake.org/cmake/help/v2.8.10/cmake.html#policy%3aCMP0003
if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if (POLICY CMP0042)
	cmake_policy(SET CMP0042 NEW)
endif()

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${PROJECT_SOURCE_DIR}/CMake)

include(cxUtilities)
include(cxInstallUtilities)
include(cxInitializeLibraries)
include(cxCatchUtilities)
include(cxGetVersion)
include(cxDocumentation)

#------------------------------------------------------------------------------
# Add some compiler flags (preprocessor definitions)
#------------------------------------------------------------------------------

cx_add_defintions()
cx_set_os_identifiers()

# _USE_MATH_DEFINES Must be defined before math.h is included
if (CX_WINDOWS)
    add_definitions(-D_USE_MATH_DEFINES)
endif(CX_WINDOWS)

# required to compile some eigen code on at least Ubuntu
if(CX_LINUX)
    SET ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-depth=1024 -no-pie")
endif(CX_LINUX)

if(SSC_PRINT_CALLER_INFO)
        add_definitions(-DSSC_PRINT_CALLER_INFO)
endif()

set(BUILD_SHARED_LIBS OFF CACHE BOOL "build shared or static libs")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

# Use the old RPATH functionality that was the default in Ubuntu 16.04, instead of the new default: RUNPATH, or using LD_LIBRARY_PATH
if(CX_LINUX)
    set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--disable-new-dtags")
    set(CMAKE_EXE_LINKER_FLAGS "-Wl,--disable-new-dtags")
endif(CX_LINUX)


message( STATUS "The " ${CMAKE_GENERATOR} " was used." )

#------------------------------------------------------------------------------
# Set CustusX version
#------------------------------------------------------------------------------

set(CX_SYSTEM_BASE_NAME "CustusX" CACHE STRING "base name of the installed system, use for root folder, version, installer..")
set(CX_SYSTEM_DEFAULT_APPLICATION "CustusX" CACHE STRING "The application to set as main installed app, must match an exe TARGET")
message(STATUS "Setting CX_SYSTEM_BASE_NAME=[${CX_SYSTEM_BASE_NAME}]")
message(STATUS "Setting CX_SYSTEM_DEFAULT_APPLICATION=[${CX_SYSTEM_DEFAULT_APPLICATION}]")

cx_read_version()

set(CMAKE_BUILD_TYPE:STRING=Debug)

cx_initialize_custusx_install()
cx_initialize_macosx_bundle()
cx_clear_catch_cache()
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
add_subdirectory( source )
add_subdirectory( doc ) # after plugins - uses global properties set there


#------------------------------------------------------------------------------
# CTest
#------------------------------------------------------------------------------
enable_testing()

#------------------------------------------------------------------------------
# Add non-source files
#------------------------------------------------------------------------------
cx_add_non_source_file("README.md")
cx_add_non_source_files_to_project_file()

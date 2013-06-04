###########################################################
##               CustusX project
###########################################################

project(CustusX3)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH
        ${CMAKE_MODULE_PATH}
        ${PROJECT_SOURCE_DIR}/CMake
        ${PROJECT_SOURCE_DIR}/externals/ssc/CMake
        )
include(cxUtilities)
include(cxInstallUtilities)
include(cxInitializeLibraries)

#------------------------------------------------------------------------------
# Add some compiler flags (preprocessor definitions)
#------------------------------------------------------------------------------

if(SSC_USE_GCOV)
	add_definitions(--coverage)
endif()

cx_set_os_identifiers()

message( STATUS "The " ${CMAKE_GENERATOR} " was used." )

#------------------------------------------------------------------------------
# Set CustusX version
#------------------------------------------------------------------------------

set(CX_VERSION_TYPE ALPHA) # use when releasing an alpha
#set(CX_VERSION_TYPE BETA) # use when releasing a beta
#set(CX_VERSION_TYPE RELEASE) # use when releasing a normal release
#set(CX_VERSION_TYPE ".rc1") # set release candidate during release procedure
cx_define_version(3 5 0 CX_VERSION_TYPE)

include(cxCotireMacros)
CX_COTIRE_INITIALIZE()

set(CMAKE_BUILD_TYPE:STRING=Debug)

cx_initialize_custusx_install()

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
add_subdirectory( externals )
add_subdirectory( doc )
add_subdirectory( source )

#------------------------------------------------------------------------------
# Optional software
#------------------------------------------------------------------------------
if (LINUX)
		option(BUILD_OPEN_IGTLINK_SERVER "build open igtlink server module" ON) #default is on for linux
else (LINUX)
		option(BUILD_OPEN_IGTLINK_SERVER "build open igtlink server module" OFF) #default is off for others
endif (LINUX)

option(BUILD_GRABBER_SERVER "build grabber server" ON) #default is off
if(BUILD_GRABBER_SERVER)
		add_subdirectory( source/apps/grabberServer )
endif(BUILD_GRABBER_SERVER)

#------------------------------------------------------------------------------
# CTest
#------------------------------------------------------------------------------
enable_testing()




###########################################################
##               CustusX project
###########################################################

project(CustusX3)

cmake_minimum_required(VERSION 2.6)

#------------------------------------------------------------------------------
# Add some compiler flags (preprocessor definitions)
#------------------------------------------------------------------------------

if(SSC_USE_GCOV)
	add_definitions(--coverage)
endif()

# Use:
# CX_WINDOWS for all windows
# CX_WIN32 for 32 bit windows
# CX_WIN64 for 64 bit windows
if(WIN32)
	set(CX_WINDOWS 1) #for cmake
	add_definitions(-DCX_WINDOWS) #for compiler
	message( STATUS "This is a Windows build.")
	add_definitions(-D__WIN32__) #  -because they are needed at moc'ing time
	if (CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(CX_WIN64 1) #for cmake
		add_definitions(-DCX_WIN64) #for compiler
		message( STATUS "This is a 64 bit build.")
	else()
		set(CX_WIN32 1) #for cmake
		add_definitions(-DCX_WIN32) #for compiler
		message( STATUS "This is a 32 bit build.")
	endif()
endif()
message( STATUS "The " ${CMAKE_GENERATOR} " was used." )

if(APPLE)
	add_definitions(-D__APPLE__)
endif(APPLE)

set(CMAKE_MODULE_PATH
	${CMAKE_MODULE_PATH}
	${PROJECT_SOURCE_DIR}/CMake
	${PROJECT_SOURCE_DIR}/externals/ssc/CMake
	)

include(cxUtilities)

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

#------------------------------------------------------------------------------
# Variables
#------------------------------------------------------------------------------
set(CMAKE_BUILD_TYPE:STRING=Debug)

set(CPACK_PACKAGE_VERSION "${CustusX3_VERSION_STRING}")
set(CPACK_PACKAGE_VERSION_MAJOR "${CustusX3_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${CustusX3_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${CustusX3_VERSION_PATCH}")

set(CPACK_PACKAGE_FILE_NAME "${CustusX3_VERSION_STRING}")
set(CPACK_RESOURCE_FILE_WELCOME "${CustusX3_SOURCE_DIR}/install/Shared/install_text/install_welcome.txt")
set(CPACK_RESOURCE_FILE_LICENSE "${CustusX3_SOURCE_DIR}/install/Shared/install_text/install_license.txt")

# append build type to name if not Release:
if (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Release")
	set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_FILE_NAME}_${CMAKE_BUILD_TYPE})
endif ()

# Variables
#----------

# define LINUX convenience variable
if (UNIX AND NOT APPLE)
		set(CX_LINUX 1)
endif (UNIX AND NOT APPLE)

# Set root folder for entire installation
set(CX_INSTALL_ROOT_DIR .)

if (APPLE)
		set(CPACK_PACKAGING_INSTALL_PREFIX "/")
		set(CX_INSTALL_ROOT_DIR "Applications/CustusX")
		set(CPACK_RESOURCE_FILE_README "${CustusX3_SOURCE_DIR}/install/Apple/apple_install_readme.rtf")
		
		option (CPACK_BINARY_STGZ "Enable to build STGZ packages" OFF)
		option (CPACK_BINARY_TGZ "Enable to build TGZ packages" OFF)
		option (CPACK_SOURCE_TBZ2 "Enable to build TBZ2 source packages" OFF)
		option (CPACK_SOURCE_TGZ "Enable to build TGZ source packages" OFF)
endif(APPLE)

if (CX_LINUX)
		set(CX_INSTALL_ROOT_DIR "CustusX")
		set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_FILE_NAME}_${CMAKE_SYSTEM})
		set(CPACK_RESOURCE_FILE_README "${CustusX3_SOURCE_DIR}/install/Linux/copy/linux_install_readme.txt")
        #set(CPACK_GENERATOR "TGZ;STGZ")
		set(CPACK_GENERATOR "TGZ")
		set(CPACK_PACKAGING_INSTALL_PREFIX "/")
endif (CX_LINUX)

if (CX_WINDOWS)
		set(CPACK_NSIS_MUI_ICON "${CustusX3_SOURCE_DIR}/source/gui/icons\\\\CustusX.ico")
		set(CPACK_PACKAGE_ICON "${CustusX3_SOURCE_DIR}/source/gui/icons\\\\CustusX.png")
		set(CPACK_RESOURCE_FILE_README "${CustusX3_SOURCE_DIR}/install/Windows\\\\Windows_Install_ReadMe.rtf")
		set(CPACK_NSIS_INSTALLED_ICON_NAME "bin/CustusX.exe")
		set(OpenIGTLinkServerName OpenIGTLinkServer)
		if(CX_WIN32)
			set(OpenIGTLinkServerName UltrasonixServer)
		endif()
		set(CPACK_PACKAGE_EXECUTABLES 
				"CustusX" "CustusX"
				)
		if(CX_WIN32 AND BUILD_GRABBER_SERVER)
			set(CPACK_PACKAGE_EXECUTABLES 
				${CPACK_PACKAGE_EXECUTABLES}
				"GrabberServer" "GrabberServer"
			)
		endif()
		if(BUILD_OPEN_IGTLINK_SERVER)
			set(CPACK_PACKAGE_EXECUTABLES
				${CPACK_PACKAGE_EXECUTABLES}
				"${OpenIGTLinkServerName}" "${OpenIGTLinkServerName}"
			)
		endif()

		set(CPACK_NSIS_MENU_LINKS
				"doc/Windows_Install_ReadMe.rtf" "README"
		)
endif (CX_WINDOWS)

#------------------------------------------------------------------------------
# Dart
#------------------------------------------------------------------------------
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

# CPPUnit
#----------
find_path(CPPUNIT_INCLUDE_DIR cppunit/TestCase.h
	/usr/local/include
	/usr/include
)

if(CX_WINDOWS)
	find_library(CPPUNIT_LIBRARY cppunit
							 ${CPPUNIT_INCLUDE_DIR}/../lib
							 /usr/local/lib
							 /usr/lib)
	find_library(CPPUNIT_DEBUG_LIBRARY cppunitd
							 ${CPPUNIT_INCLUDE_DIR}/../lib
							 /usr/local/lib
							 /usr/lib)
else(CX_WINDOWS)
	find_library(CPPUNIT_LIBRARIES cppunit /opt/local/lib REQUIRED)
endif(CX_WINDOWS)

if(CPPUNIT_INCLUDE_DIR)
	if(CPPUNIT_LIBRARY)
		set(CPPUNIT_FOUND "YES")
		set(CPPUNIT_LIBRARIES ${CPPUNIT_LIBRARY} ${CMAKE_DL_LIBS})
		set(CPPUNIT_DEBUG_LIBRARIES ${CPPUNIT_DEBUG_LIBRARY} ${CMAKE_DL_LIBS})
	endif(CPPUNIT_LIBRARY)
endif(CPPUNIT_INCLUDE_DIR)


# openCV
#----------
if (CX_USE_OpenCV)
    find_package( OpenCV REQUIRED)
else ()
    find_package( OpenCV QUIET)
endif ()
IF( OpenCV_FOUND )
    option(CX_USE_OpenCV "use OpenCV" ON)
ELSE()
    option(CX_USE_OpenCV "use OpenCV" OFF)
ENDIF()

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

option(BUILD_GRABBER_SERVER "build grabber server" OFF) #default is off
if(BUILD_GRABBER_SERVER)
		add_subdirectory( source/apps/grabberServer )
endif(BUILD_GRABBER_SERVER)

#------------------------------------------------------------------------------
# CTest
#------------------------------------------------------------------------------
enable_testing()

#------------------------------------------------------------------------------
# CPack
#-----------------------------------------------------------------------------

include(CPack)



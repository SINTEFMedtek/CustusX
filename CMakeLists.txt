###########################################################
##               CustusX project
###########################################################

project(CustusX3)

cmake_minimum_required(VERSION 2.6)

#------------------------------------------------------------------------------
# Add some compiler flags (preprocessor definitions)
#------------------------------------------------------------------------------
add_definitions(-DCustusX3_VERSION_MAJOR=${CustusX3_VERSION_MAJOR}
	-DCustusX3_VERSION_MINOR=${CustusX3_VERSION_MINOR}
	-DCustusX3_VERSION_PATCH=${CustusX3_VERSION_PATCH})


if(SSC_USE_GCOV)
	add_definitions(--coverage)
endif()

# Needed for Tube-Segmentation-Framework on Linux, but won't work because ITK (and other dependencies?) don't suppport C++11 (C++0x)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

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

	# get rid of very annoying deprecated warnings in VTK 5.8. Remove when VTK if fixed...
        #SET(CMAKE_CPP_FLAGS "${CMAKE_CPP_FLAGS} -Wno-deprecated") # did not work ... must set in cmake-gui ... ??

        # support running on 10.6 Snow Leopard
        SET(CMAKE_OSX_DEPLOYMENT_TARGET "10.6" CACHE STRING "Minimum OSX deployment version")


	message(STATUS "setting flag " ${CMAKE_CPP_FLAGS})
endif(APPLE)

#------------------------------------------------------------------------------
# Set CustusX version
#------------------------------------------------------------------------------
set(CustusX3_VERSION_MAJOR  3)
set(CustusX3_VERSION_MINOR  3)
set(CustusX3_VERSION_PATCH  2)

# set alpha when not a release
set(CustusX3_VERSION_POSTFIX ".alpha")
# set beta when a quick release
#set(CustusX3_VERSION_POSTFIX ".beta")
# set nothing when a normal release
#set(CustusX3_VERSION_POSTFIX "")

set(VERSION_NUMBER "${CustusX3_VERSION_MAJOR}.${CustusX3_VERSION_MINOR}.${CustusX3_VERSION_PATCH}${CustusX3_VERSION_POSTFIX}")
set(VERSION_NUMBER_VERBOSE "${CustusX3_VERSION_MAJOR}.${CustusX3_VERSION_MINOR}.${CustusX3_VERSION_PATCH}${CustusX3_VERSION_POSTFIX}")


#-----------------------------------------------------------------------------
# Cotire (COmpile TIme REducer)
# Cross-platform cmake module for reducing compile time by generating 
# precomiled headers (pch) and single compilation unit builds
# Go to https://github.com/sakra/cotire for more information
#-----------------------------------------------------------------------------
option(COTIRE_ENABLE_PRECOMPILED_HEADERS "use cotire to autogenerate precompiled headers (pch)" OFF)
option(COTIRE_ADD_UNITY_BUILDS "use cotire to autogenerate single comilation unit builds" OFF)

if(COTIRE_ENABLE_PRECOMPILED_HEADERS OR COTIRE_ADD_UNITY_BUILDS)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/externals/ssc/CMake)
    include(cotire)
    # The properties COTIRE_ENABLE_PRECOMPILED_HEADER and COTIRE_ADD_UNITY_BUILD can also be set on targets. 
    # A target inherits the property value from its enclosing directory.
    set_directory_properties(
                PROPERTIES 
                COTIRE_ENABLE_PRECOMPILED_HEADER ${COTIRE_ENABLE_PRECOMPILED_HEADERS} 
                COTIRE_ADD_UNITY_BUILD ${COTIRE_ADD_UNITY_BUILDS}
                )
endif()

#------------------------------------------------------------------------------
# Variables
#------------------------------------------------------------------------------
set(CMAKE_BUILD_TYPE:STRING=Debug)

set(CPACK_PACKAGE_VERSION ${VERSION_NUMBER_VERBOSE})
set(CPACK_PACKAGE_VERSION_MAJOR "${CustusX3_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${CustusX3_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${CustusX3_VERSION_PATCH}")

set(CPACK_PACKAGE_FILE_NAME "CustusX_${VERSION_NUMBER_VERBOSE}")
set(CPACK_RESOURCE_FILE_WELCOME "${CustusX3_SOURCE_DIR}/install/Shared/install_text/install_welcome.txt")
set(CPACK_RESOURCE_FILE_LICENSE "${CustusX3_SOURCE_DIR}/install/Shared/install_text/install_license.txt")

# append build type to name if not Release:
if    (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Release")
		set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_FILE_NAME}_${CMAKE_BUILD_TYPE})
endif (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Release")

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
#if (USE_OpenCV)
#    ADD_DEFINITIONS(-DUSE_OpenCV)
#    message(STATUS "USE_OpenCV II: " ${USE_OpenCV})
#endif()

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
if(BUILD_OPEN_IGTLINK_SERVER)
		add_subdirectory( modules/OpenIGTLinkServer )
endif(BUILD_OPEN_IGTLINK_SERVER)

option(BUILD_GRABBER_SERVER "build grabber server" OFF) #default is off
if(BUILD_GRABBER_SERVER)
		add_subdirectory( modules/grabberServer )
endif(BUILD_GRABBER_SERVER)

#------------------------------------------------------------------------------
# CTest
#------------------------------------------------------------------------------
enable_testing()

#------------------------------------------------------------------------------
# CPack
#-----------------------------------------------------------------------------

include(CPack)
#cpack_add_component(CustusX
#                    DISPLAYNAME "${BUNDLE_NAME} ${VERSION_NUMBER_VERBOSE}"
#                    )
#cpack_add_component(GrabberServer
#                    DISPLAYNAME "GrabberServer ${VERSION_NUMBER_VERBOSE}"
#                    )



###########################################################
##               CustusX project
###########################################################

project(CustusX3)

cmake_minimum_required(VERSION 2.6)

#------------------------------------------------------------------------------
# Set CustusX version
#------------------------------------------------------------------------------
set(CustusX3_VERSION_MAJOR  3)
set(CustusX3_VERSION_MINOR  2)
set(CustusX3_VERSION_PATCH  2)
set(VERSION_NUMBER "${CustusX3_VERSION_MAJOR}.${CustusX3_VERSION_MINOR}.${CustusX3_VERSION_PATCH}")
set(VERSION_NUMBER_VERBOSE "${CustusX3_VERSION_MAJOR}.${CustusX3_VERSION_MINOR}.${CustusX3_VERSION_PATCH}")

if(APPLE)
  set(CMAKE_INSTALL_PREFIX "Applications/" ) # used during package installation. Appended to some default which seems to be / , might be able to change using DESTDIR env var.
endif(APPLE)

#------------------------------------------------------------------------------
# Set as alpha when not a release
#------------------------------------------------------------------------------
set(ALPHA ON)
  
if(ALPHA)
  set(VERSION_NUMBER "${VERSION_NUMBER} alpha")
  set(VERSION_NUMBER_VERBOSE "${VERSION_NUMBER_VERBOSE} alpha")
endif(ALPHA)

#------------------------------------------------------------------------------
# Variables
#------------------------------------------------------------------------------
set(BUNDLE_NAME CustusX)
set(CMAKE_BUILD_TYPE:STRING=Debug)

set(CPACK_PACKAGE_VERSION ${VERSION_NUMBER_VERBOSE})
set(CPACK_PACKAGE_VERSION_MAJOR "${CustusX3_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${CustusX3_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${CustusX3_VERSION_PATCH}")

set(CPACK_PACKAGE_FILE_NAME "${BUNDLE_NAME} ${VERSION_NUMBER_VERBOSE}")
set(CPACK_RESOURCE_FILE_WELCOME "${CustusX3_SOURCE_DIR}/install/doc/install_welcome.txt")
set(CPACK_RESOURCE_FILE_README "${CustusX3_SOURCE_DIR}/install/doc/install_readme.rtf")
set(CPACK_RESOURCE_FILE_LICENSE "${CustusX3_SOURCE_DIR}/install/doc/install_license.txt")

if (APPLE)
    set(CPACK_SET_DESTDIR "ON") # required when installing absolute paths
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "CustusX${CustusX3_VERSION_MAJOR}.${CustusX3_VERSION_MINOR}.${CustusX3_VERSION_PATCH}")
    set (CPACK_PACKAGING_INSTALL_PREFIX "Applications/") # guess: not used when CPACK_SET_DESTDIR is set
    
    option (CPACK_BINARY_STGZ "Enable to build STGZ packages" OFF)
    option (CPACK_BINARY_TGZ "Enable to build TGZ packages" OFF)
    option (CPACK_SOURCE_TBZ2 "Enable to build TBZ2 source packages" OFF)
    option (CPACK_SOURCE_TGZ "Enable to build TGZ source packages" OFF)
endif(APPLE)

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

if(WIN32)
  find_library(CPPUNIT_LIBRARY cppunit
               ${CPPUNIT_INCLUDE_DIR}/../lib
               /usr/local/lib
               /usr/lib)
  find_library(CPPUNIT_DEBUG_LIBRARY cppunitd
               ${CPPUNIT_INCLUDE_DIR}/../lib
               /usr/local/lib
               /usr/lib)
else(WIN32)
  find_library(CPPUNIT_LIBRARIES cppunit /opt/local/lib REQUIRED)
endif(WIN32)

if(CPPUNIT_INCLUDE_DIR)
  if(CPPUNIT_LIBRARY)
    set(CPPUNIT_FOUND "YES")
    set(CPPUNIT_LIBRARIES ${CPPUNIT_LIBRARY} ${CMAKE_DL_LIBS})
    set(CPPUNIT_DEBUG_LIBRARIES ${CPPUNIT_DEBUG_LIBRARY} ${CMAKE_DL_LIBS})
  endif(CPPUNIT_LIBRARY)
endif(CPPUNIT_INCLUDE_DIR)

#------------------------------------------------------------------------------
# Add some definitions
#  -because they are needed at moc'ing time
#------------------------------------------------------------------------------
add_definitions(-DCustusX3_VERSION_MAJOR=${CustusX3_VERSION_MAJOR}
  -DCustusX3_VERSION_MINOR=${CustusX3_VERSION_MINOR}
  -DCustusX3_VERSION_PATCH=${CustusX3_VERSION_PATCH})

if(WIN32)
  add_definitions(-D__WIN32__)
endif(WIN32)

if(APPLE)
  add_definitions(-D__APPLE__)
endif(APPLE)

#------------------------------------------------------------------------------
# Add subdirectories
#------------------------------------------------------------------------------
add_subdirectory( externals )
add_subdirectory( doc )
add_subdirectory( modules/OpenIGTLinkServer )
add_subdirectory( source )

#------------------------------------------------------------------------------
# Optional software
#------------------------------------------------------------------------------
#option(BUILD_US_ACQUISITION "build us acquisition module" OFF) #default is off
#if(BUILD_US_ACQUISITION)
#    add_subdirectory(modules/ultrasoundAcquisition)
#endif(BUILD_US_ACQUISITION)

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



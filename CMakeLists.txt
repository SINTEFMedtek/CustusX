PROJECT(CustusX3)

cmake_minimum_required(VERSION 2.6)

SET(CustusX3_VERSION_MAJOR	3)
SET(CustusX3_VERSION_MINOR	0)
SET(CustusX3_VERSION_PATCH	10)
SET(VERSION_NUMBER "${CustusX3_VERSION_MAJOR}.${CustusX3_VERSION_MINOR}.${CustusX3_VERSION_PATCH}")
SET(VERSION_NUMBER_VERBOSE "${CustusX3_VERSION_MAJOR}.${CustusX3_VERSION_MINOR}.${CustusX3_VERSION_PATCH}")

#set(CMAKE_INSTALL_RPATH .;${CMAKE_INSTALL_RPATH})
#message(STATUS "CMAKE_INSTALL_RPATH " ${CMAKE_INSTALL_RPATH})

set(CMAKE_INSTALL_PREFIX "Applications/" ) # used during package installation. Appended to some default which seems to be / , might be able to change using DESTDIR env var.
#set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/../install_test/ )

# Set ALPHA to OFF to build release
SET(ALPHA ON)

#FIND_PACKAGE(Subversion)
#IF(Subversion_FOUND)
#  Subversion_WC_INFO(${PROJECT_SOURCE_DIR} CustusX3)
#  SET(VERSION_NUMBER_VERBOSE "${VERSION_NUMBER_VERBOSE} (${CustusX3_WC_REVISION})")
#  IF(ALPHA)
#    SET(VERSION_NUMBER "${VERSION_NUMBER} (${CustusX3_WC_REVISION})")
#  ENDIF(ALPHA)
#ENDIF(Subversion_FOUND)
  
IF(ALPHA)
  SET(VERSION_NUMBER "${VERSION_NUMBER} alpha")
  SET(VERSION_NUMBER_VERBOSE "${VERSION_NUMBER_VERBOSE} alpha")
ENDIF(ALPHA)

SET(CMAKE_BUILD_TYPE:STRING=Debug)

ADD_DEFINITIONS(-DCustusX3_VERSION_MAJOR=${CustusX3_VERSION_MAJOR}
  -DCustusX3_VERSION_MINOR=${CustusX3_VERSION_MINOR}
  -DCustusX3_VERSION_PATCH=${CustusX3_VERSION_PATCH})

INCLUDE(Dart)

include_directories( /usr/local/include )
include_directories( /opt/local/include )

# cppunit -start
FIND_PATH(CPPUNIT_INCLUDE_DIR cppunit/TestCase.h
  /usr/local/include
  /usr/include
)

if (WIN32)
  FIND_LIBRARY(CPPUNIT_LIBRARY cppunit
               ${CPPUNIT_INCLUDE_DIR}/../lib
               /usr/local/lib
               /usr/lib)
  FIND_LIBRARY(CPPUNIT_DEBUG_LIBRARY cppunitd
               ${CPPUNIT_INCLUDE_DIR}/../lib
               /usr/local/lib
               /usr/lib)
ELSE (WIN32)
  find_library(CPPUNIT_LIBRARIES cppunit /opt/local/lib REQUIRED)
ENDIF (WIN32)

IF(CPPUNIT_INCLUDE_DIR)
  IF(CPPUNIT_LIBRARY)
    SET(CPPUNIT_FOUND "YES")
    SET(CPPUNIT_LIBRARIES ${CPPUNIT_LIBRARY} ${CMAKE_DL_LIBS})
    SET(CPPUNIT_DEBUG_LIBRARIES ${CPPUNIT_DEBUG_LIBRARY}
${CMAKE_DL_LIBS})
  ENDIF(CPPUNIT_LIBRARY)
ENDIF(CPPUNIT_INCLUDE_DIR)
# cppunit -end

IF (WIN32)
  ADD_DEFINITIONS(-D__WIN32__)
ENDIF (WIN32)

# name of folder where all executables, config etc are assembled
SET(INSTALL_FOLDER_NAME "CustusX")

add_subdirectory( externals )
ADD_SUBDIRECTORY ( doc )
add_subdirectory( modules/cxCommon )
ADD_SUBDIRECTORY ( source )

option(BUILD_US_ACQUISITION "build us acquisition module" OFF)
if(BUILD_US_ACQUISITION)
    add_subdirectory(modules/ultrasoundAcquisition)
endif(BUILD_US_ACQUISITION)

enable_testing()
#add_subdirectory(testing)




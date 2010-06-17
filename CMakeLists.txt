PROJECT(CustusX3)

cmake_minimum_required(VERSION 2.6)

SET(CustusX3_VERSION_MAJOR	3)
SET(CustusX3_VERSION_MINOR	0)
SET(CustusX3_VERSION_PATCH	6)
SET(VERSION_NUMBER "${CustusX3_VERSION_MAJOR}.${CustusX3_VERSION_MINOR}.${CustusX3_VERSION_PATCH}")

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/../install_test/ )

FIND_PACKAGE(Subversion)
IF(Subversion_FOUND)
  Subversion_WC_INFO(${PROJECT_SOURCE_DIR} CustusX3)
  SET(VERSION_NUMBER "${VERSION_NUMBER} (${CustusX3_WC_REVISION})")
ENDIF(Subversion_FOUND)
  
SET(VERSION_NUMBER "${VERSION_NUMBER} alpha")

SET(CMAKE_BUILD_TYPE:STRING=Debug)

ADD_DEFINITIONS(-DCustusX3_VERSION_MAJOR=${CustusX3_VERSION_MAJOR}
  -DCustusX3_VERSION_MINOR=${CustusX3_VERSION_MINOR}
  -DCustusX3_VERSION_PATCH=${CustusX3_VERSION_PATCH})

INCLUDE(Dart)

IF (WIN32)
  ADD_DEFINITIONS(-D__WIN32__)
ENDIF (WIN32)

add_subdirectory( externals )
ADD_SUBDIRECTORY ( doc )
ADD_SUBDIRECTORY ( source )

enable_testing()
add_subdirectory(testing)

#install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/config DESTINATION target_config COMPONENT config)
#install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt DESTINATION "/Library/CustusX")# COMPONENT config)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/config/ DESTINATION "./config" 
    USE_SOURCE_PERMISSIONS
    PATTERN ".svn" EXCLUDE
    #PERMISSIONS WORLD_READ WORLD_WRITE WORLD_EXECUTE
    ) # COMPONENT config)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/igstk.links DESTINATION "/Library/CustusX/" 
    USE_SOURCE_PERMISSIONS
    PATTERN ".svn" EXCLUDE
    #PERMISSIONS WORLD_READ WORLD_WRITE WORLD_EXECUTE
    ) # COMPONENT config)

#set(CPACK_SET_DESTDIR ON)
#set(CPACK__PACKAGE_NAME "CustusX")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CustusX descriptioni summary")# does not work..?
set(CPACK_PACKAGE_VENDOR "Sintef blabla")# does not work..?
set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/source/gui/icons/CustusX.png")# does not work..?
message(STATUS "licence fle: " "${CMAKE_CURRENT_SOURCE_DIR}/config/Licence.txt")
set(CPACK_RECOURCE_FILE_LICENCE "${CMAKE_CURRENT_SOURCE_DIR}/config/Licence.txt") # does not work..?
set(CPACK_PACKAGE_VERSION ${VERSION_NUMBER})
set(CPACK_PACKAGE_VERSION_MAJOR "${CustusX3_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${CustusX3_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${CustusX3_VERSION_PATCH}")

#set(CPACK_PACKAGE_EXECUTABLES "CustusX" "CustusX_navn")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "test_pkg_install")
if (APPLE)
    set (CPACK_PACKAGING_INSTALL_PREFIX "/Applications/")  
endif(APPLE)

include(CPack)

# dette krasjer installeren...
#cpack_add_component(application)
#cpack_add_component(config DISPLAY_NAME "Configuration files" DESCRIPTION "Detaljert beskrivelse")




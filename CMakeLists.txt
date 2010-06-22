PROJECT(CustusX3)

cmake_minimum_required(VERSION 2.6)

SET(CustusX3_VERSION_MAJOR	3)
SET(CustusX3_VERSION_MINOR	0)
SET(CustusX3_VERSION_PATCH	6)
SET(VERSION_NUMBER "${CustusX3_VERSION_MAJOR}.${CustusX3_VERSION_MINOR}.${CustusX3_VERSION_PATCH}")

#set(CMAKE_INSTALL_RPATH .;${CMAKE_INSTALL_RPATH})
#message(STATUS "CMAKE_INSTALL_RPATH " ${CMAKE_INSTALL_RPATH})

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

include_directories( /usr/local/include )

# cppunit
find_library(CPPUNIT_LIBRARIES cppunit /opt/local/lib) 

IF (WIN32)
  ADD_DEFINITIONS(-D__WIN32__)
ENDIF (WIN32)

add_subdirectory( externals )
ADD_SUBDIRECTORY ( doc )
ADD_SUBDIRECTORY ( source )

enable_testing()
add_subdirectory(testing)




if(nothing)
message(FATAL_ERROR failed)
# taken from http://www.itk.org/Wiki/BundleUtilitiesExample
#--------------------------------------------------------------------------------
# Now the installation stuff below
#--------------------------------------------------------------------------------
SET(BUNDLE_NAME CustusX)
SET(plugin_dest_dir bin)
SET(qtconf_dest_dir bin)
SET(CONFIG_DIR bin)
SET(APPS "\${CMAKE_INSTALL_PREFIX}/bin/${BUNDLE_NAME}")
IF(APPLE)
  SET(plugin_dest_dir ${BUNDLE_NAME}.app/Contents/MacOS)
  SET(qtconf_dest_dir ${BUNDLE_NAME}.app/Contents/Resources)
  SET(CONFIG_DIR ${BUNDLE_NAME}.app/Contents/config)
  SET(APPS "\${CMAKE_INSTALL_PREFIX}/${BUNDLE_NAME}.app")
ENDIF(APPLE)
IF(WIN32)
  SET(APPS "\${CMAKE_INSTALL_PREFIX}/bin/${BUNDLE_NAME}.exe")
ENDIF(WIN32)


#install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/config DESTINATION target_config COMPONENT config)
#install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt DESTINATION "/Library/CustusX")# COMPONENT config)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/config/ DESTINATION "${CONFIG_DIR}/config" 
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
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CustusX description summary")# does not work..?
set(CPACK_PACKAGE_VENDOR "Sintef blabla")# does not work..?
set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/source/gui/icons/CustusX.png")# does not work..?
#message(STATUS "licence fle: " "${CMAKE_CURRENT_SOURCE_DIR}/config/Licence.txt")
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

# special handling of shared libraries: must copy them into the bundle
if (BUILD_SHARED_LIBS)
    #--------------------------------------------------------------------------------
    # Use BundleUtilities to get all other dependencies for the application to work.
    # It takes a bundle or executable along with possible plugins and inspects it
    # for dependencies.  If they are not system dependencies, they are copied.
    #message(STATUS "APPS: " ${CMAKE_INSTALL_PREFIX}/${BUNDLE_NAME}.app)
    # directories to look for dependencies
    SET(BUNDLE_DIRS 
        ${SSC_DIR}/Code 
        ${LIB_DIRECTORIES}
        ${QT_LIBRARY_DIRS})
    #foreach (ITER ${LIB_DIRECTORIES})
    #    SET(BUNDLE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/${ITER} ${BUNDLE_DIRS})    
    #endforeach (ITER ${LIB_DIRECTORIES})
    
    set(BUNDLE_LIBS )
    
    # Now the work of copying dependencies into the bundle/package
    # The quotes are escaped and variables to use at install time have their $ escaped
    # An alternative is the do a configure_file() on a script and use install(SCRIPT  ...).
    # Note that the image plugins depend on QtSvg and QtXml, and it got those copied
    # over.
    install(CODE "
        include(BundleUtilities)
    #    fixup_bundle(\"${APPS}\" \"${BUNDLE_LIBS}\" \"${BUNDLE_DIRS}\")
        fixup_bundle(\"${APPS}\" \"\" \"${BUNDLE_DIRS}\")
        " COMPONENT Runtime)
    #FIXUP_BUNDLE(<app> <libs> <dirs>)
    #message(STATUS "BUNDLE_DIRS: "  ${BUNDLE_DIRS})
endif (BUILD_SHARED_LIBS)

# To Create a package, one can run "cpack -G DragNDrop CPackConfig.cmake" on Mac OS X
# where CPackConfig.cmake is created by including CPack
# And then there's ways to customize this as well
#set(CPACK_BINARY_DRAGNDROP ON)
include(CPack)

# dette krasjer installeren...
#cpack_add_component(application)
#cpack_add_component(config DISPLAY_NAME "Configuration files" DESCRIPTION "Detaljert beskrivelse")

endif(nothing)


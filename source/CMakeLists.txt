
cmake_minimum_required(VERSION 2.6)

# Boost
#----------
find_package( Boost REQUIRED )

# Qt
#----------
# find and setup Qt4 for this project
SET (QT_USE_QTXML TRUE)
find_package(Qt4 REQUIRED)
IF(QT_USE_FILE)
  INCLUDE(${QT_USE_FILE})
ELSE(QT_USE_FILE)
  SET(QT_LIBRARIES  ${QT_QT_LIBRARY})
ENDIF(QT_USE_FILE)

# VTK
#----------
FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

# ITK
#----------
FIND_PACKAGE(ITK REQUIRED)
INCLUDE(${ITK_USE_FILE})

# IGTL
#----------
FIND_PACKAGE(OpenIGTLink REQUIRED)
INCLUDE(${OpenIGTLink_USE_FILE})


# IGSTK
#----------
FIND_PACKAGE(IGSTK REQUIRED)
INCLUDE(${IGSTK_USE_FILE})

# SSC
#----------
find_package (SSC PATHS "../externals/ssc/CMake" REQUIRED)
#if (SSC_FOUND)
    include (${SSC_USE_FILE})
#	link_directories (${SSC_DIR}/Code)
#endif (SSC_FOUND)
#message(STATUS "verdi av libfolders: " ${SSC_LIBRARY_DIRS} ) 
#----------

SET(SOURCES cxMain.cpp)

set(SUB_FOLDERS
    gui
    viewManager
    repManager
    stateMachineManager
    registrationManager
    toolManager
    dataManager
    3DUSReconstruction
    UsReconstruction
    algorithms
    )
    
foreach (ITER ${SUB_FOLDERS})
    SET(LIB_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}/${ITER} ${LIB_DIRECTORIES})    
endforeach (ITER ${SUB_FOLDERS})
set (LIB_DIRECTORIES 
    ${CMAKE_CURRENT_BINARY_DIR}
    ${LIB_DIRECTORIES}
    #${SSC_DIR}/Code 
    ${CustusX3_SOURCE_DIR}/modules/cxCommon
    ${CMAKE_CURRENT_BINARY_DIR}/UsReconstruction/Thunder)

# rescope LIB_DIRECTORIES to parent scope
set(LIB_DIRECTORIES ${LIB_DIRECTORIES} PARENT_SCOPE)

INCLUDE_DIRECTORIES(
    .
    ${SUB_FOLDERS}
    /usr/local/include
    ${CustusX3_SOURCE_DIR}/modules/cxCommon
    gui/infoWidgets # hack
    gui/dataAdapters
    )
    

ADD_SUBDIRECTORY(gui)
ADD_SUBDIRECTORY(algorithms)
ADD_SUBDIRECTORY(viewManager)
ADD_SUBDIRECTORY(repManager)
ADD_SUBDIRECTORY(toolManager)
ADD_SUBDIRECTORY(stateMachineManager)
ADD_SUBDIRECTORY(dataManager)
ADD_SUBDIRECTORY(registrationManager)
ADD_SUBDIRECTORY(UsReconstruction)
ADD_SUBDIRECTORY(apps)
IF(APPLE) # doesnt compile on ca's linux box - ??
    ADD_SUBDIRECTORY(3DUSReconstruction)
ENDIF(APPLE)

#--------------------------------------------------------------------------------
# For Apple set the icns file containing icons
IF(APPLE)
  # set how it shows up in the Info.plist file
  SET(MACOSX_BUNDLE_ICON_FILE application.icns) 
  # set where in the bundle to put the icns file
  SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/gui/icons/application.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  # include the icns file in the target
  SET(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/gui/icons/application.icns)
ENDIF(APPLE)

IF(APPLE)
  SET(MACOSX_BUNDLE_INFO_STRING "CustusX")
  SET(MACOSX_BUNDLE_BUNDLE_NAME "CustusX")
  SET(MACOSX_BUNDLE_BUNDLE_VERSION "${VERSION_NUMBER_VERBOSE}")
  SET(MACOSX_BUNDLE_SHORT_VERSION_STRING "${MACOSX_BUNDLE_BUNDLE_VERSION}")
  SET(MACOSX_BUNDLE_LONG_VERSION_STRING "CustusX ${MACOSX_BUNDLE_BUNDLE_VERSION}")
  SET(MACOSX_BUNDLE_COPYRIGHT "Copyright SINTEF\nAll rights reserved")
  ADD_EXECUTABLE(CustusX MACOSX_BUNDLE ${SOURCES} )
ELSE(APPLE)
  ADD_EXECUTABLE(CustusX ${SOURCES} )
ENDIF(APPLE)

#----------
# LINK
TARGET_LINK_LIBRARIES( CustusX gui cxCommon )


#--------------------------------------------------------------------------------
# Now the installation/bundle stuff below
#--------------------------------------------------------------------------------
# See http://www.itk.org/Wiki/BundleUtilitiesExample for example on how to use bundle utilities
#
# On Apple, the bundle is at the root of the
# install tree, and on other platforms it'll go into the bin directory.

SET(BUNDLE_NAME CustusX)

install(TARGETS CustusX
    BUNDLE DESTINATION ${INSTALL_FOLDER_NAME}
    RUNTIME DESTINATION bin
    )

if(WIN32)
  include (InstallRequiredSystemLibraries)
endif(WIN32)

SET(RESOURCES_DIR bin)
SET(CONFIG_DIR bin)
SET(SYMLINK_DIR bin)

if(APPLE)
    SET(SYMLINK_DIR /Library/CustusX)
else(APPLE)
    # Applies to Linux. We should really have chosen /var/tmp/CustusX or similar here, but this causes lots of ifdefs.
    # Change for both unixlike platform at some time instead.
    SET(SYMLINK_DIR /Library/CustusX)
endif(APPLE)

SET(APPS "\${CMAKE_INSTALL_PREFIX}/${INSTALL_FOLDER_NAME}/bin/${BUNDLE_NAME}")

IF(APPLE)
  SET(RESOURCES_DIR ${INSTALL_FOLDER_NAME}/${BUNDLE_NAME}.app/Contents/Resources)
  SET(CONFIG_DIR ${INSTALL_FOLDER_NAME}/config)
  SET(APPS "\${CMAKE_INSTALL_PREFIX}/${INSTALL_FOLDER_NAME}/${BUNDLE_NAME}.app")
ENDIF(APPLE)

IF(WIN32)
  SET(APPS "\${CMAKE_INSTALL_PREFIX}/bin/CustusX.exe")
  SET(CONFIG_DIR bin/config/)
ENDIF(WIN32)

# install configuration files to config folder
install(DIRECTORY ${CustusX3_SOURCE_DIR}/config/ DESTINATION "${CONFIG_DIR}" 
    USE_SOURCE_PERMISSIONS
    PATTERN ".svn" EXCLUDE
    )

# install openCL shaders into bundle
install(FILES ${CustusX3_SOURCE_DIR}/source/UsReconstruction/Thunder/kernels.ocl 
        DESTINATION ${RESOURCES_DIR}/shaders/ 
    )

# install folder for storage of igstk<->CustusX symlinks
install(DIRECTORY ${CustusX3_SOURCE_DIR}/install/igstk.links DESTINATION ${SYMLINK_DIR} 
    FILE_PERMISSIONS
        OWNER_READ OWNER_EXECUTE OWNER_WRITE
        GROUP_READ GROUP_EXECUTE GROUP_WRITE
        WORLD_READ WORLD_EXECUTE WORLD_WRITE
    DIRECTORY_PERMISSIONS
        OWNER_READ OWNER_EXECUTE OWNER_WRITE
        GROUP_READ GROUP_EXECUTE GROUP_WRITE
        WORLD_READ WORLD_EXECUTE WORLD_WRITE
    PATTERN ".svn" EXCLUDE
    )

# Install all files in install/nextToCustusX direcly into the CustusX folder.
# Use this to add additional stuff to the installation.
if(WIN32)
  install(DIRECTORY ${CustusX3_SOURCE_DIR}/install/nextToCustusX/ 
      DESTINATION .
      USE_SOURCE_PERMISSIONS
      PATTERN ".svn" EXCLUDE
      PATTERN "readme.txt" EXCLUDE
      PATTERN "GrabberServer.app" EXCLUDE # we don't want mac applications installed on windows
      PATTERN "ultrasoundVideo.app" EXCLUDE # we don't want mac applications installed on windows
      )
else(WIN32)
  install(DIRECTORY ${CustusX3_SOURCE_DIR}/install/nextToCustusX/ 
      DESTINATION ${INSTALL_FOLDER_NAME} 
      USE_SOURCE_PERMISSIONS
      PATTERN ".svn" EXCLUDE
      PATTERN "readme.txt" EXCLUDE
      )
endif(WIN32)

set(CPACK_PACKAGE_VERSION ${VERSION_NUMBER_VERBOSE})
set(CPACK_PACKAGE_VERSION_MAJOR "${CustusX3_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${CustusX3_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${CustusX3_VERSION_PATCH}")

set(CPACK_PACKAGE_FILE_NAME "${BUNDLE_NAME} ${VERSION_NUMBER_VERBOSE}")
set(CPACK_RESOURCE_FILE_WELCOME "${CustusX3_SOURCE_DIR}/install/doc/install_welcome.txt")
set(CPACK_RESOURCE_FILE_README "${CustusX3_SOURCE_DIR}/install/doc/install_readme.rtf")
set(CPACK_RESOURCE_FILE_LICENSE "${CustusX3_SOURCE_DIR}/install/doc/install_license.txt")

if (WIN32)
else (WIN32)
    SET(CPACK_SET_DESTDIR "ON") # required when installing absolute paths
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "CustusX${CustusX3_VERSION_MAJOR}.${CustusX3_VERSION_MINOR}.${CustusX3_VERSION_PATCH}")
endif (WIN32)

if (APPLE)
    set (CPACK_PACKAGING_INSTALL_PREFIX "Applications/") # guess: not used when CPACK_SET_DESTDIR is set
    option (CPACK_BINARY_STGZ "Enable to build STGZ packages" OFF)
    option (CPACK_BINARY_TGZ "Enable to build TGZ packages" OFF)
    option (CPACK_SOURCE_TBZ2 "Enable to build TBZ2 source packages" OFF)
    option (CPACK_SOURCE_TGZ "Enable to build TGZ source packages" OFF)
    option (CPACK_SOURCE_TZ "Enable to build TZ source packages" OFF)
endif(APPLE)

if (WIN32)
  SET(LIBS ${QT_LIBRARY_DIR})
  SET(DIRS ${QT_INCLUDES} ${QT_LIBRARY_DIRS} ${INCLUDE_DIRECTORIES})

  INSTALL(CODE "
     include(BundleUtilities)
     fixup_bundle(\"${APPS}\"   \"${LIBS}\"   \"${DIRS}\") ")
endif (WIN32)

include(CPack)
    
add_subdirectory(testing)      

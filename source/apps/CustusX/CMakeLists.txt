###########################################################
##               CustusX Application
###########################################################

#------------------------------------------------------------------------------
# Include
#-----------------------------------------------------------------------------

set(SOURCES cxMain.cpp)

include_directories(
    .
    ${CustusX3_SOURCE_DIR}/source/gui
    ${CustusX3_SOURCE_DIR}/source/plugins/PluginBase
    ${CustusX3_SOURCE_DIR}/source/plugins/UsReconstruction/gui
    ${CustusX3_SOURCE_DIR}/source/plugins/Acquisition/gui
    ${CustusX3_SOURCE_DIR}/source/plugins/Calibration/gui
    ${CustusX3_SOURCE_DIR}/source/plugins/Algorithm/gui
    ${CustusX3_SOURCE_DIR}/source/plugins/Registration/gui
    )

#------------------------------------------------------------------------------
# Set variables
#-----------------------------------------------------------------------------
if(APPLE) # For Apple set the icns file containing icons
  set_source_files_properties(${CustusX3_SOURCE_DIR}/source/gui/icons/application.icns 
                              PROPERTIES MACOSX_PACKAGE_LOCATION Resources)  # set where in the bundle to put the icns file
  
  set(MACOSX_BUNDLE_ICON_FILE ${CustusX3_SOURCE_DIR}/source/gui/icons/application.icns) # set how it shows up in the Info.plist file 
  set(SOURCES ${SOURCES} ${CustusX3_SOURCE_DIR}/source/gui/icons/application.icns)  # include the icns file in the target
  set(MACOSX_BUNDLE_INFO_STRING "CustusX")
  set(MACOSX_BUNDLE_BUNDLE_NAME "CustusX")
  set(MACOSX_BUNDLE_BUNDLE_VERSION "${VERSION_NUMBER_VERBOSE}")
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${MACOSX_BUNDLE_BUNDLE_VERSION}")
  set(MACOSX_BUNDLE_LONG_VERSION_STRING "CustusX ${MACOSX_BUNDLE_BUNDLE_VERSION}")
  set(MACOSX_BUNDLE_COPYRIGHT "Copyright SINTEF\nAll rights reserved")
endif(APPLE)

#------------------------------------------------------------------------------
# Executabel
#-----------------------------------------------------------------------------
if (APPLE)
  add_executable(CustusX MACOSX_BUNDLE ${SOURCES} )
else (APPLE)
  add_executable(CustusX ${SOURCES} )
endif (APPLE)

#------------------------------------------------------------------------------
# Linking
#-----------------------------------------------------------------------------
target_link_libraries( CustusX 
    cxGui 
    cxPluginCalibrationGUI 
    cxPluginAlgorithmGUI 
    cxPluginRegistrationGUI
    cxPluginUSReconstructionGUI 
    cxPluginAcquisitionGUI
    )

#--------------------------------------------------------------------------------
# Installer
#--------------------------------------------------------------------------------
# See http://www.itk.org/Wiki/BundleUtilitiesExample for example on how to use bundle utilities
#
# On Apple, the bundle is at the root of the
# install tree, and on other platforms it'll go into the bin directory.


# Variables
#----------
set(CUSTUSX_INSTALL_FOLDER_NAME "CustusX") # name of folder where all executables, config etc are assembled
set(RESOURCES_DIR bin)
set(CONFIG_DIR bin)
set(SYMLINK_DIR bin)

if(APPLE)
    set(SYMLINK_DIR /Library/CustusX)
endif(APPLE)

# Install files
#----------
install(TARGETS CustusX
    BUNDLE DESTINATION ${CUSTUSX_INSTALL_FOLDER_NAME}
    RUNTIME DESTINATION bin
    #COMPONENT CustusX
    )

if(WIN32)
  #Installing required run time libs for Windows, msv*.dll
  set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
  include (InstallRequiredSystemLibraries)
  if(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
      install(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
              DESTINATION bin
              PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ
              GROUP_EXECUTE WORLD_READ
              #COMPONENT CustusX
    )
  endif(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
endif(WIN32)

if(APPLE)
  set(RESOURCES_DIR ${CUSTUSX_INSTALL_FOLDER_NAME}/${BUNDLE_NAME}.app/Contents/Resources)
  set(CONFIG_DIR ${CUSTUSX_INSTALL_FOLDER_NAME}/config)
endif(APPLE)

if(WIN32)
  set(APPS "\${CMAKE_INSTALL_PREFIX}/bin/CustusX.exe")
  set(CONFIG_DIR bin/config/)
endif(WIN32)

# install configuration files to config folder
install(DIRECTORY ${CustusX3_SOURCE_DIR}/config/ 
    DESTINATION ${CONFIG_DIR}
    FILE_PERMISSIONS
        OWNER_READ OWNER_EXECUTE OWNER_WRITE
        GROUP_READ GROUP_EXECUTE GROUP_WRITE
        WORLD_READ WORLD_EXECUTE WORLD_WRITE
    DIRECTORY_PERMISSIONS
        OWNER_READ OWNER_EXECUTE OWNER_WRITE
        GROUP_READ GROUP_EXECUTE GROUP_WRITE
        WORLD_READ WORLD_EXECUTE WORLD_WRITE
    #COMPONENT CustusX
    PATTERN ".svn" EXCLUDE
    )

# install openCL shaders into bundle
install(FILES ${CustusX3_SOURCE_DIR}/source/plugins/UsReconstruction/logic/Thunder/kernels.ocl 
        DESTINATION ${CONFIG_DIR}/shaders/
        #COMPONENT CustusX
        )

# install folder for storage of igstk<->CustusX symlinks
install(DIRECTORY ${CustusX3_SOURCE_DIR}/install/igstk.links 
    DESTINATION ${SYMLINK_DIR} 
    FILE_PERMISSIONS
        OWNER_READ OWNER_EXECUTE OWNER_WRITE
        GROUP_READ GROUP_EXECUTE GROUP_WRITE
        WORLD_READ WORLD_EXECUTE WORLD_WRITE
    DIRECTORY_PERMISSIONS
        OWNER_READ OWNER_EXECUTE OWNER_WRITE
        GROUP_READ GROUP_EXECUTE GROUP_WRITE
        WORLD_READ WORLD_EXECUTE WORLD_WRITE
    #COMPONENT CustusX
    PATTERN ".svn" EXCLUDE
    )

# Install all files in install/nextToCustusX direcly into the CustusX folder.
# Use this to add additional stuff to the installation.
if(WIN32)
  install(DIRECTORY ${CustusX3_SOURCE_DIR}/install/nextToCustusX/ 
      DESTINATION .
      #COMPONENT CustusX
      USE_SOURCE_PERMISSIONS
      PATTERN ".svn" EXCLUDE
      PATTERN "readme.txt" EXCLUDE
      PATTERN "GrabberServer.app" EXCLUDE # we don't want mac applications installed on windows
      PATTERN "ultrasoundVideo.app" EXCLUDE # we don't want mac applications installed on windows
      )
endif(WIN32)

if(APPLE)
  install(DIRECTORY ${CustusX3_SOURCE_DIR}/install/nextToCustusX/ 
      DESTINATION ${CUSTUSX_INSTALL_FOLDER_NAME}
      #COMPONENT CustusX
      USE_SOURCE_PERMISSIONS
      PATTERN ".svn" EXCLUDE
      PATTERN "readme.txt" EXCLUDE
      )
endif(APPLE)

#--------------------------------------------------------------------------------
# Bundle utilities
#--------------------------------------------------------------------------------
if (WIN32)
  set(LIBS ${QT_LIBRARY_DIR})
  set(DIRS ${QT_INCLUDES} ${QT_LIBRARY_DIRS} ${INCLUDE_DIRECTORIES})

  install(CODE "
     include(BundleUtilities)
     fixup_bundle(\"${APPS}\"   \"${LIBS}\"   \"${DIRS}\") "
     #COMPONENT CustusX
     )
endif (WIN32)

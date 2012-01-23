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
  set(MACOSX_BUNDLE_ICON_FILE "application.icns") # set how it shows up in the Info.plist file 
  set_source_files_properties(${CustusX3_SOURCE_DIR}/source/gui/icons/application.icns 
                              PROPERTIES MACOSX_PACKAGE_LOCATION Resources)  # set where in the bundle to put the icns file
  
  set(SOURCES ${SOURCES} ${CustusX3_SOURCE_DIR}/source/gui/icons/application.icns)  # include the icns file in the target
  set(MACOSX_BUNDLE_INFO_STRING "CustusX")
  set(MACOSX_BUNDLE_BUNDLE_NAME "CustusX")
  set(MACOSX_BUNDLE_BUNDLE_VERSION "${VERSION_NUMBER_VERBOSE}")
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${MACOSX_BUNDLE_BUNDLE_VERSION}")
  set(MACOSX_BUNDLE_LONG_VERSION_STRING "CustusX ${MACOSX_BUNDLE_BUNDLE_VERSION}")
  set(MACOSX_BUNDLE_COPYRIGHT "Copyright SINTEF\nAll rights reserved")
endif(APPLE)

#------------------------------------------------------------------------------
# Executable
#-----------------------------------------------------------------------------
if (APPLE)
  add_executable(CustusX MACOSX_BUNDLE ${SOURCES} )
else (APPLE)
  if(WIN32)
    add_executable(CustusX ${SOURCES} CustusX.rc)
  else(WIN32)
    add_executable(CustusX ${SOURCES} )
  endif(WIN32)
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
# The installer creates the following file structure:
#
# CustusX - bin
#         - config - tool
#                  - shader
#                  - settings 
#
# On Apple, the bin folder is replaced by bundles.
#

# Variables
#----------

# define LINUX convenience variable
if (UNIX AND NOT APPLE)
    set(LINUX 1)
endif (UNIX AND NOT APPLE)

# Set root folder for entire installation
set(ROOT_DIR .)
if(APPLE )
  set(ROOT_DIR "CustusX")
endif(APPLE)
if(LINUX)
  set(ROOT_DIR "CustusX")
endif(LINUX)

# Install files
#----------

# Install CustusX
install(TARGETS CustusX
    BUNDLE DESTINATION ${ROOT_DIR}
    RUNTIME DESTINATION ${ROOT_DIR}/bin
    #COMPONENT CustusX
    )

# Installing required run time libs for Windows, msv*.dll
# TODO: move into the other WIN32 conditional?
if(WIN32)
  set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
  include (InstallRequiredSystemLibraries)
  if(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
      install(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
              DESTINATION ${ROOT_DIR}/bin
              PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ
              GROUP_EXECUTE WORLD_READ
              #COMPONENT CustusX
    )
  endif(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
endif(WIN32)


# Install configuration files to config folder
install(DIRECTORY ${CustusX3_SOURCE_DIR}/config/ 
    DESTINATION ${ROOT_DIR}/config
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

# Install openCL shaders into bundle
install(FILES ${CustusX3_SOURCE_DIR}/externals/ssc/Code/Ultrasound/3DReconstruction/Thunder/kernels.ocl 
        DESTINATION ${ROOT_DIR}/config/shaders/
        )


# Install Shared files
install(DIRECTORY ${CustusX3_SOURCE_DIR}/install/Shared/doc 
  DESTINATION ${ROOT_DIR}
  #COMPONENT CustusX
  USE_SOURCE_PERMISSIONS
  )

# Install Windows-specific files
if(WIN32)
  install(FILES ${CustusX3_SOURCE_DIR}/install/Windows/win_install_readme.rtf 
      DESTINATION ${ROOT_DIR}/doc
      #COMPONENT CustusX
      )
endif(WIN32)

# Install Linux-specific files
if (LINUX)
  install(FILES 
      ${CustusX3_SOURCE_DIR}/install/Linux/copy/linux_install_readme.txt
      ${CustusX3_SOURCE_DIR}/install/Linux/copy/runCustusX.sh
      ${CustusX3_SOURCE_DIR}/install/Linux/copy/runOpenIGTLinkServer.sh 
      DESTINATION ${ROOT_DIR}
      #COMPONENT CustusX
    PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ
      )
endif (LINUX)

# Install Apple-specific files
if(APPLE)
  install(FILES ${CustusX3_SOURCE_DIR}/install/Apple/apple_install_readme.rtf 
      DESTINATION ${ROOT_DIR}/doc
      )
  install(DIRECTORY ${CustusX3_SOURCE_DIR}/install/Apple/GrapperServer.app 
      DESTINATION ${ROOT_DIR}
      #COMPONENT CustusX
      USE_SOURCE_PERMISSIONS
      )
      
    # Install folder for storage of igstk<->CustusX symlinks. 
    # The symlinks must be in an absolute location.
    # Apple only (Win uses COM-ports, Linux installs with OS)
    install(DIRECTORY ${CustusX3_SOURCE_DIR}/install/Apple/igstk.links 
        DESTINATION /Library/CustusX
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
endif(APPLE)

#--------------------------------------------------------------------------------
# Bundle utilities
#--------------------------------------------------------------------------------

# Install dependencies on Linux
if (LINUX)
  set(APPS "\${CMAKE_INSTALL_PREFIX}/${ROOT_DIR}/bin/CustusX")
  set(LIBS "")
  set(DIRS 
      ${QT_INCLUDES} 
      ${QT_LIBRARY_DIRS}
      ${SSC_INCLUDE_DIRS} 
      ${VTK_LIBRARY_DIRS}
      ${OpenCV_LIB_DIR}
      ${IGSTK_LIBRARY_DIRS}
      ${OpenIGTLink_LIBRARY_DIRS}
      ${ITK_DIR}/lib
      ${DCMTK_DIR}/lib
      ${INCLUDE_DIRECTORIES}      
      ${SSC_BINARY_DIR}/Code
      ${SSC_BINARY_DIR}/Code/Ultrasound/3DReconstruction
      ${SSC_BINARY_DIR}/DICOMLib
      ${CustusX3_BINARY_DIR}
      ${CustusX3_BINARY_DIR}/source
      ${CustusX3_BINARY_DIR}/source/service/patient
      ${CustusX3_BINARY_DIR}/source/service/state
      ${CustusX3_BINARY_DIR}/source/service/tracking
      ${CustusX3_BINARY_DIR}/source/service/video
      ${CustusX3_BINARY_DIR}/source/service/visualization
      ${CustusX3_BINARY_DIR}/source/resource/algorithms
      ${CustusX3_BINARY_DIR}/source/resource/settings
      ${CustusX3_BINARY_DIR}/source/resource/utilities
      ${CustusX3_BINARY_DIR}/source/plugins/Acquisition/logic
      ${CustusX3_BINARY_DIR}/source/plugins/Acquisition/gui
      ${CustusX3_BINARY_DIR}/source/plugins/Algorithm/logic
      ${CustusX3_BINARY_DIR}/source/plugins/Algorithm/gui
      ${CustusX3_BINARY_DIR}/source/plugins/Calibration/logic
      ${CustusX3_BINARY_DIR}/source/plugins/Calibration/gui
      ${CustusX3_BINARY_DIR}/source/plugins/Registration/logic
      ${CustusX3_BINARY_DIR}/source/plugins/Registration/gui
      ${CustusX3_BINARY_DIR}/source/plugins/UsReconstruction/logic
      ${CustusX3_BINARY_DIR}/source/plugins/UsReconstruction/gui
      ${CustusX3_BINARY_DIR}/source/logic/
      ${CustusX3_BINARY_DIR}/source/gui
      )
      
  install(CODE "
     include(BundleUtilities)
     fixup_bundle(\"${APPS}\"   \"${LIBS}\"   \"${DIRS}\") "
     #COMPONENT CustusX
     )      
endif (LINUX)

# Install dependencies on Windows
if (WIN32)
  set(APPS "\${CMAKE_INSTALL_PREFIX}/bin/CustusX.exe")
  set(LIBS ${QT_LIBRARY_DIR})
  set(DIRS ${QT_INCLUDES} ${QT_LIBRARY_DIRS} ${INCLUDE_DIRECTORIES} ${ULTERIUS_INCLUDE_DIR}/../../bin)

  install(CODE "
     include(BundleUtilities)
     fixup_bundle(\"${APPS}\"   \"${LIBS}\"   \"${DIRS}\") "
     #COMPONENT CustusX
     )
endif (WIN32)

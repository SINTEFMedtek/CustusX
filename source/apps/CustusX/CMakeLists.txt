###########################################################
##               CustusX Application
###########################################################

#------------------------------------------------------------------------------
# Variables
#-----------------------------------------------------------------------------
set(SOURCES cxMain.cpp)

#------------------------------------------------------------------------------
# Include
#-----------------------------------------------------------------------------

include_directories( ${CX_RESOURCE_INCLUDE_DIRECTORIES} )
include_directories(
    .
    ${CustusX3_SOURCE_DIR}/source/gui
    ${CustusX3_SOURCE_DIR}/source/gui/utilities
    ${CustusX3_SOURCE_DIR}/source/logic
    ${CustusX3_SOURCE_DIR}/source/modules/GUIExtender
    ${CustusX3_SOURCE_DIR}/source/modules/UsReconstruction/gui
    ${CustusX3_SOURCE_DIR}/source/modules/Acquisition/gui
    ${CustusX3_SOURCE_DIR}/source/modules/Calibration/gui
    ${CustusX3_SOURCE_DIR}/source/modules/Algorithm/gui
    ${CustusX3_SOURCE_DIR}/source/modules/Registration/gui
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
    set(MACOSX_BUNDLE_BUNDLE_VERSION "${CustusX3_VERSION_STRING}")
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
    if(CX_WIN32)
        add_executable(CustusX ${SOURCES} CustusX.rc) #32bit dunno why
    elseif(CX_WIN64)
        add_executable(CustusX WIN32 ${SOURCES} CustusX.rc) #64bit
    else() # WINDOWS or LINUX
        add_executable(CustusX ${SOURCES} )
    endif()
endif (APPLE)

#------------------------------------------------------------------------------
# Linking
#------------------------------------------------------------------------------
SET(LINK_LIBRARIES
                cxGui
                cxPluginCalibration
                cxPluginAlgorithm
                cxPluginRegistration
                cxPluginUSReconstruction
                cxPluginAcquisition
		)

target_link_libraries(CustusX ${LINK_LIBRARIES} )
CX_COTIRE_ADD_LIBRARY(CustusX "${LINK_LIBRARIES}")

# Install CustusX
install(TARGETS CustusX
	BUNDLE DESTINATION ${CX_INSTALL_ROOT_DIR}
	RUNTIME DESTINATION ${CX_INSTALL_ROOT_DIR}/bin)
# Perform full install of package, including config and docs:
include(cxInstallCustusX)

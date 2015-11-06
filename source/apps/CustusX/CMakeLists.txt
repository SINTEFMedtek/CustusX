###########################################################
##
##               CustusX Application
##                  Public version
##
###########################################################

set(SOURCES
    cxMain.cpp
)

#set(CX_COPYRIGHT_TEXT "Copyright SINTEF Medical Technology. BSD licence")

# additions to defaults from cx_initialize_macosx_bundle()
if(APPLE) # For Apple set the icns file containing icons
	set(MACOSX_BUNDLE_ICON_FILE "CustusX.icns") # set how it shows up in the Info.plist file
	set_source_files_properties(${CustusX_SOURCE_DIR}/source/gui/icons/CustusX/CustusX.icns
								PROPERTIES MACOSX_PACKAGE_LOCATION Resources)  # set where in the bundle to put the icns file
	set(SOURCES ${SOURCES} ${CustusX_SOURCE_DIR}/source/gui/icons/CustusX/CustusX.icns)  # include the icns file in the target
#	set(MACOSX_BUNDLE_INFO_STRING ${CX_SYSTEM_DEFAULT_APPLICATION})
#	set(MACOSX_BUNDLE_BUNDLE_NAME ${CX_SYSTEM_DEFAULT_APPLICATION})
#	set(MACOSX_BUNDLE_BUNDLE_VERSION "${CustusX_VERSION_STRING}")
#	set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${MACOSX_BUNDLE_BUNDLE_VERSION}")
#	set(MACOSX_BUNDLE_LONG_VERSION_STRING "${CX_SYSTEM_DEFAULT_APPLICATION} ${MACOSX_BUNDLE_BUNDLE_VERSION}")
#	set(MACOSX_BUNDLE_COPYRIGHT ${CX_COPYRIGHT_TEXT})
endif(APPLE)

#------------------------------------------------------------------------------
# Executable
#-----------------------------------------------------------------------------
if(APPLE)
    add_executable(CustusX MACOSX_BUNDLE ${SOURCES} ) #APPLE
else(APPLE)
    if(CX_WINDOWS)
        add_executable(CustusX ${SOURCES} CustusX.rc) #WINDOWS
        set_target_properties(CustusX PROPERTIES LINK_FLAGS    "/MANIFESTUAC:\"level='asInvoker' uiAccess='false'\"")
    else()
        add_executable(CustusX ${SOURCES} ) #LINUX
    endif()
endif(APPLE)

cx_set_target_high_resolution(CustusX)


#------------------------------------------------------------------------------
# Linking
#------------------------------------------------------------------------------

target_link_libraries(CustusX
    PRIVATE

	cxGui
	cxLogicManager
	cxGUIExtenderService
	cxPluginUtilities
#	cxPluginCalibration
#	cxPluginAlgorithm
#	org_custusx_usreconstruction
#	org_custusx_acquisition
    ${SSC_GCOV_LIBRARY}
)

#------------------------------------------------------------------------------
# Install CustusX
#------------------------------------------------------------------------------

install(TARGETS CustusX
    BUNDLE DESTINATION ${CX_INSTALL_ROOT_DIR}
    RUNTIME DESTINATION ${CX_INSTALL_ROOT_DIR}/bin
)

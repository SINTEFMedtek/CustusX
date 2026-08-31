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
        # CustusX.rc only references a local .ico file, but adding it as a normal
        # source of the CustusX target (CMake's native RC-language handling) makes
        # it inherit the target's entire aggregated include-directory list (every
        # linked VTK/ITK/CTK/... module), same as any .cpp file. Unlike cl.exe,
        # rc.exe has no response-file fallback for long command lines, so enough
        # dependencies (e.g. IGSTK, or downstream apps linking in more plugins)
        # can trip Windows' ~32767-char command-line limit ("CreateProcess
        # failed... command line too long"). Compile it via a plain custom
        # command instead, so it only ever sees the two paths it actually needs.
        set(CX_RC_FILE "${CMAKE_CURRENT_SOURCE_DIR}/CustusX.rc")
        set(CX_RES_FILE "${CMAKE_CURRENT_BINARY_DIR}/CustusX.res")
        add_custom_command(
            OUTPUT ${CX_RES_FILE}
            COMMAND ${CMAKE_RC_COMPILER} /fo ${CX_RES_FILE} ${CX_RC_FILE}
            DEPENDS ${CX_RC_FILE} "${CMAKE_CURRENT_SOURCE_DIR}/../../gui/icons/CustusX/CustusX.ico"
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Compiling CustusX.rc"
            VERBATIM
        )
        # Without EXTERNAL_OBJECT, the Ninja/Makefiles generators don't recognize
        # the precompiled .res as something to feed to the linker (unlike the
        # Visual Studio generator, which understands .res natively) and silently
        # drop it from the link line -- the exe still links fine, just with no
        # icon resource embedded.
        set_source_files_properties(${CX_RES_FILE} PROPERTIES EXTERNAL_OBJECT TRUE)
        add_executable(CustusX ${SOURCES} ${CX_RES_FILE}) #WINDOWS
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

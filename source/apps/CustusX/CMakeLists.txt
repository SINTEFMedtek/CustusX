###########################################################
##               CustusX Application
###########################################################

#------------------------------------------------------------------------------
# Variables
#-----------------------------------------------------------------------------
set(SOURCES
    cxMain.cpp
)

#------------------------------------------------------------------------------
# Set variables
#-----------------------------------------------------------------------------
if(APPLE) # For Apple set the icns file containing icons
    set(MACOSX_BUNDLE_ICON_FILE "application.icns") # set how it shows up in the Info.plist file
    set_source_files_properties(${CustusX_SOURCE_DIR}/source/gui/icons/application.icns
                                PROPERTIES MACOSX_PACKAGE_LOCATION Resources)  # set where in the bundle to put the icns file
    set(SOURCES ${SOURCES} ${CustusX_SOURCE_DIR}/source/gui/icons/application.icns)  # include the icns file in the target
    set(MACOSX_BUNDLE_INFO_STRING "CustusX")
    set(MACOSX_BUNDLE_BUNDLE_NAME "CustusX")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "${CustusX_VERSION_STRING}")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${MACOSX_BUNDLE_BUNDLE_VERSION}")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "CustusX ${MACOSX_BUNDLE_BUNDLE_VERSION}")
    set(MACOSX_BUNDLE_COPYRIGHT "Copyright SINTEF\nAll rights reserved")
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

#------------------------------------------------------------------------------
# Linking
#------------------------------------------------------------------------------

target_link_libraries(CustusX
    PRIVATE
    cxGui
    cxPluginCalibration
    cxPluginAlgorithm
    org_custusx_usreconstruction
    org_custusx_acquisition
    ${SSC_GCOV_LIBRARY}
)

#------------------------------------------------------------------------------
# Install CustusX
#------------------------------------------------------------------------------

#Needed to make sure all users get write permissions on the folder CustusX is installed in
#To make this tric work an empty CustusX dir is needed in source/apps/CustusX
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${CX_SYSTEM_BASE_NAME})
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${CX_SYSTEM_BASE_NAME}
    DESTINATION ${CX_INSTALL_ROOT_DIR}/..
    DIRECTORY_PERMISSIONS ${CX_FULL_PERMISSIONS}
)

install(TARGETS CustusX
    BUNDLE DESTINATION ${CX_INSTALL_ROOT_DIR}
    RUNTIME DESTINATION ${CX_INSTALL_ROOT_DIR}/bin
)

# Perform full install of package, including config and docs:
include(cxInstallCustusX)


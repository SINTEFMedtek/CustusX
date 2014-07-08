# This file is part of CustusX, an Image Guided Therapy Application.
#
# Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
#
# CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
# code and binaries can only be used by SMT and those with explicit permission
# from SMT. CustusX shall not be distributed to anyone else.
#
# CustusX is a research tool. It is NOT intended for use or certified for use
# in a normal clinical setting. SMT does not take responsibility for its use
# in any way.
#
# See CustusX_License.txt for more information.

###############################################################################
## Installation script for CustusX
#
# Include into CustusX app dir.
#
###############################################################################


include(cxInstallUtilities)

cx_install_configuration_files()
cx_install_documentation_files()

cx_install_windows_runtime_libs(${CX_INSTALL_ROOT_DIR}/bin)

# Install Linux-specific files
if (CX_LINUX)
    install(FILES
        ${CustusX3_SOURCE_DIR}/install/Linux/copy/run_v2u.sh
        ${CustusX3_SOURCE_DIR}/install/Linux/copy/v2u
        DESTINATION ${CX_INSTALL_ROOT_DIR}
        PERMISSIONS ${CX_FULL_PERMISSIONS})
endif (CX_LINUX)

# Install Apple-specific files
if(APPLE)
    # Install folder for storage of igstk<->CustusX symlinks.
    # The symlinks must be in an absolute location.
    # Apple only (Win uses COM-ports, Linux installs with OS)
    # Because CPACK_PACKAGING_INSTALL_PREFIX is "/", this will install
    # in /Library/CustusX.
    install(DIRECTORY ${CustusX3_SOURCE_DIR}/install/Apple/igstk.links
            DESTINATION Library/CustusX
            FILE_PERMISSIONS ${CX_FULL_PERMISSIONS}
            DIRECTORY_PERMISSIONS ${CX_FULL_PERMISSIONS}
            )
endif(APPLE)

#--------------------------------------------------------------------------------
# Bundle utilities
#--------------------------------------------------------------------------------

set(CUSTUSX_EXECUTABLE "${CX_INSTALL_BINARY_DIR}/${CX_BUNDLE_NAME}")
if(CX_WINDOWS)
	set( CUSTUSX_EXECUTABLE ${CUSTUSX_EXECUTABLE}".exe")
endif()

set(ALL_LIBRARY_DIRS
    ${ULTERIUS_BIN_DIR}
    ${QT_LIBRARY_DIRS}
    ${QT_BINARY_DIR}
    ${GEStreamer_LIBRARY_DIRS}
    ${OpenCV_LIB_DIR}
    ${OPENCL_LIBRARY_DIRS}
    ${IGSTK_LIBRARY_DIRS}
    ${OpenIGTLink_LIBRARY_DIRS}
    ${VTK_DIR}/lib
    ${CTK_DIR}/CTK-build/bin
    ${ITK_DIR}/lib
    ${DCMTK_DIR}/lib
    ${Tube-Segmentation-Framework_LIBRARY_DIRS}
    ${OpenCLUtilityLibrary_LIBRARY_DIRS}

	${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
	${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/plugins
	)

cx_install_all_stored_targets(${CX_INSTALL_BINARY_DIR})

cx_fixup_and_add_qtplugins_to_bundle(
	"${CUSTUSX_EXECUTABLE}"
	${CX_INSTALL_BINARY_DIR}
	"${ALL_LIBRARY_DIRS}")

cxCreateConfigurationDescription()
message(STATUS ${CX_CONFIGURATION_DESCRIPTION})

include(CPack)

  #hiding packages created by ctk: we install the plugins ourselves
  cpack_add_component(Development HIDDEN DISABLED)
  cpack_add_component(RuntimePlugins HIDDEN DISABLED)

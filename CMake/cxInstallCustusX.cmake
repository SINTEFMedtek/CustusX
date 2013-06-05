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


#--------------------------------------------------------------------------------
# Installer
#--------------------------------------------------------------------------------
# The installer creates the following file structure:
#
# CustusX - bin
#         - config - tool
#                  - shader
#                  - settings
#                  - tsf
#
# On Apple, the bin folder is replaced by a bundle.
#


#message(STATUS "CX_INSTALL_ROOT_DIR " ${CX_INSTALL_ROOT_DIR})

include(cxInstallUtilities)

cx_install_configuration_files()
cx_install_documentation_files()

cx_install_windows_runtime_libs(${CX_INSTALL_ROOT_DIR}/bin)

# Install Linux-specific files
if (CX_LINUX)
    install(FILES
        ${CustusX3_SOURCE_DIR}/install/Linux/copy/run_v2u.sh
        ${CustusX3_SOURCE_DIR}/install/Linux/copy/v2u
        ${CustusX3_SOURCE_DIR}/install/Linux/copy/runCustusX.sh
        ${CustusX3_SOURCE_DIR}/install/Linux/copy/runOpenIGTLinkServer.sh
        DESTINATION ${CX_INSTALL_ROOT_DIR}
        PERMISSIONS ${CX_FULL_PERMISSIONS})
endif (CX_LINUX)

# Install Apple-specific files
if(APPLE)
    install(FILES ${CustusX3_SOURCE_DIR}/install/Apple/SupportedGrabbers.txt
            DESTINATION ${CX_INSTALL_ROOT_DIR})

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

if(CX_LINUX)
	set( CUSTUSX_EXECUTABLE "${CX_INSTALL_ROOT_DIR}/bin/CustusX")
endif()
if(CX_WINDOWS)
	set( CUSTUSX_EXECUTABLE "bin/CustusX.exe")
endif()
if(APPLE)
	set( CUSTUSX_EXECUTABLE "${CX_INSTALL_ROOT_DIR}/CustusX.app/Contents/MacOS/CustusX")
endif()

SET(CX_INSTALL_BINARY_DIR ${CX_INSTALL_ROOT_DIR}/bin)
SET(CX_INSTALL_CONFIG_DIR ${CX_INSTALL_ROOT_DIR}/bin)
IF(APPLE)
	SET(CX_INSTALL_BINARY_DIR "${CX_INSTALL_ROOT_DIR}/CustusX.app/Contents/MacOS")
	SET(CX_INSTALL_CONFIG_DIR "${CX_INSTALL_ROOT_DIR}/CustusX.app/Contents/Resources")
ENDIF(APPLE)


set(ALL_LIBRARY_DIRS
    ${QT_INCLUDES}
    ${QT_LIBRARY_DIRS}
    ${QT_BINARY_DIR}
    ${SSC_INCLUDE_DIRS}
    ${GEStreamer_LIBRARY_DIRS}
    ${VTK_LIBRARY_DIRS}
    ${OpenCV_LIB_DIR}
    ${OPENCL_LIBRARY_DIRS}
    ${IGSTK_LIBRARY_DIRS}
    ${OpenIGTLink_LIBRARY_DIRS}
    ${ITK_DIR}/lib
    ${DCMTK_DIR}/lib
    ${INCLUDE_DIRECTORIES}
    ${Tube-Segmentation-Framework_LIBRARY_DIRS}
    ${SSC_BINARY_DIR}/Code
    ${SSC_BINARY_DIR}/DICOMLib
    ${SSC_BINARY_DIR}/Testing/cisst_code
    ${CustusX3_BINARY_DIR}
    ${CustusX3_BINARY_DIR}/source
    ${CustusX3_BINARY_DIR}/source/service/patient
    ${CustusX3_BINARY_DIR}/source/service/state
    ${CustusX3_BINARY_DIR}/source/service/tracking
    ${CustusX3_BINARY_DIR}/source/service/video
    ${CustusX3_BINARY_DIR}/source/service/visualization
    ${CustusX3_BINARY_DIR}/source/resource
    ${CustusX3_BINARY_DIR}/source/resource/algorithms
    ${CustusX3_BINARY_DIR}/source/resource/settings
    ${CustusX3_BINARY_DIR}/source/resource/utilities
    ${CustusX3_BINARY_DIR}/source/resource/OpenIGTLinkUtilities
    ${CustusX3_BINARY_DIR}/source/plugins/Acquisition
    ${CustusX3_BINARY_DIR}/source/plugins/Acquisition/logic
    ${CustusX3_BINARY_DIR}/source/plugins/Acquisition/gui
    ${CustusX3_BINARY_DIR}/source/plugins/Algorithm
    ${CustusX3_BINARY_DIR}/source/plugins/Algorithm/logic
    ${CustusX3_BINARY_DIR}/source/plugins/Algorithm/gui
    ${CustusX3_BINARY_DIR}/source/plugins/Calibration
    ${CustusX3_BINARY_DIR}/source/plugins/Calibration/logic
    ${CustusX3_BINARY_DIR}/source/plugins/Calibration/gui
    ${CustusX3_BINARY_DIR}/source/plugins/Registration
    ${CustusX3_BINARY_DIR}/source/plugins/Registration/logic
    ${CustusX3_BINARY_DIR}/source/plugins/Registration/gui
    ${CustusX3_BINARY_DIR}/source/plugins/UsReconstruction
    ${CustusX3_BINARY_DIR}/source/plugins/UsReconstruction/logic
    ${CustusX3_BINARY_DIR}/source/plugins/UsReconstruction/logic/3DReconstruction
    ${CustusX3_BINARY_DIR}/source/plugins/UsReconstruction/gui
    ${CustusX3_BINARY_DIR}/source/gui
    ${CustusX3_BINARY_DIR}/source/logic/
    ${CustusX3_BINARY_DIR}/source/apps/grabberServer
    ${CustusX3_BINARY_DIR}/source/apps/OpenIGTLinkServer
    ${CustusX3_BINARY_DIR}/source/resource/videoServer
    )

cx_install_all_stored_targets(${CX_INSTALL_BINARY_DIR})

cx_fixup_and_add_qtplugins_to_bundle(
	"${CUSTUSX_EXECUTABLE}"
	${CX_INSTALL_BINARY_DIR}
	${CX_INSTALL_CONFIG_DIR}
	"${CX_INSTALL_LIBRARIES_PATTERN}"
	"${ALL_LIBRARY_DIRS}")

cxCreateConfigurationDescription()
message(STATUS ${CX_CONFIGURATION_DESCRIPTION})

include(CPack)


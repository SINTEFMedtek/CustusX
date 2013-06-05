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

# Utitily variable for setting permissions with install(FILE) or install(DIRECTORY)
set(CX_FULL_PERMISSIONS
	OWNER_READ OWNER_EXECUTE OWNER_WRITE
	GROUP_READ GROUP_EXECUTE GROUP_WRITE
	WORLD_READ WORLD_EXECUTE WORLD_WRITE)

# Utitily variable for setting permissions with install(FILE) or install(DIRECTORY)
set(CX_READ_PERMISSIONS
	OWNER_READ GROUP_READ WORLD_READ)


###############################################################################
#
# Set variables used throughout the build.
# Call first, in root dir!
#
###############################################################################
macro(cx_initialize_custusx_install)

    set(CPACK_PACKAGE_VERSION "${CustusX3_VERSION_STRING}")
    set(CPACK_PACKAGE_VERSION_MAJOR "${CustusX3_VERSION_MAJOR}")
    set(CPACK_PACKAGE_VERSION_MINOR "${CustusX3_VERSION_MINOR}")
    set(CPACK_PACKAGE_VERSION_PATCH "${CustusX3_VERSION_PATCH}")

    set(CPACK_PACKAGE_FILE_NAME "CustusX_${CustusX3_VERSION_STRING}")
    set(CPACK_RESOURCE_FILE_WELCOME "${CustusX3_SOURCE_DIR}/install/Shared/install_text/install_welcome.txt")
    set(CPACK_RESOURCE_FILE_LICENSE "${CustusX3_SOURCE_DIR}/install/Shared/install_text/install_license.txt")

    # append build type to name if not Release:
    if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Release")
        set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_FILE_NAME}_${CMAKE_BUILD_TYPE})
    endif()

    # Set root folder for entire installation
    set(CX_INSTALL_ROOT_DIR .)

    if(APPLE)
        set(CPACK_PACKAGING_INSTALL_PREFIX "/")
        set(CX_INSTALL_ROOT_DIR "Applications/CustusX")
        set(CPACK_RESOURCE_FILE_README "${CustusX3_SOURCE_DIR}/install/Apple/apple_install_readme.rtf")

        option (CPACK_BINARY_STGZ "Enable to build STGZ packages" OFF)
        option (CPACK_BINARY_TGZ "Enable to build TGZ packages" OFF)
        option (CPACK_SOURCE_TBZ2 "Enable to build TBZ2 source packages" OFF)
        option (CPACK_SOURCE_TGZ "Enable to build TGZ source packages" OFF)
    endif(APPLE)

    if(CX_LINUX)
        set(CX_INSTALL_ROOT_DIR "CustusX")

        # Enable relative paths in Linux:
        # http://www.cmake.org/Wiki/CMake_RPATH_handling
        # http://www.cmake.org/pipermail/cmake/2008-January/019329.html
        # Mac handles this differently
        SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:\\\$ORIGIN/")

        set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_FILE_NAME}_${CMAKE_SYSTEM})
        set(CPACK_RESOURCE_FILE_README "${CustusX3_SOURCE_DIR}/install/Linux/copy/linux_install_readme.txt")
        #set(CPACK_GENERATOR "TGZ;STGZ")
        set(CPACK_GENERATOR "TGZ")
        set(CPACK_PACKAGING_INSTALL_PREFIX "/")
    endif(CX_LINUX)

    if(CX_WINDOWS)
        set(CPACK_NSIS_MUI_ICON "${CustusX3_SOURCE_DIR}/source/gui/icons\\\\CustusX.ico")
        set(CPACK_PACKAGE_ICON "${CustusX3_SOURCE_DIR}/source/gui/icons\\\\CustusX.png")
        set(CPACK_RESOURCE_FILE_README "${CustusX3_SOURCE_DIR}/install/Windows\\\\Windows_Install_ReadMe.rtf")
        set(CPACK_NSIS_INSTALLED_ICON_NAME "bin/CustusX.exe")
        set(OpenIGTLinkServerName OpenIGTLinkServer)
        if(CX_WIN32)
            set(OpenIGTLinkServerName UltrasonixServer)
        endif()
        set(CPACK_PACKAGE_EXECUTABLES
            "CustusX" "CustusX")
        if(CX_WIN32 AND BUILD_GRABBER_SERVER)
            set(CPACK_PACKAGE_EXECUTABLES
                ${CPACK_PACKAGE_EXECUTABLES}
                "GrabberServer" "GrabberServer")
        endif()
        if(BUILD_OPEN_IGTLINK_SERVER)
            set(CPACK_PACKAGE_EXECUTABLES
                ${CPACK_PACKAGE_EXECUTABLES}
                "${OpenIGTLinkServerName}" "${OpenIGTLinkServerName}")
        endif()
        set(CPACK_NSIS_MENU_LINKS "doc/Windows_Install_ReadMe.rtf" "README")
    endif (CX_WINDOWS)

endmacro()

###############################################################################
#
# Install all configuration files required by CustusX
#
###############################################################################
function(cx_install_configuration_files)
	install(DIRECTORY ${CustusX3_SOURCE_DIR}/config/
			DESTINATION ${CX_INSTALL_ROOT_DIR}/config
			FILE_PERMISSIONS ${CX_FULL_PERMISSIONS}
			DIRECTORY_PERMISSIONS ${CX_FULL_PERMISSIONS}
			PATTERN "settings/*" EXCLUDE)

	# Install openCL shaders into bundle
	install(FILES ${CustusX3_SOURCE_DIR}/source/plugins/UsReconstruction/logic/3DReconstruction/Thunder/kernels.ocl
			DESTINATION ${CX_INSTALL_ROOT_DIR}/config/shaders/)

	if(CX_USE_ISB_GE)
		install(FILES ${GEStreamer_KERNEL_PATH}/ScanConvertCL.cl
				DESTINATION ${CX_INSTALL_ROOT_DIR}/config/shaders/)
	endif()

	if(CX_USE_TSF)
		message(STATUS "KERNELS_DIR is set to " ${Tube-Segmentation-Framework_KERNELS_DIR})
		install(FILES
				${Tube-Segmentation-Framework_KERNELS_DIR}/kernels.cl
				${Tube-Segmentation-Framework_KERNELS_DIR}/kernels_no_3d_write.cl
				DESTINATION ${CX_INSTALL_ROOT_DIR}/config/tsf/)
		message(STATUS "PARAMETERS_DIR is set to " ${Tube-Segmentation-Framework_PARAMETERS_DIR})
		install(DIRECTORY ${Tube-Segmentation-Framework_PARAMETERS_DIR}
				DESTINATION ${CX_INSTALL_ROOT_DIR}/config/tsf/
				FILE_PERMISSIONS ${CX_FULL_PERMISSIONS}
				DIRECTORY_PERMISSIONS ${CX_FULL_PERMISSIONS})
	endif()
endfunction()

###############################################################################
#
# Install all documentation files for CustusX
#
###############################################################################
function(cx_install_documentation_files)
	# Install Shared files
	install(FILES
		${CustusX3_SOURCE_DIR}/install/Shared/doc/ChangeLog.rtf
		${CustusX3_SOURCE_DIR}/install/Shared/doc/CustusX_Tutorial.pdf
		${CustusX3_SOURCE_DIR}/install/Shared/doc/CustusX_Specifications.pdf
		DESTINATION ${CX_INSTALL_ROOT_DIR}/doc/
		PERMISSIONS ${CX_FULL_PERMISSIONS})

	# Install Windows-specific files
	if(CX_WINDOWS)
		install(FILES ${CustusX3_SOURCE_DIR}/install/Windows/Windows_Install_ReadMe.rtf
				DESTINATION ${CX_INSTALL_ROOT_DIR}/doc)
	endif(CX_WINDOWS)

	# Install Linux-specific files
	if (CX_LINUX)
		install(FILES
				${CustusX3_SOURCE_DIR}/install/Linux/copy/Fedora_Linux_Installation_Guide.pdf
				DESTINATION ${CX_INSTALL_ROOT_DIR}
				PERMISSIONS ${CX_FULL_PERMISSIONS})
	endif (CX_LINUX)

	# Install Apple-specific files
	if(APPLE)
		install(FILES ${CustusX3_SOURCE_DIR}/install/Apple/apple_install_readme.rtf
				DESTINATION ${CX_INSTALL_ROOT_DIR}/doc)
	endif(APPLE)
endfunction()

###############################################################################
#
# Installing required run time libs for Windows, msv*.dll
#
###############################################################################
function(cx_install_windows_runtime_libs DESTINATION_FOLDER)
	if(CX_WINDOWS)
		set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
		include (InstallRequiredSystemLibraries)
		if(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
				install(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
								DESTINATION ${DESTINATION_FOLDER}
								PERMISSIONS ${CX_FULL_PERMISSIONS})
		endif(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
	endif(CX_WINDOWS)
endfunction()


###############################################################################
# Create a text string describing essential build information
# for CustusX, such as library versions, built modules etc.
#
# The result is provided as the variable CX_CONFIGURATION_DESCRIPTION
#
###############################################################################
function(cxCreateConfigurationDescription)
	# this text can be inserted into the about box with some effort...
	set(CONFIGURATION_TEXT
"
Configuration for CustusX ${CustusX3_VERSION_STRING}

	Build Settings:
		Built on system: ${CMAKE_SYSTEM} ${CMAKE_ARCHITECTURE} ${CMAKE_SYSTEM_PROCESSOR}
		Build type: ${CMAKE_BUILD_TYPE}
		Shared Libraries: ${BUILD_SHARED_LIBRARIES}
		Coverage: ${SSC_USE_GCOV}
		Doxygen: ${BUILD_DOCUMENTATION}

	Libraries:
		Qt Version ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}
		VTK Version: ${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}.${VTK_BUILD_VERSION}
		ITK Version: ${ITK_VERSION}
		OpenCV Version: ${OpenCV_VERSION}
		CMake Version: ${CMAKE_VERSION}
		IGSTK Version: ${IGSTK_VERSION_MAJOR}.${IGSTK_VERSION_MINOR}.${IGSTK_VERSION_PATCH}
		OpenIGTLink Version: ${OpenIGTLink_VERSION_MAJOR}.${OpenIGTLink_VERSION_MINOR}.${OpenIGTLink_VERSION_PATCH}
		Boost Version: ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}

	Internal libraries:
		Tube-Segmentation-Toolkit: ${CX_USE_TSF}
		SSC Thunder OpenCL US Reconstruction: ${SSC_USE_OpenCL}
		Grabber Servers:
			Mac QuickTime Grabber Server: ${BUILD_GRABBER_SERVER}
			IGTLink Grabber Server: ${BUILD_OPEN_IGTLINK_SERVER}
				ISB GE Scanner Grabber Interface: ${CX_USE_ISB_GE}
				OpenCV Grabber Interface: ${CX_USE_OpenCV}

")
	set(CX_CONFIGURATION_DESCRIPTION ${CONFIGURATION_TEXT} PARENT_SCOPE)
endfunction()

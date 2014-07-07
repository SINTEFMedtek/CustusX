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

include( cxUtilities )

###############################################################################
#
# Define which generators to use, i.e what kind of installers to create
#
###############################################################################
macro(cx_install_set_generators)

	if(APPLE)
		# keep only packagemaker
		option (CPACK_BINARY_STGZ "Enable to build STGZ packages" OFF)
		option (CPACK_BINARY_TGZ "Enable to build TGZ packages" OFF)
		option (CPACK_SOURCE_TBZ2 "Enable to build TBZ2 source packages" OFF)
		option (CPACK_SOURCE_TGZ "Enable to build TGZ source packages" OFF)
	endif(APPLE)

	if(CX_LINUX)
		set(CPACK_GENERATOR "TGZ")
	endif(CX_LINUX)

	if(CX_WINDOWS)
		# looks like NSIS is the default
	endif (CX_WINDOWS)

endmacro()

###############################################################################
#
# Define variables used by various installer generators to customize
# the installer look&feel.
#
###############################################################################
macro(cx_install_decorate_generators)

	set(CPACK_RESOURCE_FILE_WELCOME "${PROJECT_SOURCE_DIR}/install/Shared/install_text/install_welcome.txt")
	set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/install/Shared/install_text/install_license.txt")

	if(APPLE)
		set(CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/install/Apple/apple_install_readme.rtf")
	endif(APPLE)

	if(CX_LINUX)
		set(CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/install/Linux/copy/linux_install_readme.txt")
	endif(CX_LINUX)

	if(CX_WINDOWS)
		set(CPACK_NSIS_MUI_ICON "${PROJECT_SOURCE_DIR}/source/gui/icons\\\\CustusX.ico")
		set(CPACK_PACKAGE_ICON "${PROJECT_SOURCE_DIR}/source/gui/icons\\\\CustusX.png")
		set(CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/install/Windows\\\\Windows_Install_ReadMe.rtf")
		set(CPACK_NSIS_INSTALLED_ICON_NAME "bin/CustusX.exe")
		set(CPACK_NSIS_MENU_LINKS "doc/Windows_Install_ReadMe.rtf" "README")

		set(OpenIGTLinkServerName OpenIGTLinkServer)
		if(CX_WIN32)
			set(OpenIGTLinkServerName UltrasonixServer)
		endif()
		set(CPACK_PACKAGE_EXECUTABLES
			"CustusX" "CustusX")
		set(CPACK_PACKAGE_EXECUTABLES
			${CPACK_PACKAGE_EXECUTABLES}
			"${OpenIGTLinkServerName}" "${OpenIGTLinkServerName}")

	endif (CX_WINDOWS)
endmacro()

###############################################################################
#
# Make sure that binaries can use paths relative to itself when looking up
# libraries. This enables packaging of libs and exes without depending on
# system locations.
#
###############################################################################
macro(cx_install_set_relative_path)
	if(CX_LINUX)
		# Enable relative paths in Linux:
		# http://www.cmake.org/Wiki/CMake_RPATH_handling
		# http://www.cmake.org/pipermail/cmake/2008-January/019329.html
		# Mac handles this differently
		SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:\\\$ORIGIN/")
	endif(CX_LINUX)
endmacro()


###############################################################################
#
# Define variable CPACK_PACKAGE_FILE_NAME which sets the name of the installer file.
#
###############################################################################
macro(cx_install_set_generator_filename)
	set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}_${${PROJECT_NAME}_VERSION_STRING}")
	# append build type to name if not Release:
	if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Release")
		set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_FILE_NAME}_${CMAKE_BUILD_TYPE})
	endif()
	# append system info if linux:
	if(CX_LINUX)
		set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_FILE_NAME}_${CMAKE_SYSTEM})
	endif(CX_LINUX)
endmacro()

###############################################################################
#
# Set version info in CPACK
#
###############################################################################
macro(cx_install_set_version_info)
	set(CPACK_PACKAGE_VERSION ${${PROJECT_NAME}_VERSION_STRING})
	set(CPACK_PACKAGE_VERSION_MAJOR "${${PROJECT_NAME}_VERSION_MAJOR}")
	set(CPACK_PACKAGE_VERSION_MINOR "${${PROJECT_NAME}_VERSION_MINOR}")
	set(CPACK_PACKAGE_VERSION_PATCH "${${PROJECT_NAME}_VERSION_PATCH}")
endmacro()

###############################################################################
#
# Setup a folder structure for the installed system.
#
# Structure on linux
#
#   bundle/bin/bundle
#              binary*
#              lib*.so
#              qt.conf
#              liborg_plugin*.so
#   bundle/doc/something.txt
#   bundle/config/...
#
# Structure on mac
#
#   bundle/bundle.app/Contents/MacOS/bundle
#                                    binary*
#                                    lib*.so
#                                    liborg_plugin*.so
#   bundle/bundle.app/Contents/Resources/qt.conf
#   bundle/doc/something.txt
#   bundle/config/...
#
#
#  Creates the following variables:
#    CX_BUNDLE_NAME          Name used to identify bundle.
#    CX_INSTALL_ROOT_DIR     Base folder for installation, relative to CMAKE_INSTALL_PREFIX
#                            All files are installed into this location.
#    CX_INSTALL_BINARY_DIR   Location of binaries
#    CX_INSTALL_CONFIG_DIR   Location of config stuff... (qt.conf fex)
#    CX_INSTALL_PLUGIN_DIR   Location of runtime plugins
#
###############################################################################
macro(cx_install_set_folder_structure)

	set(CX_BUNDLE_NAME "CustusX")

	set(CX_INSTALL_ROOT_DIR .)
	if(APPLE)
		set(CPACK_PACKAGING_INSTALL_PREFIX "/")
		set(CX_INSTALL_ROOT_DIR "Applications/${CX_BUNDLE_NAME}")
	endif(APPLE)
	if(CX_LINUX)
		set(CPACK_PACKAGING_INSTALL_PREFIX "/")
		set(CX_INSTALL_ROOT_DIR ${CX_BUNDLE_NAME})
	endif(CX_LINUX)

	set(CX_INSTALL_BINARY_DIR ${CX_INSTALL_ROOT_DIR}/bin)
	if(APPLE)
		set(CX_INSTALL_BINARY_DIR "${CX_INSTALL_ROOT_DIR}/${CX_BUNDLE_NAME}.app/Contents/MacOS")
	endif(APPLE)

#	set(CX_INSTALL_PLUGIN_DIR "${CX_INSTALL_BINARY_DIR}/plugins") - did not work: get into trouble with relative paths and fixup_bundle
	set(CX_INSTALL_PLUGIN_DIR "${CX_INSTALL_BINARY_DIR}")

endmacro()



###############################################################################
#
# Set variables used throughout the build.
# Call first, in root dir!
#
###############################################################################
macro(cx_initialize_custusx_install)
	set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME "Core")

	cx_install_set_version_info()

	cx_install_set_relative_path()

	cx_install_set_generators()
	cx_install_decorate_generators()
	cx_install_set_generator_filename()

	cx_install_set_folder_structure()

	# used as a global variable: clear at start of run
	unset(CX_APPLE_TARGETS_TO_COPY CACHE)

endmacro()

###############################################################################
#
# Install one target to the default location.
#
# If the platform is APPLE, the target is stored in a variable for later
# copying into the bundle. This copy should be done by the bundle code.
# Why: fixup_bundle doesnt work properly on CX_APPLE with the normal install(TARGET...),
# while the opposite is true on CX_LINUX.
#
###############################################################################
function(cx_install_target TARGET_ID)
	cx_assert_variable_exists(${CX_INSTALL_ROOT_DIR})

	if(CX_APPLE)
		set( NEW_ENTRY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_ID}" )
		#set( NEW_ENTRY "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_ID}" )
		list(FIND CX_APPLE_TARGETS_TO_COPY ${NEW_ENTRY} PREEXISTING)
		if( ${PREEXISTING} EQUAL -1 )
			set(CX_APPLE_TARGETS_TO_COPY
				${NEW_ENTRY} ${CX_APPLE_TARGETS_TO_COPY}
				CACHE INTERNAL
				"List of all project targets that should be copied into the bundle (for use with fixup_bundle)")
			message(STATUS "Storing target for later file copy: " ${NEW_ENTRY})
		else()
			#message(STATUS "########### Already present: " ${PREEXISTING})
		endif()
		#message(STATUS "########### List content: " ${CX_APPLE_TARGETS_TO_COPY})
	else()
		install(TARGETS ${TARGET_ID}
			DESTINATION ${CX_INSTALL_ROOT_DIR}/bin)
	endif()

endfunction()

###############################################################################
#
# Install all the targets in CX_APPLE_TARGETS_TO_COPY to the BUNDLE_BINARY_DIR.
# See cx_install_target() for more.
#
###############################################################################
function(cx_install_all_stored_targets BUNDLE_BINARY_DIR)
	if(CX_APPLE)
		#message(STATUS "############################## All exes: " ${CX_APPLE_TARGETS_TO_COPY})
		foreach(EXE ${CX_APPLE_TARGETS_TO_COPY} )
			message(STATUS "Install target by file copy: " ${EXE})
			install(FILES
				${EXE}
				DESTINATION ${BUNDLE_BINARY_DIR}
				PERMISSIONS ${CX_FULL_PERMISSIONS})
		endforeach()
	else()
		# NA: cx_install_target() does all the work.
	endif()
endfunction()


###############################################################################
#
# Install all configuration files required by CustusX
#
###############################################################################
function(cx_install_configuration_files)

set(CONFIG_EXCLUDE_PATTERN "")
if (NOT CX_LINUX)
    set(CONFIG_EXCLUDE_PATTERN "elastix_linux64_v4.6/*")
endif ()
if (NOT CX_APPLE)
    set(CONFIG_EXCLUDE_PATTERN "elastix_macosx64_v4.6/*")
endif ()

	install(DIRECTORY ${CustusX3_SOURCE_DIR}/config/
			DESTINATION ${CX_INSTALL_ROOT_DIR}/config
			FILE_PERMISSIONS ${CX_FULL_PERMISSIONS}
			DIRECTORY_PERMISSIONS ${CX_FULL_PERMISSIONS}
			PATTERN "settings/*" EXCLUDE
			PATTERN ${CONFIG_EXCLUDE_PATTERN} EXCLUDE)

	# Install OpenCL kernels into bundle
	install(FILES ${CustusX3_SOURCE_DIR}/source/plugins/org.custusx.vnnclreconstruction/kernels.cl
			DESTINATION ${CX_INSTALL_ROOT_DIR}/config/shaders/)

	if(CX_USE_ISB_GE)
		install(FILES ${GEStreamer_KERNEL_PATH}/ScanConvertCL.cl
				DESTINATION ${CX_INSTALL_ROOT_DIR}/config/shaders/)
	endif()

	if(CX_USE_TSF)
		install(FILES
				${Tube-Segmentation-Framework_KERNELS_DIR}/kernels.cl
				${Tube-Segmentation-Framework_KERNELS_DIR}/kernels_no_3d_write.cl
				DESTINATION ${CX_INSTALL_ROOT_DIR}/config/tsf/)
		install(DIRECTORY ${Tube-Segmentation-Framework_PARAMETERS_DIR}
				DESTINATION ${CX_INSTALL_ROOT_DIR}/config/tsf/
				FILE_PERMISSIONS ${CX_FULL_PERMISSIONS}
				DIRECTORY_PERMISSIONS ${CX_FULL_PERMISSIONS})
	endif()
	
	if(CX_USE_OPENCL_UTILITY)
		install(FILES
				${OpenCLUtilityLibrary_KERNELS_DIR}/HistogramPyramids.cl
				${OpenCLUtilityLibrary_KERNELS_DIR}/HistogramPyramids.clh
				DESTINATION ${CX_INSTALL_ROOT_DIR}/config/tsf/)
	endif()

	if(CX_BUILD_US_SIMULATOR)
		install(FILES
				${CustusX3_SOURCE_DIR}/../../UltrasoundSimulation/UltrasoundSimulation/speckle.mhd
				${CustusX3_SOURCE_DIR}/../../UltrasoundSimulation/UltrasoundSimulation/speckle.raw
				DESTINATION ${CX_INSTALL_ROOT_DIR}/config/simulator)
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
# wrapper around the fixup_bundle function.
#   - will add qtplugins to bundle automatically.
#
#
# Input Variables:
#    APPS_LOCAL                      : Name of executable to base bundle on, with full path
#                                      relative to CMAKE_INSTALL_PREFIX
#    INSTALL_BINARY_DIR              : where to put binaries
#    INSTALL_CONFIG_DIR              : where to put configuration
#    INSTALL_LIBRARIES_PATTERN_LOCAL : List of names of additional libs/plugins to add,
#                                      path relative to CMAKE_INSTALL_PREFIX
#    DIRS_LOCAL                      : List of all paths where libraries can be found.
#                                      This is pretty much the entire application source.
#
# See
#     http://www.itk.org/Wiki/BundleUtilitiesExample
# for example on how to use bundle utilities.
#
# More info on Mac+Qt, plugins, and pitfalls related to this:
#     http://lists.trolltech.com/qt-interest/2008-09/thread00258-0.html
#     http://doc.qt.nokia.com/4.7-snapshot/qt-conf.html
#
# How to debug install: cpack --debug --verbose
#  ( from http://www.cmake.org/Wiki/CMake:Packaging_With_CPack)
#
###############################################################################
function(cx_fixup_and_add_qtplugins_to_bundle APPS_LOCAL INSTALL_BINARY_DIR DIRS_LOCAL)
	cx_assert_variable_exists(${QT_PLUGINS_DIR})
	cx_assert_variable_exists(${CX_INSTALL_ROOT_DIR})
	cx_assert_variable_exists(${INSTALL_BINARY_DIR})
	cx_assert_variable_exists(${CX_INSTALL_PLUGIN_DIR})


	# Install plugins in the default location as given by http://qt-project.org/doc/qt-4.8/qt-conf.html
	if(CX_LINUX)
		SET(INSTALL_QTPLUGIN_DIR "${INSTALL_BINARY_DIR}/plugins")
		SET(INSTALL_QTCONF_DIR "${INSTALL_BINARY_DIR}")
	endif()
	if(CX_WINDOWS)
		SET(INSTALL_QTPLUGIN_DIR "${INSTALL_BINARY_DIR}/plugins")
		SET(INSTALL_QTCONF_DIR "${INSTALL_BINARY_DIR}")
	endif()
	IF(APPLE)
		SET(INSTALL_QTPLUGIN_DIR "${CX_INSTALL_ROOT_DIR}/${CX_BUNDLE_NAME}.app/Contents/plugins")
		SET(INSTALL_QTCONF_DIR   "${CX_INSTALL_ROOT_DIR}/${CX_BUNDLE_NAME}.app/Contents/Resources")
	ENDIF(APPLE)

# Install plugins in the default location as given by http://qt-project.org/doc/qt-4.8/qt-conf.html
#if(CX_LINUX)
#	SET(INSTALL_PLUGIN_DIR "${INSTALL_BINARY_DIR}")
#endif()
#if(CX_WINDOWS)#
#	SET(INSTALL_PLUGIN_DIR "${INSTALL_BINARY_DIR}")
#endif()
#IF(APPLE)
#	SET(INSTALL_PLUGIN_DIR "${INSTALL_BINARY_DIR}/../plugins")
#ENDIF(APPLE)

	# Install needed Qt plugins by copying directories from the qt installation
	# One can cull what gets copied by using 'REGEX "..." EXCLUDE'
	install(DIRECTORY "${QT_PLUGINS_DIR}/imageformats" "${QT_PLUGINS_DIR}/sqldrivers"
		DESTINATION ${INSTALL_QTPLUGIN_DIR}
		DIRECTORY_PERMISSIONS ${CX_FULL_PERMISSIONS})

	# install runtime plugins
	install(DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/plugins/" # trailing slash copies contents, not plugin folder
		DESTINATION ${CX_INSTALL_PLUGIN_DIR}
		DIRECTORY_PERMISSIONS ${CX_FULL_PERMISSIONS})

	# collect all installations here. They will be used by fixup_bundle to collect dependencies.
	# this is a sum of the input pattern (if any) and the qtplugins
	set(INSTALL_LIBRARIES_PATTERN_LOCAL
		${CX_INSTALL_PLUGIN_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}
		${INSTALL_QTPLUGIN_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}
		)

	# Install needed Qt plugins by copying directories from the qt installation
	# One can cull what gets copied by using 'REGEX "..." EXCLUDE'
#	install(DIRECTORY "${QT_PLUGINS_DIR}/imageformats" "${QT_PLUGINS_DIR}/sqldrivers"
#		DESTINATION ${INSTALL_PLUGIN_DIR}
#		DIRECTORY_PERMISSIONS ${CX_FULL_PERMISSIONS})

	# collect all installations here. They will be used by fixup_bundle to collect dependencies.
	# this is a sum of the input pattern (if any) and the qtplugins
#	set(INSTALL_LIBRARIES_PATTERN_LOCAL
#		${INSTALL_BINARY_DIR}/../plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}
#                ${INSTALL_BINARY_DIR}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}
#				${INSTALL_PLUGIN_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}
#				${INSTALL_LIBRARIES_PATTERN_LOCAL}
#		)

	# install a qt.conf file
	# this inserts some cmake code into the install script to write the file
	# This is a requirement to get qt plugins in a relative path working!
	install(CODE "
		file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${INSTALL_QTCONF_DIR}/qt.conf\" \"\")
		")

	# create a code snippet : Search for the files in pattern,
	#                         add files to PLUGINS
	foreach(PATTERN ${INSTALL_LIBRARIES_PATTERN_LOCAL} )
		set(LIB_PATTERN_CODE
			"${LIB_PATTERN_CODE}
			set(TEMP)
			file\(GLOB_RECURSE TEMP \"\${CMAKE_INSTALL_PREFIX}/${PATTERN}\"\)
			set(PLUGINS \${PLUGINS} \${TEMP})"
			)
	endforeach()

	# fixup_bundle resets link paths for all targets within the bundle.
	# this code appears in cmake_install.cmake in the CURRENT_BINARY_DIR. Check there when changing.
	install(CODE "
		# Begin inserted fixup_bundle snippet
		${LIB_PATTERN_CODE}
		include(BundleUtilities)
		fixup_bundle(\"\${CMAKE_INSTALL_PREFIX}/${APPS_LOCAL}\"   \"\${PLUGINS}\"   \"${DIRS_LOCAL}\") "
		# End inserted fixup_bundle snippet
		)
endfunction()

function (getListOfVarsStartingWith _prefix _varResult)
	get_cmake_property(_vars VARIABLES)
	string (REGEX MATCHALL "(^|;)${_prefix}[A-Za-z0-9_\\.]*" _matchedVars "${_vars}")
#	string (REGEX MATCHALL "(^|;)${_prefix}.*" _matchedVars "${_vars}")
	set (${_varResult} ${_matchedVars} PARENT_SCOPE)
endfunction()

###############################################################################
# Create a text string describing essential build information
# for CustusX, such as library versions, built modules etc.
#
# The result is provided as the variable CX_CONFIGURATION_DESCRIPTION
#
###############################################################################
function(cxCreateConfigurationDescription)

cx_initialize_cppunit()
cx_initialize_opencv()
cx_initialize_OpenIGTLink()
cx_initialize_IGSTK()

set(PLUGINS_DESCRIPTION
"	Plugins:
")
getListOfVarsStartingWith("plugins/" matchedVars)
foreach (_var IN LISTS matchedVars)
	string(REPLACE "BUILD_plugins/" "" PLUGIN_NAME ${${_var}})
	#	message("${_var}=${${_var}} :: ${${${_var}}} :: ${PLUGIN_NAME}")
	set(PLUGINS_DESCRIPTION ${PLUGINS_DESCRIPTION}
"		${PLUGIN_NAME}: ${${${_var}}}
")
endforeach()

	cx_assert_variable_exists(${CustusX3_VERSION_STRING})
	cx_assert_variable_exists(${SSC_USE_GCOV})
	cx_assert_variable_exists(${CX_USE_TSF})
	cx_assert_variable_exists(${CX_USE_LEVEL_SET})
	cx_assert_variable_exists(${CX_USE_OPENCL_UTILITY})
	cx_assert_variable_exists(${CX_USE_ISB_GE})
	cx_assert_variable_exists(${CX_USE_OpenCV})
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
		CTK Version: ${CTK_VERSION}
		OpenCV Version: ${OpenCV_VERSION}
		CMake Version: ${CMAKE_VERSION}
		IGSTK Version: ${IGSTK_VERSION_MAJOR}.${IGSTK_VERSION_MINOR}.${IGSTK_VERSION_PATCH}
		OpenIGTLink Version: ${OpenIGTLink_VERSION_MAJOR}.${OpenIGTLink_VERSION_MINOR}.${OpenIGTLink_VERSION_PATCH}
		Boost Version: ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}
		OpenCL Version: ${OPENCL_VERSION}
		OpenGL Version: ${OPENGL_VERSION}

	Internal libraries:
		Tube-Segmentation-Framework: ${CX_USE_TSF}
		Level Set Segmentation: ${CX_USE_LEVEL_SET}
		OpenCL Utility Library: ${CX_USE_OPENCL_UTILITY}
	
	Grabber Servers:
		ISB GE Scanner Grabber Interface: ${CX_USE_ISB_GE}
		OpenCV Grabber Interface: ${CX_USE_OpenCV}

${PLUGINS_DESCRIPTION}
")
	set(CX_CONFIGURATION_DESCRIPTION ${CONFIGURATION_TEXT} PARENT_SCOPE)
endfunction()

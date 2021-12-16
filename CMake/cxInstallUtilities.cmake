# =========================================================================
# This file is part of CustusX, an Image Guided Therapy Application.
#
# Copyright (c) SINTEF Department of Medical Technology.
# All rights reserved.
#
# CustusX is released under a BSD 3-Clause license.
#
# See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
# =========================================================================

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
#		option (CPACK_BINARY_STGZ "Enable to build STGZ packages" OFF)
#		option (CPACK_BINARY_TGZ "Enable to build TGZ packages" OFF)
#		option (CPACK_SOURCE_TBZ2 "Enable to build TBZ2 source packages" OFF)
#		option (CPACK_SOURCE_TGZ "Enable to build TGZ source packages" OFF)
#		set(CPACK_GENERATOR "PackageMaker")
#                set(CPACK_GENERATOR "Bundle")
                set(CPACK_GENERATOR "DragNDrop")
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
# Initialize a few properties to be used during package generation.
#
# These properties can be overriden anywhere in the system, and will
# be converted to normal variable prior to package generation.
#
# Matched by cx_install_apply_customizable_properties()
#
###############################################################################
macro(cx_install_initialize_customizable_properties)

	cx_install_set_generator_filename()

	set(CX_LICENSE_FILE "${CustusX_SOURCE_DIR}/License.txt")
	set_property(GLOBAL PROPERTY CX_LICENSE_FILE "${CX_LICENSE_FILE}")

    set_property(GLOBAL PROPERTY CPACK_PACKAGE_NAME "${CX_SYSTEM_BASE_NAME}")
	set_property(GLOBAL PROPERTY CPACK_PACKAGE_ICON "${PROJECT_SOURCE_DIR}/source/gui/icons/CustusX/CustusX.png")
	set_property(GLOBAL PROPERTY CPACK_PACKAGE_VENDOR "SINTEF Medical Technology")
	set_property(GLOBAL PROPERTY CPACK_RESOURCE_FILE_WELCOME "${PROJECT_SOURCE_DIR}/install/install_text/install_welcome.txt")
	set_property(GLOBAL PROPERTY CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/install/install_text/install_readme.rtf")
	set_property(GLOBAL PROPERTY CPACK_RESOURCE_FILE_LICENSE "${CX_LICENSE_FILE}")

	if(CX_WINDOWS)
		set_property(GLOBAL PROPERTY CPACK_NSIS_MUI_ICON "${PROJECT_SOURCE_DIR}/source/gui/icons/CustusX\\\\CustusX.ico")
		set_property(GLOBAL PROPERTY CPACK_PACKAGE_ICON "${PROJECT_SOURCE_DIR}/source/gui/icons/CustusX\\\\CustusX.png")
	endif (CX_WINDOWS)
endmacro()

###############################################################################
#
# Initialize a few properties to be used during package generation.
#
# These properties can be overriden anywhere in the system, and will
# be converted to normal variable prior to package generation.
#
###############################################################################
macro(cx_install_apply_customizable_properties)

    get_property(CPACK_PACKAGE_NAME          GLOBAL PROPERTY CPACK_PACKAGE_NAME)
	get_property(CPACK_PACKAGE_ICON          GLOBAL PROPERTY CPACK_PACKAGE_ICON)
	get_property(CPACK_PACKAGE_VENDOR        GLOBAL PROPERTY CPACK_PACKAGE_VENDOR)
	get_property(CPACK_RESOURCE_FILE_WELCOME GLOBAL PROPERTY CPACK_RESOURCE_FILE_WELCOME)
	get_property(CPACK_RESOURCE_FILE_README  GLOBAL PROPERTY CPACK_RESOURCE_FILE_README)
	get_property(CPACK_RESOURCE_FILE_LICENSE GLOBAL PROPERTY CPACK_RESOURCE_FILE_LICENSE)

	if(CX_WINDOWS)
		get_property(CPACK_NSIS_MUI_ICON     GLOBAL PROPERTY CPACK_NSIS_MUI_ICON)
		get_property(CPACK_PACKAGE_ICON      GLOBAL PROPERTY CPACK_PACKAGE_ICON)

		set(CPACK_NSIS_INSTALLED_ICON_NAME "bin/${CX_SYSTEM_DEFAULT_APPLICATION}.exe")
		set(CPACK_NSIS_MENU_LINKS "doc/Windows_Install_ReadMe.rtf" "README")

		set(CPACK_PACKAGE_EXECUTABLES
			"${CX_SYSTEM_DEFAULT_APPLICATION}" "${CX_SYSTEM_DEFAULT_APPLICATION}"
			"OpenIGTLinkServer" "OpenIGTLinkServer"
			)

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
        if(CX_APPLE)
                # Add support for Frameworks installed into the bundle:
                # http://cmake.org/Wiki/CMake_RPATH_handling
                SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:@executable_path/../Frameworks/")
        endif(CX_APPLE)

endmacro()


###############################################################################
#
# Define variable CPACK_PACKAGE_FILE_NAME which sets the name of the installer file.
#
###############################################################################
macro(cx_install_set_generator_filename)
	set(CPACK_PACKAGE_FILE_NAME "${CX_SYSTEM_BASE_NAME}_${${PROJECT_NAME}_VERSION_STRING}")
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
# Structure on linux:
#
#   bundle/bin/bundle
#              binary*
#              lib*.so
#              qt.conf
#              liborg_plugin*.so
#   bundle/doc/something.txt
#   bundle/config/...
#
# Structure on mac:
#
#   bundle/bundle.app/Contents/MacOS/bundle
#                                    binary*
#                                    lib*.so
#                                    liborg_plugin*.so
#   bundle/bundle.app/Contents/Resources/qt.conf
#   bundle/bundle.app/doc/something.txt
#   bundle/bundle.app/config/...
#
# Structure on windows:
#
#  bundle/bin/*.exe
#             *.dll
#             plugins/
#  bundle/config/...
#  bundle/doc/...
#  bundle/documentation/...
#  bundle/include/...
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

        cx_assert_variable_exists(CX_SYSTEM_DEFAULT_APPLICATION)
        cx_assert_variable_exists(CX_SYSTEM_BASE_NAME)

	set(CX_BUNDLE_NAME ${CX_SYSTEM_DEFAULT_APPLICATION})

        set(CX_INSTALL_ROOT_DIR ".")
        set(CPACK_PACKAGING_INSTALL_PREFIX "/")
        if(APPLE)

#                set(CPACK_BUNDLE_NAME ${CX_BUNDLE_NAME})
#                set(CPACK_BUNDLE_PLIST "Info.plist")
#                set(CPACK_BUNDLE_ICON "${PROJECT_SOURCE_DIR}/source/gui/icons/CustusX/CustusX.ico")
#                set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY TRUE)

#                set(CPACK_PACKAGING_INSTALL_PREFIX "/")
                #set(CX_INSTALL_ROOT_DIR "Applications/${CX_SYSTEM_BASE_NAME}") - NA when creating dragndrop
	endif(APPLE)
	if(CX_LINUX)
#                set(CPACK_PACKAGING_INSTALL_PREFIX "/")
		set(CX_INSTALL_ROOT_DIR ${CX_SYSTEM_BASE_NAME})
	endif(CX_LINUX)
        if(CX_WINDOWS)
#            set(CPACK_PACKAGING_INSTALL_PREFIX "/")
#            set(CX_INSTALL_ROOT_DIR ".")
        endif(CX_WINDOWS)


        set(CX_INSTALL_BINARY_DIR "${CX_INSTALL_ROOT_DIR}/bin")
        set(CX_INSTALL_CONFIG_DIR "${CX_INSTALL_ROOT_DIR}/config")
        set(CX_INSTALL_DOC_DIR    "${CX_INSTALL_ROOT_DIR}/doc")
        set(CX_INSTALL_PLUGIN_DIR "${CX_INSTALL_BINARY_DIR}")

        if(APPLE)
                set(CX_INSTALL_BUNDLE_CONTENTS_DIR "${CX_INSTALL_ROOT_DIR}/${CX_BUNDLE_NAME}.app/Contents")
                set(CX_INSTALL_BINARY_DIR "${CX_INSTALL_BUNDLE_CONTENTS_DIR}/MacOS")
                set(CX_INSTALL_CONFIG_DIR "${CX_INSTALL_BUNDLE_CONTENTS_DIR}/config")
                set(CX_INSTALL_DOC_DIR    "${CX_INSTALL_BUNDLE_CONTENTS_DIR}/doc")
                set(CX_INSTALL_PLUGIN_DIR "${CX_INSTALL_BINARY_DIR}")
        endif(APPLE)

        set(CX_CONFIG_ROOT_RELATIVE_INSTALLED ../config)
        set(CX_DOC_ROOT_RELATIVE_INSTALLED ../doc)

        # Turn off installation of CTK plugins. We do this ourselves using fixup_bundle
        set(CX_CTK_PLUGIN_NO_INSTALL NO_INSTALL)


message("-----------------CX_INSTALL_ROOT_DIR: " ${CX_INSTALL_ROOT_DIR})
message("-----------------CX_INSTALL_BINARY_DIR: " ${CX_INSTALL_BINARY_DIR})
message("-----------------CX_INSTALL_CONFIG_DIR: " ${CX_INSTALL_CONFIG_DIR})
message("-----------------CX_BUNDLE_NAME: " ${CX_BUNDLE_NAME})

#file(TO_CMAKE_PATH ${CX_INSTALL_BINARY_DIR} CX_INSTALL_BINARY_DIR)

#message("-----------------CX_INSTALL_BINARY_DIR: " ${CX_INSTALL_ROOT_DIR})
#message("-----------------CX_INSTALL_BINARY_DIR: " ${CX_INSTALL_BINARY_DIR})
#message("-----------------CX_BUNDLE_NAME: " ${CX_BUNDLE_NAME})

#	set(CX_INSTALL_PLUGIN_DIR "${CX_INSTALL_BINARY_DIR}/plugins") - did not work: get into trouble with relative paths and fixup_bundle

endmacro()

###############################################################################
#
# Needed to make sure all users get write permissions on the folder CustusX is installed in
#
# Note: might not be necessary any more as settings have been moved to ~/cx_settings folder.
#
###############################################################################
#macro(cx_install_root_directory_full_permissions)
#	set(TEMP_INSTALL_ROOT_DIR_SOURCE ${CMAKE_CURRENT_BINARY_DIR}/tmp/${CX_SYSTEM_BASE_NAME})
#	file(MAKE_DIRECTORY ${TEMP_INSTALL_ROOT_DIR_SOURCE})
#	install(DIRECTORY ${TEMP_INSTALL_ROOT_DIR_SOURCE}
#		DESTINATION ${CX_INSTALL_ROOT_DIR}/..
#		DIRECTORY_PERMISSIONS ${CX_FULL_PERMISSIONS}
#		)
#endmacro()


###############################################################################
#
# Set variables used throughout the build.
# Call first, in root dir!
#
###############################################################################
macro(cx_initialize_custusx_install)

#	cx_install_root_directory_full_permissions()

	set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME "Core")

	cx_install_set_version_info()

	cx_install_set_relative_path()

	cx_install_set_generators()
	cx_install_initialize_customizable_properties()

	cx_install_set_folder_structure()

	# used as a global variable: clear at start of run
	unset(CX_APPLE_TARGETS_TO_COPY CACHE)
	unset(CX_ALL_LIBRARY_DIRS CACHE)

endmacro()

###############################################################################
#
# Add library directories that should be parsed for libs during the install/fixup_bundle step.
#
###############################################################################
macro(cx_install_add_library_dirs)
#	message(STATUS "adding: "${ARGV})
    set(CX_ALL_LIBRARY_DIRS
        ${CX_ALL_LIBRARY_DIRS}
        ${ARGV}
        CACHE INTERNAL "List of dirs to look for libraries to use in fixup_bundle")
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
        cx_assert_variable_exists(CX_INSTALL_ROOT_DIR)

	if(CX_APPLE)
		get_property(is_bundle TARGET ${TARGET_ID} PROPERTY MACOSX_BUNDLE SET)
		if(${is_bundle})
			set( NEW_ENTRY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_ID}.app/Contents/MacOS/${TARGET_ID}" )
		else()
			set( NEW_ENTRY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_ID}" )
		endif()

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
                        DESTINATION ${CX_INSTALL_BINARY_DIR})
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
			if(IS_DIRECTORY ${EXE})
				install(DIRECTORY
					${EXE}
					DESTINATION ${CX_INSTALL_BINARY_DIR}
					FILE_PERMISSIONS ${CX_FULL_PERMISSIONS}
					DIRECTORY_PERMISSIONS ${CX_FULL_PERMISSIONS})
			else()
				install(FILES
					${EXE}
					DESTINATION ${CX_INSTALL_BINARY_DIR}
					PERMISSIONS ${CX_FULL_PERMISSIONS})
			endif()
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

	install(DIRECTORY ${CustusX_SOURCE_DIR}/config/
                        DESTINATION ${CX_INSTALL_CONFIG_DIR}/
			FILE_PERMISSIONS ${CX_FULL_PERMISSIONS}
			DIRECTORY_PERMISSIONS ${CX_FULL_PERMISSIONS}
			PATTERN ${CONFIG_EXCLUDE_PATTERN} EXCLUDE)
	
	if(CX_USE_OPENCL_UTILITY)
		install(FILES
				${OpenCLUtilityLibrary_KERNELS_DIR}/HistogramPyramids.cl
				${OpenCLUtilityLibrary_KERNELS_DIR}/HistogramPyramids.clh
                                DESTINATION ${CX_INSTALL_CONFIG_DIR}/tsf/)
	endif()
endfunction()

###############################################################################
#
# Installing required run time libs for Windows, msv*.dll
#
###############################################################################
function(cx_install_windows_runtime_libs DESTINATION_FOLDER)
	if(CX_WINDOWS)
		set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
        #OpenMP is required by FAST
        set(CMAKE_INSTALL_OPENMP_LIBRARIES TRUE)
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
    find_qt_plugin_dir(QT_PLUGINS_DIR)
    find_qt_libs_dir(QT_LIBS_DIR)
    find_qt_qml_dir(QT_QML_DIR)
        cx_assert_variable_exists(QT_PLUGINS_DIR)
        cx_assert_variable_exists(QT_LIBS_DIR)
        cx_assert_variable_exists(QT_QML_DIR)
        cx_assert_variable_exists(CX_INSTALL_ROOT_DIR)
        cx_assert_variable_exists(INSTALL_BINARY_DIR)
        cx_assert_variable_exists(CX_INSTALL_PLUGIN_DIR)

        #Add Qt libraries (frameworks for Apple) to fixup_bundle search path
        set(DIRS_LOCAL ${DIRS_LOCAL} ${QT_LIBS_DIR})

	# Install plugins in the default location as given by http://qt-project.org/doc/qt-4.8/qt-conf.html
	if(CX_LINUX)
		SET(INSTALL_QTPLUGIN_DIR "${INSTALL_BINARY_DIR}/plugins")
		SET(INSTALL_QTCONF_DIR "${INSTALL_BINARY_DIR}")

    # Install .so and versioned .so.x.y
    file(GLOB INSTALL_FILE_LIST
      LIST_DIRECTORIES false
      "${QT_LIBS_DIR}/*xcb*"
      "${QT_LIBS_DIR}/libQt*"
      "${QT_LIBS_DIR}/*-qt5*"
      # These libraries are needed for the Linux installation:
      "${QT_LIBS_DIR}/libicui18n.*"
      "${QT_LIBS_DIR}/libicuuc.*"
      "${QT_LIBS_DIR}/libicudata.*"
    )

    file(GLOB REMOVE_FILE_LIST
      LIST_DIRECTORIES false
     "${QT_LIBS_DIR}/*.a"
      )

    list(REMOVE_ITEM INSTALL_FILE_LIST ${REMOVE_FILE_LIST} )

    install(FILES
      ${INSTALL_FILE_LIST}
      DESTINATION ${INSTALL_BINARY_DIR}/)

	endif()
	if(CX_WINDOWS)
		SET(INSTALL_QTPLUGIN_DIR "${INSTALL_BINARY_DIR}/plugins")
		SET(INSTALL_QTCONF_DIR "${INSTALL_BINARY_DIR}")
	endif()
	IF(APPLE)
		SET(INSTALL_QTPLUGIN_DIR "${CX_INSTALL_ROOT_DIR}/${CX_BUNDLE_NAME}.app/Contents/plugins")
		SET(INSTALL_QTCONF_DIR   "${CX_INSTALL_ROOT_DIR}/${CX_BUNDLE_NAME}.app/Contents/Resources")
	ENDIF(APPLE)

	# Install needed Qt plugins by copying directories from the qt installation
	# Only install one sql driver (libqsqlite), as the other ones need additional packages installed to work
	install(DIRECTORY "${QT_PLUGINS_DIR}/" 
		DESTINATION ${INSTALL_QTPLUGIN_DIR}
		DIRECTORY_PERMISSIONS ${CX_FULL_PERMISSIONS}
		PATTERN "*sqldrivers*" EXCLUDE
	)

	install(DIRECTORY "${QT_PLUGINS_DIR}/"
		DESTINATION ${INSTALL_QTPLUGIN_DIR}
		DIRECTORY_PERMISSIONS ${CX_FULL_PERMISSIONS}
		FILES_MATCHING PATTERN "*qsqlite*"
	)

# Install Qt qml by copying all files in the qml directory
SET(INSTALL_QTQML_DIR "${INSTALL_BINARY_DIR}")
install(DIRECTORY "${QT_QML_DIR}/"
	DESTINATION ${INSTALL_QTQML_DIR}
	DIRECTORY_PERMISSIONS ${CX_FULL_PERMISSIONS}
)

  message(STATUS "QT_PLUGINS_DIR: " ${QT_PLUGINS_DIR})
  message(STATUS "QT_LIBS_DIR: " ${QT_LIBS_DIR})
  message(STATUS "QT_QML_DIR: " ${QT_QML_DIR})

	# install runtime plugins
        set(CX_PLUGIN_DIR "/plugins")
        if(CX_WINDOWS)
            set(CX_PLUGIN_DIR "")
        endif(CX_WINDOWS)
        install(DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}${CX_PLUGIN_DIR}/" # trailing slash copies contents, not plugin folder
		DESTINATION ${CX_INSTALL_PLUGIN_DIR}
		DIRECTORY_PERMISSIONS ${CX_FULL_PERMISSIONS})

	# explicitly tell which executables that should be fixed up.
        # why: fixup_bundle seems to fail to assemble exes in some cases.
	foreach(TARGET ${CX_APPLE_TARGETS_TO_COPY})
		get_filename_component(TARGET_FILENAME ${TARGET} NAME)
		set(TARGET_FILEPATH ${INSTALL_BINARY_DIR}/${TARGET_FILENAME})
                set(LIB_PATTERN_CODE "
    ${LIB_PATTERN_CODE}
    set\(TEMP \"\${CMAKE_INSTALL_PREFIX}/${TARGET_FILEPATH}\"\)
    set(PLUGINS \${PLUGINS} \${TEMP})
                        ")
	endforeach()

	# collect all installations here. They will be used by fixup_bundle to collect dependencies.
	# this is a sum of the input pattern (if any) and the qtplugins
	set(INSTALL_LIBRARIES_PATTERN_LOCAL
		${INSTALL_LIBRARIES_PATTERN_LOCAL}
		${CX_INSTALL_PLUGIN_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}
		${INSTALL_QTPLUGIN_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}
		${INSTALL_QTQML_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}
		)

	# install a qt.conf file
	# this inserts some cmake code into the install script to write the file
	# This is a requirement to get qt plugins in a relative path working!
	install(CODE "
		file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${INSTALL_QTCONF_DIR}/qt.conf\" \"\")
		")


	# create a code snippet : Search for the files in pattern,
	#                         add files to PLUGINS
	foreach(PATTERN ${INSTALL_LIBRARIES_PATTERN_LOCAL} )
                message(STATUS "PATTERN=" ${PATTERN})

		set(LIB_PATTERN_CODE
    "${LIB_PATTERN_CODE}
    set(TEMP)
    file\(GLOB_RECURSE TEMP \"\${CMAKE_INSTALL_PREFIX}/${PATTERN}\"\)
    set(PLUGINS \${PLUGINS} \${TEMP})
    "
			)
	endforeach()


        if(CX_WINDOWS)
            #On Windows verify_app fails if path contains a .
            set(TEMP_APPS_LOCAL "")
            foreach(APP ${APPS_LOCAL})
                string(REPLACE "./" "/" APP ${APP})
                list(APPEND TEMP_APPS_LOCAL ${APP})
            endforeach()
            set(APPS_LOCAL ${TEMP_APPS_LOCAL})
        endif(CX_WINDOWS)

        # fixup_bundle resets link paths for all targets within the bundle.
        # this code appears in cmake_install.cmake in the CURRENT_BINARY_DIR. Check there when changing.
        install(CODE "
    # Begin inserted fixup_bundle snippet
    ${LIB_PATTERN_CODE}
    set(CMAKE_MODULE_PATH
        ${CMAKE_MODULE_PATH}
        ${PROJECT_SOURCE_DIR}/CMake)
function(gp_resolved_file_type_override resolved_file type_var)
	if(resolved_file MATCHES \"^/usr/lib/libGLEW\")
		message(\"resolving \${resolved_file} as other\")
		set(\${type_var} other PARENT_SCOPE)
	endif()
endfunction()
    #include(BundleUtilities)
    include(cxBundleUtilities)
    message(STATUS \"CMAKE_INSTALL_PREFIX:  \${CMAKE_INSTALL_PREFIX}\")
    message(STATUS \"APPS_LOCAL:  ${APPS_LOCAL}\")
    message(STATUS \"PLUGINS:  \${PLUGINS}\")
    file(TO_CMAKE_PATH \"\${CMAKE_INSTALL_PREFIX}/${APPS_LOCAL}\" _APP_PATH)
    message(STATUS \"_APP_PATH:  \${_APP_PATH}\")
    #fixup_bundle(\"\${_APP_PATH}\"   \"\${PLUGINS}\"   \"${DIRS_LOCAL}\")
    cx_fixup_bundle(\"\${_APP_PATH}\"   \"\${PLUGINS}\"   \"${DIRS_LOCAL}\")
    # End inserted fixup_bundle snippet
    ")
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
find_package(Qt5Core)


set(PLUGINS_DESCRIPTION "	Plugins:")
getListOfVarsStartingWith("CX_PLUGIN_" matchedVars)
foreach (_var IN LISTS matchedVars)
        string(REPLACE "CX_PLUGIN_" "" PLUGIN_NAME ${_var})
		# skip the ":" between name and value in because this spams console with one issue per line in QtCreator...
		set(PLUGINS_DESCRIPTION "${PLUGINS_DESCRIPTION}\n		${PLUGIN_NAME} ${${_var}}")
endforeach()

        cx_assert_variable_exists(CX_SYSTEM_BASE_NAME)
        cx_assert_variable_exists(CustusX_VERSION_STRING)
        cx_assert_variable_exists(SSC_USE_GCOV)
        cx_assert_variable_exists(CX_USE_OPENCL_UTILITY)
	set(CONFIGURATION_TEXT
"
Configuration for ${CX_SYSTEM_BASE_NAME} ${CustusX_VERSION_STRING}

	Build Settings:
		System Base Name: ${CX_SYSTEM_BASE_NAME}
		Built on system: ${CMAKE_SYSTEM} ${CMAKE_ARCHITECTURE} ${CMAKE_SYSTEM_PROCESSOR}
		Build type: ${CMAKE_BUILD_TYPE}
		Shared Libraries: ${BUILD_SHARED_LIBRARIES}
		Coverage: ${SSC_USE_GCOV}
		Dev docs: ${BUILD_DOCUMENTATION}
		User docs: ${CX_BUILD_USER_DOCUMENTATION}

	Libraries:
		Qt Version ${Qt5Core_VERSION_STRING}
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
		Eigen Version: ${EIGEN_VERSION}

		OpenCL Utility Library: ${CX_USE_OPENCL_UTILITY}
	
${PLUGINS_DESCRIPTION}
")
    set(CX_CONFIGURATION_DESCRIPTION ${CONFIGURATION_TEXT} PARENT_SCOPE)
endfunction()

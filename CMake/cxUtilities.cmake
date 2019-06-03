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


###############################################################################
# Adding compiler flags to the CMAKE_CXX_FLAGS variable
#
###############################################################################
function(cx_add_defintions)
    message(STATUS 'Before CMAKE_CXX_FLAGS ' ${CMAKE_CXX_FLAGS})
    message(STATUS 'CX_CMAKE_CXX_FLAGS ' ${CX_CMAKE_CXX_FLAGS})
    foreach(_ITER ${CX_CMAKE_CXX_FLAGS})
        message(STATUS '            _ITER ' ${_ITER})
        add_definitions(${_ITER})
    endforeach()
    message(STATUS 'After CMAKE_CXX_FLAGS ' ${CMAKE_CXX_FLAGS})

    get_directory_property( DirDefs DIRECTORY ${CMAKE_SOURCE_DIR} COMPILE_DEFINITIONS )
    message( STATUS "DirDefs: " ${DirDefs} )
endfunction()

###############################################################################
# Make file path absolute if needed. Assuming
#
## Input variables:
#    INPUT_PATH : Path to file.
#
## Output variables:
#    RESULT_PATH : Absolute path to file.
###############################################################################
function(cx_make_path_absolute INPUT_PATH RESULT_PATH)
    if(IS_ABSOLUTE ${INPUT_PATH})
        set(ABS_SOURCE_FILE
            ${INPUT_PATH}
            )
    else()
        set(ABS_SOURCE_FILE
            ${CMAKE_CURRENT_LIST_DIR}/${INPUT_PATH}
            )
    endif()
    set(${RESULT_PATH} ${ABS_SOURCE_FILE} PARENT_SCOPE)
endfunction()

###############################################################################
# Clean /./ - like structures from a path list
#
## Input variables:
#    INPUT_PATH : List of paths to file.
#
## Output variables:
#    RESULT_PATH : List of cleaned paths to files
###############################################################################
function(cx_clean_path INPUT_PATH RESULT_PATH)
    set(_TEMP_INPUT ${INPUT_PATH})
    set(_TEMP_RETVAL "")
    foreach(_ITER ${_TEMP_INPUT})
        string(REPLACE "/./" "/" _ITER ${_ITER})
        list(APPEND _TEMP_RETVAL ${_ITER})
    endforeach()
    set(${RESULT_PATH} ${_TEMP_RETVAL} PARENT_SCOPE)
endfunction()

###############################################################################
# Assert that the input variable exists.
# Use to check prerequisites in code.
###############################################################################
function(cx_assert_variable_exists _var_name)
    if( NOT DEFINED ${_var_name} )
#    if( ${ARGC} EQUAL 0)
            message(SEND_ERROR "ERROR: Input variable does not exist: ${_var_name}")
    else()
            #message(STATUS "FOUND VARIABLE [${ARGC}]  [${ARGV}]")
    endif()
endfunction()

###############################################################################
# wrapper for option() with a convenience boolean setter for the value.
#
# Input arguments:
#
# NAME          : Name of option.
# DESCRIPTION   : Description of option
# BOOLEAN_VALUE : Value convertible to boolean that is used to set the value
#                 of option. No value means false.
#
###############################################################################
function( cx_define_option_from_boolean NAME DESCRIPTION)
    set( BOOLEAN_VALUE False)
    if( ARGC GREATER 2 )
            set( BOOLEAN_VALUE ${ARGV2})
    endif()

    if(${BOOLEAN_VALUE})
            option(${NAME} ${DESCRIPTION} ON)
    else()
            option(${NAME} ${DESCRIPTION} OFF)
    endif()
endfunction()

###############################################################################
# Define the following Operating System identifiers both in cmake and cpp:
#    CX_WINDOWS    for all windows
#    CX_WIN64      for 64 bit windows
#    CX_WIN32      for 32 bit windows
#    CX_LINUX
#    CX_APPLE
#
#  Additionally, the
#    __APPLE__
#  is defined for mac
###############################################################################
macro(cx_set_os_identifiers)
    if(WIN32)
        set(CX_WINDOWS 1) #for cmake
        add_definitions(-DCX_WINDOWS) #for compiler
        message( STATUS "This is a Windows build.")
        add_definitions(-D__WIN32__) #  -because they are needed at moc'ing time
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(CX_WIN64 1) #for cmake
            add_definitions(-DCX_WIN64) #for compiler
            message( STATUS "This is a 64 bit build.")
        else()
            set(CX_WIN32 1) #for cmake
            add_definitions(-DCX_WIN32) #for compiler
            message( STATUS "This is a 32 bit build.")
        endif()
    endif()

    if(APPLE)
        add_definitions(-D__APPLE__)
        add_definitions(-DCX_APPLE)
        set(CX_APPLE 1)
    endif(APPLE)

    # define LINUX convenience variable
    if(UNIX AND NOT APPLE)
        add_definitions(-DCX_LINUX) #for compiler
        set(CX_LINUX 1)
    endif(UNIX AND NOT APPLE)
endmacro()

###############################################################################
# Find date for latest cmake rebuild, in ISO 8601 format
#
###############################################################################
MACRO(cx_get_today RESULT)
	string(TIMESTAMP ${RESULT} "%Y-%m-%d")
ENDMACRO()


###############################################################################
# Print a cmake list to stdout using lots of wrap.
#
# Usage: print_list_verbose(LIST_VAR "Header text")
###############################################################################
function(cx_print_list_verbose LIST_VAR HEADER_TEXT)
    message(STATUS "-----------------------------------------")
    message(STATUS "---- " ${HEADER_TEXT} " ----")
    message(STATUS "{" )
    foreach(VAR ${${LIST_VAR}})
            message(STATUS "    " ${VAR})
    endforeach()
    message(STATUS "}" )
    message(STATUS "-----------------------------------------" )
endfunction()

###############################################################################
# private
###############################################################################
macro(_cx_query_is_full_filename RESULT CLASS_NAME_WITH_PATH)
    STRING(REGEX MATCH "(\\.h|\\.cpp|\\.cxx|\\.qrc|\\.hxx|\\.hpp|\\.txx)$" VALID_SUFFIX ${CLASS_NAME_WITH_PATH})
    if("${VALID_SUFFIX}" STREQUAL "")
            set(${RESULT} "False")
    else()
            set(${RESULT} "True")
    endif()
endmacro()

###############################################################################
# private
###############################################################################
function(_cx_add_one_class SOURCE_FILES CLASS_NAME_WITH_PATH)
    _cx_query_is_full_filename(IS_FULL_FILENAME ${CLASS_NAME_WITH_PATH})
    if(${IS_FULL_FILENAME} MATCHES "False")
            set(RESULT_add_one_class ${CLASS_NAME_WITH_PATH}.h ${CLASS_NAME_WITH_PATH}.cpp)
    else()
            set(RESULT_add_one_class ${CLASS_NAME_WITH_PATH})
    endif()

    set(${SOURCE_FILES} ${${SOURCE_FILES}} ${RESULT_add_one_class} PARENT_SCOPE)
endfunction()

###############################################################################
# private
###############################################################################
function(_cx_add_header_name SOURCE_FILES CLASS_NAME_WITH_PATH)
    _cx_query_is_full_filename(IS_FULL_FILENAME ${CLASS_NAME_WITH_PATH})
    if(${IS_FULL_FILENAME} MATCHES "False")
            set(RESULT_cx_add_header_name ${CLASS_NAME_WITH_PATH}.h)
    else()
            set(RESULT_cx_add_header_name ${CLASS_NAME_WITH_PATH})
    endif()

    set(${SOURCE_FILES} ${${SOURCE_FILES}} ${RESULT_cx_add_header_name} PARENT_SCOPE) # set retval
endfunction()

###############################################################################
# Add classes and files to SOURCE_FILES.
#
# First argument:  List to populate with generated files.
# Other arguments: Classes/files to add to list. Classes will expand to a h and cpp file.
#                  Either input class names or file names, prepended with folder.
#
# Usage: add_class(SOURCE_FILES class1 class2 folder/class3 file1.h ...)
###############################################################################
function(cx_add_class SOURCE_FILES_ARGUMENT)
    set(CLASS_NAME_WITH_PATH ${ARGV})
    list(REMOVE_AT CLASS_NAME_WITH_PATH 0)

    foreach(CLASS_NAME ${CLASS_NAME_WITH_PATH})
            _cx_add_one_class(RESULT_add_class ${CLASS_NAME})
    endforeach()

    set(${SOURCE_FILES_ARGUMENT} ${${SOURCE_FILES_ARGUMENT}} ${RESULT_add_class} PARENT_SCOPE)
endfunction()

###############################################################################
# Add Qt moc classes and files to SOURCE_FILES.
#
# As add_class(), but the class is run through the Qt Moc system as well.
###############################################################################
function(cx_add_class_qt_moc SOURCE_FILES_ARGUMENT)
    set(CLASS_NAME_WITH_PATH ${ARGV})
    list(REMOVE_AT CLASS_NAME_WITH_PATH 0)

    foreach(CLASS_NAME ${CLASS_NAME_WITH_PATH})
            _cx_add_one_class(RESULT_add_class_qt_moc ${CLASS_NAME})
    endforeach()

    foreach(CLASS_NAME ${CLASS_NAME_WITH_PATH})
            _cx_add_header_name(HEADER_NAMES ${CLASS_NAME})
    endforeach()

    # optimized: QT5_WRAP_CPP has large overhead: call once.
    qt5_wrap_cpp( RESULT_add_class_qt_moc ${HEADER_NAMES} )

    set(${SOURCE_FILES_ARGUMENT} ${${SOURCE_FILES_ARGUMENT}} ${RESULT_add_class_qt_moc} PARENT_SCOPE)
endfunction()

###############################################################################
# Determin OpenGL Version
#
# INFO: does not work on windows
###############################################################################
macro(cx_opengl_version)

    if(OPENGL_FOUND AND NOT DEFINED CX_WINDOWS AND NOT DEFINED CX_APPLE)
        execute_process(COMMAND glxinfo COMMAND grep "OpenGL version" OUTPUT_VARIABLE _OPENGL_VERSION_STRING)

        string(REGEX REPLACE "[^:]*:()" "\\1" OPENGL_VERSION "${_OPENGL_VERSION_STRING}")

        if("${OPENGL_VERSION}" STREQUAL "")
            message(WARNING "Cannot determine OpenGL's version")
        endif("${OPENGL_VERSION}" STREQUAL "")
    else(OPENGL_FOUND AND NOT DEFINED CX_WINDOWS AND NOT DEFINED CX_APPLE)
        set(OPENGL_VERSION "<cannot_find_on_windows/mac>")
    endif(OPENGL_FOUND AND NOT DEFINED CX_WINDOWS AND NOT DEFINED CX_APPLE)
    
endmacro(cx_opengl_version)

###############################################################################
#
# Get a list of all variables with _prefix, return in _varResult
#
###############################################################################
function(getListOfVarsStartingWith _prefix _varResult)
    get_cmake_property(_vars VARIABLES)
    string (REGEX MATCHALL "(^|;)${_prefix}[A-Za-z0-9/_\\.:-]*" _matchedVars "${_vars}")
    set (${_varResult} ${_matchedVars} PARENT_SCOPE)
endfunction()


###############################################################################
# Borrowed from
#          https://github.com/lxde/lxqt-qtplugin/blob/master/qt5/CMakeLists.txt
#
# quote:
#   there is no standard way to get the plugin dir of Qt5 with cmake
#   The best way is get_target_property(QT_PLUGINS_DIR Qt5::QGtk2ThemePlugin LOCATION).
#   This directly returns the platformthemes dir. However, this does not work
#   in some distros, such as ubuntu.
#   Finally, I came up with a more reliable way by using qmake.
###############################################################################
function(find_qt_plugin_dir _varResult)
    get_target_property(QT_QMAKE_EXECUTABLE ${Qt5Core_QMAKE_EXECUTABLE} IMPORTED_LOCATION)
    if(NOT QT_QMAKE_EXECUTABLE)
        message(FATAL_ERROR "qmake is not found.")
    endif()
    # execute the command "qmake -query QT_INSTALL_PLUGINS" to get the path of plugins dir.
    execute_process(COMMAND ${QT_QMAKE_EXECUTABLE} -query QT_INSTALL_PLUGINS
        OUTPUT_VARIABLE _QT_PLUGINS_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    if(_QT_PLUGINS_DIR)
        message(STATUS "Qt5 plugin directory:" ${_QT_PLUGINS_DIR})
    else()
        message(FATAL_ERROR "Qt5 plugin directory cannot be detected.")
    endif()
	set (${_varResult} ${_QT_PLUGINS_DIR} PARENT_SCOPE)
endfunction()


###############################################################################
#
# Find the location of qts bin folder
#
###############################################################################
function(find_qt_bin_dir _varResult)
    find_package(Qt5Core REQUIRED)
    get_target_property(QtCore_location Qt5::Core LOCATION)
    get_filename_component(_QT_BIN_DIR ${QtCore_location} DIRECTORY)
    set(${_varResult} ${_QT_BIN_DIR} PARENT_SCOPE)
endfunction()

###############################################################################
#
# Find the location of qts qml folder
#
###############################################################################
function(find_qt_qml_dir _varResult)
get_target_property(QT_QMAKE_EXECUTABLE ${Qt5Core_QMAKE_EXECUTABLE} IMPORTED_LOCATION)
if(NOT QT_QMAKE_EXECUTABLE)
    message(FATAL_ERROR "qmake is not found.")
endif()
# execute the command "qmake -query QT_INSTALL_QML" to get the path of qml dir.
execute_process(COMMAND ${QT_QMAKE_EXECUTABLE} -query QT_INSTALL_QML
    OUTPUT_VARIABLE _QT_QML_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
if(_QT_QML_DIR)
    message(STATUS "Qt5 qml directory:" ${_QT_QML_DIR})
else()
    message(FATAL_ERROR "Qt5 qml directory cannot be detected.")
endif()
set (${_varResult} ${_QT_QML_DIR} PARENT_SCOPE)
endfunction()

###############################################################################
#
#Find Qt libraries directory (Qt frameworks directory for APPLE)
#
###############################################################################
function(find_qt_libs_dir _varResult)
    get_target_property(_QT_LIBRARY_DIRS Qt5::Core LOCATION)
    get_filename_component(_QT_LIBRARY_DIRS ${_QT_LIBRARY_DIRS} DIRECTORY)
    if(APPLE)
        get_filename_component(_QT_LIBRARY_DIRS ${_QT_LIBRARY_DIRS} DIRECTORY)
    endif()
    set(${_varResult} ${_QT_LIBRARY_DIRS} PARENT_SCOPE)
endfunction()

###############################################################################
#
# Create a list of all subdirs in curdir. 
# Return as list of paths relative to curdir. 
#
###############################################################################
macro(SUBDIRLIST result curdir)
  file(GLOB children RELATIVE ${curdir} ${curdir}/*)
  #message(STATUS "children: " ${children})
  set(dirlist "")
  foreach(child ${children})
    if(IS_DIRECTORY ${curdir}/${child})
        list(APPEND dirlist ${child})
    endif()
  endforeach()
  set(${result} ${dirlist})
endmacro()


###############################################################################
#
# Utility for printing all variables known to CMake at a given point.
#
###############################################################################
function(cx_print_variables REGEX_TO_MATCH)
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        if(_variableName MATCHES ${REGEX_TO_MATCH})
            message(STATUS "${_variableName}=${${_variableName}}")
        endif()
    endforeach()
endfunction()


###############################################################################
#
# Creates a export header.
#
# MY_LIBRARY_NAME: Prefix for the export header name
# OUT_EXPORT_HEADER: Path to the prefix header
#
###############################################################################
macro(cx_create_export_header MY_LIBRARY_NAME)
    set(MY_EXPORT_HEADER_PREFIX ${MY_LIBRARY_NAME})
    set(MY_LIBRARY_EXPORT_DIRECTIVE ${MY_LIBRARY_NAME}_EXPORT)
    set(MY_LIBNAME ${MY_LIBRARY_NAME})
    configure_file(
        ${CTK_SOURCE_DIR}/Libs/ctkExport.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/${MY_LIBRARY_NAME}Export.h
    )
endmacro()

###############################################################################
#
# Removes duplicate include directories.
#
# Confirmed on Windows:
# 1. Speeds up the configuration step significantly when generating a target.
# 2. Slightly speeds up mocing if used before QT5_WRAP_CPP, because duplicate include
# directories are removed and thus not added to moc_<filename>.cpp_parameters (the -IC: tags)
#
###############################################################################
#macro(cx_remove_duplicate_include_directories)
#    get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
#    list(REMOVE_DUPLICATES dirs)
#    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES ${dirs})
#endmacro()


###############################################################################
#
# http://stackoverflow.com/questions/148570/using-pre-compiled-headers-with-cmake
#
###############################################################################
macro(ADD_MSVC_PRECOMPILED_HEADER PrecompiledHeader PrecompiledSource SourcesVar)
  if(MSVC)
    message(STATUS "Generating precompiled headers.")
    get_filename_component(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
    set(PrecompiledBinary "${CMAKE_CURRENT_BINARY_DIR}/${PrecompiledBasename}.pch")
    set(Sources ${${SourcesVar}})

    set_source_files_properties(${PrecompiledSource}
                                PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_OUTPUTS "${PrecompiledBinary}")
    set_source_files_properties(${Sources}
                                PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_DEPENDS "${PrecompiledBinary}")
    # Add precompiled header to SourcesVar
    list(APPEND ${SourcesVar} ${PrecompiledSource})
  endif(MSVC)
endmacro(ADD_MSVC_PRECOMPILED_HEADER)

###############################################################################
#
# Define the option CX_APP_${APP_NAME}, default value ON
# If the option is ON, add the subdirectory APP_NAME.
#
###############################################################################
macro(cx_add_optional_app_subdirectory APP_NAME)
  option(CX_APP_${APP_NAME} "Build the application ${APP_NAME}" ON)
  if(${CX_APP_${APP_NAME}})
	add_subdirectory(${APP_NAME})
  endif()
endmacro()

###############################################################################
#
# Set some default values for the MACOSX_BUNDLE variable family
# Customize for each application,
# in particular the icon, which must have the resource installed int
#
###############################################################################
macro(cx_initialize_macosx_bundle)
	set(CX_COPYRIGHT_TEXT "Copyright SINTEF Medical Technology. BSD licence")

	if(APPLE) # For Apple set the icns file containing icons
		set(MACOSX_BUNDLE_ICON_FILE "CustusX.icns") # set how it shows up in the Info.plist file
		set(MACOSX_BUNDLE_INFO_STRING ${CX_SYSTEM_DEFAULT_APPLICATION})
		set(MACOSX_BUNDLE_BUNDLE_NAME ${CX_SYSTEM_DEFAULT_APPLICATION})
		set(MACOSX_BUNDLE_BUNDLE_VERSION "${CustusX_VERSION_STRING}")
		set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${MACOSX_BUNDLE_BUNDLE_VERSION}")
		set(MACOSX_BUNDLE_LONG_VERSION_STRING "${CX_SYSTEM_DEFAULT_APPLICATION} ${MACOSX_BUNDLE_BUNDLE_VERSION}")
		set(MACOSX_BUNDLE_COPYRIGHT ${CX_COPYRIGHT_TEXT})
	endif(APPLE)
endmacro()

###############################################################################
#
# Add a non-source file to the project file.
#
# Call this for each file that sould be added to the project for easy edit
# access from an IDE (VS, XCode, QtCreator)
#
# See also http://public.kitware.com/Bug/view.php?id=7835
#
###############################################################################
function(cx_add_non_source_file NON_SOURCE_FILE)
	if(IS_ABSOLUTE ${NON_SOURCE_FILE})
		cx_add_string_to_global_property_list(CX_NON_SOURCE_FILES "${NON_SOURCE_FILE}")
	else()
		cx_add_string_to_global_property_list(CX_NON_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${NON_SOURCE_FILE}")
	endif()
endfunction()

###############################################################################
#
# Add all non-source files to the project file.
# (Files that shall not be compiled with this project file.)
##
###############################################################################
function(cx_add_non_source_files_to_project_file)
	get_property(NON_SOURCE_FILES GLOBAL PROPERTY CX_NON_SOURCE_FILES)
	add_custom_target(NonSourceFiles SOURCES ${NON_SOURCE_FILES})
endfunction()

###############################################################################
#
# Set Retina display resolution on the input TARGET
#
#   http://blog.qt.io/blog/2013/04/25/retina-display-support-for-mac-os-ios-and-x11/
#   http://mpiannucci.com/view/cmakeretina
#   https://blog.inventic.eu/2013/02/problem-with-qt-application-on-macos-retina-display/
#
# Must be combined with disabling retina on VTK/GL windows, as described in
#   http://public.kitware.com/pipermail/vtkusers/2015-February/090117.html
#
##
###############################################################################
function(cx_set_target_high_resolution TARGET)
	if(APPLE)
		set_target_properties(${TARGET} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CustusX_SOURCE_DIR}/CMake/MacOSXBundleInfo_cx_custom.plist.in)
#        set(MACOSX_BUNDLE_INFO_PLIST ${CustusX_SOURCE_DIR}/CMake/MacOSXBundleInfo_cx_custom.plist.in)
	endif(APPLE)
endfunction()


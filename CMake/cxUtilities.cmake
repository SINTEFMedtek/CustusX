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
# Assert that the input variable exists.
# Use to check prerequisites in code.
###############################################################################
function(cx_assert_variable_exists)
	if( ${ARGC} EQUAL 0)
		message(SEND_ERROR "ERROR: Input variable does not exist")
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
# Find date for latest cmake rebuild
#
# Inspired by http://marc.info/?l=cmake&m=129077863120656&w=2
###############################################################################
MACRO(cx_get_today RESULT)
    IF(WIN32)
        EXECUTE_PROCESS(
            COMMAND "cmd" "/C date /T"
            OUTPUT_VARIABLE ${RESULT}
            )
	ELSEIF(UNIX)

        EXECUTE_PROCESS(
            COMMAND "date" "+%d/%m/%Y" 
            OUTPUT_VARIABLE ${RESULT}
            )
        string(REGEX REPLACE "(..)/(..)/(....).*" "\\3-\\2-\\1" ${RESULT} ${${RESULT}})
#        EXECUTE_PROCESS(COMMAND "date" "+%Y-%m-%d" OUTPUT_VARIABLE ${RESULT})
        ELSE(WIN32)
        MESSAGE(SEND_ERROR "date not implemented")
        SET(${RESULT} 000000)
    ENDIF(WIN32)
ENDMACRO()

###############################################################################
# Get a description of the current build from git.
#
# http://stackoverflow.com/questions/1435953/how-can-i-pass-git-sha1-to-compiler-as-definition-using-cmake
# use the entire output of git describe here instead.
#
###############################################################################
MACRO(cx_get_git_build_description RESULT)
	find_package(Git REQUIRED)
	IF(WIN32)
	    execute_process(
		    COMMAND "git.exe"} describe --tags
		    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
		    OUTPUT_VARIABLE ${RESULT}
		    )
		message(STATUS "Result: " ${RESULT})
	ELSEIF(UNIX)	
		execute_process(
		    COMMAND git describe --tags
		    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
		    OUTPUT_VARIABLE ${RESULT}
		    )
    ENDIF(WIN32)
	# remove first character - should always be a "v" as first in f.ex. "v3.5.3"
	string(SUBSTRING ${${RESULT}} 1 -1 ${RESULT})
ENDMACRO()

###############################################################################
# Define variables for the current version.
# Input (major, minor, patch) define the version,
# while the type determines additional info:
#
# type==ALPHA:
#   add detailed information to the postfix, such as git sha and date.
# type==RELEASE:
#   add no extra information.
# type==<other>:
#   the postfix is set to <other>
#
# The following variables are defined:
#
#	${PROJECT_NAME}_VERSION_MAJOR
#	${PROJECT_NAME}_VERSION_MINOR
#	${PROJECT_NAME}_VERSION_PATCH
#	${PROJECT_NAME}_VERSION_POSTFIX
#	${PROJECT_NAME}_VERSION_STRING
#
###############################################################################
MACRO(cx_define_version major minor patch type)
	set(${PROJECT_NAME}_VERSION_MAJOR  ${major})
	set(${PROJECT_NAME}_VERSION_MINOR  ${minor})
	set(${PROJECT_NAME}_VERSION_PATCH  ${patch})

	cx_get_git_build_description(GIT_DESCRIBE)

	if(${type} STREQUAL ALPHA)
		cx_get_today(TODAY_DATE)
		set(GIT_DESCRIBE "${GIT_DESCRIBE}.${TODAY_DATE}")
		set(POSTFIX ".alpha")
	elseif(${type} STREQUAL BETA)
		set(POSTFIX ".beta")
	elseif(${type} STREQUAL RELEASE)
		set(POSTFIX "")
	else()
		set(POSTFIX "." ${type})
	endif()
	
	#on windows this string contains newlines and whitespaces that needs to be removed
	STRING(REGEX REPLACE "\r|\n" "" GIT_DESCRIBE ${GIT_DESCRIBE})
	STRING(REGEX REPLACE " " "" GIT_DESCRIBE ${GIT_DESCRIBE})

	set(${PROJECT_NAME}_VERSION_POSTFIX ${POSTFIX})
	set(GENERATED_VERSION_STRING ${major}.${minor}.${patch}${POSTFIX})
	string(REGEX MATCH ${GENERATED_VERSION_STRING} MATCHING_STRING ${GIT_DESCRIBE})
	if("${MATCHING_STRING}" STREQUAL "")
		message("Warning:
		Version string extracted from git: ${GIT_DESCRIBE}
		Version string generated: ${GENERATED_VERSION_STRING}
		The generated string should be contained in the git string.
		Reverting to generated string, setting dirty tag.")
		set(${PROJECT_NAME}_VERSION_STRING ${GENERATED_VERSION_STRING}.dirty)
	else()
		set(${PROJECT_NAME}_VERSION_STRING ${GIT_DESCRIBE})
	endif()
#	set(${PROJECT_NAME}_VERSION_STRING ${major}.${minor}.${patch}${${PROJECT_NAME}_VERSION_POSTFIX})

# no good - leads to full rebuild for every commit. Moved to resource/settings/cxConfig.h
#	add_definitions(
#		-D${PROJECT_NAME}_VERSION_STRING="${${PROJECT_NAME}_VERSION_STRING}"
#		-D${PROJECT_NAME}_VERSION_MAJOR=${${PROJECT_NAME}_VERSION_MAJOR}
#		-D${PROJECT_NAME}_VERSION_MINOR=${${PROJECT_NAME}_VERSION_MINOR}
#		-D${PROJECT_NAME}_VERSION_PATCH=${${PROJECT_NAME}_VERSION_PATCH}
#	)
ENDMACRO()

###############################################################################
#
###############################################################################
MACRO(cx_read_version)
	file(READ ${PROJECT_SOURCE_DIR}/version.ini CX_VERSION_FILE_DATA)

	string(REGEX REPLACE ".*major[ ]*=[ ]*([0-9]+).*" "\\1" VERSION_MAJOR ${CX_VERSION_FILE_DATA})
	string(REGEX REPLACE ".*minor[ ]*=[ ]*([0-9]+).*" "\\1" VERSION_MINOR ${CX_VERSION_FILE_DATA})
	string(REGEX REPLACE ".*patch[ ]*=[ ]*([0-9]+).*" "\\1" VERSION_PATCH ${CX_VERSION_FILE_DATA})
	string(REGEX REPLACE ".*type[ ]*=[ ]*([^$]*).*" "\\1" VERSION_TYPE ${CX_VERSION_FILE_DATA})
	STRING(REGEX REPLACE "(\r?\n)+$" "" VERSION_TYPE "${VERSION_TYPE}")

	cx_define_version(${VERSION_MAJOR} ${VERSION_MINOR} ${VERSION_PATCH} ${VERSION_TYPE})
	message(STATUS "Version: ${CustusX_VERSION_STRING}")

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
        cx_remove_duplicate_include_directories()
	QT5_WRAP_CPP( RESULT_add_class_qt_moc ${HEADER_NAMES} )

    set(${SOURCE_FILES_ARGUMENT} ${${SOURCE_FILES_ARGUMENT}} ${RESULT_add_class_qt_moc} PARENT_SCOPE)
endfunction()

###############################################################################
# Determin OpenGL Version
#
# INFO: does not work on windows
###############################################################################
macro(cx_opengl_version)

    IF (OPENGL_FOUND AND NOT DEFINED CX_WINDOWS AND NOT DEFINED CX_APPLE)
      execute_process(COMMAND glxinfo COMMAND grep "OpenGL version" OUTPUT_VARIABLE _OPENGL_VERSION_STRING)
    
      STRING (REGEX REPLACE "[^:]*:()" "\\1" OPENGL_VERSION "${_OPENGL_VERSION_STRING}")
        
      IF ("${OPENGL_VERSION}" STREQUAL "")
        MESSAGE (WARNING "Cannot determine OpenGL's version")
      ENDIF ("${OPENGL_VERSION}" STREQUAL "")
    ELSE (OPENGL_FOUND AND NOT DEFINED CX_WINDOWS AND NOT DEFINED CX_APPLE)
        SET(OPENGL_VERSION "<cannot_find_on_windows/mac>")
    ENDIF (OPENGL_FOUND AND NOT DEFINED CX_WINDOWS AND NOT DEFINED CX_APPLE)
    
endmacro(cx_opengl_version)

###############################################################################
#
# Get a list of all variables with _prefix, return in _varResult
#
###############################################################################
function (getListOfVarsStartingWith _prefix _varResult)
	get_cmake_property(_vars VARIABLES)
	string (REGEX MATCHALL "(^|;)${_prefix}[A-Za-z0-9/_\\.:]*" _matchedVars "${_vars}")
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
function (find_qt_plugin_dir _varResult)
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
# Create a list of all subdirs in curdir. 
# Return as list of paths relative to curdir. 
#
###############################################################################
MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  #message(STATUS "children: " ${children})
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
        LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

###############################################################################
#
# Given a PLUGINS_LIST of plugin name:VAL pairs, 
# insert the NEW_PLUGIN with NEW_PLUGIN_VALUE
# if it is not already present in the list
#
# PLUGINS_VARIABLE: variable containing <pluginname>:<val> pairs
#
###############################################################################
function(cx_insert_plugin_if_not_in_list PLUGINS_LIST_VARIABLE NEW_PLUGIN_NAME NEW_PLUGIN_VALUE PLUGIN_BUILD_OPTION_PREFIX)
    set(plugins_list ${${PLUGINS_LIST_VARIABLE}})
    list(FIND plugins_list ${NEW_PLUGIN_NAME}:OFF FOUND_OFF)
    list(FIND plugins_list ${NEW_PLUGIN_NAME}:ON FOUND_ON)
    getListOfVarsStartingWith(${PLUGIN_BUILD_OPTION_PREFIX}${NEW_PLUGIN_NAME} matchedVars)
    list(LENGTH matchedVars VAR_DEFINED)
    #message(STATUS "plugin_name " ${NEW_PLUGIN_NAME} "  found OFF=" ${FOUND_OFF} " ON=" ${FOUND_ON} " DEFINED=" ${VAR_DEFINED} " existing: " ${matchedVars})
    if((${FOUND_OFF} EQUAL -1) AND (${FOUND_ON} EQUAL -1))
        if(${VAR_DEFINED} EQUAL 0)
            message("Found new plugin " ${NEW_PLUGIN_NAME} ", adding option " ${NEW_PLUGIN_VALUE} ".")
        endif()
        set(plugins_list ${plugins_list} ${NEW_PLUGIN_NAME}:${NEW_PLUGIN_VALUE})
    endif()

    set(${PLUGINS_LIST_VARIABLE} ${plugins_list} PARENT_SCOPE)
endfunction()

###############################################################################
#
# Given a list of plugin name:VAL pairs, look for other plugins not defined in the list.
# Undefined plugins are added with the default value of OFF.
#
# PLUGINS_VARIABLE: variable containing <pluginname>:<val> pairs
# PLUGIN_BUILD_OPTION_PREFIX: prefix used by ctkMacroSetupPlugins to generate build variables
#
###############################################################################
function(cx_insert_undefined_plugins PLUGINS_VARIABLE PLUGIN_BUILD_OPTION_PREFIX)
    SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})
    set(plugins_list ${${PLUGINS_VARIABLE}})

    foreach(plugin_name ${SUBDIRS})
        cx_insert_plugin_if_not_in_list(plugins_list ${plugin_name} "OFF" ${PLUGIN_BUILD_OPTION_PREFIX})
    endforeach()

    set(${PLUGINS_VARIABLE} ${plugins_list} PARENT_SCOPE)
endfunction()

###############################################################################
#
# Utility funciton for printing all variables known to CMake at a given point.
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
    include_directories(
        ${CMAKE_CURRENT_BINARY_DIR}
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
macro(cx_remove_duplicate_include_directories)
    get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
    list(REMOVE_DUPLICATES dirs)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES ${dirs})
endmacro()


###############################################################################
#
# http://stackoverflow.com/questions/148570/using-pre-compiled-headers-with-cmake
#
###############################################################################
MACRO(ADD_MSVC_PRECOMPILED_HEADER PrecompiledHeader PrecompiledSource SourcesVar)
  IF(MSVC)
    MESSAGE(STATUS "Generating precompiled headers.")
    GET_FILENAME_COMPONENT(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
    SET(PrecompiledBinary "${CMAKE_CURRENT_BINARY_DIR}/${PrecompiledBasename}.pch")
    SET(Sources ${${SourcesVar}})

    SET_SOURCE_FILES_PROPERTIES(${PrecompiledSource}
                                PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_OUTPUTS "${PrecompiledBinary}")
    SET_SOURCE_FILES_PROPERTIES(${Sources}
                                PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_DEPENDS "${PrecompiledBinary}")
    # Add precompiled header to SourcesVar
    LIST(APPEND ${SourcesVar} ${PrecompiledSource})
  ENDIF(MSVC)
ENDMACRO(ADD_MSVC_PRECOMPILED_HEADER)

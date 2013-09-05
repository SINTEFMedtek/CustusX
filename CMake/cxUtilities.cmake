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
	#message(STATUS "testing init value for ${NAME}: " ${BOOLEAN_VALUE})
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
		    COMMAND "C:/Program Files (x86)/Git/bin/git.exe" describe --tags
		    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
		    OUTPUT_VARIABLE ${RESULT}
		    )
	ELSEIF(UNIX)	
		execute_process(
		    COMMAND git describe --tags
		    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
		    OUTPUT_VARIABLE ${RESULT}
		    )
    ENDIF(WIN32)
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
# The same variables are added to the code using add_definitions -D
###############################################################################
MACRO(cx_define_version major minor patch type)
	set(${PROJECT_NAME}_VERSION_MAJOR  ${major})
	set(${PROJECT_NAME}_VERSION_MINOR  ${minor})
	set(${PROJECT_NAME}_VERSION_PATCH  ${patch})

	if(${type} STREQUAL ALPHA)
		cx_get_git_build_description(GIT_DESCRIBE)
		cx_get_today(TODAY_DATE)
		set(POSTFIX ".alpha.git_${GIT_DESCRIBE}.${TODAY_DATE}")
	elseif(${type} STREQUAL BETA)
		set(POSTFIX ".beta")
	elseif(${type} STREQUAL RELEASE)
		set(POSTFIX "")
	else()
		set(POSTFIX "."${type})
	endif()
	
	#on windows this string contains newlines and whitespaces that needs to be removed
	STRING(REGEX REPLACE "\r|\n" "" POSTFIX_TEMP ${POSTFIX})
	STRING(REGEX REPLACE " " "" POSTFIX_CLEAN ${POSTFIX_TEMP})

	set(${PROJECT_NAME}_VERSION_POSTFIX ${POSTFIX_CLEAN})
	set(${PROJECT_NAME}_VERSION_STRING ${major}.${minor}.${patch}${${PROJECT_NAME}_VERSION_POSTFIX})

# no good - leads to full rebuild for every commit. Moved to resource/settings/cxConfig.h
#	add_definitions(
#		-D${PROJECT_NAME}_VERSION_STRING="${${PROJECT_NAME}_VERSION_STRING}"
#		-D${PROJECT_NAME}_VERSION_MAJOR=${${PROJECT_NAME}_VERSION_MAJOR}
#		-D${PROJECT_NAME}_VERSION_MINOR=${${PROJECT_NAME}_VERSION_MINOR}
#		-D${PROJECT_NAME}_VERSION_PATCH=${${PROJECT_NAME}_VERSION_PATCH}
#	)
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
	STRING(REGEX MATCH "(\\.h|\\.cpp|\\.cxx|\\.qrc|\\.hxx|\\.hpp)$" VALID_SUFFIX ${CLASS_NAME_WITH_PATH})
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
	# optimized: QT4_WRAP_CPP has large overhead: call once.
	QT4_WRAP_CPP( RESULT_add_class_qt_moc ${HEADER_NAMES} )

        set(${SOURCE_FILES_ARGUMENT} ${${SOURCE_FILES_ARGUMENT}} ${RESULT_add_class_qt_moc} PARENT_SCOPE)
endfunction()


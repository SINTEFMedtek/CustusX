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
        EXECUTE_PROCESS(COMMAND "cmd" " /C date /T" OUTPUT_VARIABLE ${RESULT})
	ELSEIF(UNIX)

        EXECUTE_PROCESS(COMMAND "date" "+%d/%m/%Y" OUTPUT_VARIABLE ${RESULT})
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
	exec_program(
		"git"
		${PROJECT_SOURCE_DIR}
		ARGS "describe --tags"
		OUTPUT_VARIABLE ${RESULT}
		)
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

	set(${PROJECT_NAME}_VERSION_POSTFIX "${POSTFIX}")
	set(${PROJECT_NAME}_VERSION_STRING "${major}.${minor}.${patch}${${PROJECT_NAME}_VERSION_POSTFIX}")

	add_definitions(
		-D${PROJECT_NAME}_VERSION_STRING="${${PROJECT_NAME}_VERSION_STRING}"
		-D${PROJECT_NAME}_VERSION_MAJOR=${${PROJECT_NAME}_VERSION_MAJOR}
		-D${PROJECT_NAME}_VERSION_MINOR=${${PROJECT_NAME}_VERSION_MINOR}
		-D${PROJECT_NAME}_VERSION_PATCH=${${PROJECT_NAME}_VERSION_PATCH}
	)
ENDMACRO()


# This file is part of SSC,
# a C++ Library supporting Image Guided Therapy Applications.
#
# Copyright (C) 2008- SINTEF Medical Technology
# Copyright (C) 2008- Sonowand AS
#
# SSC is owned by SINTEF Medical Technology and Sonowand AS,
# hereafter named the owners. Each particular piece of code
# is owned by the part that added it to the library.
# SSC source code and binaries can only be used by the owners
# and those with explicit permission from the owners.
# SSC shall not be distributed to anyone else.
#
# SSC is distributed WITHOUT ANY WARRANTY; without even
# the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE.
#
# See sscLicense.txt for more information.

# Find date for latest cmake rebuild
#
# Inspired by http://marc.info/?l=cmake&m=129077863120656&w=2
MACRO (cx_get_today RESULT)
    IF (WIN32)
        EXECUTE_PROCESS(COMMAND "cmd" " /C date /T" OUTPUT_VARIABLE ${RESULT})
	ELSEIF(UNIX)

        EXECUTE_PROCESS(COMMAND "date" "+%d/%m/%Y" OUTPUT_VARIABLE ${RESULT})
        string(REGEX REPLACE "(..)/(..)/(....).*" "\\3-\\2-\\1" ${RESULT} ${${RESULT}})
#        EXECUTE_PROCESS(COMMAND "date" "+%Y-%m-%d" OUTPUT_VARIABLE ${RESULT})
	ELSE (WIN32)
        MESSAGE(SEND_ERROR "date not implemented")
        SET(${RESULT} 000000)
    ENDIF (WIN32)
ENDMACRO ()


# Get a description of the current build from git.
#
# http://stackoverflow.com/questions/1435953/how-can-i-pass-git-sha1-to-compiler-as-definition-using-cmake
# use the entire output of git describe here instead.
#
MACRO (cx_get_git_build_description RESULT)
	find_package(Git REQUIRED)
	exec_program(
		"git"
		${PROJECT_SOURCE_DIR}
		ARGS "describe --tags"
		OUTPUT_VARIABLE ${RESULT}
		)
ENDMACRO ()

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
#
MACRO (cx_define_version major minor patch type)
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
ENDMACRO ()


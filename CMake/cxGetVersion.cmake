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
# Get a bool telling if git is on a tag or not
#
###############################################################################
function(cx_get_git_is_on_tag _varResult)
	find_package(Git REQUIRED)
	execute_process(
		COMMAND ${GIT_EXECUTABLE} describe --exact-match
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
		OUTPUT_VARIABLE GIT_OUTPUT
		RESULT_VARIABLE GIT_RESULT
		)

	if(${GIT_RESULT} EQUAL 0)
		set(RESULT ON)
	else()
		set(RESULT OFF)
	endif()

	set (${_varResult} ${RESULT} PARENT_SCOPE)
endfunction()


###############################################################################
# Get a description of the current build from git.
#
# http://stackoverflow.com/questions/1435953/how-can-i-pass-git-sha1-to-compiler-as-definition-using-cmake
# use the entire output of git describe here instead.
#
###############################################################################
function(cx_get_git_build_description _varResult)
	find_package(Git REQUIRED)
	execute_process(
		COMMAND ${GIT_EXECUTABLE} describe --tags
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
		OUTPUT_VARIABLE RESULT
		)
	# remove first character - should always be a "v" as first in f.ex. "v3.5.3"
	string(SUBSTRING ${RESULT} 1 -1 RESULT)

	STRING(REGEX REPLACE "\r|\n" "" RESULT ${RESULT})
	STRING(REGEX REPLACE " " "" RESULT ${RESULT})
	string(REPLACE "/" "_" RESULT ${RESULT}) # make filename-friendly

	set (${_varResult} ${RESULT} PARENT_SCOPE)
endfunction()


###############################################################################
# Get a description of the current build from git.
#
# http://stackoverflow.com/questions/1435953/how-can-i-pass-git-sha1-to-compiler-as-definition-using-cmake
# use the entire output of git describe here instead.
#
###############################################################################
function(cx_get_git_sha _varResult)
	find_package(Git REQUIRED)
	execute_process(
		COMMAND ${GIT_EXECUTABLE} rev-parse --short=4 HEAD
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
		OUTPUT_VARIABLE RESULT
		)
	STRING(REGEX REPLACE "\r|\n" "" RESULT ${RESULT})
	STRING(REGEX REPLACE " " "" RESULT ${RESULT})
	string(REPLACE "/" "_" RESULT ${RESULT}) # make filename-friendly

	set (${_varResult} ${RESULT} PARENT_SCOPE)
endfunction()

###############################################################################
# Get a description of the current branch from git.
#
###############################################################################
function(cx_get_git_branch _varResult)
	find_package(Git REQUIRED)
	execute_process(
		COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
		OUTPUT_VARIABLE RESULT
		)
	STRING(REGEX REPLACE "\r|\n" "" RESULT ${RESULT})
	STRING(REGEX REPLACE " " "" RESULT ${RESULT})
	string(REPLACE "/" "_" RESULT ${RESULT}) # make filename-friendly

	set (${_varResult} ${RESULT} PARENT_SCOPE)
endfunction()



###############################################################################
#
# Define the variables
#
#	${PROJECT_NAME}_VERSION_MAJOR
#	${PROJECT_NAME}_VERSION_MINOR
#	${PROJECT_NAME}_VERSION_PATCH
#	${PROJECT_NAME}_VERSION_STRING
#
###############################################################################
macro(cx_define_version MAJOR MINOR PATCH)
	set(${PROJECT_NAME}_VERSION_MAJOR  ${MAJOR})
	set(${PROJECT_NAME}_VERSION_MINOR  ${MINOR})
	set(${PROJECT_NAME}_VERSION_PATCH  ${PATCH})

	set(${PROJECT_NAME}_VERSION_STRING ${MAJOR})

	if(NOT ${MINOR} STREQUAL "")
		set(${PROJECT_NAME}_VERSION_STRING ${${PROJECT_NAME}_VERSION_STRING}.${MINOR})
	endif()
	if(NOT ${PATCH} STREQUAL "")
		set(${PROJECT_NAME}_VERSION_STRING ${${PROJECT_NAME}_VERSION_STRING}.${PATCH})
	endif()

endmacro()


###############################################################################
#
# Set the CustusX version based on the current git tag.
#
# git describe --tags is used to get a string interpreted as
#    v<major>.<minor>.<patch>
#
#
###############################################################################
macro(cx_set_version_from_git)
	cx_get_git_build_description(GIT_DESCRIBE)
	# GIT_DESCRIBE is a dot-separated string: Interpret the first two elements
	# as minor, major, and the rest as patch
	string(REPLACE "." ";" GIT_DESCRIBE_LIST ${GIT_DESCRIBE})
	list(GET GIT_DESCRIBE_LIST 0 major)
	list(GET GIT_DESCRIBE_LIST 1 minor)
	list(REMOVE_AT GIT_DESCRIBE_LIST 0 1)
	string(REPLACE ";" "." patch "${GIT_DESCRIBE_LIST}")

	cx_define_version(${major} ${minor} "${patch}")
endmacro()


###############################################################################
#
# Set the CustusX version based on the current date and git SHA.
#
# Format based on rules from http://semver.org/
# The format is YYYY.MM.DD-dev+GIT_BRANCH.GIT_SHA
#
#
###############################################################################
MACRO(cx_set_version_today_alpha)
	cx_get_today(TODAY_DATE)
	cx_get_git_sha(GIT_SHA)
	cx_get_git_branch(GIT_BRANCH)

	string(REPLACE "-" ";" TODAY_DATE_LIST ${TODAY_DATE})
	# truncate branch name - this is encoded into the exename and should be
	# kept within reasonable size (think windows filepath restrictions)
	string(LENGTH ${GIT_BRANCH} TEXT_LENGTH)
	if(${TEXT_LENGTH} GREATER 30)
		string(SUBSTRING ${GIT_BRANCH} 0, 30, GIT_BRANCH)
	endif()

	list(GET TODAY_DATE_LIST 0 YEAR)
	list(GET TODAY_DATE_LIST 1 MONTH)
	list(GET TODAY_DATE_LIST 2 DAY)

	set(major  ${YEAR})
	set(minor  ${MONTH})
	set(patch  ${DAY}-dev+${GIT_BRANCH}.${GIT_SHA})

	cx_define_version(${major} ${minor} ${patch})

ENDMACRO()


###############################################################################
#
# Define the variables
#
#	${PROJECT_NAME}_VERSION_MAJOR
#	${PROJECT_NAME}_VERSION_MINOR
#	${PROJECT_NAME}_VERSION_PATCH
#	${PROJECT_NAME}_VERSION_STRING
#
# Based on the git tag. If not on a git tag, generate development
# values based on date and SHA.
#
#
###############################################################################
MACRO(cx_read_version)
	cx_get_git_is_on_tag(GIT_ON_TAG)

	if(${GIT_ON_TAG})
		cx_set_version_from_git()
		message(STATUS "CustusX Tagged Version: ${CustusX_VERSION_STRING}")
	else()
		cx_set_version_today_alpha()
		message(STATUS "CustusX Alpha Version: ${CustusX_VERSION_STRING}")
	endif()

ENDMACRO()












###############################################################################
# DISABLED
#
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
MACRO(cx_define_version_disabled major minor patch type)
	set(${PROJECT_NAME}_VERSION_MAJOR  ${major})
	set(${PROJECT_NAME}_VERSION_MINOR  ${minor})
	set(${PROJECT_NAME}_VERSION_PATCH  ${patch})

	cx_get_git_is_dirty(GIT_IS_DIRTY)
	message(STATUS "cx_get_git_is_dirty retva: " ${GIT_IS_DIRTY})

	cx_get_git_build_description(GIT_DESCRIBE)
	cx_get_git_sha(GIT_SHA)

	if(${type} STREQUAL ALPHA)
			cx_get_today(TODAY_DATE)
			set(GIT_DESCRIBE "${TODAY_DATE}.${GIT_SHA}")
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

	if(${type} STREQUAL ALPHA)
		set(${PROJECT_NAME}_VERSION_STRING ${GIT_DESCRIBE}${POSTFIX})
	elseif("${MATCHING_STRING}" STREQUAL "")
			message("Warning:
			Version string extracted from git: ${GIT_DESCRIBE}
			Version string generated: ${GENERATED_VERSION_STRING}
			The generated string should be contained in the git string.
			Reverting to generated string, setting dirty tag.")
			set(${PROJECT_NAME}_VERSION_STRING ${GENERATED_VERSION_STRING}.dirty)
	else()
			set(${PROJECT_NAME}_VERSION_STRING ${GIT_DESCRIBE})
	endif()
ENDMACRO()


MACRO(cx_read_version_disabled)
	file(READ ${PROJECT_SOURCE_DIR}/version.ini CX_VERSION_FILE_DATA)

	string(REGEX REPLACE ".*major[ ]*=[ ]*([0-9]+).*" "\\1" VERSION_MAJOR ${CX_VERSION_FILE_DATA})
	string(REGEX REPLACE ".*minor[ ]*=[ ]*([0-9]+).*" "\\1" VERSION_MINOR ${CX_VERSION_FILE_DATA})
	string(REGEX REPLACE ".*patch[ ]*=[ ]*([0-9]+).*" "\\1" VERSION_PATCH ${CX_VERSION_FILE_DATA})
	string(REGEX REPLACE ".*type[ ]*=[ ]*([^$]*).*" "\\1" VERSION_TYPE ${CX_VERSION_FILE_DATA})
	STRING(REGEX REPLACE "(\r?\n)+$" "" VERSION_TYPE "${VERSION_TYPE}")

	cx_define_version(${VERSION_MAJOR} ${VERSION_MINOR} ${VERSION_PATCH} ${VERSION_TYPE})
	message(STATUS "CustusX Version: ${CustusX_VERSION_STRING}")

ENDMACRO()


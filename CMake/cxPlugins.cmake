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
#
# This macro is required by CTK.
# It defines plugin names internal to the project.
#
# Extract all library names starting with org_mydomain_
#
###############################################################################
macro(GetMyTargetLibraries all_target_libraries varname)
  set(re_ctkplugin "^org_custusx_[a-zA-Z0-9_]+$")
  set(_tmp_list)
  list(APPEND _tmp_list ${all_target_libraries})
  ctkMacroListFilter(_tmp_list re_ctkplugin OUTPUT_VARIABLE ${varname})
endmacro()


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
function(cx_append_plugins_in_current_folder_not_already_defined PLUGINS_VARIABLE PLUGIN_BUILD_OPTION_PREFIX)
    SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})
    set(plugins_list ${${PLUGINS_VARIABLE}})

    foreach(plugin_name ${SUBDIRS})
            cx_insert_plugin_if_not_in_list(plugins_list ${plugin_name} "OFF" ${PLUGIN_BUILD_OPTION_PREFIX})
    endforeach()

    set(${PLUGINS_VARIABLE} ${plugins_list} PARENT_SCOPE)
endfunction()


###############################################################################
#
# Find all variables prefixed with CX_EXTERNAL_PLUGIN_,
# then add them as plugins.
# Add paths to external plugins this way, i.e plugins not defined within CustusX.
#
# Usage:
#  When calling cmake from the command line, add
#      -DCX_EXTERNAL_PLUGIN_<plugin_name>=<full/path/to/plugin>
#
# PLUGINS_VARIABLE: return variable containing <pluginname>:<val> pairs, only ADD to this var
#
###############################################################################
function(cx_append_plugins_external_to_core PLUGINS_VARIABLE)
    # define local copy of input
    set(plugins_list ${${PLUGINS_VARIABLE}})

    getListOfVarsStartingWith("CX_EXTERNAL_PLUGIN_" matchedVars)
    foreach (_var IN LISTS matchedVars)
            set(FULL_NAME ${${_var}})
            file(TO_CMAKE_PATH ${FULL_NAME} FULL_NAME)
            file(RELATIVE_PATH REL_NAME ${CMAKE_CURRENT_SOURCE_DIR} ${FULL_NAME})
            string(FIND ${REL_NAME} ".." FOUND_DOTDOT)
            file(TO_CMAKE_PATH ${REL_NAME} REL_NAME) #fixing the paths on windows
                    #message(STATUS "REL_NAME " ${REL_NAME})
                    #message(STATUS "  FULL_NAME " ${FULL_NAME})
                    #message(STATUS "  FOUND_DOTDOT " ${FOUND_DOTDOT})
            if(${FOUND_DOTDOT} EQUAL -1)
                    set(plugins_list ${plugins_list} "${REL_NAME}:ON")
            else()
                    set(plugins_list ${plugins_list} "${FULL_NAME}:ON")
            endif()
    endforeach()

    # copy modified input back as retval
    set(${PLUGINS_VARIABLE} ${plugins_list} PARENT_SCOPE)
endfunction()



###############################################################################
#
# Find all previously defined plugins, add them to CX_PLUGINS
#
# PLUGINS_VARIABLE: return variable containing <pluginname>:<val> pairs, only ADD to this var
# PLUGIN_BUILD_OPTION_PREFIX: prefix used by ctkMacroSetupPlugins to generate build variables
#
###############################################################################
function(cx_append_plugins_already_present PLUGINS_VARIABLE PLUGIN_BUILD_OPTION_PREFIX)
    # define local copy of input
    set(plugins_list ${${PLUGINS_VARIABLE}})

    # Find all previously defined plugins, add them to CX_PLUGINS
    getListOfVarsStartingWith(${PLUGIN_BUILD_OPTION_PREFIX} matchedVars)
    foreach (_var IN LISTS matchedVars)
            string(REPLACE ${PLUGIN_BUILD_OPTION_PREFIX} "" plugin_name ${_var})
# is this a bug? referring CX_PLUGINS directly in here makes it a local var...
			cx_insert_plugin_if_not_in_list(CX_PLUGINS ${plugin_name} "OFF" ${PLUGIN_BUILD_OPTION_PREFIX})
#			cx_insert_plugin_if_not_in_list(plugins_list ${plugin_name} "OFF" ${PLUGIN_BUILD_OPTION_PREFIX})
	endforeach()

    # copy modified input back as retval
    set(${PLUGINS_VARIABLE} ${plugins_list} PARENT_SCOPE)
endfunction()

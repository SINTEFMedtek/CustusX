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


#.rst:
# cxBundleUtilities
# ---------------
#
# Functions to help assemble a standalone bundle application.
#
# Depends upon and is an enhancement of the vanilla BundleUtilities.cmake,
#
#
# The following functions are provided by this module:
#
# ::
#
#    cx_fixup_bundle
#    cx_get_bundle_keys
#
# ::
#
#   CX_FIXUP_BUNDLE(<app> <libs> <dirs>)
#
# Fix up a bundle in-place and make it standalone, such that it can be
# drag-n-drop copied to another machine and run on that machine as long
# as all of the system libraries are compatible.
#
# If you pass plugins to fixup_bundle as the libs parameter, you should
# install them or copy them into the bundle before calling fixup_bundle.
# The "libs" parameter is a list of libraries that must be fixed up, but
# that cannot be determined by otool output analysis.  (i.e., plugins)
#
# Gather all the keys for all the executables and libraries in a bundle,
# and then, for each key, copy each prerequisite into the bundle.  Then
# fix each one up according to its own list of prerequisites.
#
# Then clear all the keys and call verify_app on the final bundle to
# ensure that it is truly standalone.
#
# As an optional parameter (IGNORE_ITEM) a list of file names can be passed,
# which are then ignored (e.g. IGNORE_ITEM "vcredist_x86.exe;vcredist_x64.exe")
#
# ::
#
#   CX_GET_BUNDLE_KEYS(<app> <libs> <dirs> <keys_var>)
#
# Loop over all the executable and library files within the bundle (and
# given as extra <libs>) and accumulate a list of keys representing
# them.  Set values associated with each key such that we can loop over
# all of them and copy prerequisite libs into the bundle and then do
# appropriate install_name_tool fixups.
#
# As an optional parameter (IGNORE_ITEM) a list of file names can be passed,
# which are then ignored (e.g. IGNORE_ITEM "vcredist_x86.exe;vcredist_x64.exe")

#=============================================================================
# Copyright 2008-2009 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================

include(CMakeParseArguments)

# Inject feature into GetPrerequisites: 
#   Tag GStreamer framework as SYSTEM, thus keeping it out of the bundle,
#   and requiring the user to install GStreamer on his Mac.
#
# This is a workaround for not being able to install the whole of GStreamer
# into the bundle - gst plugins dont load properly.
# 
function(gp_resolved_file_type_override resolved_file type)
    if(APPLE)
      if(resolved_file MATCHES "^(/Library/Frameworks/GStreamer.framework/)")
        set(${type} "system" PARENT_SCOPE)
      endif()
    endif()
endfunction()

include(BundleUtilities)

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

#######################################################################
#
# get_new_prerequisites()
#
# Given a top_binary, search recursively for prerequisites in dirs,
# return prereqs in found_binaries.
#
# Ignore binaries that are already present in found_binaries
#
#######################################################################
function(cx_get_new_prerequisites context target exepath dirs found_binaries found_binaries_retval msg_indent rpaths)
    get_item_key("${target}" target_key) # debug
    set(MY_MESSAGE_INDENT ".${msg_indent}")

    gp_resolve_item("${context}" "${target}" "${exepath}" "${dirs}" resolved_target)

    # look for target in found_binaries. 
    # If already present: return
    list (FIND found_binaries ${resolved_target} _index)
    if (${_index} GREATER -1)
#      message(STATUS "    get_new_prerequisites: target found in list, returning... "${target_key})
      return()
    endif()

    set(show_status 0)
    if(show_status)
      message(STATUS ${MY_MESSAGE_INDENT} "get_new_prerequisites: append+investigate: " ${target_key})
      message(STATUS ${MY_MESSAGE_INDENT} "  ${target}")
    endif()

    list(APPEND found_binaries ${resolved_target})

    set(prereqs "")
    get_prerequisites("${resolved_target}" prereqs 1 0 "${exepath}" "${dirs}" "${rpaths}")

    list(LENGTH prereqs prereqs_length)
    if(show_status)
      message(STATUS ${MY_MESSAGE_INDENT} "  Investigating " ${prereqs_length}  " prereqs {")
    endif()

    foreach(pr ${prereqs})
      list (FIND found_binaries ${pr} _index)
      if (${_index} EQUAL -1)
        cx_get_new_prerequisites("${resolved_target}" "${pr}" "${exepath}" "${dirs}" 
                                 "${found_binaries}" found_binaries "${MY_MESSAGE_INDENT}" "${rpaths}")
#        cx_get_new_prerequisites("${context}" "${pr}" "${exepath}" "${dirs}" 
#                                 "${found_binaries}" found_binaries "${MY_MESSAGE_INDENT}" "${rpaths}")
      endif()
    endforeach()

    # Propagate values to caller's scope:
    set(${found_binaries_retval} ${found_binaries} PARENT_SCOPE)

    if(show_status)
      set(found_binaries_length 0)
      list(LENGTH found_binaries found_binaries_length)
      message(STATUS ${MY_MESSAGE_INDENT} "} found binaries so far: " ${found_binaries_length})
      foreach(key ${${found_binaries}})
        message(STATUS ${MY_MESSAGE_INDENT} "  binary: " ${key} )
      endforeach()
    endif()

endfunction()

# Modified version of the vanilla get_bundle_keys,
# - Uses cx_get_new_prerequisites() to find prereqs,
# - Treats all binaries (libs and exes) similarly, thus assuming
#   that the RPATH settings are the same for all. This assumption is 
#   used because all binaries are copied into the same folder, thus
#   possibly overwriting each other, thus should be the same.
#
function(cx_get_bundle_keys app libs dirs keys_var)
  set(${keys_var} PARENT_SCOPE)

  set(options)
  set(oneValueArgs)
  set(multiValueArgs IGNORE_ITEM)
  cmake_parse_arguments(CFG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

  get_bundle_and_executable("${app}" bundle executable valid)
  if(valid)
    # Always use the exepath of the main bundle executable for @executable_path
    # replacements:
    #
    get_filename_component(exepath "${executable}" PATH)

    # But do fixups on all executables in the bundle:
    #
    get_bundle_all_executables("${bundle}" exes)

    # Set keys for main executable first:
    #
    set_bundle_key_values(${keys_var} "${executable}" "${executable}" "${exepath}" "${dirs}" 0)

    # Get rpaths specified by main executable:
    #
    get_item_key("${executable}" executable_key)
    set(main_rpaths "${${executable_key}_RPATHS}")

    # --begin optimize code
    # Loop over all libs and exes, assemble list of 
    # prerequisites in found_binaries
    set(targets ${libs} ${exes})
    set(found_binaries "")
    foreach(target ${targets})
    
      set(ignoreFile FALSE)
      get_filename_component(binary_filename ${target} NAME)
      if(NOT "${CFG_IGNORE_ITEM}" STREQUAL "" )
        foreach(item ${CFG_IGNORE_ITEM})
            if("${item}" STREQUAL "${binary_filename}")
              set(ignoreFile TRUE)
            endif()
        endforeach()
      endif()
      
      
      if(NOT ignoreFile)
        message(STATUS "Examining prerequisites for target: " ${target} )
        cx_get_new_prerequisites("${target}" "${target}" "${exepath}" "${dirs}" 
                                 "${found_binaries}" found_binaries "." "${main_rpaths}")
#      list(LENGTH found_binaries found_binaries_length)
#      message(STATUS "    completed, found preqs: " ${found_binaries_length})
      else()
        message(STATUS "Ignoring file: ${target}")
      endif()
    endforeach()
    
    # Set key values for all found binaries
    foreach(binary ${found_binaries})       
      set(copyflag 1)
      get_filename_component(binary_filename ${binary} NAME)
      list (FIND targets ${binary} _index)
      if (${_index} GREATER -1)
        set(copyflag 0)
        message(STATUS "In bundle: " ${binary_filename} )
      else()
        message(STATUS "Required : " ${binary} )
      endif()

      set(prereqs "")
      set(ignoreFile FALSE)
      if(NOT "${CFG_IGNORE_ITEM}" STREQUAL "" )
        foreach(item ${CFG_IGNORE_ITEM})
            if("${item}" STREQUAL "${binary_filename}")
              set(ignoreFile TRUE)
            endif()
        endforeach()
      endif()

      if(NOT ignoreFile)
        set_bundle_key_values(${keys_var} "${executable}" "${binary}" "${exepath}" "${dirs}" "${copyflag}" "${main_rpaths}")
      else()
        message(STATUS "Ignoring file: ${binary}")
      endif()
    endforeach()
    
    # --end optimize code


    # Propagate values to caller's scope:
    #
    set(${keys_var} ${${keys_var}} PARENT_SCOPE)
    foreach(key ${${keys_var}})
      set(${key}_ITEM "${${key}_ITEM}" PARENT_SCOPE)
      set(${key}_RESOLVED_ITEM "${${key}_RESOLVED_ITEM}" PARENT_SCOPE)
      set(${key}_DEFAULT_EMBEDDED_PATH "${${key}_DEFAULT_EMBEDDED_PATH}" PARENT_SCOPE)
      set(${key}_EMBEDDED_ITEM "${${key}_EMBEDDED_ITEM}" PARENT_SCOPE)
      set(${key}_RESOLVED_EMBEDDED_ITEM "${${key}_RESOLVED_EMBEDDED_ITEM}" PARENT_SCOPE)
      set(${key}_COPYFLAG "${${key}_COPYFLAG}" PARENT_SCOPE)
      set(${key}_RPATHS "${${key}_RPATHS}" PARENT_SCOPE)
      set(${key}_RDEP_RPATHS "${${key}_RDEP_RPATHS}" PARENT_SCOPE)
    endforeach()
  endif()
endfunction()

# Based on the vanilla version (3.6.2),
# EXCEPT: - cx_get_bundle_keys() is called instead of get_bundle_keys()
#         - binaries are chmoded to writable prior to fixup (BU_CHMOD_BUNDLE_ITEMS=ON)
#
function(cx_fixup_bundle app libs dirs)
  message(STATUS "fixup_bundle")
  message(STATUS "  app='${app}'")
  message(STATUS "  libs='${libs}'")
  message(STATUS "  dirs='${dirs}'")

  cx_clean_path("${app}" app)
  cx_clean_path("${libs}" libs)
  cx_clean_path("${dirs}" dirs)

  set(options)
  set(oneValueArgs)
  set(multiValueArgs IGNORE_ITEM)
  cmake_parse_arguments(CFG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

  message(STATUS "  ignoreItems='${CFG_IGNORE_ITEM}'")

  get_bundle_and_executable("${app}" bundle executable valid)
  if(valid)
    get_filename_component(exepath "${executable}" PATH)

    message(STATUS "fixup_bundle: preparing...")
    cx_get_bundle_keys("${app}" "${libs}" "${dirs}" keys IGNORE_ITEM "${CFG_IGNORE_ITEM}")

    message(STATUS "fixup_bundle: copying...")
    list(LENGTH keys n)
    math(EXPR n ${n}*2)

    set(i 0)
    foreach(key ${keys})
      math(EXPR i ${i}+1)
      if(${${key}_COPYFLAG})
        message(STATUS "${i}/${n}: copying '${${key}_RESOLVED_ITEM}'")
      else()
        message(STATUS "${i}/${n}: *NOT* copying '${${key}_RESOLVED_ITEM}'")
      endif()

      set(show_status 0)
      if(show_status)
        message(STATUS "key='${key}'")
        message(STATUS "item='${${key}_ITEM}'")
        message(STATUS "resolved_item='${${key}_RESOLVED_ITEM}'")
        message(STATUS "default_embedded_path='${${key}_DEFAULT_EMBEDDED_PATH}'")
        message(STATUS "embedded_item='${${key}_EMBEDDED_ITEM}'")
        message(STATUS "resolved_embedded_item='${${key}_RESOLVED_EMBEDDED_ITEM}'")
        message(STATUS "copyflag='${${key}_COPYFLAG}'")
        message(STATUS "rpaths='${${key}_RPATHS}'")
        message(STATUS "rdep_rpaths='${${key}_RDEP_RPATHS}'")
        message(STATUS "")
      endif()

      if(${${key}_COPYFLAG})
        set(item "${${key}_ITEM}")
        if(item MATCHES "[^/]+\\.framework/")
          copy_resolved_framework_into_bundle("${${key}_RESOLVED_ITEM}"
            "${${key}_RESOLVED_EMBEDDED_ITEM}")
        else()
          if(UNIX)
            set (BU_CHMOD_BUNDLE_ITEMS ON)
          endif()
          copy_resolved_item_into_bundle("${${key}_RESOLVED_ITEM}"
            "${${key}_RESOLVED_EMBEDDED_ITEM}")
        endif()
      endif()
    endforeach()

    message(STATUS "fixup_bundle: fixing...")
    foreach(key ${keys})
      math(EXPR i ${i}+1)
      if(APPLE)
        message(STATUS "${i}/${n}: fixing up '${${key}_RESOLVED_EMBEDDED_ITEM}'")
        fixup_bundle_item("${${key}_RESOLVED_EMBEDDED_ITEM}" "${exepath}" "${dirs}")
      else()
        message(STATUS "${i}/${n}: fix-up not required on this platform '${${key}_RESOLVED_EMBEDDED_ITEM}'")
      endif()
    endforeach()

    message(STATUS "fixup_bundle: cleaning up...")
    clear_bundle_keys(keys)

    message(STATUS "fixup_bundle: verifying...")
    verify_app("${app}" IGNORE_ITEM "${CFG_IGNORE_ITEM}")
  else()
    message(SEND_ERROR "error: fixup_bundle: not a valid bundle")
  endif()

  message(STATUS "fixup_bundle: done")
endfunction()





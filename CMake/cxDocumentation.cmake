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
# Converts a CMake list to a string containing elements separated by spaces
#
# http://opensource.apple.com/source/curl/curl-83.1.2/curl/CMake/Utilities.cmake
###############################################################################
function(TO_LIST_SPACES _LIST_NAME OUTPUT_VAR)
  set(NEW_LIST_SPACE)
  foreach(ITEM ${${_LIST_NAME}})
    set(NEW_LIST_SPACE "${NEW_LIST_SPACE} ${ITEM}")
  endforeach()
  string(STRIP ${NEW_LIST_SPACE} NEW_LIST_SPACE)
  set(${OUTPUT_VAR} "${NEW_LIST_SPACE}" PARENT_SCOPE)
endfunction()

###############################################################################
#
# Add a string VALUE_TO_APPEND to a global property PROPERTY_NAME.
#
###############################################################################
function(cx_add_string_to_global_property_list PROPERTY_NAME VALUE_TO_APPEND)
    get_property(PROPERTY_VALUE GLOBAL PROPERTY ${PROPERTY_NAME})
    set(PROPERTY_VALUE ${PROPERTY_VALUE} "${VALUE_TO_APPEND}")
    set_property(GLOBAL PROPERTY ${PROPERTY_NAME} ${PROPERTY_VALUE})
endfunction()

###############################################################################
#
# Add a folder containing documentation files.
#
#
###############################################################################
function(cx_doc_define_user_docs PLUGIN_DOC_FOLDER)
	cx_add_string_to_global_property_list(CX_PLUGIN_DOC_FOLDERS "${PLUGIN_DOC_FOLDER}")
endfunction()

###############################################################################
#
# Define documentation for a plugin.
#
# Call this once for each plugin that contains a documentation folder,
# typically in ${CMAKE_CURRENT_SOURCE_DIR}/doc.
#
###############################################################################
function(cx_doc_define_plugin_user_docs PLUGIN_NAME PLUGIN_DOC_FOLDER)
    cx_add_string_to_global_property_list(CX_PLUGIN_DOC_FOLDERS "${PLUGIN_DOC_FOLDER}")
    cx_add_string_to_global_property_list(CX_PLUGIN_DOC_NAMES "${PLUGIN_NAME}")
endfunction()

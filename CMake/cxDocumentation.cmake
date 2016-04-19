# =========================================================================
# This file is part of CustusX, an Image Guided Therapy Application.
#
# Copyright (c) 2008-2014, SINTEF Department of Medical Technology
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
	get_filename_component(PLUGIN_DOC_FOLDER_ABSOLUTE "${PLUGIN_DOC_FOLDER}" ABSOLUTE) # qhelp files need to strip ../
	cx_add_string_to_global_property_list(CX_PLUGIN_DOC_FOLDERS "${PLUGIN_DOC_FOLDER_ABSOLUTE}")
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

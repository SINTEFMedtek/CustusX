set(PLUGIN_BUILD_OPTION_PREFIX CX_PLUGIN_)

#> Extract all library names starting with org_mydomain_
macro(GetMyTargetLibraries all_target_libraries varname)
  set(re_ctkplugin "^org_custusx_[a-zA-Z0-9_]+$")
  set(_tmp_list)
  list(APPEND _tmp_list ${all_target_libraries})
  ctkMacroListFilter(_tmp_list re_ctkplugin OUTPUT_VARIABLE ${varname})
endmacro()
#>


cmake_policy(SET CMP0012 NEW)

# Find all variables prefixed with CX_EXTERNAL_PLUGIN_,
# then add them as plugins.
# Usage: add paths to external plugin this way.
getListOfVarsStartingWith("CX_EXTERNAL_PLUGIN_" matchedVars)
foreach (_var IN LISTS matchedVars)
	set(FULL_NAME ${${_var}})
	file(RELATIVE_PATH REL_NAME ${CMAKE_CURRENT_SOURCE_DIR} ${FULL_NAME})
	string(FIND ${REL_NAME} ".." FOUND_DOTDOT)
#	message(STATUS "REL_NAME " ${REL_NAME})
#	message(STATUS "  FULL_NAME " ${FULL_NAME})
#	message(STATUS "  FOUND_DOTDOT " ${FOUND_DOTDOT})
	if(${FOUND_DOTDOT} EQUAL -1)
        set(CX_PLUGINS ${CX_PLUGINS} "${REL_NAME}:ON")
	else()
        set(CX_PLUGINS ${CX_PLUGINS} "${FULL_NAME}:ON")
	endif()
endforeach()

#foreach(var ${CX_PLUGINS})
#  message("==plugin: " ${var})
#endforeach()

getListOfVarsStartingWith(${PLUGIN_BUILD_OPTION_PREFIX} matchedVars)

# Create a list of available plug-ins and make them known to the build system
set(CX_PLUGINS
  ${CX_PLUGINS}
  org.custusx.dicom:ON
#  org.custusx.example:OFF
  org.custusx.vnnclreconstruction:ON
  org.custusx.pnnreconstruction:ON
  org.custusx.patientmodel:ON
#  org.custusx.filter.tubesegmentation:OFF
  org.custusx.ctusreg:OFF
)

#foreach(var ${CX_PLUGINS})
#  message("==plugin: " ${var})
#endforeach()

# Parse current folder, assume each subfolder is a plugin, add those not already present.
cx_insert_undefined_plugins(CX_PLUGINS ${PLUGIN_BUILD_OPTION_PREFIX})

# Find all previously defined plugins, add them to CX_PLUGINS
getListOfVarsStartingWith(${PLUGIN_BUILD_OPTION_PREFIX} matchedVars)
foreach (_var IN LISTS matchedVars)
    string(REPLACE ${PLUGIN_BUILD_OPTION_PREFIX} "" plugin_name ${_var})         
    cx_insert_plugin_if_not_in_list(CX_PLUGINS ${plugin_name} "OFF" ${PLUGIN_BUILD_OPTION_PREFIX})
endforeach()

ctkMacroSetupPlugins(
    ${CX_PLUGINS} 
    BUILD_OPTION_PREFIX ${PLUGIN_BUILD_OPTION_PREFIX})

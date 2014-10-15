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
    file(TO_CMAKE_PATH ${REL_NAME} REL_NAME) #fixing the paths on windows
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
  org.custusx.core.patientmodel:ON
  org.custusx.core.visualization:ON
  org.custusx.core.tracking:ON
  org.custusx.core.video:ON
  org.custusx.dicom:ON
#  org.custusx.example:OFF
  org.custusx.vnnclreconstruction:ON
  org.custusx.pnnreconstruction:ON
#  org.custusx.filter.tubesegmentation:OFF
  org.custusx.registration:ON
  org.custusx.registration.gui:ON
  org.custusx.registration.method.manual:ON
  org.custusx.registration.method.modelToUS:OFF
  org.custusx.registration.method.landmark:ON
  org.custusx.registration.method.bronchoscopy:ON
  org.custusx.registration.method.vessel:ON
  org.custusx.registration.method.commandline:ON
  org.custusx.registration.method.plate:ON
  org.custusx.help:ON
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

######
# CTK requires QT_QMAKE_EXECUTABLE to be set on Windows.
######
if(NOT QT_QMAKE_EXECUTABLE)
    get_target_property(QT_QMAKE_EXECUTABLE Qt5::qmake LOCATION)
endif(NOT QT_QMAKE_EXECUTABLE)
######
cx_remove_duplicate_include_directories()
ctkMacroSetupPlugins(
    ${CX_PLUGINS} 
    BUILD_OPTION_PREFIX ${PLUGIN_BUILD_OPTION_PREFIX})

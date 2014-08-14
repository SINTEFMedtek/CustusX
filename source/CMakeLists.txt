###########################################################
##               CustusX source
###########################################################

#------------------------------------------------------------------------------
# External libraries
#-----------------------------------------------------------------------------

cx_initialize_Boost()
cx_initialize_QT()
cx_initialize_Eigen()
cx_initialize_VTK()
cx_initialize_ITK()

cx_initialize_OpenIGTLink()
cx_initialize_CTK()

cx_initialize_ISB_GE()
cx_initialize_TSF()
cx_initialize_level_set_segmentation()
cx_initialize_OpenCLUtilityLibrary()

find_package(OpenGL)
cx_opengl_version()
find_package(X11)

#------------------------------------------------------------------------------
# Add subdirectories
#------------------------------------------------------------------------------

add_subdirectory(ThirdParty)

add_subdirectory(resource)
add_subdirectory(service)
add_subdirectory(logic)
add_subdirectory(modules)
add_subdirectory(gui)
#add_subdirectory(plugins)

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
	#message("${_var} = ${${_var}}")
    set(CX_PLUGINS ${CX_PLUGINS} "${${_var}}:ON")
	#message("${_var}=${${_var}} :: ${${${_var}}}")
endforeach()

#set (CX_EXTERNAL_PLUGINS "" CACHE INTERNAL "list of plugins originating outside of custusx. Add to list in order to build.")

#> Create a list of available plug-ins and make them known to the build system
set(CX_PLUGINS
  ${CX_PLUGINS}
  plugins/org.custusx.dicom:ON
  plugins/org.custusx.example:OFF
  plugins/org.custusx.vnnclreconstruction:ON
  plugins/org.custusx.pnnreconstruction:ON
  plugins/org.custusx.patientmodel:ON
  plugins/org.custusx.filter.tubesegmentation:OFF
  #plugins/org.custusx.ussimulator:ON
  #plugins/org.custusx.gestreamer:ON
)
 
set(PLUGIN_BUILD_OPTION_PREFIX CX_BUILD_PLUGIN_)
#message(STATUS "CX_PLUGINS PRE: " ${CX_PLUGINS})
cx_insert_undefined_plugins(CX_PLUGINS "plugins" ${PLUGIN_BUILD_OPTION_PREFIX})
#message(STATUS "CX_PLUGINS POST: " ${CX_PLUGINS})

#foreach(var ${CX_PLUGINS})
#  message("==plugin: " ${var})
#endforeach(var ${CX_PLUGINS})

ctkMacroSetupPlugins(
    ${CX_PLUGINS} 
    BUILD_OPTION_PREFIX ${PLUGIN_BUILD_OPTION_PREFIX})

# Testing for CustusX
add_subdirectory(testing)

# Applications: add last (create bundle last).
add_subdirectory(apps)



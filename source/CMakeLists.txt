###########################################################
##               CustusX source
###########################################################

#------------------------------------------------------------------------------
# External libraries
#-----------------------------------------------------------------------------

cx_initialize_Boost()
cx_initialize_QT()
cx_initialize_VTK()
cx_initialize_ITK()
cx_initialize_OpenIGTLink()
cx_initialize_IGSTK()
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
#> Create a list of available plug-ins and make them known to the build system
set(CX_PLUGINS
  plugins/org.custusx.dicom:ON
  plugins/org.custusx.example:ON
  plugins/org.custusx.vnnclreconstruction:ON
  plugins/org.custusx.pnnreconstruction:ON
  plugins/org.custusx.patientmodel:ON
  plugins/org.custusx.ussimulator:ON
)

ctkMacroSetupPlugins(${CX_PLUGINS})

# Testing for CustusX
add_subdirectory(testing)

# Applications: add last (create bundle last).
add_subdirectory(apps)



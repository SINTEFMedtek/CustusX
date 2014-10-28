set(CX_PLUGIN_BUILD_OPTION_PREFIX CX_PLUGIN_)

include(cxPlugins)

cmake_policy(SET CMP0012 NEW)

# Create a list of available plug-ins and make them known to the build system
set(CX_PLUGINS
  ${CX_PLUGINS}
  org.custusx.core.patientmodel:ON
  org.custusx.core.visualization:ON
  org.custusx.core.tracking:ON
  org.custusx.core.video:ON
#  org.custusx.dicom:OFF
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
#  org.custusx.calibration:ON
  org.custusx.help:ON
)

# DICOM/CTK currently not linking on mac (dcmtk libs not found)
if (NOT CX_APPLE)
	set(CX_PLUGINS
	  ${CX_PLUGINS}
	  org.custusx.dicom:ON
)
endif()

# Add plugins defined externally, i.e defined outside CustusX core.
# Why: Enables enabling of external plugins,
#      possibly outside current folder.
cx_append_plugins_external_to_core(CX_PLUGINS)

# Parse current folder, assume each subfolder is a plugin, add those not already present.
# Why: This ensures plugins added to current folder but not enabled in cmake is available
#      as a cmake option.
cx_append_plugins_in_current_folder_not_already_defined(CX_PLUGINS ${CX_PLUGIN_BUILD_OPTION_PREFIX})

# Find all previously defined plugins, add those not already present.
# Why: This keeps knowledge of external plugins defined
#      on the cmake command line between runs
cx_append_plugins_already_present(CX_PLUGINS ${CX_PLUGIN_BUILD_OPTION_PREFIX})

#foreach(var ${CX_PLUGINS})
#  message("==plugin: " ${var})
#endforeach()

# CTK requires QT_QMAKE_EXECUTABLE to be set on Windows.
if(NOT QT_QMAKE_EXECUTABLE)
    get_target_property(QT_QMAKE_EXECUTABLE Qt5::qmake LOCATION)
endif(NOT QT_QMAKE_EXECUTABLE)

cx_remove_duplicate_include_directories()

ctkMacroSetupPlugins(
    ${CX_PLUGINS} 
	BUILD_OPTION_PREFIX ${CX_PLUGIN_BUILD_OPTION_PREFIX})

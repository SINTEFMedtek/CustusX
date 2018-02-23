set(CX_PLUGIN_BUILD_OPTION_PREFIX CX_PLUGIN_)

include(cxPlugins)

cmake_policy(SET CMP0012 NEW)

# Create a list of available plug-ins and make them known to the build system
set(CX_PLUGINS
  ${CX_PLUGINS}
  org.custusx.core.patientmodel:ON
  org.custusx.core.view:ON
  org.custusx.core.tracking:ON
  org.custusx.core.tracking.system.igstk:ON
  org.custusx.core.openigtlink3:ON
  org.custusx.core.video:ON
  org.custusx.core.state:ON
  org.custusx.dicom:ON
  org.custusx.usreconstruction.vnncl:ON
  org.custusx.usreconstruction.pnn:ON
  org.custusx.registration:ON
  org.custusx.registration.gui:ON
  org.custusx.registration.method.manual:ON
  org.custusx.registration.method.landmark:ON
  org.custusx.registration.method.pointcloud:ON
  org.custusx.registration.method.bronchoscopy:ON
  org.custusx.registration.method.centerline:ON
  org.custusx.filter.routetotarget:ON
  org.custusx.registration.method.vessel:ON
  org.custusx.registration.method.commandline:ON
  org.custusx.registration.method.plate:ON
  org.custusx.usreconstruction:ON
  org.custusx.acquisition:ON
  org.custusx.calibration:ON
  org.custusx.help:ON
  org.custusx.training:ON
  org.custusx.filter.accusurf:ON
  org.custusx.virtualbronchoscopy:ON
)

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

ctkMacroSetupPlugins(
    ${CX_PLUGINS}
    BUILD_OPTION_PREFIX ${CX_PLUGIN_BUILD_OPTION_PREFIX})

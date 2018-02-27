Customize Applications {#customize_applications}
===========================================================

Overviews {#customize_applications_overview}
===========================================================

In general there are two ways to customize CustusX to suit your own needs: 
Creating plugins and creating a new application.

Creating a new plugin
--------------------------------------
The \ref architecture_plugin_framework is the tool to use for developers 
wanting to extend the system with their own features. By creating a plugin 
that implements some of the extension point services, the feature will be 
automatically added to the build system.

A new plugin in the `CX/source/plugins` folder will be automatically detected and
added to the build system.

The Extension Points are used to insert features into the existing system. The 
simplest and most generic is the cx::GUIExtenderService, which enables you to 
add widgets to the cx::MainWindow.

### The example plugin

In the `CX/CX/source/plugins/org.custusx.example` folder, you can see the example
plugin. This code is a good starting point for a plugin. To build it you must add
it to the list of plugins in the file `CX/CX/source/plugins/CMakeLists.txt`:

    org.custusx.example:ON

An entry in this list is only needed for the internal plugins in *CustusX*.
You also need to delete the file:

    CX/build_Release/CMakeCache.txt

to make sure that CMake is reconfigured so that the plugin will be built.
Then rerun the build script. Now you should see an *Example* widget in the
*Window - Widgets* list, and you can see that the plugin is running in the
*Window - Widgets - Plugin Framework*.

To create your own plugin, you can simply copy the example folder and replace
all instances of *example* with a name of your own. To remove the example plugin
from your build, you must remove the entry in the CMakeLists file, delete the CMakeCache
file and delete the built lib in the build folder.

### Adding your plugin as a component

When writing a plugin you might want to keep it in a repository of your own.
To download your repo and build your plugin as a part of the build script,
add entries in:

    CX/CX/install/cx/build/cxComponents.py
    CX/CX/install/cx/build/cxComponentAssembly.py

Creating a new application
--------------------------------------
The cx::MainWindow is the standard frontend for CustusX. However, it is possible 
create another frontend and rather use CustusX as a toolkit.

New applications can be added by creating a new application under source/apps. 

Start out by copying the code in source/apps/CustusX and adapt it to your own 
needs. The system assumes that cx::LogicManager is initialized/shutdown normally. 
Override the GUI by inserting a cx::ApplicationComponent into the 
cx::LogicManager, as the CustusX application does.

The default application for the install can also be changed, by setting the two 
cmake variables

    CX_SYSTEM_BASE_NAME
    CX_SYSTEM_DEFAULT_APPLICATION

Alternatively, set the variable 

    cxInstallData.py/Common/system_base_name

in the superbuild script.

Things to remember
--------------------------------------

* Adding more libraries: If a new library is needed, remember that is
must be available under the LGPL, BSD or similar license, and must run
on all platforms. It should also be easy to install on all platforms,
or be Cmake-compatible.
* Use the \ref code_style when adding your own code.

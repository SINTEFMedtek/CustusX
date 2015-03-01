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

A new plugin in the source/plugins folder will be automatically detected and 
added to the build system. You might have to enable the new option in cmake:

    CX_ORG_CUSTUSX_SOME_PLUGIN_NAME

Look at an existing plugin and read the documentation on OSGii before starting.

The Extension Points are used to insert features into the existing system. The 
simplest and most generic is the cx::GUIExtenderService, which enables you to 
add widgets to the cx::MainWindow.


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
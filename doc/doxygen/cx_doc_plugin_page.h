// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

namespace cx {

/**
 * \defgroup cx_modules Plugins
 * \ingroup cx_base
 *
 * \brief Plugins overview
 *
 * A plugin is a module that can be inserted into the CustusX application in
 * order to provide a piece of functionality or a collection of such. This is
 * the way to go for researchers who wants to insert their own code in a
 * minimally-invasive way. Check out the existing plugins for examples when
 * creating your own.
 *
 * Examples: A registration technique, a segmentation toolbox, US reconstruction.
 *
 * A plugin consists of three parts:
 *  - \b Algorithm: Dedicates algorithms used here. This code belongs to the
 *    Logic layer.
 *  - \b Logic: Control logic that executes whatever the plugin does, connections
 *    to services and other plugins. This code belongs to the Logic layer.
 *  - \b GUI: Widgets dedicated to controlling/visualizing this plugin. This code
 *    belongs to the GUI layer.
 *
 * Each of these parts should have their own folder inside the folder for that
 * plugin. Not all parts are needed to make a full plugin.
 *
 * A plugin has access to all services in the system, and may also depend on
 * other plugins. This should be clearly stated in the description. All plugins
 * must inherit from cx::PluginBase. Add dependencies to other plugins in the
 * constructor, and then add the object to cx::MainWindow in the main() function.
 *
 * <TBD: more tutorial-stuff on creating plugins>
 *
 * \image html cxArchitecture_Plugin_Examples.png "The Registration Plugin"
 *
 */

} // namespace ssc


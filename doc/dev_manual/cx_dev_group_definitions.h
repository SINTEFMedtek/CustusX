/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


namespace cx
{

/**\namespace cx
 *
 * \brief Namespace for all CustusX production code.
 */

/**
 * \defgroup cx_base CustusX System
 *
 * \brief An IGT library.
 */

/**
 * \defgroup cx_gui GUI Layer
 * \ingroup cx_base
 *
 * \brief The GUI layer is all the widgets that is available to the user.
 *
 * The GUI layer is all the widgets that is available to the user. The central
 * idea is the QMainWindow+DockWidgets designs. The default MainWindow can be
 * replaced by any other GUI if that is better for your particular needs.
 *
 *
 * The GUI is centered around the QMainWindow.
 * It has the following main parts:
 *
 * - Layout: The views fill the central part of the window, and the view
 *   composition is called the Layout (ex: 3D ACS x1). This part is
 *   controlled by Visualization 5.2.
 * - Menu: A Main menu with access to all common functionality.
 * - Toolbars: Shortcuts to menu items.
 * - DockWidgets: All other GUI are available as dockwidgets. You can create
 *   new widgets at will, and easily publish them to the QMainWindow as a
 *   dockwidget. Alternatively, create them in your plugin and publish them
 *   in the cx::PluginBase interface.
 *
 * The main idea here is that QMainWindow can store its configuration. Each
 * workflow state has a default configuration, and the user can override it.
 * Thus, each user can create her own version of CustusX with her favourite
 * widgets visible. MainWindow is created by the main() function, and this is
 * not a singleton or similar.
 *
 *
 */

/**
 * \defgroup cx_logic Logic Layer
 * \ingroup cx_base
 *
 * \brief Application lifecycle and plugin management.
 *
 * \ref cx::LogicManager is the main class.
 *
 */

/**
 * \defgroup cx_plugins Plugins
 * \ingroup cx_base
 *
 * OSGii/CTK style plugins.
 *
 */



} // namespace ssc


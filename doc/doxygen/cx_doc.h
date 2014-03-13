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

namespace cx
{

/**\namespace cx
 *
 * \brief Namespace for all CustusX production code.
 */

/**
 * \defgroup cx_base CustusX System
 *
 * \brief An IGT library used by Sintef Medical Technology.
 */

/**
 * \defgroup cx_gui GUI Layer
 * \ingroup cx_base
 *
 * \brief The GUI layer is all the widgets that is available to the user.
 *
 * The GUI layer is all the widgets that is available to the user. The central
 * idea is the QMainWindow+DockWidgets designs, as described in 4.3. The
 * default MainWindow can be replaced by any other GUI if that is better for
 * your particular needs.
 *
 * Other utilities:
 *  - Core widgets: Widgets that enable core functionality.
 *  - Utility widgets: Widgets usable as building blocks elsewhere.
 *  - DataAdapters: Adapters that provide a common interface to getting/setting
 *    of backend data, such as strings, scalars and integers.
 *
 *  \ref cx_plugins also provide widgets for their particular functionality.
 *
 *
 *
 *	The GUI is centered around the QMainWindow.
 *	It has the following main parts:
 *
 *	 - Layout: The views fill the central part of the window, and the view
 *	   composition is called the Layout (ex: 3D ACS x1). This part is
 *	   controlled by Visualization 5.2.
 *	 - Menu: A Main menu with access to all common functionality.
 *	 - Toolbars: Shortcuts to menu items.
 *	 - DockWidgets: All other GUI are available as dockwidgets. You can create
 *	   new widgets at will, and easily publish them to the QMainWindow as a
 *	   dockwidget. Alternatively, create them in your plugin and publish them
 *	   in the cx::PluginBase interface.
 *
 *	The main idea here is that QMainWindow can store its configuration. Each
 *	workflow state has a default configuration, and the user can override it.
 *	Thus, each user can create her own version of CustusX with her favourite
 *	widgets visible. MainWindow is created by the main() function, and this is
 *	not a singleton or similar.
 *
 *
 */

/**
 * \defgroup cx_logic Logic Layer
 * \ingroup cx_base
 *
 * \brief The logic layer add blocks of functionality on top of the services.
 *
 * A few core classes exist to provide initialization of the services
 * and connections between them. The rest of the functionality is
 * provided as \ref cx_plugins .
 *
 * \ref cx::LogicManager is the main class.
 *
 */

/** \defgroup cxNotUsed Not used
 *  \brief All classes that seem to be unused in real code.
 *
 * Everything in this group are candidates for removal.
 *
 */


} // namespace ssc


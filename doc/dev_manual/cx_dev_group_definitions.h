/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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


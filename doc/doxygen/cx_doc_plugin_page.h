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


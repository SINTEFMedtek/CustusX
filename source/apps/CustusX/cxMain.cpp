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
#include <QApplication>
#include <iostream>
#include "cxMainWindow.h"
#include "cxReporter.h"

#include "cxUsReconstructionPlugin.h"
#include "cxAcquisitionPlugin.h"
#include "cxCalibrationPlugin.h"
#include "cxAlgorithmPlugin.h"
#include "cxRegistrationPlugin.h"

#include "cxTypeConversions.h"
#include "cxLogicManager.h"
#include "cxApplication.h"
#include "cxPluginFramework.h"

#ifdef WIN32
int __stdcall WinMain(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
  Q_INIT_RESOURCE(cxResources);
  
  cx::Application app(argc, argv);
  app.setOrganizationName("SINTEF");
  app.setOrganizationDomain("www.sintef.no");
  app.setApplicationName("CustusX");
  app.setWindowIcon(QIcon(":/icons/CustusX.png"));
  app.setAttribute(Qt::AA_DontShowIconsInMenus, false);

  cx::LogicManager::initialize();

	std::vector<cx::GUIExtenderServicePtr> plugins;

	cx::UsReconstructionPluginPtr reconstructionPlugin(new cx::UsReconstructionPlugin());
	plugins.push_back(reconstructionPlugin);
	cx::AcquisitionPluginPtr acquisitionPlugin(new cx::AcquisitionPlugin(reconstructionPlugin->getReconstructer()));
	plugins.push_back(   acquisitionPlugin);
	cx::CalibrationPluginPtr calibrationPlugin(new cx::CalibrationPlugin(acquisitionPlugin->getAcquisitionData()));
	plugins.push_back(   calibrationPlugin);
	cx::AlgorithmPluginPtr algorithmPlugin(new cx::AlgorithmPlugin());
	plugins.push_back(   algorithmPlugin);
	cx::RegistrationPluginPtr registrationPlugin(new cx::RegistrationPlugin(acquisitionPlugin->getAcquisitionData(),
																			cx::LogicManager::getInstance()->getPluginContext()));
	plugins.push_back(   registrationPlugin);


	cx::MainWindow* mainWin = new cx::MainWindow(plugins);

#ifdef __APPLE__ // needed on mac for bringing to front: does the opposite on linux
  mainWin->activateWindow();
#endif
  mainWin->raise();

  int retVal = app.exec();

  plugins.clear();
  delete mainWin;
  cx::LogicManager::shutdown(); // shutdown all global resources, _after_ gui is deleted.
  return retVal;
  
}

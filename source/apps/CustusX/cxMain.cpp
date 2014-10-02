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


#if !defined(WIN32)
#include <langinfo.h>
#include <locale>
#include "cxReporter.h"

void force_C_locale_decimalseparator()
{
	QString radixChar = nl_langinfo(RADIXCHAR);
	QString C_radixChar = ".";

	if (radixChar != C_radixChar)
	{
		setlocale(LC_NUMERIC,"C");

		cx::reportWarning(QString("Detected non-standard decimal separator [%1], changing to standard [%1].")
				.arg(radixChar)
				.arg(C_radixChar));

		QString number = QString("%1").arg(0.5);
//		QString setRadixChar = nl_langinfo(RADIXCHAR);
//		if (setRadixChar!=C_radicChar)
		if (!number.contains(C_radixChar))
			cx::reportError(QString("Failed to set decimal separator."));
	}

//	std::cout << "locale: " << QLocale::system().nativeLanguageName() << std::endl;
//	std::cout << "locale radix: " << QLocale::system().decimalPoint() << std::endl;
//	std::cout << "RADIXCHAR " << std::string(nl_langinfo(RADIXCHAR)) << std::endl;
//	std::cout << "___________ " << std::endl;
//	std::cout << "c locale: " << QLocale::c().nativeLanguageName() << std::endl;
//	std::cout << "clocale radix: " << QLocale::c().decimalPoint() << std::endl;
//	std::cout << "formatted double: " << 2.5 << std::endl;

//	std::cout << std::endl << "change locale: " << std::endl;
//	//  setlocale(LC_NUMERIC,"C");
//	//  QLocale::setDefault(QLocale::c());

//	std::cout << "locale: " << QLocale::system().nativeLanguageName() << std::endl;
//	std::cout << "locale radix: " << QLocale::system().decimalPoint() << std::endl;
//	std::cout << "RADIXCHAR " << std::string(nl_langinfo(RADIXCHAR)) << std::endl;
//	std::cout << "formatted double: " << 2.5 << std::endl;

}
#endif


#ifdef WIN32
int WinMain(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{

#if !defined(WIN32)
  //for some reason this does not work with dynamic linking on windows
  //instead we solve the problem by adding a handmade header for the cxResources.qrc file
  Q_INIT_RESOURCE(cxResources);
#endif
  
  cx::Application app(argc, argv);
  app.setOrganizationName("SINTEF");
  app.setOrganizationDomain("www.sintef.no");
  app.setApplicationName("CustusX");
  app.setWindowIcon(QIcon(":/icons/CustusX.png"));
  app.setAttribute(Qt::AA_DontShowIconsInMenus, false);

#if !defined(WIN32)
  force_C_locale_decimalseparator();
#endif

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

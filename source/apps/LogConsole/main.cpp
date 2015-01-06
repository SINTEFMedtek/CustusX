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

#include "cxTypeConversions.h"
#include "cxLogicManager.h"
#include "cxApplication.h"
#include "cxPluginFramework.h"
#include "cxDataLocations.h"
#include "cxLogFileWatcher.h"

#include "cxConsoleWidgetCollection.h"


int main(int argc, char *argv[])
{

#if !defined(WIN32)
	//for some reason this does not work with dynamic linking on windows
	//instead we solve the problem by adding a handmade header for the cxResources.qrc file
	Q_INIT_RESOURCE(cxResources);
#endif

	cx::Application app(argc, argv);
	app.setOrganizationName("SINTEF");
	app.setOrganizationDomain("www.sintef.no");
	app.setApplicationName("LogConsole");
	app.setWindowIcon(QIcon(":/icons/CustusX.png"));
	app.setAttribute(Qt::AA_DontShowIconsInMenus, false);


	QString optionsFile = cx::DataLocations::getSettingsPath()+"/logconsole.xml";
	cx::XmlOptionFile options = cx::XmlOptionFile(optionsFile);
//	std::cout << "Options file: " << optionsFile << std::endl;
	CX_LOG_INFO() << "Options file: " << optionsFile;
	cx::LogPtr log = cx::LogFileWatcher::create();
	cx::ConsoleWidgetCollection*  mainWin = new cx::ConsoleWidgetCollection(NULL, "ConsoleWidgets", "Log Console", options, log);
	mainWin->show();

#ifdef __APPLE__ // needed on mac for bringing to front: does the opposite on linux
	mainWin->activateWindow();
#endif
	mainWin->raise();

	int retVal = app.exec();

	delete mainWin;
	options.save();

	return retVal;
}

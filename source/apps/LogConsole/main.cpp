/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include <QApplication>

#include "cxApplication.h"
#include "cxLogConsole.h"

int main(int argc, char *argv[])
{

#if !defined(WIN32)
	//for some reason this does not work with dynamic linking on windows
	//instead we solve the problem by adding a handmade header for the cxResources.qrc file
	Q_INIT_RESOURCE(cxResources);
#endif

	cx::Application app(argc, argv);
	app.setOrganizationName("CustusX");
	app.setOrganizationDomain("www.custusx.org");
	app.setApplicationName("LogConsole");
	app.setWindowIcon(QIcon(":/icons/CustusX/CustusX.png"));
	app.setAttribute(Qt::AA_DontShowIconsInMenus, false);

	cx::LogConsole console;
	cx::bringWindowToFront(&console);

	return app.exec();
}

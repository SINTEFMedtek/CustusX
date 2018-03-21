/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include <string>
#include <QApplication>
#include "cxMemoryTester.h"

/** Test app for SSC
 */
int main(int argc, char **argv)
{
    //Q_INIT_RESOURCE(resource); // seems to be uneccesary... need if rc in a lib.

	QApplication app(argc, argv);
	app.setAttribute(Qt::AA_DontShowIconsInMenus, false);
//  app.setWindowIcon(QIcon(":/images/exec.png"));
  app.setWindowIcon(QIcon(":/images/application.png"));

	cx::MemoryTester mainWindow;
	mainWindow.show();
  mainWindow.activateWindow();
  mainWindow.raise();

	int val = app.exec();
	return val;
}

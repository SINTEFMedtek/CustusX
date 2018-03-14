/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include <iostream>
#include "cxMainWindow.h"
#include "cxMainWindowApplicationComponent.h"
#include "cxLogicManager.h"
#include "cxApplication.h"
#include "cxDataLocations.h"
#include "cxConfig.h"

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
  app.setApplicationName(CX_SYSTEM_BASE_NAME);
  app.setWindowIcon(QIcon(":/icons/CustusX/CustusX.png"));
  app.setAttribute(Qt::AA_DontShowIconsInMenus, false);

  cx::DataLocations::setWebsiteURL("http://www.custusx.org");
  cx::ApplicationComponentPtr mainwindow(new cx::MainWindowApplicationComponent<cx::MainWindow>());
  cx::LogicManager::initialize(mainwindow);

  int retVal = app.exec();

  cx::LogicManager::shutdown(); // shutdown all global resources, _after_ gui is deleted.

  return retVal;
}

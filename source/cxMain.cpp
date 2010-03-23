/**
 * cxMain.cpp
 *
 * \brief
 *
 * \date Jan 20, 2009
 * \author: jbake
 */

#include <QApplication>
#include <iostream>
#include "cxMainWindow.h"

int main(int argc, char *argv[])
{
  Q_INIT_RESOURCE(cxResources);
  
  QApplication app(argc, argv);
  app.setOrganizationName("SINTEF");
  app.setOrganizationDomain("www.sintef.no");
  app.setApplicationName("CustusX3");
  cx::MainWindow mainWin;
  mainWin.show();
  mainWin.activateWindow();
  mainWin.raise();
  int retVal = app.exec();
  return retVal;
  
}

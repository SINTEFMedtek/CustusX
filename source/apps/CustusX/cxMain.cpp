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
#include "sscMessageManager.h"

#include "cxUsReconstructionPlugin.h"
#include "cxAcquisitionPlugin.h"
#include "cxCalibrationPlugin.h"
#include "cxAlgorithmPlugin.h"
#include "cxRegistrationPlugin.h"

int main(int argc, char *argv[])
{
  Q_INIT_RESOURCE(cxResources);
  
  QApplication app(argc, argv);
  app.setOrganizationName("SINTEF");
  app.setOrganizationDomain("www.sintef.no");
  app.setApplicationName("CustusX3");
  app.setWindowIcon(QIcon(":/icons/CustusX.png"));
//  app.setWindowIcon(QIcon(":/icons/.png"));
  app.setAttribute(Qt::AA_DontShowIconsInMenus, false);

  cx::MainWindow::initialize();

	std::vector<cx::PluginBasePtr> plugins;

	cx::UsReconstructionPluginPtr reconstructionPlugin(new cx::UsReconstructionPlugin());
	plugins.push_back(reconstructionPlugin);
	cx::AcquisitionPluginPtr acquisitionPlugin(new cx::AcquisitionPlugin(reconstructionPlugin->getReconstructer()));
	plugins.push_back(   acquisitionPlugin);
	cx::CalibrationPluginPtr calibrationPlugin(new cx::CalibrationPlugin(acquisitionPlugin->getAcquisitionData()));
	plugins.push_back(   calibrationPlugin);
	cx::AlgorithmPluginPtr algorithmPlugin(new cx::AlgorithmPlugin());
	plugins.push_back(   algorithmPlugin);
	cx::RegistrationPluginPtr registrationPlugin(new cx::RegistrationPlugin(acquisitionPlugin->getAcquisitionData()));
	plugins.push_back(   registrationPlugin);


	cx::MainWindow* mainWin = new cx::MainWindow(plugins);


  //mainWin->show();
#ifdef __APPLE__ // needed on mac for bringing to front: does the opposite on linux
  mainWin->activateWindow();
#endif
  mainWin->raise();

  int retVal = app.exec();
  delete mainWin;
  cx::MainWindow::shutdown(); // shutdown all global resources, _after_ gui is deleted.
  return retVal;
  
}

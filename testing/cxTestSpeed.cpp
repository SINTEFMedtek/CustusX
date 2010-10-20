#include "cxTestSpeed.h"
#include "cxMainWindow.h"

void cxTestSpeed::setUp()
{
	// this stuff will be performed just before all tests in this class
}

void cxTestSpeed::tearDown()
{
	// this stuff will be performed just after all tests in this class
}

/**
 */
void cxTestSpeed::testInitialize()
{
//  Q_INIT_RESOURCE(cxResources);

//  QApplication app(argc, argv);
//  app.setOrganizationName("SINTEF");
//  app.setOrganizationDomain("www.sintef.no");
//  app.setApplicationName("CustusX3");
//  app.setWindowIcon(QIcon(":/icons/CustusX.png"));
//  app.setWindowIcon(QIcon(":/icons/.png"));

  cx::MainWindow::initialize();

  cx::MainWindow* mainWin = new cx::MainWindow;
  mainWin->show();
#ifdef __APPLE__ // needed on mac for bringing to front: does the opposite on linux
  mainWin->activateWindow();
#endif
  mainWin->raise();

//  int retVal = app.exec();
  delete mainWin;
  cx::MainWindow::shutdown(); // shutdown all global resources, _after_ gui is deleted.
  return retVal;
}

void cxTestSpeed::testMore()
{

}



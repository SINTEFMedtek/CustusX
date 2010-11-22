#include <QApplication>

#include "cxMainWindow.h"


int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  app.setOrganizationName("SINTEF");
  app.setOrganizationDomain("www.sintef.no");
  app.setApplicationName("Grabber Server");

  cx::MainWindow window;
  window.show();
  #ifdef __APPLE__ // needed on mac for bringing to front: does the opposite on linux
  window.activateWindow();
#endif
  window.raise();

  int retVal = app.exec();
  return retVal;

}

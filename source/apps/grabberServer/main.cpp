#include <QApplication>

#include "cxMainWindow.h"
#include <iostream>


int main(int argc, char **argv)
{
  ssc::MessageManager::initialize();

  typedef ssc::Message Message;
  qRegisterMetaType<Message>("Message");

  QApplication app(argc, argv);
  app.setOrganizationName("SINTEF");
  app.setOrganizationDomain("www.sintef.no");
  app.setApplicationName("Grabber Server");

  /*int i;
  printf("argc = %d\n", argc);

  for (i = 0; i<argc; i++)
    printf("argv[%d] = %s\n", i, argv[i]);*/

  QStringList arguments;
  for(int i=1; i<argc ; ++i)
  {
    arguments.push_back(QString(argv[i]));
  }

  cx::MainWindow window(arguments);
  if(arguments.contains("--auto"))
    window.showMinimized();
  else
  {
    window.show();
#ifdef __APPLE__ // needed on mac for bringing to front: does the opposite on linux
  window.activateWindow();
#endif
  window.raise();
  }

  int retVal = app.exec();
  return retVal;

}

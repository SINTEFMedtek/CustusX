#include <iostream>
#include <math.h>
#include <cstdlib>
#include <cstdio>

#include <QApplication>
#include <QString>
#include "cxImageServer.h"
#include "cxImageSenderFactory.h"
//
#include "cxImageSenderOpenCV.h"

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef WIN32
void my_handler(int s)
{
//           printf("Caught signal %d\n",s);
           qApp->quit();
}
#endif

int main(int argc, char* argv[])
{
#ifndef WIN32
	// nice shutdown of app
	// http://stackoverflow.com/questions/1641182/how-can-i-catch-a-ctrl-c-event-c
	struct sigaction sigIntHandler;

	sigIntHandler.sa_handler = my_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;

	sigaction(SIGINT, &sigIntHandler, NULL);
#endif

  QApplication app(argc, argv);
  app.setOrganizationName("SINTEF");
  app.setOrganizationDomain("www.sintef.no");
  app.setApplicationName("OpenIGTLinkServer");
  //------------------------------------------------------------
  // Parse Arguments
  cx::StringMap args = cx::extractCommandlineOptions(app.arguments());

  cx::ImageServer server;
  if (args.count("help") || args.count("h"))
  {
	  server.printHelpText();
	  return 0;
  }

  int port = cx::convertStringWithDefault(args["port"], 18333);

  server.initialize();
  server.startListen(port);

  int retVal = app.exec();
  return retVal;

}

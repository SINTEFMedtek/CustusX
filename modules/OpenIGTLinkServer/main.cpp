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

int main(int argc, char* argv[])
{
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

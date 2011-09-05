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
  app.setApplicationName("cxOpenIGTLinkServer");
  //------------------------------------------------------------
  // Parse Arguments

  cx::StringMap args = cx::extractCommandlineOptions(app.arguments());

  if (args.count("help"))
  {
    std::cout << "Usage: " << argv[0] << " (--arg <argval>)*"    << std::endl;
    std::cout << "    --port   : Tcp/IP port # (default=18333)"   << std::endl;
    std::cout << "    --type   : Grabber type  (default=OpenCV)"   << std::endl;
    std::cout << std::endl;
    std::cout << "    Select one of the types below:"   << std::endl;

    cx::ImageSenderFactory factory;
  	QStringList types = factory.getSenderTypes();
  	for (int i=0; i<types.size(); ++i)
  	{
    	QStringList args = factory.getArgumentDescription(types[i]);
      std::cout << std::endl;
      std::cout << "      type = " << types[i].toStdString() << std::endl;
      for (int j=0; j<args.size(); ++j)
      	std::cout << "        " << args[j].toStdString() << std::endl;
  	}
    std::cout << std::endl;

    return 0;
  }

  int port = cx::convertStringWithDefault(args["port"], 18333);

//  if (argc != 3) // check number of arguments
//    {
//    // If not correct, print usage
//    std::cerr << "Usage: " << argv[0] << " <port> <fps> <imgdir>"    << std::endl;
//    std::cerr << "    <port>     : Port # (18333 CustusX default)"   << std::endl;
//    std::cerr << "    <imgdir>   : file directory, where \"igtlTestImage[1-5].raw\" are placed." << std::endl;
//    std::cerr << "                 (usually, in the Examples/Imager/img directory.)" << std::endl;
//    exit(0);
//    }

//  int    port     = atoi(argv[1]);
//  char*  filedir  = argv[2];

  cx::ImageServer server;
  server.startListen(port);

//	new cx::ImageSenderOpenCV(NULL, cx::StringMap());

  int retVal = app.exec();
  return retVal;

}

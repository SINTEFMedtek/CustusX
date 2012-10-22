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

#ifdef WIN32
#include <windows.h>
//#include <tchar.h>
#endif

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef WIN32
// Catch ctrl + c on windows: http://www.cplusplus.com/forum/beginner/1501/
BOOL WINAPI ConsoleHandler(
	DWORD dwCtrlType   //  control signal type
);

BOOL WINAPI ConsoleHandler(DWORD CEvent)
{
    char mesg[128];

    switch(CEvent)
    {
    case CTRL_C_EVENT:
        //MessageBox(NULL,_T("CTRL+C received!"),_T("CEvent"),MB_OK);
        break;
    case CTRL_BREAK_EVENT:
        //MessageBox(NULL, _T("CTRL+BREAK received!"),_T("CEvent"),MB_OK);
        break;
    case CTRL_CLOSE_EVENT:
        //MessageBox(NULL,_T("Program being closed!"),_T("CEvent"),MB_OK);
        break;
    case CTRL_LOGOFF_EVENT:
        //MessageBox(NULL,_T("User is logging off!"),_T("CEvent"),MB_OK);
        break;
    case CTRL_SHUTDOWN_EVENT:
        //MessageBox(NULL,_T("User is logging off!"),_T("CEvent"),MB_OK);
        break;

    }
	qApp->quit();
    return TRUE;
}
#endif

#ifndef WIN32
void my_handler(int s)
{
//           printf("Caught signal %d\n",s);
           qApp->quit();
}
#endif

int main(int argc, char* argv[])
{
#ifdef WIN32
	if (SetConsoleCtrlHandler( (PHANDLER_ROUTINE)ConsoleHandler,TRUE)==FALSE)
	{
		// unable to install handler... 
		// display message to the user
		printf("Unable to install handler!\n");
	//return -1;
	}

#endif
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

  bool ok = server.initialize();
  if (!ok)
  {
	  std::cout << "Can not start streaming. Quitting application" << std::endl;
	  return false;
  }
  ok = server.startListen(port);
  if (!ok)
  {
	  std::cout << "Can not start listening. Quitting..." << std::endl;
	  return false;
  }

  int retVal = app.exec();
  return retVal;

}

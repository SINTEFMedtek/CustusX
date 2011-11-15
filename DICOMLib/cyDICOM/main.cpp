// Application main

#include <QApplication>
#include <QtOpenGL/QtOpenGL>
#include <iostream>

#include "sscLogger.h"
#include "mainwindow.h"

using namespace std;

int main( int argc, char *argv[] )
{
	QApplication app( argc, argv );
	bool connected = false;
	int i = 0;

	if ( !QGLFormat::hasOpenGL() )
	{
		cerr << "This system has no OpenGL support!" << endl;
		exit( EXIT_FAILURE );
	}

	SSC_Logging_Init_Default( "com.sonowand.cyDICOM" );

//	snwdbus_signal_start( GLOBAL_PING );
//	snwdbus_signal_send();
//
//	while ( !connected )
//	{
//		enum SNWDBUS_ENUM sig = snwdbus_get_signal();
//
//		if ( sig == SCP_PONG )
//		{
//			connected = true;
//		}
//		else if ( sig == SNWDBUS_PACKET_NONE )
//		{
//			i++;
//			if ( i == 10 )
//			{
//				break;
//			}
//			usleep( 500000 );
//		}
//	}
//	if ( !connected )
//	{
//		cout << "No response from DICOM SCP server. Maybe you should start it?" << endl;
//		exit( EXIT_FAILURE );
//	}
	MainWindow mainWindow;
	mainWindow.show();

	return app.exec();
	SSC_Logging_Done();
}

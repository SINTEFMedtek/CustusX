/*
 *  main.cpp
 *  Created by Ole Vegard Solberg on 5/4/10.
 */

#include "sscMainWindow.h"

/** Test app for UsReconstruction
 */
int main(int argc, char **argv)
{
	QApplication app(argc, argv);
  
	MainWindow top;
  top.resize(700, 600);
  top.show();
#ifdef __APPLE__ // needed on mac for bringing to front: does the opposite on linux
  top.activateWindow();
#endif
  top.raise();
     
  // uncomment to perform test reconstruction during startup
  //top.automaticStart();

	int val = app.exec();
	return val;
}

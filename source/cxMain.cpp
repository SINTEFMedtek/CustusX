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

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(cxResources);

    QApplication app(argc, argv);
    cx::MainWindow mainWin;
    mainWin.show();
    int retVal = app.exec();
    return retVal;

}

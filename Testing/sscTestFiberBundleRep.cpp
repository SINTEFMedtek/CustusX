#include <iostream>
#include <string>
#include <vector>

#include <vtkPolyData.h>
#include <vtkRenderer.h>

#include <QtGui>

#include "sscFiberBundleRep.h"
#include "sscFiberBundle.h"
#include "sscAxesRep.h"
#include "sscView.h"

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

    QStringList argList = QCoreApplication::arguments();

    QString vtkFileName1;
    if (!argList.isEmpty() && argList.size() > 1)
    {
        // Extract second argument as file name
        vtkFileName1 = argList[1];
    }
    else
    {
        std::cout << "TestFiberBundleRep [filename]" << std::endl;
    }

	ssc::View* view = new ssc::View();
    ssc::FiberBundlePtr bundle = ssc::FiberBundle::New(vtkFileName1);
    bundle->setFilePath(vtkFileName1);
    ssc::FiberBundleRepPtr rep = ssc::FiberBundleRep::New(bundle->getUid());
    rep->setBundle(bundle);
	view->setRep(rep);

	ssc::AxesRepPtr axesRep = ssc::AxesRep::New("AxesRepUID");
	view->addRep(axesRep);

	QMainWindow mainWindow;
	mainWindow.setCentralWidget(view);
    mainWindow.resize(QSize(1200,1024));
    mainWindow.show();
	view->getRenderer()->ResetCamera();

    app.exec();

    return 0;
}

#include <iostream>
#include <string>
#include <vector>

#include <vtkPolyData.h>
#include <vtkRenderer.h>
#include <vtkTubeFilter.h>

#include <QtGui>

#include "sscFiberBundleRep.h"
#include "sscAxesRep.h"
#include "sscView.h"
#include "sscVtkFileMesh.h"
#include "sscTestUtilities.h"

/**
  * Test application for dti fiber bundles in vtk format.
  * Load a fiber bundle and visualize the 3d model as rep.
  * If you can see a single strain of fiber, all is well.
  */
int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    // std::cout << ssc::TestUtilities::GetDataRoot() << std::endl;
    QString vtkFileName1 = ssc::TestUtilities::ExpandDataFileName("DTI/FiberBundleNode.vtk");

    ssc::ViewWidget *view = new ssc::ViewWidget();
    ssc::MeshPtr bundle(new ssc::VtkFileMesh(vtkFileName1, vtkFileName1, vtkFileName1));
    QColor color = QColor("gold");
    // color.setAlphaF(0.6);
    bundle->setColor(color);

    /** This section specifies the default test setup, where the a fiber bundle rep is used */
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

    // This is an automatic test, so terminate the application after some time
    QTimer::singleShot(SSC_DEFAULT_TEST_TIMEOUT_SECS*1000, &app, SLOT(quit()));
    app.exec();

    return 0;
}

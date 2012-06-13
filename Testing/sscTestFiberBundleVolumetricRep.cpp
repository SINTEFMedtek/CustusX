#include <iostream>
#include <string>
#include <vector>

#include <vtkPolyData.h>
#include <vtkRenderer.h>
#include <vtkTubeFilter.h>

#include <QtGui>

#include "sscFiberBundle.h"
#include "sscAxesRep.h"
#include "sscView.h"
#include "sscTestUtilities.h"
#include "sscVolumetricRep.h"
#include "sscDataManager.h"

/**
  * Test application for dti fiber bundles in vtk format.
  * Load a fiber bundle and do a volumetric rendering through a rep.
  * If you can see a single strain of fiber, all is well.
  */
int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    // std::cout << ssc::TestUtilities::GetDataRoot() << std::endl;
    QString vtkFileName1 = ssc::TestUtilities::ExpandDataFileName("DTI/FiberBundleNode.vtk");

    ssc::View* view = new ssc::View();
    ssc::FiberBundlePtr bundle = ssc::FiberBundle::New(vtkFileName1);
    bundle->setFilePath(vtkFileName1);
    bundle->setColor(QColor("gold"));

    // Get the original poly data
    vtkPolyDataPtr pd = bundle->getVtkPolyData();

    /** Create a tube filter for the mesh and save the result
      * as polygonal data in the fiber bundle container.
      *
      * This is currently done, because it seems as if the
      * simple polyline data provided is not enough
      * (maybe because of the lack of physical extent?).
      */
    vtkSmartPointer<vtkTubeFilter> tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
    tubeFilter->SetInput(pd);
    tubeFilter->SetRadius(0.5); //default is .5
    tubeFilter->SetNumberOfSides(8);
    tubeFilter->Update();

    bundle->setVtkPolyData(tubeFilter->GetOutput());

    /** Create image from extracted image data */
    ssc::ImagePtr image = ssc::DataManager::getInstance()->createImage(bundle->getVtkImageData(), bundle->getUid(), bundle->getName());
    if (!image)
    {
        std::cout << "Could get image data from bundle." << std::endl;
        return -1;
    }

    /** Create and display the volumetric rep object */
    ssc::VolumetricRepPtr rep = ssc::VolumetricRep::New(bundle->getUid());
    rep->setMaxVolumeSize(10*1000*1000);
    rep->setUseGPUVolumeRayCastMapper(); // if available
    rep->setImage(image);
    rep->setName(bundle->getName());

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

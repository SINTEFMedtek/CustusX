#include <iostream>
#include <string>
#include <vector>

//#include <vtkPolyData.h>
#include <vtkImageData.h>
#include <vtkMetaImageReader.h>
#include <vtkImagePlaneWidget.h>
#include <vtkRenderer.h>
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

#include <QtGui>


#include "sscTestUtilities.h"
#include "sscDataManager.h"
#include "sscImage.h"
//#include "sscMesh.h"
//#include "sscGeometricRep.h"
#include "sscAxesRep.h"
#include "sscSliceRep.h"
#include "sscView.h"


int main(int argc, char **argv)
{
	// generate imageFileName
	//QString imageFileName1 = ssc::TestUtilities::ExpandDataFileName("Fantomer/Kaisa/MetaImage/Kaisa.mhd");
//	QString imageFileName1 = ssc::TestUtilities::ExpandDataFileName("Person5/person5_t2_byte.mhd");
	QString imageFileName1 = ssc::TestUtilities::ExpandDataFileName("ssc/Person5/person5_t2_unsigned.mhd");
	//QString imageFileName1 = ssc::TestUtilities::ExpandDataFileName("MetaImage/20070309T102309_MRA.mhd");
	std::cout << imageFileName1 << std::endl;

	// read image
	ssc::ImagePtr image1 = ssc::DataManager::getInstance()->loadImage(imageFileName1, ssc::rtMETAIMAGE);

	/*
	// read "directely"
	vtkMetaImageReader* reader = vtkMetaImageReader::New();
	reader->SetFileName(imageFileName1.c_str());
	reader->Update();
	*/

	QApplication app(argc, argv);
	//ssc::ViewPtr view(new ssc::View());
	ssc::View* view = new ssc::View();

	ssc::AxesRepPtr axesRep = ssc::AxesRep::New("AxesRepUID");
	view->addRep(axesRep);

	ssc::SliceRepPtr sliceRep = ssc::SliceRep::New("SliceRepUID");
	sliceRep->setImage( image1 );
	view->addRep(sliceRep);

/*
	vtkImagePlaneWidget* planeWidget = vtkImagePlaneWidget::New();
	planeWidget->SetInput( image1->getBaseVtkImageData() );
	//planeWidget->SetInput( reader->GetOutput() );

	planeWidget->SetInteractor( view->getRenderWindow()->GetInteractor() );
	planeWidget->InteractionOn();
	planeWidget->SetPlaneOrientationToZAxes();
	//planeWidget->SetOrigin(-100,-100,0);
	planeWidget->On();

	//view.reset();
*/

	QMainWindow mainWindow;
	mainWindow.setCentralWidget(view);
	mainWindow.resize(QSize(500,500));
	mainWindow.show();
	view->getRenderer()->ResetCamera();
	QTimer::singleShot(SSC_DEFAULT_TEST_TIMEOUT_SECS*1000, &app, SLOT(quit())); // terminate app after some seconds - this is an automated test!!
	app.exec();


	return 0;
}


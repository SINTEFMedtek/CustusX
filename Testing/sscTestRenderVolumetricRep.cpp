#include <iostream>
#include <string>
#include <vector>
#include <math.h>

#include <vtkImageData.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <QtGui>

#include "sscTestUtilities.h"
#include "sscDataManager.h"
#include "sscImage.h"
#include "sscView.h"
#include "sscVolumetricRep.h"
#include "sscProgressiveLODVolumetricRep.h"
#include "sscTypeConversions.h"
#include "sscAcceptanceBoxWidget.h"

//namespace ssc
//{

template<class T>
void doRender(T window)
{
	QTime pre = QTime::currentTime();

	window->Render();

	QTime post = QTime::currentTime();
	int msec = pre.msecsTo(post);
	std::cout << "render time: " << msec <<  " ms \t" << std::endl;
}

/**test app for multiple render operations.
 *
 * load a single image, insert in volumetricrep/view/mainwindow,
 * then render it several times and count the milliseconds.
 * Only the first render should take time.
 *
 */
int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	std::cout << ssc::TestUtilities::GetDataRoot() << std::endl;
	QString testFile1 = ssc::TestUtilities::ExpandDataFileName("Person5/person5_t2_unsigned.mhd");
//	QString testFile1 = ssc::TestUtilities::ExpandDataFileName("Person5/person5_t2_byte.mhd");
	//QString testFile1 = ssc::TestUtilities::ExpandDataFileName("Fantomer/Kaisa/MetaImage/Kaisa.mhd");
	std::cout << testFile1 << std::endl;

	ssc::ImagePtr image1 = ssc::DataManager::getInstance()->loadImage(testFile1, testFile1, ssc::rtMETAIMAGE);

	std::cout << "test" << std::endl;
	QWidget mainWindow;
	QHBoxLayout* layout = new QHBoxLayout(&mainWindow);
	ssc::ViewWidget* view = new ssc::ViewWidget(&mainWindow);
	view->getRenderWindow()->GetInteractor()->EnableRenderOff();
	layout->addWidget(view);
	std::cout << "test2" << std::endl;

	//ssc::VolumetricRepPtr rep = ssc::VolumetricRep::New(image1->getUid());
	ssc::ProgressiveLODVolumetricRepPtr rep = ssc::ProgressiveLODVolumetricRep::New(image1->getUid());

	rep->setImage(image1);
	view->setRep(rep);
	std::cout << "test3" << std::endl;

	mainWindow.resize(QSize(1000,1000));
	std::cout << "test3.1" << std::endl;
	view->getRenderer()->ResetCamera();
	std::cout << "test3.2" << std::endl;
	mainWindow.show();
	std::cout << "test3.3" << std::endl;

	for (unsigned i=0; i<3; ++i)
	{

		doRender(view->getRenderWindow());
	}
	std::cout << "test4" << std::endl;

//#define USE_TRANSFORM_RESCLICER
#ifdef USE_TRANSFORM_RESCLICER
	// this code is meant for testing the reslicer in ssc::Image
	std::cout << "rotating the image data..." << std::endl;
	image1->set_rMd(ssc::createTransformRotateZ(1.0*M_PI/8.0).inv());
	doRender(view->getRenderWindow());
	image1->set_rMd(ssc::createTransformRotateZ(2.0*M_PI/8.0).inv());
	doRender(view->getRenderWindow());
	image1->set_rMd(ssc::createTransformRotateZ(3.0*M_PI/8.0).inv());
	doRender(view->getRenderWindow());
#endif
	std::cout << "Uid       : " << image1->getUid() << std::endl;
	
	//QTimer::singleShot(SSC_DEFAULT_TEST_TIMEOUT_SECS*1000, &app, SLOT(quit())); // terminate app after some seconds - this is an automated test!!

	ssc::AcceptanceBoxWidget* acceptanceBox = new ssc::AcceptanceBoxWidget("OK / not OK", &mainWindow);
	layout->addWidget(acceptanceBox);
	
	app.exec();

	return 0;
}

//}//namespace ssc

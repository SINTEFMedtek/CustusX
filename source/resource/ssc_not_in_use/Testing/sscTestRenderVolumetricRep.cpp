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

    std::cout << cx::TestUtilities::GetDataRoot() << std::endl;
    QString testFile1 = cx::TestUtilities::ExpandDataFileName("ssc/Person5/person5_t2_unsigned.mhd");
//	QString testFile1 = cx::TestUtilities::ExpandDataFileName("Person5/person5_t2_byte.mhd");
    //QString testFile1 = cx::TestUtilities::ExpandDataFileName("Fantomer/Kaisa/MetaImage/Kaisa.mhd");
	std::cout << testFile1 << std::endl;

    cx::ImagePtr image1 = cx::DataManager::getInstance()->loadImage(testFile1, testFile1, cx::rtMETAIMAGE);

	std::cout << "test" << std::endl;
	QWidget mainWindow;
	QHBoxLayout* layout = new QHBoxLayout(&mainWindow);
    cx::ViewWidget* view = new cx::ViewWidget(&mainWindow);
	view->getRenderWindow()->GetInteractor()->EnableRenderOff();
	layout->addWidget(view);
	std::cout << "test2" << std::endl;

// this rep only works on linux
#ifdef WIN32
	return 1;
#endif
#ifdef __APPLE__
	return 1;
#endif

    //cx::VolumetricRepPtr rep = cx::VolumetricRep::New(image1->getUid());
    cx::ProgressiveLODVolumetricRepPtr rep = cx::ProgressiveLODVolumetricRep::New(image1->getUid());

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
    // this code is meant for testing the reslicer in cx::Image
	std::cout << "rotating the image data..." << std::endl;
    image1->set_rMd(cx::createTransformRotateZ(1.0*M_PI/8.0).inv());
	doRender(view->getRenderWindow());
    image1->set_rMd(cx::createTransformRotateZ(2.0*M_PI/8.0).inv());
	doRender(view->getRenderWindow());
    image1->set_rMd(cx::createTransformRotateZ(3.0*M_PI/8.0).inv());
	doRender(view->getRenderWindow());
#endif
	std::cout << "Uid       : " << image1->getUid() << std::endl;
	
	//QTimer::singleShot(SSC_DEFAULT_TEST_TIMEOUT_SECS*1000, &app, SLOT(quit())); // terminate app after some seconds - this is an automated test!!

    cx::AcceptanceBoxWidget* acceptanceBox = new cx::AcceptanceBoxWidget("OK / not OK", &mainWindow);
	layout->addWidget(acceptanceBox);
	
	app.exec();

	return 0;
}

//}//namespace ssc

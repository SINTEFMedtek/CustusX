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

template<class T>
void doRender(T window)
{
	QTime pre = QTime::currentTime();

	window->Render();

	QTime post = QTime::currentTime();
	int msec = pre.msecsTo(post);
	std::cout << "render time: " << msec <<  " ms \t" << std::endl;
}

/**render test.
 *
 * load a single image, insert in volumetricrep/view/mainwindow,
 * then render it.
 */
int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	std::cout << ssc::TestUtilities::GetDataRoot() << std::endl;
	std::string testFile1 = ssc::TestUtilities::ExpandDataFileName("MetaImage/20070309T105136_MRT1.mhd");
	//std::string testFile1 = ssc::TestUtilities::ExpandDataFileName("Fantomer/Kaisa/MetaImage/Kaisa.mhd");
	std::cout << testFile1 << std::endl;

	ssc::ImagePtr image1 = ssc::DataManager::getInstance()->loadImage(testFile1, testFile1, ssc::rtMETAIMAGE);

	QMainWindow mainWindow;
	ssc::ViewPtr view(new ssc::View(mainWindow.centralWidget()));
	mainWindow.setCentralWidget(view.get());

	ssc::VolumetricRepPtr rep = ssc::VolumetricRep::New(image1->getUid());
	rep->setImage(image1);
	view->setRep(rep);

	mainWindow.resize(QSize(500,500));
	mainWindow.show();
	view->getRenderer()->ResetCamera();

	doRender(view->getRenderWindow());

	QTimer::singleShot(SSC_DEFAULT_TEST_TIMEOUT_SECS*1000*3, &app, SLOT(quit())); // terminate app after some seconds - this is an automated test!!
	
#ifdef __MACOSX__ // needed on mac for bringing to front: does the opposite on linux
	mainWindow.activateWindow();
#endif
	mainWindow.raise();

	app.exec();

	return 0;
}


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
	std::string testFile1 = ssc::TestUtilities::ExpandDataFileName("Fantomer/Kaisa/MetaImage/Kaisa.mhd");
	std::cout << testFile1 << std::endl;

	ssc::ImagePtr image1 = ssc::DataManager::instance()->loadImage(testFile1, ssc::rtMETAIMAGE);
			
	QMainWindow mainWindow;
	ssc::ViewPtr view(new ssc::View());
	mainWindow.setCentralWidget(view.get());
		
	ssc::VolumetricRepPtr rep = ssc::VolumetricRep::create(image1->getUid());
	rep->setImage(image1);
	view->setRep(rep);
		
	mainWindow.resize(QSize(500,500));
	mainWindow.show();
	view->getRenderer()->ResetCamera();	
		
	for (unsigned i=0; i<5; ++i)
	{
		doRender(view->getRenderWindow());
	}
	
#define USE_TRANSFORM_RESCLICER
#ifdef USE_TRANSFORM_RESCLICER
	// this code is meant for testing the reslicer in ssc::Image
	std::cout << "rotating the image data..." << std::endl;
	image1->setTransform(ssc::createTransformRotateZ(1.0*M_PI/8.0));
	doRender(view->getRenderWindow());
	image1->setTransform(ssc::createTransformRotateZ(2.0*M_PI/8.0));
	doRender(view->getRenderWindow());
	image1->setTransform(ssc::createTransformRotateZ(3.0*M_PI/8.0));
	doRender(view->getRenderWindow());
#endif	
	std::cout << "Uid       : " << image1->getUid() << std::endl;
//	app.exec();

	return 0;
}

//}//namespace ssc

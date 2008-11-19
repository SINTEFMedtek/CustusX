#include <iostream>
#include <string>
#include <vector>

#include <vtkImageData.h>
#include <vtkRenderWindow.h>
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
		
	mainWindow.show();

	for (unsigned i=0; i<8; ++i)
	{
		doRender(view->getRenderWindow());
	}
	
	std::cout << "Uid       : " << image1->getUid() << std::endl;
	//app.exec();

	return 0;
}

//}//namespace ssc

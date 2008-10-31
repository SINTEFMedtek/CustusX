
#include <QApplication>
#include <QMainWindow>
#include <QtGui>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <vtkRenderer.h>
#include "sscDataManager.h"
#include "sscImage.h"
#include "sscVolumetricRep.h"
#include "sscView.h"

//TOOL&TOOLMANAGER TEST
#include <vtkRenderWindow.h>
#include <vtkCallbackCommand.h>
#include <vtkRenderWindowInteractor.h>
#include "sscDummyToolManager.h"
#include "sscToolRep3D.h"

/** Test app for SSC
 */
void timerUpdate(vtkObject* obj, unsigned long,
                          void*, void*)
{
  vtkRenderWindowInteractor* iren =
    vtkRenderWindowInteractor::SafeDownCast(obj);
    iren->GetRenderWindow()->Render();
    iren->CreateTimer(VTKI_TIMER_UPDATE);
}
int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	QMainWindow mainwindow;

	//TEST FOR DUMMYTOOL AND DUMMYTOOLMANAGER START
	ssc::View* view = new ssc::View();

	mainwindow.setCentralWidget(view);
	mainwindow.resize(QSize(1000,500));
	mainwindow.show();

	ssc::ToolManager* toolmanager = ssc::DummyToolManager::getInstance();
	toolmanager->configure();
	toolmanager->initialize();
	toolmanager->startTracking();

	ssc::ToolPtr tool = toolmanager->getDominantTool();

	ssc::ToolRepPtr toolrep = ssc::ToolRepPtr(new ssc::ToolRep3D(tool->getUid(),tool->getName(),tool));

	view->setRep(toolrep);

    vtkRenderWindowInteractor * iren = view->GetInteractor();

    vtkCallbackCommand* callback = vtkCallbackCommand::New();
    callback->SetCallback(timerUpdate);

    iren->AddObserver(vtkCommand::TimerEvent, callback);
    iren->CreateTimer(VTKI_TIMER_FIRST);
    std::cout << "GET THE WINDOWS FOCUS AND PRESS \"R\" TO SEE THE ACTOR!" << std::endl;
    //TEST FOR DUMMYTOOL AND DUMMYTOOLMANAGER END

    //TEST FOR VIEWS, REPS AND IMAGES START
	/*
	std::string testFile1 = //"/Users/frankl/fl/dev/Repository/data/nevro/MetaImage/3DT1.mhd";
	                       //"/Users/frankl/fl/dev2/cvsStable/ssc/Test/TestData/Fantomer/Kaisa/MetaImage/Kaisa.mhd";
						   "/Users/jbake/jbake/dev/ssc/Test/TestData/Fantomer/Kaisa/MetaImage/Kaisa.mhd";
	                       //"/home/christiana/snw2_trunk/C_code/SonowandSintefCooperation/test/TestData/Fantomer/Kaisa/MetaImage/Kaisa.mhd"
	std::string testFile2 =  "/Users/jbake/jbake/dev/CustusXData/nevro/MetaImage/3DT1.mhd";

	ssc::ImagePtr imagePtr1 = ssc::DataManager::instance()->loadImage(testFile1, ssc::rtMETAIMAGE);
	ssc::ImagePtr imagePtr2 = ssc::DataManager::instance()->loadImage(testFile2, ssc::rtMETAIMAGE);

	ssc::VolumetricRepPtr volumeRepPtr1 = ssc::VolumetricRep::create(imagePtr1->getUid());
	ssc::VolumetricRepPtr volumeRepPtr2 = ssc::VolumetricRep::create(imagePtr1->getUid());

	ssc::View* view1 = new ssc::View();
	ssc::View* view2 = new ssc::View();

	QHBoxLayout* layout = new QHBoxLayout();
	QWidget* widget = new QWidget();
	widget->setLayout(layout);

	volumeRepPtr1->setImage(imagePtr1);
	volumeRepPtr1->setName(imagePtr1->getName());

	view1->setRep(volumeRepPtr1);
	view1->getRenderer()->ResetCamera();
	view1->getRenderer()->Render();

	//view2->setRep(volumeRepPtr2);
	//view2->getRenderer()->ResetCamera();

	layout->addWidget(view1);
	layout->addWidget(view2);

	mainwindow.setCentralWidget(widget);

	mainwindow.resize(QSize(1000,500));
	mainwindow.show();

	view1->removeRep(volumeRepPtr1);
	view1->getRenderer()->ResetCamera();
	view1->getRenderer()->Render();

	view2->setRep(volumeRepPtr1);
	view2->getRenderer()->ResetCamera();
	view2->getRenderer()->Render();

	volumeRepPtr1->setImage(imagePtr2);
	view2->getRenderer()->ResetCamera();
	view2->getRenderer()->Render();
	*/
    //TEST FOR VIEWS, REPS AND IMAGES START END

	int val = app.exec();
	return val;
	//return 0;
}

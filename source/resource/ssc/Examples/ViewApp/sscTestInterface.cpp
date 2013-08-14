#include "sscTestInterface.h"

#include <QtGui>

#include <iostream>

#include <boost/shared_ptr.hpp>

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkCallbackCommand.h>
#include <vtkRenderWindowInteractor.h>

#include "sscTestUtilities.h"
#include "sscDataManager.h"
#include "sscImage.h"
#include "sscVolumetricRep.h"
#include "sscView.h"
#include "sscDummyToolManager.h"
#include "sscToolRep3D.h"
#include "vtkForwardDeclarations.h"

/**
 * sscTestInterface.cpp
 *
 * \brief
 *
 * \date Nov 3, 2008
 * \author: Janne Beate Bakeng, SINTEF
 */

namespace ssc
{
TestInterface::TestInterface() :
	mTestDummyToolAndDummyToolManagerAction(new QAction(tr("Test DummyTool and DummyToolManager"), this)),
	mTestSwitchingViewsRepsAndImagesAction(new QAction(tr("Test Views, Reps and Images"),this)),
	mView1(NULL),
	mView2(NULL)
{
	//Connect signals and slots
	connect(mTestDummyToolAndDummyToolManagerAction, SIGNAL(triggered()),
			this, SLOT(testDummyToolAndDummyToolManager()));

	connect(mTestSwitchingViewsRepsAndImagesAction, SIGNAL(triggered()),
			this, SLOT(testSwitchingViewsRepsAndImages()));

	//Create the menu
	mTestMenu = menuBar()->addMenu(tr("Test Menu"));
	mTestMenu->addAction(mTestDummyToolAndDummyToolManagerAction);
	mTestMenu->addAction(mTestSwitchingViewsRepsAndImagesAction);

	//Setup and show the mainwindow
	this->resize( QSize(1000,500) );
	this->show();

	mView1 = new ViewWidget();
	mView2 = new ViewWidget();
}
TestInterface::~TestInterface()
{}
void TestInterface::testDummyToolAndDummyToolManager()
{
	mView1 = new ViewWidget();

	this->setCentralWidget(mView1);

	mToolmanager = DummyToolManager::getInstance();
	mToolmanager->configure();
	mToolmanager->initialize();
	mToolmanager->startTracking();

	ToolPtr tool = mToolmanager->getDominantTool();

	ToolRep3DPtr toolRep = ToolRep3D::New(tool->getUid(),tool->getName());
	toolRep->setTool(tool);

	mView1->setRep(toolRep);

	this->startRendering();

	mView1->getRenderer()->ResetCamera();

    std::cout << "GET THE WINDOWS FOCUS AND PRESS \"R\" TO SEE THE ACTOR!" << std::endl;
}
void TestInterface::testSwitchingViewsRepsAndImages()
{
	//Setup gui
	QWidget* widget = new QWidget(this);
	QHBoxLayout* layout = new QHBoxLayout(widget);
	widget->setLayout(layout);

	//Create the objects
	QString testFile1 = TestUtilities::ExpandDataFileName("/Fantomer/Kaisa/MetaImage/Kaisa.mhd");
	QString testFile2 = TestUtilities::ExpandDataFileName("/MetaImage/20070309T105136_MRT1.mhd");
	
	
	mImagePtr1 = DataManager::getInstance()->loadImage(testFile1, testFile1, rtMETAIMAGE);
	mImagePtr2 = DataManager::getInstance()->loadImage(testFile2, testFile2, rtMETAIMAGE);

	mRepPtr1 = VolumetricRep::New(mImagePtr1->getUid());
	mRepPtr2 = VolumetricRep::New(mImagePtr2->getUid());

	//Setup default
	mCurrentImage =mImagePtr1;
	mCurrentRep = mRepPtr1;
	mCurrentView = mView1;

	mRepPtr1->setImage(mImagePtr1);
	mRepPtr1->setName(mImagePtr1->getName());

	mView1->setRep(mRepPtr1);
	mView1->getRenderer()->ResetCamera();
	mView1->getRenderer()->Render();

	layout->addWidget(mView1);
	layout->addWidget(mView2);

	this->setCentralWidget(widget);
	this->startRendering();

	//Images
	QButtonGroup* imageGroup = new QButtonGroup(this);
	imageGroup->setExclusive(true);
	QPushButton* imageButton1 = new QPushButton(tr("Image1"),this);
	imageButton1->setCheckable(true);
	imageButton1->setChecked(true);
	connect(imageGroup, SIGNAL(buttonClicked(int)),
			this, SLOT(setImage(int)));
	QPushButton* imageButton2 = new QPushButton(tr("Image2"),this);
	imageButton2->setCheckable(true);
	imageGroup->addButton(imageButton1,1);
	imageGroup->addButton(imageButton2,2);

	//Reps
	QButtonGroup* repGroup = new QButtonGroup(this);
	repGroup->setExclusive(true);
	QPushButton* repButton1 = new QPushButton(tr("Rep1"),this);
	repButton1->setCheckable(true);
	repButton1->setChecked(true);
	connect(repGroup, SIGNAL(buttonClicked(int)),
			this, SLOT(setRep(int)));
	//QPushButton* repButton2 = new QPushButton(tr("Rep2"),this);
	//repButton2->setCheckable(true);
	repGroup->addButton(repButton1,1);
	//repGroup->addButton(repButton2,2);

	//Views
	QButtonGroup* viewGroup = new QButtonGroup(this);
	viewGroup->setExclusive(true);
	QPushButton* viewButton1 = new QPushButton(tr("View1"),this);
	viewButton1->setCheckable(true);
	viewButton1->setChecked(true);
	connect(viewGroup, SIGNAL(buttonClicked(int)),
			this, SLOT(setView(int)));
	QPushButton* viewButton2 = new QPushButton(tr("View2"),this);
	viewButton2->setCheckable(true);
	viewGroup->addButton(viewButton1,1);
	viewGroup->addButton(viewButton2,2);

	//Widget for displaying buttons
	QDockWidget* dockWidget = new QDockWidget(this);
	QWidget* buttonWidget = new QWidget(dockWidget);
	buttonWidget->setMaximumHeight(50);
	QHBoxLayout* buttonLayout = new QHBoxLayout(buttonWidget);
	buttonLayout->addWidget(imageButton1);
	buttonLayout->addWidget(imageButton2);
	buttonLayout->addSpacing(2);
	buttonLayout->addWidget(repButton1);
	//buttonLayout->addWidget(repButton2);
	buttonLayout->addSpacing(2);
	buttonLayout->addWidget(viewButton1);
	buttonLayout->addWidget(viewButton2);
	dockWidget->setWidget(buttonWidget);
	this->addDockWidget(Qt::TopDockWidgetArea, dockWidget);
}
void TestInterface::setImage(int i)
{
	switch (i)
	{
	case 1:
		mCurrentImage = mImagePtr1;
		break;
	case 2:
		mCurrentImage = mImagePtr2;
		break;
	default:
		break;
	}
	mCurrentRep->setImage(mCurrentImage);

	mCurrentView->getRenderer()->ResetCamera();
	mCurrentView->getRenderer()->Render();
}
void TestInterface::setRep(int i)
{
	switch (i)
	{
	case 1:
		mCurrentRep = mRepPtr1;
		break;
	case 2:
		mCurrentRep = mRepPtr2;
		break;
	default:
		break;
	}
	mCurrentRep->setImage(mCurrentImage);
	mCurrentView->setRep(mCurrentRep);

	mCurrentView->getRenderer()->ResetCamera();
	mCurrentView->getRenderer()->Render();
}
void TestInterface::setView(int i)
{
	switch (i)
	{
	case 1:
		if(mCurrentView != mView1)
			mCurrentView->removeRep(mCurrentRep);
		mCurrentView = mView1;
		break;
	case 2:
		if(mCurrentView != mView2)
			mCurrentView->removeRep(mCurrentRep);
		mCurrentView = mView2;
		break;
	default:
		break;
	}
	mCurrentRep->setImage(mCurrentImage);
	mCurrentView->setRep(mCurrentRep);

	mCurrentView->getRenderer()->ResetCamera();
	mCurrentView->getRenderer()->Render();
}
void TestInterface::timerUpdate(vtkObject* obj, unsigned long, void*, void*)
{
	vtkRenderWindowInteractor* iren =
	vtkRenderWindowInteractor::SafeDownCast(obj);
	iren->GetRenderWindow()->Render();
	iren->CreateTimer(VTKI_TIMER_UPDATE);
}
void TestInterface::startRendering()
{
    mCallback = vtkCallbackCommandPtr::New();
    mCallback->SetCallback(timerUpdate);

    mView1->GetInteractor()->AddObserver(vtkCommand::TimerEvent, mCallback);
    mView1->GetInteractor()->CreateTimer(VTKI_TIMER_FIRST);

    mView2->GetInteractor()->AddObserver(vtkCommand::TimerEvent, mCallback);
    mView2->GetInteractor()->CreateTimer(VTKI_TIMER_FIRST);
}
}//namespace ssc

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


#include "sscTestUtilities.h"
#include "sscDataManager.h"
#include "sscImage.h"
//#include "sscMesh.h"
//#include "sscGeometricRep.h"
#include "sscAxesRep.h"
#include "sscSliceRep.h"

#include "sscVolumetricRep.h"
#include "sscSliceComputer.h"
#include "sscVector3D.h"
#include "sscTransform3D.h"
#include "sscToolRep3D.h"
#include "sscDummyToolManager.h"
#include "sscDummyTool.h"
#include "sscSliceProxy.h"
#include "sscSlicerRepSW.h"
#include "sscTestSliceAndToolRep.h"

using ssc::Vector3D;
using ssc::Transform3D;

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	TestSliceAndToolRep test;

	int val = app.exec();
	return val;
}

TestSliceAndToolRep::TestSliceAndToolRep()
{
//	this->resize(QSize(1000, 800));
	this->resize(QSize(800, 600));
	this->show();
	mWidget = new QWidget;
	this->setCentralWidget( mWidget );

	start();

}
TestSliceAndToolRep::~TestSliceAndToolRep()
{}

void TestSliceAndToolRep::generateView(const std::string& uid)
{
	mLayouts[uid].mView = new ssc::View();
}

void TestSliceAndToolRep::generateSlice(const std::string& uid, ssc::ToolPtr tool, ssc::ImagePtr image, ssc::PLANE_TYPE plane)
{
	ssc::SliceProxyPtr proxy(new ssc::SliceProxy());
	proxy->setTool(tool);
	proxy->setFollowType(ssc::ftFIXED_CENTER);
	proxy->setOrientation(ssc::otORTHOGONAL);
	proxy->setPlane(plane);

	ssc::SliceRepSWPtr rep = ssc::SliceRepSW::New(uid);
	rep->setImage(image);
	rep->setSliceProxy(proxy);

	mLayouts[uid].mSlicer = proxy;
	mLayouts[uid].mSliceRep = rep;
	mLayouts[uid].mView->addRep(rep);
}

ssc::View* TestSliceAndToolRep::view(const std::string& uid)
{
	return mLayouts[uid].mView;
}

void TestSliceAndToolRep::start()
{
	
	// generate imageFileName
	//std::string imageFileName1 = ssc::TestUtilities::ExpandDataFileName("Fantomer/Kaisa/MetaImage/Kaisa.mhd");
	imageFileName1 = ssc::TestUtilities::ExpandDataFileName("MetaImage/20070309T105136_MRT1.mhd");
	//std::string imageFileName1 = ssc::TestUtilities::ExpandDataFileName("MetaImage/20070309T102309_MRA.mhd");
	std::cout << imageFileName1 << std::endl;

	// read image
	ssc::ImagePtr image1 = ssc::DataManager::getInstance()->loadImage(imageFileName1, ssc::rtMETAIMAGE);

	generateView("A");
	generateView("C");
	generateView("S");
	generateView("3D");

	// volume rep
	ssc::VolumetricRepPtr mRepPtr = ssc::VolumetricRep::New( image1->getUid() );
	mRepPtr->setImage(image1);
	mRepPtr->setName(image1->getName());
	view("3D")->addRep(mRepPtr);


	//ssc::AxesRepPtr axesRep = ssc::AxesRep::New("AxesRepUID");
	//view->addRep(axesRep);

	// Initialize dummy toolmanager.
	ssc::ToolManager* mToolmanager = ssc::DummyToolManager::getInstance();
	mToolmanager->configure();
	mToolmanager->initialize();
	mToolmanager->startTracking();

	ssc::ToolPtr tool = mToolmanager->getDominantTool();
	connect( tool.get(), SIGNAL( toolTransformAndTimestamp(Transform3D ,double) ), this, SLOT( updateRender()));


	// Tool 3D rep
	ssc::ToolRep3DPtr toolRep = ssc::ToolRep3D::New( tool->getUid(), tool->getName() );
	toolRep->setTool(tool);
	view("3D")->addRep(toolRep);

	// Slices
	generateSlice("A", tool, image1, ssc::ptAXIAL);
	generateSlice("C", tool, image1, ssc::ptCORONAL);
	generateSlice("S", tool, image1, ssc::ptSAGITTAL);

	//gui controll
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mWidget->setLayout(mainLayout);

	QGridLayout* sliceLayout = new QGridLayout;
	

	mainLayout->addLayout(sliceLayout);//Slice layout
	sliceLayout->addWidget(view("C"), 0, 0);
	sliceLayout->addWidget(view("S"), 0, 1);
	sliceLayout->addWidget(view("A"), 1, 0);
	sliceLayout->addWidget(view("3D"), 1, 1);

	QHBoxLayout *controllLayout = new QHBoxLayout;
	
	QVBoxLayout *slidersLayout = new QVBoxLayout;
	mBrightnessSlider = new QSlider(Qt::Horizontal);
	mBrightnessSlider->setTickInterval(10);
	mContrastSlider= new QSlider(Qt::Horizontal);
	mContrastSlider->setTickInterval(10);
	slidersLayout->addWidget(new QLabel(tr("Brightness")) );
	slidersLayout->addWidget(mContrastSlider);
	slidersLayout->addWidget(new QLabel(tr("Contrasst")) );
	slidersLayout->addWidget(mBrightnessSlider);	
	controllLayout->addLayout(slidersLayout);
	
	mainLayout->addLayout(controllLayout); //Buttons
	
	connect(mContrastSlider, SIGNAL(sliderMoved(int)), this, SLOT(contrast(int)) );	
	connect(mBrightnessSlider, SIGNAL(sliderMoved(int)), this, SLOT(brightness(int)) );
	
	//update slidebar...
	//int window = (int)10 * image1->lookupTable2D().getWindow();
	//int level = (int)10 * image1->lookupTable2D().getLevel();
	
	mContrastSlider->setMaximum(256);
	mContrastSlider->setValue(256);
			
	mBrightnessSlider->setMaximum(256);
	mBrightnessSlider->setValue(128);
	
	for (LayoutMap::iterator iter=mLayouts.begin(); iter!=mLayouts.end(); ++iter)
		iter->second.mView->getRenderer()->ResetCamera();
	updateRender();
}
void TestSliceAndToolRep::contrast(int val)
{
	ssc::ImagePtr image1 = ssc::DataManager::getInstance()->getImage(imageFileName1);	
	
	image1->lookupTable2D().setWindow(val/1.0);	
}
	
void TestSliceAndToolRep::brightness(int val)
{
	ssc::ImagePtr image1 = ssc::DataManager::getInstance()->getImage(imageFileName1);
	image1->lookupTable2D().setLevel(val/1.0);
}

void TestSliceAndToolRep::updateRender()
{
//	std::cout<<"render begin"<<std::endl;
	for (LayoutMap::iterator iter=mLayouts.begin(); iter!=mLayouts.end(); ++iter)
		iter->second.mView->getRenderWindow()->Render();
//	std::cout<<"render end"<<std::endl;
}



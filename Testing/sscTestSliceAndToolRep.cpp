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
	//QMainWindow mainWindow;
	this->resize(QSize(1000, 800));
	this->show();
	mWidget = new QWidget;
	this->setCentralWidget( mWidget );
	
	start();
	
}
TestSliceAndToolRep::~TestSliceAndToolRep()
{}
void TestSliceAndToolRep::start()
{
	

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mWidget->setLayout(mainLayout);
		
	
	// generate imageFileName
	//std::string imageFileName1 = ssc::TestUtilities::ExpandDataFileName("Fantomer/Kaisa/MetaImage/Kaisa.mhd");
	std::string imageFileName1 = ssc::TestUtilities::ExpandDataFileName("MetaImage/20070309T105136_MRT1.mhd");
	//std::string imageFileName1 = ssc::TestUtilities::ExpandDataFileName("MetaImage/20070309T102309_MRA.mhd");
	std::cout << imageFileName1 << std::endl;

	// read image
	ssc::ImagePtr image1 = ssc::DataManager::instance()->loadImage(imageFileName1, ssc::rtMETAIMAGE);

	viewA = new ssc::View();
	viewC = new ssc::View();
	viewS = new ssc::View();
	view3D = new ssc::View();
	ssc::VolumetricRepPtr mRepPtr = ssc::VolumetricRep::New(image1->getUid() );
	mRepPtr->setImage(image1);
	mRepPtr->setName(image1->getName());
	view3D->addRep(mRepPtr);

	//ssc::AxesRepPtr axesRep = ssc::AxesRep::New("AxesRepUID");
	//view->addRep(axesRep);
	/*3D Tool */
	ssc::ToolManager* mToolmanager = ssc::DummyToolManager::getInstance();
	mToolmanager->configure();
	mToolmanager->initialize();
	mToolmanager->startTracking();

	ssc::ToolPtr tool = mToolmanager->getDominantTool();
	connect( tool.get(), SIGNAL( toolTransformAndTimestamp(Transform3D ,double) ), this, SLOT( updateRender()));
	
	ssc::ToolRep3DPtr toolRep = ssc::ToolRep3D::New( tool->getUid(), tool->getName() );
	toolRep->setTool(tool);
	view3D->addRep(toolRep);

	//ssc::DummyToolPtr tool( new ssc::DummyTool() );

	ssc::SliceRepSWPtr axialRep = ssc::SliceRepSW::New("Axial");
	axialRep->setImage(image1);
	axialRep->setTool(tool);
	axialRep->setOrientation(ssc::SliceComputer::ptAXIAL);
	axialRep->setFollowType(ssc::SliceComputer::ftFIXED_CENTER);
	//axialRep->setFollowType(ssc::SliceComputer::ftFOLLOW_TOOL);
	viewA->addRep(axialRep);

	ssc::SliceRepSWPtr sagittalRep = ssc::SliceRepSW::New("Sagittal");
	sagittalRep->setImage(image1);
	sagittalRep->setTool(tool);
	sagittalRep->setOrientation(ssc::SliceComputer::ptSAGITTAL);
	viewS->addRep(sagittalRep);

	ssc::SliceRepSWPtr CoronalRep = ssc::SliceRepSW::New("Coronal");
	CoronalRep->setImage(image1);
	CoronalRep->setTool(tool);
	CoronalRep->setOrientation(ssc::SliceComputer::ptCORONAL);
	viewC->addRep(CoronalRep);

	QGridLayout* sliceLayout = new QGridLayout;
	QHBoxLayout *buttonLayout = new QHBoxLayout;
	mainLayout->addLayout(sliceLayout);//Slice layout
	sliceLayout->addWidget(viewC, 0, 0);
	sliceLayout->addWidget(viewS, 0, 1);
	sliceLayout->addWidget(viewA, 1, 0);
	sliceLayout->addWidget(view3D, 1, 1);
	mainLayout->addLayout(buttonLayout); //Buttons
	viewA->getRenderer()->ResetCamera();
	viewC->getRenderer()->ResetCamera();
	viewS->getRenderer()->ResetCamera();
	view3D->getRenderer()->ResetCamera();
	updateRender();
}
void TestSliceAndToolRep::updateRender()
{
	std::cout<<"render"<<std::endl;
	viewA->getRenderWindow()->Render();
	viewC->getRenderWindow()->Render();
	viewS->getRenderWindow()->Render();
	view3D->getRenderWindow()->Render();

}



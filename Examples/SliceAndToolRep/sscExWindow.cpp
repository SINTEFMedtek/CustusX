#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include <vtkImageData.h>
#include <vtkMetaImageReader.h>
#include <vtkImagePlaneWidget.h>
#include <vtkRenderer.h>
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkVolumeTextureMapper3D.h"
//#include <vtkGPUVolumeRayCastMapper.h>

#include "vtkCamera.h"

#include "sscTestUtilities.h"
#include "sscDataManager.h"
#include "sscImage.h"
#include "sscAxesRep.h"
#include "sscSliceRep.h"

#include "sscVolumetricRep.h"
//#include "sscGPURayCastVolumetricRep.h"
#include "sscSliceComputer.h"
#include "sscVector3D.h"
#include "sscTransform3D.h"
#include "sscToolRep3D.h"
#include "sscDummyToolManager.h"
#include "sscDummyTool.h"
#include "sscSliceProxy.h"
#include "sscSlicerRepSW.h"
#include "sscExWindow.h"
#include "sscTypeConversions.h"

using ssc::Vector3D;
using ssc::Transform3D;


//---------------------------------------------------------
//---------------------------------------------------------

//---------------------------------------------------------
//---------------------------------------------------------

namespace {
	ssc::DummyToolPtr dummyTool()
	{
		return boost::dynamic_pointer_cast<ssc::DummyTool>(ssc::ToolManager::getInstance()->getDominantTool());
	}
}


ViewsWindow::ViewsWindow() 
{
	QRect screen = qApp->desktop()->screen()->rect();
	this->show();
	this->setGeometry(QRect(screen.bottomRight()*0.15, screen.bottomRight()*0.85));
//	this->setCentralWidget( new QWidget() );

  // Initialize dummy toolmanager.
  ssc::ToolManager* mToolmanager = ssc::DummyToolManager::getInstance();
  mToolmanager->configure();
  mToolmanager->initialize();
 mToolmanager->startTracking();

  ssc::ToolPtr tool = mToolmanager->getDominantTool();
  connect( tool.get(), SIGNAL( toolTransformAndTimestamp(Transform3D ,double) ), this, SLOT( updateRender()));

//  mDICOMWidget = new ssc::DICOMWidget(this);

  mSliceLayout = new QGridLayout(this);
  mSpeedEdit = new QLineEdit(this);
  mSliceLayout->addWidget(mSpeedEdit, 1, 0);
//  mSliceLayout->addWidget(mDICOMWidget, 2, 0);

//  ssc::View* view = new ssc::View(this);
//  view->getRenderWindow()->GetInteractor()->EnableRenderOff();
////  this->setCentralWidget(view);
//  mLayouts.insert(view);
//
//  mSliceLayout->addWidget(view, 0,0);
//  view->show();
////  mSliceLayout->addWidget(new QLabel("tekst", this), 0, 1);

  std::cout << "started" << std::endl;

  mRenderingTimer = new QTimer(this);
  mRenderingTimer->start(33);
  connect(mRenderingTimer, SIGNAL(timeout()),
          this, SLOT(updateRender()));
}

ViewsWindow::~ViewsWindow()
{
}

ssc::View* ViewsWindow::generateSlice(const QString& uid, ssc::ToolPtr tool, ssc::ImagePtr image, ssc::PLANE_TYPE plane)
{
	ssc::View* view = new ssc::View(this);
	mLayouts.insert(view);

	ssc::SliceProxyPtr proxy(new ssc::SliceProxy());
	proxy->setTool(tool);
	
	proxy->initializeFromPlane(plane, false, Vector3D(0,0,-1), false, 1, 0);
	
	ssc::SliceRepSWPtr rep = ssc::SliceRepSW::New(uid);
	rep->setImage(image);
	rep->setSliceProxy(proxy);

	view->addRep(rep);
	return view;
}

void ViewsWindow::defineSlice(const QString& uid, const QString& imageFilename, ssc::PLANE_TYPE plane, int r, int c)
{
	ssc::ToolManager* mToolmanager = ssc::DummyToolManager::getInstance();
	ssc::ToolPtr tool = mToolmanager->getDominantTool();

	ssc::ImagePtr image = loadImage(imageFilename);
	
	ssc::View* view = generateSlice(uid, tool, image, plane);
	insertView(view, uid, imageFilename, r, c);
	view->getRenderer()->ResetCamera();
}

ssc::ImagePtr ViewsWindow::loadImage(const QString& imageFilename)
{
	QString filename = ssc::TestUtilities::ExpandDataFileName(imageFilename);
	ssc::ImagePtr image = ssc::DataManager::getInstance()->loadImage(filename, filename, ssc::rtMETAIMAGE);
	Vector3D center = image->boundingBox().center();
	center = image->get_rMd().coord(center);
	ssc::DataManager::getInstance()->setCenter(center);
	
	// side effect: set tool movement box to data box,
	dummyTool()->setToolPositionMovementBB(image->boundingBox());
	
	return image;
}

void ViewsWindow::insertView(ssc::View* view, const QString& uid, const QString& volume, int r, int c)
{
  view->getRenderWindow()->GetInteractor()->EnableRenderOff();
  view->getRenderer()->GetActiveCamera();
	QVBoxLayout* layout = new QVBoxLayout;
	mSliceLayout->addLayout(layout, r,c);
	
	layout->addWidget(view);
	layout->addWidget(new QLabel(uid+" "+volume, this));
}

void ViewsWindow::defineGPU_3D(const QString& imageFilename, int r, int c)
{
//	QString uid = "3D";
//	ssc::View* view = new ssc::View(this);
//	mLayouts.insert(view);
//
//	ssc::ImagePtr image = loadImage(imageFilename);
//
//	// volume rep
//	ssc::GPURayCastVolumetricRepPtr mRepPtr = ssc::GPURayCastVolumetricRep::New(image->getUid());
//	mRepPtr->setImage(image);
//	mRepPtr->setName(image->getName());
//	view->addRep(mRepPtr);
//
//	// Tool 3D rep
//	ssc::ToolManager* mToolmanager = ssc::DummyToolManager::getInstance();
//	ssc::ToolPtr tool = mToolmanager->getDominantTool();
//	ssc::ToolRep3DPtr toolRep = ssc::ToolRep3D::New( tool->getUid(), tool->getName() );
//	toolRep->setTool(tool);
//	view->addRep(toolRep);
//
//	insertView(view, uid, imageFilename, r, c);
//
//	view->getRenderer()->ResetCamera();
}


void ViewsWindow::define3D(const QString& imageFilename, int r, int c)
{
	ssc::ImagePtr image = loadImage(imageFilename);
	this->define3D(image, r, c);
}

void ViewsWindow::define3D(ssc::ImagePtr image, int r, int c)
{
	QString uid = "3D";
	ssc::View* view = new ssc::View(this);
	mLayouts.insert(view);

//	ssc::ImagePtr image = loadImage(imageFilename);

	// volume rep
	ssc::VolumetricRepPtr mRepPtr = ssc::VolumetricRep::New(image->getUid());
	mRepPtr->setMaxVolumeSize(5 * 1000 * 1000);
	mRepPtr->setUseGPUVolumeRayCastMapper();
	//mRepPtr->setResampleFactor(0.2);
	mRepPtr->setImage(image);
	mRepPtr->setName(image->getName());
	view->addRep(mRepPtr);
	//mVolumetricRep = mRepPtr;

	// Tool 3D rep
	ssc::ToolManager* mToolmanager = ssc::DummyToolManager::getInstance();
	ssc::ToolPtr tool = mToolmanager->getDominantTool();
	ssc::ToolRep3DPtr toolRep = ssc::ToolRep3D::New(tool->getUid(), tool->getName());
	toolRep->setTool(tool);
	view->addRep(toolRep);

	insertView(view, uid, image->getFilePath(), r, c);

	view->getRenderer()->ResetCamera();
}

void ViewsWindow::updateRender()
{
	std::vector<int> times;
	int sum = 0;
	//static int count = 0;


	for (LayoutMap::iterator iter = mLayouts.begin(); iter != mLayouts.end(); ++iter)
	{
		if ((*iter)->isVisible())
		{
			QTime time = QTime::currentTime();

			(*iter)->getRenderWindow()->Render(); // previous version: renders even when nothing is changed
			//  (*iter)->render(); // render only changed scenegraph
			int t_render = time.msecsTo(QTime::currentTime());
			sum += t_render;
			times.push_back(t_render);
		}
	}

//	if (!count)
//	{
//		typedef vtkGPUVolumeRayCastMapper MapperType;
//		MapperType* mapper = MapperType::SafeDownCast(mVolumetricRep->getVtkVolume()->GetMapper());
//		int valid = mapper->IsRenderSupported((*mLayouts.begin())->getRenderWindow(),
//				mVolumetricRep->getVtkVolume()->GetProperty());
//		//	vtkVolumeTextureMapper3D* mapper = vtkVolumeTextureMapper3D::SafeDownCast(mVolumetricRep->getVtkVolume()->GetMapper());
//		//	mapper->Print(std::cout);
//		std::cout << "is render supported: " << valid << std::endl;
//	}
//	else
//	{
//		return;
//	}
//	++count;
//
//
//	mVolumetricRep->getVtkVolume()->GetMapper()->Print(std::cout);

	QString text = qstring_cast(sum);
	for (unsigned i = 0; i < times.size(); ++i)
	{
		text += "\t" + qstring_cast(times[i]);
	}

	//std::cout << "render:\t" << text << std::endl;
	mSpeedEdit->setText(qstring_cast(text));
}



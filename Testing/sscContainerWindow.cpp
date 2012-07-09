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
#include "vtkCamera.h"
#include "vtkLookupTable.h"

#include "sscTestUtilities.h"
#include "sscDataManager.h"
#include "sscImage.h"
#include "sscAxesRep.h"
#include "sscSliceRep.h"
#include "sscImageTF3D.h"
#include "sscVolumetricRep.h"
#include "sscSliceComputer.h"
#include "sscVector3D.h"
#include "sscTransform3D.h"
#include "sscToolRep3D.h"
#include "sscDummyToolManager.h"
#include "sscDummyTool.h"
#include "sscSliceProxy.h"
#include "sscSlicerRepSW.h"
#include "sscTexture3DSlicerRep.h"
#include "sscGPURayCastVolumeRep.h"
#include "sscContainerWindow.h"
#include "sscImageTF3D.h"

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

ContainerWindow::ContainerWindow(QString displayText, bool showSliders) : mDisplayText(displayText)
{
	mZoomFactor = 1;
	mDumpSpeedData = false;
	mRenderCount = 0;
	mTotalRender = 0;
	mTotalOther = 0;
	mLastRenderEnd = QTime::currentTime();
	mShaderFolder = qApp->applicationDirPath() + "/../Code/Rep/";
	
	QRect screen = qApp->desktop()->screen()->rect();
	this->setGeometry(QRect(screen.bottomRight()*0.15, screen.bottomRight()*0.85));
	this->setCentralWidget( new QWidget(this) );

	start(showSliders);
}

void ContainerWindow::setDescription(const QString& desc)
{
	mAcceptanceBox->setText(desc);
}

ContainerWindow::~ContainerWindow()
{
}

void ContainerWindow::containerGPUSlice(ssc::ViewItem *view, const QString &uid, const QString &imageFilename, ssc::PLANE_TYPE plane)
{
	ssc::ToolManager *mToolmanager = ssc::DummyToolManager::getInstance();
	ssc::ToolPtr tool = mToolmanager->getDominantTool();
	ssc::ImagePtr image = loadImage(imageFilename);
	view->getRenderer()->GetActiveCamera()->ParallelProjectionOn();
	view->getRenderWindow()->GetInteractor()->Disable();
	view->setZoomFactor(mZoomFactor);
	mLayouts.insert(view);
	ssc::SliceProxyPtr proxy(new ssc::SliceProxy());
	proxy->setTool(tool);
	proxy->initializeFromPlane(plane, false, Vector3D(0,0,-1), false, 1, 0);
	ssc::Texture3DSlicerRepPtr rep = ssc::Texture3DSlicerRep::New(uid);
	rep->setShaderFile(mShaderFolder + "Texture3DOverlay.frag");
	rep->setSliceProxy(proxy);
	rep->setImages(std::vector<ssc::ImagePtr>(1, image));
	view->addRep(rep);
}

ssc::ImagePtr ContainerWindow::loadImage(const QString& imageFilename)
{
	QString filename = ssc::TestUtilities::ExpandDataFileName(imageFilename);
	ssc::ImagePtr image = ssc::DataManager::getInstance()->loadImage(filename, filename, ssc::rtMETAIMAGE);
	Vector3D center = image->boundingBox().center();
	center = image->get_rMd().coord(center);
	ssc::DataManager::getInstance()->setCenter(center);
	
	// side effect: set tool movement box to data box,
	dummyTool()->setToolPositionMovementBB(transform(image->get_rMd(), image->boundingBox()));
	
	return image;
}

void ContainerWindow::setupViewContainer(ssc::ViewContainer *view, const QString& uid, const QString& volume, int r, int c)
{
	QVBoxLayout *layout = new QVBoxLayout;
	mSliceLayout->addLayout(layout, r, c);
	layout->addWidget(view);
	layout->addWidget(new QLabel(uid+" "+volume, this));
	mContainer = view;
}

void ContainerWindow::container3D(ssc::ViewItem *view, const QString& imageFilename)
{
	ssc::ImagePtr image = loadImage(imageFilename);
	mLayouts.insert(view);

	// volume rep
	ssc::VolumetricRepPtr mRepPtr = ssc::VolumetricRep::New(image->getUid());
	mRepPtr->setMaxVolumeSize(10*1000*1000);
	mRepPtr->setUseGPUVolumeRayCastMapper(); // if available
	mRepPtr->setImage(image);
	mRepPtr->setName(image->getName());
	view->addRep(mRepPtr);

	// Tool 3D rep
	ssc::ToolManager* mToolmanager = ssc::DummyToolManager::getInstance();
	ssc::ToolPtr tool = mToolmanager->getDominantTool();
	ssc::ToolRep3DPtr toolRep = ssc::ToolRep3D::New(tool->getUid(), tool->getName());
	toolRep->setTool(tool);
	view->addRep(toolRep);
}

void ContainerWindow::containerGPU3D(ssc::ViewItem *view, const QStringList& imageFilenames, const ImageParameters* parameters)
{
	mLayouts.insert(view);

	std::vector<ssc::ImagePtr> images;

	double numImages = imageFilenames.size();
	for (int i = 0; i < numImages; ++i)
	{
		ssc::ImagePtr image = loadImage(imageFilenames[i]);
		if (parameters != NULL)
		{
			image->getTransferFunctions3D()->setLLR(parameters[i].llr);
			image->getTransferFunctions3D()->setAlpha(parameters[i].alpha);
			image->getTransferFunctions3D()->setLut(parameters[i].lut);
		}
		images.push_back(image);
	}

	// volume rep

#ifndef WIN32
	ssc::GPURayCastVolumeRepPtr mRepPtr = ssc::GPURayCastVolumeRep::New( images[0]->getUid() );
	mRepPtr->setShaderFolder(mShaderFolder);
	mRepPtr->setImages(images);
	mRepPtr->setName(images[0]->getName());
	view->addRep(mRepPtr);
#endif //WIN32

	// Tool 3D rep
	ssc::ToolManager* mToolmanager = ssc::DummyToolManager::getInstance();
	ssc::ToolPtr tool = mToolmanager->getDominantTool();
	ssc::ToolRep3DPtr toolRep = ssc::ToolRep3D::New( tool->getUid(), tool->getName() );
	toolRep->setTool(tool);
	view->addRep(toolRep);
}

void ContainerWindow::start(bool showSliders)
{
	// Initialize dummy toolmanager.
	ssc::ToolManager* mToolmanager = ssc::DummyToolManager::getInstance();
	mToolmanager->configure();
	mToolmanager->initialize();
	mToolmanager->startTracking();

	ssc::ToolPtr tool = mToolmanager->getDominantTool();
	connect( tool.get(), SIGNAL( toolTransformAndTimestamp(Transform3D ,double) ), this, SLOT( updateRender()));

	//gui controll
	QVBoxLayout *mainLayout = new QVBoxLayout;
	this->centralWidget()->setLayout(mainLayout);

	mSliceLayout = new QGridLayout;

	mainLayout->addLayout(mSliceLayout);//Slice layout

	QHBoxLayout *controlLayout = new QHBoxLayout;
	controlLayout->addStretch();
	
	mAcceptanceBox = new ssc::AcceptanceBoxWidget(mDisplayText, this);
	controlLayout->addWidget(mAcceptanceBox);

	controlLayout->addStretch();
	mainLayout->addLayout(controlLayout); //Buttons
}

void ContainerWindow::updateRender()
{
	for (std::set<ssc::View *>::iterator iter=mLayouts.begin(); iter!=mLayouts.end(); ++iter)
	{
		ssc::View *view = *iter;

		if (view->getZoomFactor()<0)
		  continue;

		ssc::DoubleBoundingBox3D bb_s  = view->getViewport_s();
		double viewportHeightmm = bb_s.range()[1];//viewPortHeightPix*mmPerPix(view);
		double parallelscale = viewportHeightmm/2/view->getZoomFactor();

		vtkCamera* camera = view->getRenderer()->GetActiveCamera();
		camera->SetParallelScale(parallelscale);
	}

	++mRenderCount;
	QTime pre = QTime::currentTime();
	int other = mLastRenderEnd.msecsTo(pre);

	mContainer->renderAll();

	mLastRenderEnd = QTime::currentTime();

	int render = pre.msecsTo(mLastRenderEnd);
	mTotalRender += render;
	mTotalOther += other;

	if (mDumpSpeedData)
	{
		if (mRenderCount%50==0)
		{
			std::cout << "averagerender: " << 1.0*mTotalRender/mRenderCount << "/" << 1.0*mTotalOther/mRenderCount << std::endl;
			std::cout << "fps: " << 1000*(float)mRenderCount/(mTotalRender + mTotalOther) << std::endl;
			mTotalRender = 0;
			mTotalOther = 0;
			mRenderCount = 0;
		}
	}
}

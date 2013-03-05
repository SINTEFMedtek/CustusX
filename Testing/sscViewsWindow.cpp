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
#include "sscViewsWindow.h"
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

vtkLookupTablePtr getCreateLut(int tableRangeMin, int tableRangeMax, double hueRangeMin, double hueRangeMax,
	double saturationRangeMin, double saturationRangeMax, double valueRangeMin, double valueRangeMax)
{
	vtkLookupTablePtr lut = vtkLookupTablePtr::New();
	lut->SetTableRange(tableRangeMin, tableRangeMax);
	lut->SetHueRange(hueRangeMin, hueRangeMax);
	lut->SetSaturationRange(saturationRangeMin, saturationRangeMax);
	lut->SetValueRange(valueRangeMin, valueRangeMax);
	lut->Build();

	return lut;
}

ViewsWindow::ViewsWindow(QString displayText, bool showSliders) : mDisplayText(displayText)
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

void ViewsWindow::setDescription(const QString& desc)
{
	mAcceptanceBox->setText(desc);
}

ViewsWindow::~ViewsWindow()
{
}

bool ViewsWindow::defineGPUSlice(const QString& uid, const QString& imageFilename, ssc::PLANE_TYPE plane, int r, int c)
{	
	ssc::ToolManager* mToolmanager = ssc::DummyToolManager::getInstance();
	ssc::ToolPtr tool = mToolmanager->getDominantTool();
	ssc::ImagePtr image = loadImage(imageFilename);
	ssc::ViewWidget* view = new ssc::ViewWidget(centralWidget());

	if (!view || !view->getRenderWindow() || !view->getRenderWindow()->SupportsOpenGL())
		return false;
	if (!ssc::Texture3DSlicerRep::isSupported(view->getRenderWindow()))
		return false;

	view->getRenderer()->GetActiveCamera()->ParallelProjectionOn();
	view->GetRenderWindow()->GetInteractor()->Disable();
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
	insertView(view, uid, imageFilename, r, c);

	return true;
}

void ViewsWindow::defineSlice(const QString& uid, const QString& imageFilename, ssc::PLANE_TYPE plane, int r, int c)
{
	ssc::ToolManager* mToolmanager = ssc::DummyToolManager::getInstance();
	ssc::ToolPtr tool = mToolmanager->getDominantTool();
	ssc::ImagePtr image = loadImage(imageFilename);
	ssc::ViewWidget* view = new ssc::ViewWidget(centralWidget());
	view->getRenderer()->GetActiveCamera()->ParallelProjectionOn();
	view->GetRenderWindow()->GetInteractor()->Disable();
	view->setZoomFactor(mZoomFactor);
	mLayouts.insert(view);
	ssc::SliceProxyPtr proxy(new ssc::SliceProxy());
	proxy->setTool(tool);
	proxy->initializeFromPlane(plane, false, Vector3D(0,0,-1), false, 1, 0);
	ssc::SliceRepSWPtr rep = ssc::SliceRepSW::New(uid);
	rep->setImage(image);
	rep->setSliceProxy(proxy);
	view->addRep(rep);
	insertView(view, uid, imageFilename, r, c);
}

ssc::ImagePtr ViewsWindow::loadImage(const QString& imageFilename)
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

void ViewsWindow::insertView(ssc::ViewWidget *view, const QString& uid, const QString& volume, int r, int c)
{
	QVBoxLayout *layout = new QVBoxLayout;
	mSliceLayout->addLayout(layout, r,c);

	layout->addWidget(view);
	layout->addWidget(new QLabel(uid+" "+volume, this));
}

void ViewsWindow::define3D(const QString& imageFilename, const ImageParameters* parameters, int r, int c)
{
	QString uid = "3D";
	ssc::ViewWidget* view = new ssc::ViewWidget(centralWidget());
	mLayouts.insert(view);
	
	ssc::ImagePtr image = loadImage(imageFilename);

	if (parameters != NULL)
	{
		image->getTransferFunctions3D()->setLLR(parameters->llr);
		image->getTransferFunctions3D()->setAlpha(parameters->alpha);
		image->getTransferFunctions3D()->setLut(parameters->lut);
	}

	// volume rep
	ssc::VolumetricRepPtr mRepPtr = ssc::VolumetricRep::New( image->getUid() );
	mRepPtr->setMaxVolumeSize(10*1000*1000);
	mRepPtr->setUseGPUVolumeRayCastMapper(); // if available
	mRepPtr->setImage(image);	
	mRepPtr->setName(image->getName());
	view->addRep(mRepPtr);
	
	// Tool 3D rep
	ssc::ToolManager* mToolmanager = ssc::DummyToolManager::getInstance();
	ssc::ToolPtr tool = mToolmanager->getDominantTool();
	ssc::ToolRep3DPtr toolRep = ssc::ToolRep3D::New( tool->getUid(), tool->getName() );
	toolRep->setTool(tool);
	view->addRep(toolRep);
	
	insertView(view, uid, imageFilename, r, c);
}

bool ViewsWindow::define3DGPU(const QStringList& imageFilenames, const ImageParameters* parameters, int r, int c)
{
	QString uid = "3D";
	ssc::ViewWidget* view = new ssc::ViewWidget(centralWidget());
	mLayouts.insert(view);

	std::vector<ssc::ImagePtr> images;

	double numImages = imageFilenames.size();
	for (int i = 0; i < numImages; ++i)
	{
		ssc::ImagePtr image = loadImage(imageFilenames[i]);
		if (!image)
		{
			return false;
		}
		if (parameters != NULL)
		{
			image->getTransferFunctions3D()->setLLR(parameters[i].llr);
			image->getTransferFunctions3D()->setAlpha(parameters[i].alpha);
			image->getTransferFunctions3D()->setLut(parameters[i].lut);
		}
		images.push_back(image);
	}

	// volume rep
	if (!ssc::GPURayCastVolumeRep::isSupported(view->getRenderWindow()))
		return false;
//#ifndef WIN32
	ssc::GPURayCastVolumeRepPtr mRepPtr = ssc::GPURayCastVolumeRep::New( images[0]->getUid() );
	mRepPtr->setShaderFolder(mShaderFolder);
	mRepPtr->setImages(images);
	mRepPtr->setName(images[0]->getName());
	view->addRep(mRepPtr);
//#endif //WIN32

	// Tool 3D rep
	ssc::ToolManager* mToolmanager = ssc::DummyToolManager::getInstance();
	ssc::ToolPtr tool = mToolmanager->getDominantTool();
	ssc::ToolRep3DPtr toolRep = ssc::ToolRep3D::New( tool->getUid(), tool->getName() );
	toolRep->setTool(tool);
	view->addRep(toolRep);
	
	insertView(view, uid, imageFilenames[0], r, c);

	return true;
}

void ViewsWindow::start(bool showSliders)
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

void ViewsWindow::updateRender()
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

	QSet<vtkRenderWindow *> windowList;
	for (LayoutMap::iterator iter=mLayouts.begin(); iter!=mLayouts.end(); ++iter)
	{
		windowList.insert((*iter)->getRenderWindow().GetPointer());
	}
	foreach (vtkRenderWindow *win, windowList)
	{
		win->Render();
	}

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

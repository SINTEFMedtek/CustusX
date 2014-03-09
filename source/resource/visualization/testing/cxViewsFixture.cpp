// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.
#include "cxViewsFixture.h"

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

#include "cxtestUtilities.h"
#include "sscDataManagerImpl.h"
#include "sscImage.h"
#include "sscAxesRep.h"
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
#include "cxViewsFixture.h"
#include "sscImageTF3D.h"
#include "cxDataLocations.h"
#include "cxtestRenderTester.h"
#include "sscLogger.h"
#include "cxViewsFixture.h"
#include "sscViewsWindow.h"

#include "catch.hpp"

using cx::Vector3D;
using cx::Transform3D;


namespace cxtest
{

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

ViewsFixture::ViewsFixture(QString displayText)
{
	mServices = cxtest::TestServices::create();
	mMessageListener = cx::MessageListener::create();

//	this->setDescription(displayText);
//	mZoomFactor = 1;
	mShaderFolder = cx::DataLocations::getShaderPath();
//	QRect screen = qApp->desktop()->screenGeometry(qApp->desktop()->primaryScreen());
//	screen.adjust(screen.width()*0.15, screen.height()*0.15, -screen.width()*0.15, -screen.height()*0.15);
//	this->setGeometry(screen);
//	this->setCentralWidget( new QWidget(this) );

	// Initialize dummy toolmanager.
	mServices->trackingService()->configure();
	mServices->trackingService()->initialize();
	mServices->trackingService()->startTracking();

	mWindow.reset(new ViewsWindow());
	mWindow->setDescription(displayText);

//	//gui controll
//	QVBoxLayout *mainLayout = new QVBoxLayout;
//	this->centralWidget()->setLayout(mainLayout);

//	mSliceLayout = new QGridLayout;

//	mainLayout->addLayout(mSliceLayout);//Slice layout

//	mRenderingTimer = new QTimer(this);
//	mRenderingTimer->start(33);
//	connect(mRenderingTimer, SIGNAL(timeout()), this, SLOT(updateRender()));
}

//void ViewsFixture::setDescription(const QString& desc)
//{
//	this->setWindowTitle(desc);
//}

ViewsFixture::~ViewsFixture()
{
	mWindow.reset();

//	mRenderingTimer->stop();
	mServices.reset();
	CHECK(!mMessageListener->containsErrors());
}

cx::DummyToolPtr ViewsFixture::dummyTool()
{
	return boost::dynamic_pointer_cast<cx::DummyTool>(mServices->trackingService()->getDominantTool());
}

cx::ViewWidget* ViewsFixture::addView(QString caption, int row, int col)
{
	return mWindow->addView(caption, row, col);
//	cx::ViewWidget* view = fixture.addView("empty", 0, 0);
//	cx::ViewWidget* view = new cx::ViewWidget(fixture.centralWidget());
//	fixture.insertView(view, "dummy", "none", 0, 0);
//	return view;
}


bool ViewsFixture::defineGPUSlice(const QString& uid, const QString& imageFilename, cx::PLANE_TYPE plane, int r, int c)
{
	std::vector<cx::ImagePtr> images(1);
	images[0] = this->loadImage(imageFilename);

	return this->defineGPUSlice(uid, images, plane, r, c);
}

bool ViewsFixture::defineGPUSlice(const QString& uid, const std::vector<cx::ImagePtr> images, cx::PLANE_TYPE plane, int r, int c)
{
	cx::ViewWidget* view = mWindow->add2DView("several images", r, c);
//	cx::ViewWidget* view = this->create2DView("several images", r, c);

//	if (!view || !view->getRenderWindow())
//		return false;

// always fails on mac:
//	if (!view->getRenderWindow()->SupportsOpenGL())
//		return false;

// gives error when called outside of a gl render loop
//	if (!cx::Texture3DSlicerRep::isSupported(view->getRenderWindow()))
//		return false;

	cx::SliceProxyPtr proxy = this->createSliceProxy(plane);
	cx::Texture3DSlicerRepPtr rep = cx::Texture3DSlicerRep::New(uid);
	rep->setShaderPath(mShaderFolder);
	rep->setSliceProxy(proxy);
	rep->setImages(images);
	view->addRep(rep);
//	insertView(view, uid, "several images", r, c);

	return true;
}

void ViewsFixture::defineSlice(const QString& uid, const QString& imageFilename, cx::PLANE_TYPE plane, int r, int c)
{
	cx::ViewWidget* view = mWindow->add2DView(imageFilename, r, c);
	cx::ImagePtr image = loadImage(imageFilename);

	cx::SliceProxyPtr proxy = this->createSliceProxy(plane);
	cx::SliceRepSWPtr rep = cx::SliceRepSW::New(uid);
	rep->setImage(image);
	rep->setSliceProxy(proxy);
	view->addRep(rep);
//	insertView(view, uid, imageFilename, r, c);
}

//cx::ViewWidget* ViewsFixture::create2DView(const QString& title, int r, int c)
//{
//	cx::ViewWidget* view = new cx::ViewWidget(centralWidget());

//	view->getRenderer()->GetActiveCamera()->ParallelProjectionOn();
//	view->GetRenderWindow()->GetInteractor()->Disable();
//	view->setZoomFactor(mZoomFactor);

//	return view;
//}

cx::SliceProxyPtr ViewsFixture::createSliceProxy(cx::PLANE_TYPE plane)
{
	cx::ToolPtr tool = mServices->trackingService()->getDominantTool();

	cx::SliceProxyPtr proxy = cx::SliceProxy::create(mServices->dataService());
	proxy->setTool(tool);
	proxy->initializeFromPlane(plane, false, Vector3D(0,0,-1), false, 1, 0);
	return proxy;
}

cx::ImagePtr ViewsFixture::loadImage(const QString& imageFilename)
{
	QString filename = cxtest::Utilities::getDataRoot(imageFilename);
	cx::ImagePtr image = mServices->dataService()->loadImage(filename, filename);
	Vector3D center = image->boundingBox().center();
	center = image->get_rMd().coord(center);
	mServices->dataService()->setCenter(center);

	if (!image)
		return cx::ImagePtr();

	// side effect: set tool movement box to data box,
	dummyTool()->setToolPositionMovementBB(transform(image->get_rMd(), image->boundingBox()));
	this->fixToolToCenter();

	return image;
}

void ViewsFixture::fixToolToCenter()
{
	Vector3D c = mServices->dataService()->getCenter();
	cx::Transform3D prMt = cx::createTransformTranslate(c);
	dummyTool()->setToolPositionMovement(std::vector<Transform3D>(1, prMt));
	dummyTool()->set_prMt(prMt);
}

//void ViewsFixture::insertView(cx::ViewWidget *view, const QString& uid, const QString& volume, int r, int c)
//{
////	view->GetRenderWindow()->SetErase(false);
////	view->GetRenderWindow()->SetDoubleBuffer(false);

//	QVBoxLayout *layout = new QVBoxLayout;
//	mSliceLayout->addLayout(layout, r,c);

//	mLayouts.push_back(view);
//	layout->addWidget(view);
//	layout->addWidget(new QLabel(uid+" "+volume, this));
//}

void ViewsFixture::define3D(const QString& imageFilename, const ImageParameters* parameters, int r, int c)
{
//	QString uid = "3D";
//	cx::ViewWidget* view = new cx::ViewWidget(centralWidget());
	cx::ViewWidget* view = mWindow->addView("3D "+imageFilename, r, c);

	cx::ImagePtr image = loadImage(imageFilename);
	this->applyParameters(image, parameters);

	// volume rep
	cx::VolumetricRepPtr mRepPtr = cx::VolumetricRep::New( image->getUid() );
	mRepPtr->setMaxVolumeSize(10*1000*1000);
	mRepPtr->setUseGPUVolumeRayCastMapper(); // if available
	mRepPtr->setImage(image);
	mRepPtr->setName(image->getName());
	view->addRep(mRepPtr);

//	insertView(view, uid, imageFilename, r, c);
}

void ViewsFixture::applyParameters(cx::ImagePtr image, const ImageParameters *parameters)
{
	if (!parameters)
		return;
	image->getTransferFunctions3D()->setLLR(parameters->llr);
	image->getTransferFunctions3D()->setAlpha(parameters->alpha);
//	image->getTransferFunctions3D()->setLut(parameters->lut);
}

//void ViewsFixture::updateRender()
//{
//	for (std::vector<cx::View *>::iterator iter=mLayouts.begin(); iter!=mLayouts.end(); ++iter)
//	{
//		this->prettyZoom(*iter);
//	}

//	for (unsigned i=0; i<mLayouts.size(); ++i)
//	{
//		mLayouts[i]->getRenderWindow()->Render();
//	}

////	for (unsigned i=0; i<mLayouts.size(); ++i)
////	{
////		mLayouts[i]->getRenderWindow()->Render();
////	}

////	if (mRemaindingRenderings>=0)
////	{
////		--mRemaindingRenderings;
////		if (mRemaindingRenderings<0)
////			QTimer::singleShot(0, qApp, SLOT(quit()));
////	}
//}

//void ViewsFixture::prettyZoom(cx::View *view)
//{
//	if (view->getZoomFactor()<0)
//	{
//		view->getRenderer()->ResetCamera();
//		return;
//	}

//	cx::DoubleBoundingBox3D bb_s  = view->getViewport_s();
//	double viewportHeightmm = bb_s.range()[1];//viewPortHeightPix*mmPerPix(view);
//	double parallelscale = viewportHeightmm/2/view->getZoomFactor();

//	vtkCamera* camera = view->getRenderer()->GetActiveCamera();
//	camera->SetParallelScale(parallelscale);
//}

RenderTesterPtr ViewsFixture::getRenderTesterForView(int viewIndex)
{
	vtkRenderWindowPtr renderWindow = mWindow->getView(viewIndex)->getRenderWindow();
	RenderTesterPtr renderTester = RenderTester::create(renderWindow);
	return renderTester;
}

void ViewsFixture::dumpDebugViewToDisk(QString text, int viewIndex)
{
	cxtest::RenderTesterPtr renderTester = this->getRenderTesterForView(viewIndex);
//	vtkRenderWindowPtr renderWindow = mWindow->getView(viewIndex)->getRenderWindow();
//	cxtest::RenderTesterPtr renderTester = cxtest::RenderTester::create(renderWindow);
	vtkImageDataPtr output = renderTester->getImageFromRenderWindow();
	renderTester->printFractionOfVoxelsAboveZero(text, output);
}

double ViewsFixture::getFractionOfBrightPixelsInView(int viewIndex, int threshold, int component)
{
	cxtest::RenderTesterPtr renderTester = this->getRenderTesterForView(viewIndex);
//	vtkRenderWindowPtr renderWindow = mWindow->getView(viewIndex)->getRenderWindow();
//	cxtest::RenderTesterPtr renderTester = cxtest::RenderTester::create(renderWindow);
	vtkImageDataPtr output = renderTester->getImageFromRenderWindow();
//	vtkImageDataPtr output = renderTester->renderToImage();
	return cxtest::Utilities::getFractionOfVoxelsAboveThreshold(output, threshold,component);
}

bool ViewsFixture::quickRunWidget()
{
	return mWindow->quickRunWidget();
//	this->show();
//	this->updateRender();

//	return true;
}

//bool ViewsFixture::quickRunWidget()
//{
//	this->show();
//	this->updateRender();

//	return true;
//}


} // namespace cxtest


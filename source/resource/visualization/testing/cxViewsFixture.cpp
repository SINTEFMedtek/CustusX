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

#include "vtkLookupTable.h"
#include "cxtestUtilities.h"
#include "sscDataManager.h"
#include "sscImage.h"
#include "sscImageTF3D.h"
#include "sscVolumetricRep.h"
#include "sscDummyToolManager.h"
#include "sscSliceProxy.h"
#include "sscSlicerRepSW.h"
#include "sscTexture3DSlicerRep.h"
#include "cxDataLocations.h"
#include "cxtestRenderTester.h"
#include "sscViewsWindow.h"

#include "catch.hpp"

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

	mShaderFolder = cx::DataLocations::getShaderPath();

	// Initialize dummy toolmanager.
	mServices->trackingService()->configure();
	mServices->trackingService()->initialize();
	mServices->trackingService()->startTracking();

	mWindow.reset(new ViewsWindow());
	mWindow->setDescription(displayText);
}

ViewsFixture::~ViewsFixture()
{
	mWindow.reset();

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

	cx::SliceProxyPtr proxy = this->createSliceProxy(plane);
	cx::Texture3DSlicerRepPtr rep = cx::Texture3DSlicerRep::New(uid);
	rep->setShaderPath(mShaderFolder);
	rep->setSliceProxy(proxy);
	rep->setImages(images);
	view->addRep(rep);

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
}

cx::SliceProxyPtr ViewsFixture::createSliceProxy(cx::PLANE_TYPE plane)
{
	cx::ToolPtr tool = mServices->trackingService()->getDominantTool();

	cx::SliceProxyPtr proxy = cx::SliceProxy::create(mServices->dataService());
	proxy->setTool(tool);
	proxy->initializeFromPlane(plane, false, cx::Vector3D(0,0,-1), false, 1, 0);
	return proxy;
}

cx::ImagePtr ViewsFixture::loadImage(const QString& imageFilename)
{
	QString filename = cxtest::Utilities::getDataRoot(imageFilename);
	cx::ImagePtr image = mServices->dataService()->loadImage(filename, filename);
	cx::Vector3D center = image->boundingBox().center();
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
	cx::Vector3D c = mServices->dataService()->getCenter();
	cx::Transform3D prMt = cx::createTransformTranslate(c);
	dummyTool()->setToolPositionMovement(std::vector<cx::Transform3D>(1, prMt));
	dummyTool()->set_prMt(prMt);
}

void ViewsFixture::define3D(const QString& imageFilename, const ImageParameters* parameters, int r, int c)
{
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
}

void ViewsFixture::applyParameters(cx::ImagePtr image, const ImageParameters *parameters)
{
	if (!parameters)
		return;
	image->getTransferFunctions3D()->setLLR(parameters->llr);
	image->getTransferFunctions3D()->setAlpha(parameters->alpha);
}

RenderTesterPtr ViewsFixture::getRenderTesterForView(int viewIndex)
{
	vtkRenderWindowPtr renderWindow = mWindow->getView(viewIndex)->getRenderWindow();
	RenderTesterPtr renderTester = RenderTester::create(renderWindow);
	return renderTester;
}

void ViewsFixture::dumpDebugViewToDisk(QString text, int viewIndex)
{
	cxtest::RenderTesterPtr renderTester = this->getRenderTesterForView(viewIndex);
	vtkImageDataPtr output = renderTester->getImageFromRenderWindow();
	renderTester->printFractionOfVoxelsAboveZero(text, output);
}

double ViewsFixture::getFractionOfBrightPixelsInView(int viewIndex, int threshold, int component)
{
	cxtest::RenderTesterPtr renderTester = this->getRenderTesterForView(viewIndex);
	vtkImageDataPtr output = renderTester->getImageFromRenderWindow();
	return cxtest::Utilities::getFractionOfVoxelsAboveThreshold(output, threshold,component);
}

bool ViewsFixture::quickRunWidget()
{
	return mWindow->quickRunWidget();
}

} // namespace cxtest


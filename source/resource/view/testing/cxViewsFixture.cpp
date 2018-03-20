/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxViewsFixture.h"

#include "vtkLookupTable.h"
#include "cxtestUtilities.h"
#include "cxImage.h"
#include "cxImageTF3D.h"
#include "cxVolumetricRep.h"
#include "cxDummyToolManager.h"
#include "cxSliceProxy.h"
#include "cxSlicerRepSW.h"
#include "cxTexture3DSlicerRep.h"
#include "cxDataLocations.h"
#include "cxtestRenderTester.h"
#include "cxViewsWindow.h"
#include <QApplication>
#include "cxPatientModelService.h"
#include <vtkOpenGLRenderWindow.h>
#include "cxSharedOpenGLContext.h"
#include "cxRenderWindowFactory.h"

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
	mServices = cxtest::TestVisServices::create();
	mMessageListener = cx::MessageListener::createWithQueue();

	mShaderFolder = cx::DataLocations::findConfigFolder("/shaders");

	// Initialize dummy toolmanager.
	mServices->tracking()->setState(cx::Tool::tsTRACKING);

	mFactory = cx::RenderWindowFactoryPtr(new cx::RenderWindowFactory());

	mWindow.reset(new ViewsWindow(mFactory));//TODO: Create moc viewService with RenderWindowFactory?
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
	return boost::dynamic_pointer_cast<cx::DummyTool>(mServices->tracking()->getActiveTool());
}

void ViewsFixture::clear()
{
	mWindow->clearLayoutWidget();
}

cx::ViewPtr ViewsFixture::addView(int row, int col)
{
	return mWindow->addView(cx::View::VIEW, row, col);
}

bool ViewsFixture::defineGPUSlice(const QString& uid, const QString& imageFilename, cx::PLANE_TYPE plane, int r, int c)
{
	std::vector<cx::ImagePtr> images(1);
	images[0] = this->loadImage(imageFilename);

	return this->defineGPUSlice(uid, images, plane, r, c);
}

bool ViewsFixture::defineGPUSlice(const QString& uid, const std::vector<cx::ImagePtr> images, cx::PLANE_TYPE plane, int r, int c)
{
	cx::ViewPtr view = mWindow->add2DView(r, c);

	cx::SliceProxyPtr proxy = this->createSliceProxy(plane);
	cx::SharedOpenGLContextPtr sharedOpenGLContext = mFactory->getSharedOpenGLContext();
	cx::Texture3DSlicerRepPtr rep = cx::Texture3DSlicerRep::New(sharedOpenGLContext, uid);
	rep->setShaderPath(mShaderFolder);
	rep->setSliceProxy(proxy);
	rep->setImages(images);
	view->addRep(rep);

	return true;
}

void ViewsFixture::defineSlice(const QString& uid, const QString& imageFilename, cx::PLANE_TYPE plane, int r, int c)
{
	cx::ViewPtr view = mWindow->add2DView(r, c);
//	return;

	cx::ImagePtr image = loadImage(imageFilename);

	cx::SliceProxyPtr proxy = this->createSliceProxy(plane);
	cx::SliceRepSWPtr rep = cx::SliceRepSW::New(uid);
	rep->setImage(image);
	rep->setSliceProxy(proxy);
	view->addRep(rep);
}

cx::SliceProxyPtr ViewsFixture::createSliceProxy(cx::PLANE_TYPE plane)
{
	cx::ToolPtr tool = mServices->tracking()->getActiveTool();

	cx::SliceProxyPtr proxy = cx::SliceProxy::create(mServices->patient());
	proxy->setTool(tool);
	proxy->initializeFromPlane(plane, false, false, 1, 0);
	return proxy;
}

cx::ImagePtr ViewsFixture::loadImage(const QString& imageFilename)
{
	QString filename = cxtest::Utilities::getDataRoot(imageFilename);
	QString dummy;
	cx::DataPtr data = mServices->patient()->importData(filename, dummy);
	cx::ImagePtr image = boost::dynamic_pointer_cast<cx::Image>(data);
	cx::Vector3D center = image->boundingBox().center();
	center = image->get_rMd().coord(center);
	mServices->patient()->setCenter(center);

	if (!image)
		return cx::ImagePtr();

	// side effect: set tool movement box to data box,
	dummyTool()->setToolPositionMovementBB(transform(image->get_rMd(), image->boundingBox()));
	this->fixToolToCenter();

	return image;
}

void ViewsFixture::fixToolToCenter()
{
	cx::Vector3D c = mServices->patient()->getCenter();
	cx::Transform3D prMt = cx::createTransformTranslate(c);
	dummyTool()->setToolPositionMovement(std::vector<cx::Transform3D>(1, prMt));
	dummyTool()->set_prMt(prMt);
}

void ViewsFixture::define3D(const QString& imageFilename, const ImageParameters* parameters, int r, int c)
{
	cx::ViewPtr view = mWindow->addView(cx::View::VIEW_3D, r, c);
//	return;

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

RenderTesterPtr ViewsFixture::getRenderTesterForRenderWindow(int viewIndex)
{
	cx::ViewPtr view = mWindow->getView(viewIndex);
//	vtkRenderWindowPtr renderWindow = mWindow->getView(viewIndex)->getRenderWindow();
	RenderTesterPtr renderTester = RenderTester::create(view->getRenderWindow(), view->getRenderer());
	return renderTester;
}

void ViewsFixture::dumpDebugViewToDisk(QString text, int viewIndex)
{
	cxtest::RenderTesterPtr renderTester = this->getRenderTesterForRenderWindow(viewIndex);
//	vtkImageDataPtr output = renderTester->getImageFromRenderWindow();
	vtkImageDataPtr output = renderTester->renderToImage();
	renderTester->printFractionOfVoxelsAboveZero(text, output);
}

double ViewsFixture::getFractionOfBrightPixelsInRenderWindowForView(int viewIndex, int threshold, int component)
{
	cxtest::RenderTesterPtr renderTester = this->getRenderTesterForRenderWindow(viewIndex);
	vtkImageDataPtr output = renderTester->renderToImage();
//	vtkImageDataPtr output = renderTester->getImageFromRenderWindow();
	return cxtest::Utilities::getFractionOfVoxelsAboveThreshold(output, threshold,component);
}

bool ViewsFixture::quickRunWidget()
{	
	return mWindow->quickRunWidget();
}

bool ViewsFixture::runWidget()
{
	mWindow->show();
	return qApp->exec();
}


} // namespace cxtest


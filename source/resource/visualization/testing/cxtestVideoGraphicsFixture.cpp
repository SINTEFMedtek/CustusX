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

#include "cxtestVideoGraphicsFixture.h"

#include "cxVideoGraphics.h"
#include <vtkImageActor.h>
#include <vtkCamera.h>
#include <vtkImageData.h>
#include <vtkActor.h>

#include "cxBoundingBox3D.h"
#include "cxView.h"
#include "cxTypeConversions.h"

#include "cxProbeData.h"
#include "cxProbeSector.h"
#include "cxDummyTool.h"
#include "cxUtilHelpers.h"
#include "cxXmlOptionItem.h"
#include "cxtestUtilities.h"

#include "catch.hpp"

namespace cxtest
{

VideoGraphicsFixture::VideoGraphicsFixture()
{
	mMachine = cxtest::RenderTester::create();
	mVideoGraphics.reset(new cx::VideoGraphics());
}

vtkImageDataPtr VideoGraphicsFixture::readImageData(QString filename, QString description)
{
	QString path = cxtest::Utilities::getDataRoot("testing/videographics/"+filename);

	vtkImageDataPtr retval = mMachine->readFromFile(path);
	INFO(("Looking for "+description).toStdString());
	REQUIRE(retval);
	return retval;
}

cx::ProbeDefinition VideoGraphicsFixture::readProbeData(QString filename)
{
	QString path = cxtest::Utilities::getDataRoot("testing/videographics/"+filename);
	QString probeDataFilename = cx::changeExtension(path, "probedata.xml");

	cx::ProbeDefinition retval;
	cx::XmlOptionFile file = cx::XmlOptionFile(probeDataFilename, "navnet");
	retval.parseXml(file.getElement("configuration"));

	return retval;
}

void VideoGraphicsFixture::addImageToRenderer(vtkImageDataPtr image)
{
	vtkImageActorPtr imageActor = vtkImageActorPtr::New();
	imageActor->SetInputData(image);
	mMachine->addProp(imageActor);
}

void VideoGraphicsFixture::renderImageAndCompareToExpected(vtkImageDataPtr input, vtkImageDataPtr expected)
{
	mVideoGraphics->setInputVideo(input);
	mVideoGraphics->update();
	mVideoGraphics->setVisibility(true); // seems to be invis by default- investigate.
	mMachine->addProp(mVideoGraphics->getActor());

	mMachine->alignRenderWindowWithImage(input);
//	mMachine->renderToFile("TestVideoGraphics_render_screencapture.png");
	vtkImageDataPtr renderedImage = mMachine->renderToImage();

	CHECK(mMachine->findDifference(renderedImage, expected));

//	mMachine->enterRunLoop();
}

} /* namespace cxtest */

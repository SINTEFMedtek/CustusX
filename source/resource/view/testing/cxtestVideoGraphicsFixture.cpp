/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxtestVideoGraphicsFixture.h"

#include "cxVideoGraphics.h"
#include <vtkImageActor.h>
#include <vtkCamera.h>
#include <vtkImageData.h>
#include <vtkActor.h>

#include "cxBoundingBox3D.h"
#include "cxView.h"
#include "cxTypeConversions.h"

#include "cxProbeDefinition.h"
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

cx::ProbeDefinition VideoGraphicsFixture::readProbeDefinition(QString filename)
{
	QString path = cxtest::Utilities::getDataRoot("testing/videographics/"+filename);
	QString probeDataFilename = cx::changeExtension(path, "probedata.xml");

	cx::ProbeDefinition retval;
	cx::XmlOptionFile file = cx::XmlOptionFile(probeDataFilename);
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

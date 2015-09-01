/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

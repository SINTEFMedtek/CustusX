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

#include "sscTestVideoGraphics.h"
#include "sscVtkRenderTester.h"

#include "sscVideoGraphics.h"
#include <vtkImageActor.h>
#include <vtkCamera.h>
#include <vtkImageData.h>
#include <vtkActor.h>

#include "sscBoundingBox3D.h"
#include "sscView.h"
#include "sscTypeConversions.h"

#include "sscProbeData.h"
#include "sscProbeSector.h"
#include "sscDummyTool.h"
#include "sscUtilHelpers.h"
#include "sscXmlOptionItem.h"

void TestVideoGraphics::setUp()
{
	mMachine = ssc::TestVtkRendering::create();
	mVideoGraphics.reset(new ssc::VideoGraphics());
}

void TestVideoGraphics::tearDown()
{

}

vtkImageDataPtr TestVideoGraphics::readImageData(QString filename, QString description)
{
	QString folder = QString("%1/%2/").arg(SSC_DATA_ROOT).arg("test");
	vtkImageDataPtr retval = mMachine->readFromFile(folder+filename);
	CPPUNIT_ASSERT_MESSAGE(("Looking for "+description).toStdString(), retval!=NULL);
	return retval;
}

ssc::ProbeData TestVideoGraphics::readProbeData(QString filename)
{
	QString folder = QString("%1/%2/").arg(SSC_DATA_ROOT).arg("test");
	QString probeDataFilename = ssc::changeExtension(folder+filename, "probedata.xml");

	ssc::ProbeData retval;
	ssc::XmlOptionFile file = ssc::XmlOptionFile(probeDataFilename, "navnet");
	retval.parseXml(file.getElement("configuration"));

//	std::cout << probeDataFilename << " -- " << streamXml2String(retval) << std::endl;
	return retval;
}

void TestVideoGraphics::addImageToRenderer(vtkImageDataPtr image)
{
	vtkImageActorPtr imageActor = vtkImageActorPtr::New();
	imageActor->SetInput(image);
	mMachine->addProp(imageActor);
}

void TestVideoGraphics::renderImageAndCompareToExpected(vtkImageDataPtr input, vtkImageDataPtr expected)
{
	mVideoGraphics->setInputVideo(input);
	mVideoGraphics->update();
	mVideoGraphics->setVisibility(true); // seems to be invis by default- investigate.
	mMachine->addProp(mVideoGraphics->getActor());

	mMachine->alignRenderWindowWithImage(input);
//	mMachine->renderToFile("TestVideoGraphics_render_screencapture.png");
	vtkImageDataPtr renderedImage = mMachine->renderToImage();

	CPPUNIT_ASSERT(mMachine->findDifference(renderedImage, expected));

//	mMachine->enterRunLoop();
}



void TestVideoGraphics::testRenderImage()
{
	vtkImageDataPtr videoImage0 = this->readImageData("testImage01.png", "input image");
	this->addImageToRenderer(videoImage0);

	mMachine->alignRenderWindowWithImage(videoImage0);
	vtkImageDataPtr renderedImage = mMachine->renderToImage();

	CPPUNIT_ASSERT(mMachine->findDifference(videoImage0, renderedImage));
}

void TestVideoGraphics::testRenderMetaHeaderImage()
{
	vtkImageDataPtr videoImage0 = this->readImageData("US_small.mhd", "input image");
	this->addImageToRenderer(videoImage0);

	mMachine->alignRenderWindowWithImage(videoImage0);
	vtkImageDataPtr renderedImage = mMachine->renderToImage();

	CPPUNIT_ASSERT(mMachine->findDifference(videoImage0, renderedImage));
}

void TestVideoGraphics::testPassSinglePNGImage()
{
	vtkImageDataPtr videoImage0 = this->readImageData("testImage01.png", "input image");

	mVideoGraphics->setFlipVertical(false);

	this->renderImageAndCompareToExpected(videoImage0, videoImage0);
}

void TestVideoGraphics::testPassSingleMHDImage()
{
	vtkImageDataPtr videoImage0 = this->readImageData("US_small.mhd", "input image");

	mVideoGraphics->setFlipVertical(false);

	this->renderImageAndCompareToExpected(videoImage0, videoImage0);
}

void TestVideoGraphics::testMask()
{
	vtkImageDataPtr videoImage0 = this->readImageData("testImage01.png", "input image");
	vtkImageDataPtr mask = this->readImageData("testImage01_sectormask.png", "input mask");
	vtkImageDataPtr expected = this->readImageData("testImage01_sectormasked.png", "input expected");

	mVideoGraphics->setFlipVertical(false);
	mVideoGraphics->setMask(mask);

	this->renderImageAndCompareToExpected(videoImage0, expected);
}

void TestVideoGraphics::testSector()
{
	QString imageFilename = "US_small.mhd";
	vtkImageDataPtr videoImage0 = this->readImageData(imageFilename, "input image");
	vtkImageDataPtr expected = this->readImageData("US_small_sector_masked.png", "input expected");

	ssc::ProbeSector probeSector;
	probeSector.mData = this->readProbeData(imageFilename);

	mVideoGraphics->setClip(probeSector.getSector());

	this->renderImageAndCompareToExpected(videoImage0, expected);
}


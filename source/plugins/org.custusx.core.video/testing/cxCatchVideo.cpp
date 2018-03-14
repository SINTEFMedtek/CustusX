/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include <QFile>
#include <QString>
#include <vtkImageData.h>
#include "vtkForwardDeclarations.h"
#include "cxImage.h"
#include "cxDataLocations.h"
#include "cxMHDImageStreamer.h"

#include "cxtestJenkinsMeasurement.h"

#include "cxSettings.h"
#include "cxTypeConversions.h"
//#include "cxtestSimulatedImageStreamerFixture.h"
#include "cxtestSender.h"
#include "cxtestQueuedSignalListener.h"

namespace cxtest
{
namespace {

cx::DummyImageStreamerPtr createRunningDummyImageStreamer(TestSenderPtr& sender, bool secondaryStream = false, bool sendonce = false)
{
	QString filename = cx::DataLocations::getTestDataPath() + "/testing/default_volume/Default.mhd";
	REQUIRE(QFile::exists(filename));

	cx::DummyImageStreamerPtr imagestreamer(new cx::DummyImageStreamer());
	REQUIRE(imagestreamer);

	imagestreamer->initialize(filename, secondaryStream, sendonce);
	imagestreamer->startStreaming(sender);
	REQUIRE(imagestreamer->isStreaming());
	return imagestreamer;
}

//Similar to SimulatedImageStreamerFixture::checkSenderGotImageFromStreamer()
void checkSenderGotImageFromStreamer(TestSenderPtr& sender)
{
	REQUIRE(sender);
	cx::PackagePtr package = sender->getSentPackage();
	REQUIRE(package);
	cx::ImagePtr image = package->mImage;
	REQUIRE(image);
}

//Similar to SimulatedImageStreamerFixture::checkSimulatedFrame()
void checkSimulatedFrame(TestSenderPtr& sender)
{
	REQUIRE(sender);
	REQUIRE(waitForQueuedSignal(sender.get(), SIGNAL(newPackage()), 200, true));
	checkSenderGotImageFromStreamer(sender);
}

//Similar to SimulatedImageStreamerFixture::checkSimulatedFrames()
void checkSimulatedFrames(TestSenderPtr& sender, int numFrames)
{
	for(int i = 0; i < numFrames; ++i)
		checkSimulatedFrame(sender);
}

} //empty namespace

TEST_CASE("DummyImageStreamer: File should be read and sent only once", "[streaming][unit]")
{
	TestSenderPtr sender(new TestSender());
	bool sendImageOnce = true;
	bool sendTwoStreams = false;
	cx::StreamerPtr imagestreamer = createRunningDummyImageStreamer(sender, sendTwoStreams, sendImageOnce);

//	SimulatedImageStreamerFixture fixture(sender);
//	fixture.checkSimulatedFrames(1);
	checkSimulatedFrames(sender, 1);

	REQUIRE_FALSE(waitForQueuedSignal(sender.get(), SIGNAL(newPackage())));

	imagestreamer->stopStreaming();
}

TEST_CASE("DummyImageStreamer: File should be read and send slices with a given interval", "[streaming][unit]")
{
	TestSenderPtr sender(new TestSender());
	bool sendTwoStreams = false;
	cx::StreamerPtr imagestreamer = createRunningDummyImageStreamer(sender,sendTwoStreams);

	int numFrames = 2;
//	SimulatedImageStreamerFixture fixture(sender);
//	fixture.checkSimulatedFrames(numFrames);
	checkSimulatedFrames(sender, numFrames);

	imagestreamer->stopStreaming();
}

}//namespace cxtest

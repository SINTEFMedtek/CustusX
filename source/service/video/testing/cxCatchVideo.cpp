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

#include "catch.hpp"

#include <QFile>
#include <QString>
#include <vtkImageData.h>
#include "vtkForwardDeclarations.h"
#include "sscImage.h"
#include "sscDummyTool.h"
#include "cxDataLocations.h"
#include "cxMHDImageStreamer.h"
#include "cxSimulatedImageStreamer.h"
#include "sscDummyToolManager.h"
#include "cxtestSender.h"
#include "cxtestQueuedSignalListener.h"
#include "cxtestUtilities.h"

namespace cxtest
{

cx::DummyImageStreamerPtr createRunningDummyImageStreamer(TestSenderPtr& sender, bool secondaryStream = false, bool sendonce = false)
{
	QString filename = cx::DataLocations::getTestDataPath() + "/testing/TubeSegmentationFramework/Default.mhd";
	REQUIRE(QFile::exists(filename));

	cx::DummyImageStreamerPtr imagestreamer(new cx::DummyImageStreamer());
	REQUIRE(imagestreamer);

	imagestreamer->initialize(filename, secondaryStream, sendonce);
	REQUIRE(imagestreamer->startStreaming(sender));
	return imagestreamer;
}

cx::SimulatedImageStreamerPtr createRunningSimulatedImageStreamer(TestSenderPtr& sender)
{
	cx::ImagePtr image = cxtest::Utilities::create3DImage();
	REQUIRE(image);
	cx::DummyToolPtr tool = cx::DummyToolTestUtilities::createDummyTool(cx::DummyToolTestUtilities::createProbeDataLinear(), cx::DummyToolManager::getInstance());
	REQUIRE(tool);
	cx::SimulatedImageStreamerPtr imagestreamer(new cx::SimulatedImageStreamer());
	REQUIRE(imagestreamer);

	imagestreamer->initialize(image, tool);
	REQUIRE(imagestreamer->startStreaming(sender));
	return imagestreamer;
}

void checkSenderGotImageFromStreamer(TestSenderPtr sender)
{
	cx::PackagePtr package = sender->getSentPackage();
	REQUIRE(package);
	cx::ImagePtr image = package->mImage;
	REQUIRE(image);
}

TEST_CASE("DummyImageStreamer: File should be read and sent only once", "[streaming][unit]")
{
	TestSenderPtr sender(new TestSender());
	bool sendImageOnce = true;
	bool sendTwoStreams = false;
	cx::ImageStreamerPtr imagestreamer = createRunningDummyImageStreamer(sender, sendTwoStreams, sendImageOnce);

	REQUIRE(waitForQueuedSignal(sender.get(), SIGNAL(newPackage())));
	checkSenderGotImageFromStreamer(sender);

	REQUIRE_FALSE(waitForQueuedSignal(sender.get(), SIGNAL(newPackage())));

	imagestreamer->stopStreaming();
}

TEST_CASE("DummyImageStreamer: File should be read and send slices with a given interval", "[streaming][unit]")
{
	TestSenderPtr sender(new TestSender());
	bool sendTwoStreams = false;
	cx::ImageStreamerPtr imagestreamer = createRunningDummyImageStreamer(sender,sendTwoStreams);

	REQUIRE(waitForQueuedSignal(sender.get(), SIGNAL(newPackage())));
	checkSenderGotImageFromStreamer(sender);

	REQUIRE(waitForQueuedSignal(sender.get(), SIGNAL(newPackage())));
	checkSenderGotImageFromStreamer(sender);

	imagestreamer->stopStreaming();
}

TEST_CASE("SimulatedImageStreamer: Should stream 2D images from a volume given a probe", "[streaming][unit]")
{
	TestSenderPtr sender(new TestSender());
	REQUIRE(sender);

	cx::SimulatedImageStreamerPtr imagestreamer = createRunningSimulatedImageStreamer(sender);

	REQUIRE(waitForQueuedSignal(sender.get(), SIGNAL(newPackage()), 200));
	checkSenderGotImageFromStreamer(sender);

	REQUIRE(waitForQueuedSignal(sender.get(), SIGNAL(newPackage()), 200));
	checkSenderGotImageFromStreamer(sender);

	imagestreamer->stopStreaming();
	cx::ToolManager::shutdown();
}

}//namespace cx

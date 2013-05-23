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
#include "cxToolManager.h"
#include "cxtestSender.h"
#include "cxtestSignalListener.h"
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
	cx::ToolManager::initializeObject();
	ssc::ImagePtr image = cxtest::Utilities::create3DImage();
	REQUIRE(image);
	ssc::DummyToolPtr tool = ssc::DummyToolTestUtilities::createDummyTool(ssc::DummyToolTestUtilities::createProbeDataLinear(), cx::ToolManager::getInstance());
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
	ssc::ImagePtr image = package->mImage;
	REQUIRE(image);
}

TEST_CASE("DummyImageStreamer: File should be read and sent only once", "[video][unit]")
{
	TestSenderPtr sender(new TestSender());
	bool sendImageOnce = true;
	bool sendTwoStreams = false;
	cx::ImageStreamerPtr imagestreamer = createRunningDummyImageStreamer(sender, sendTwoStreams, sendImageOnce);

	REQUIRE(waitForSignal(sender.get(), SIGNAL(newPackage())));
	checkSenderGotImageFromStreamer(sender);

	REQUIRE_FALSE(waitForSignal(sender.get(), SIGNAL(newPackage())));

	imagestreamer->stopStreaming();
}

TEST_CASE("DummyImageStreamer: File should be read and send slices with a given interval", "[video][unit]")
{
	TestSenderPtr sender(new TestSender());
	bool sendTwoStreams = false;
	cx::ImageStreamerPtr imagestreamer = createRunningDummyImageStreamer(sender,sendTwoStreams);

	REQUIRE(waitForSignal(sender.get(), SIGNAL(newPackage())));
	checkSenderGotImageFromStreamer(sender);

	REQUIRE(waitForSignal(sender.get(), SIGNAL(newPackage())));
	checkSenderGotImageFromStreamer(sender);

	imagestreamer->stopStreaming();
}

TEST_CASE("Should stream 2D images from a volume given a probe", "[video][unit]")
{
	TestSenderPtr sender(new TestSender());
	REQUIRE(sender);

	cx::SimulatedImageStreamerPtr imagestreamer = createRunningSimulatedImageStreamer(sender);

	REQUIRE(waitForSignal(sender.get(), SIGNAL(newPackage())));
	checkSenderGotImageFromStreamer(sender);

	REQUIRE(waitForSignal(sender.get(), SIGNAL(newPackage())));
	checkSenderGotImageFromStreamer(sender);

	imagestreamer->stopStreaming();
}

//TEST_CASE("Probe should get simulated data", "[video][integration]")
//{
//
//	//TODO
//
//	ssc::ToolPtr tool = cx::ToolManager::getInstance()->findFirstProbe();
//	ssc::ProbePtr probe = tool->getProbe();
//
//	TestSenderPtr sender(new TestSender());
//	bool sendImageOnce = true;
//	cx::ImageStreamerPtr imagestreamer = createRunningImageStreamer(sender, sendImageOnce);
//
//	ssc::VideoSourcePtr videosource(new cx::ToolResamplingVolumeVideoSource());
//	videosource->setTool(tool);
//	videosource->setStreamer(imagestreamer);
//
//	probe->setRTSource(videosource);
//
//}


}//namespace cx

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
#include "cxImage.h"
#include "cxDataLocations.h"
#include "cxMHDImageStreamer.h"

#include "cxtestJenkinsMeasurement.h"
#include "cxReporter.h"
#include "cxSettings.h"
#include "cxTypeConversions.h"
#include "cxtestSimulatedImageStreamerFixture.h"

namespace cxtest
{
namespace {

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

} //empty namespace

TEST_CASE("DummyImageStreamer: File should be read and sent only once", "[streaming][unit]")
{
	TestSenderPtr sender(new TestSender());
	bool sendImageOnce = true;
	bool sendTwoStreams = false;
	cx::ImageStreamerPtr imagestreamer = createRunningDummyImageStreamer(sender, sendTwoStreams, sendImageOnce);

	SimulatedImageStreamerFixture fixture(sender);
	fixture.checkSimulatedFrames(1);

	REQUIRE_FALSE(waitForQueuedSignal(sender.get(), SIGNAL(newPackage())));

	imagestreamer->stopStreaming();
}

TEST_CASE("DummyImageStreamer: File should be read and send slices with a given interval", "[streaming][unit]")
{
	TestSenderPtr sender(new TestSender());
	bool sendTwoStreams = false;
	cx::ImageStreamerPtr imagestreamer = createRunningDummyImageStreamer(sender,sendTwoStreams);

	int numFrames = 2;
	SimulatedImageStreamerFixture fixture(sender);
	fixture.checkSimulatedFrames(numFrames);

	imagestreamer->stopStreaming();
}

#ifdef CX_BUILD_US_SIMULATOR
TEST_CASE("ImageSimulator: Constructor", "[streaming][unit]")
{
	SimulatedImageStreamerFixture::constructImageSimulatorVariable();
	SimulatedImageStreamerFixture::constructImageSimulatorBoostPtr();
}
#endif //CX_BUILD_US_SIMULATOR

TEST_CASE("SimulatedImageStreamer: Init", "[streaming][unit]")
{
	SimulatedImageStreamerFixture fixture;
	cx::SimulatedImageStreamerPtr imagestreamer = fixture.createRunningSimulatedImageStreamer();

	imagestreamer->stopStreaming();
}

TEST_CASE("SimulatedImageStreamer: Should stream 2D images from a volume given a probe", "[streaming][unit]")
{
	SimulatedImageStreamerFixture fixture;
	cx::SimulatedImageStreamerPtr imagestreamer = fixture.createRunningSimulatedImageStreamer();

	int numFrames = 2;
	fixture.checkSimulatedFrames(numFrames);

	imagestreamer->stopStreaming();
}

TEST_CASE("SimulatedImageStreamer: Won't return an image if not initialized", "[streaming][unit]")
{
	SimulatedImageStreamerFixture fixture;
	cx::SimulatedImageStreamerPtr imagestreamer = fixture.createSimulatedImageStreamer();

	fixture.requireNoSimulatedFrame();

	REQUIRE_FALSE(imagestreamer->startStreaming(fixture.getSender()));
}

TEST_CASE("SimulatedImageStreamer: Basic test of streamers", "[streaming][unit]")
{
	SimulatedImageStreamerFixture fixture;
	cx::DataLocations::setTestMode();
	int numFrames = 1;
	QStringList simulationTypes;
	simulationTypes << "Original data" << "CT to US" << "MR to US";
	for (int i = 0; i < 3; ++i)
	{
		cx::settings()->setValue("USsimulation/type", simulationTypes[i]);
		INFO("Simulation failed: " + string_cast(simulationTypes[i]));
		fixture.simulateAndCheckUS(numFrames);
	}
}

TEST_CASE("SimulatedImageStreamer: Speed", "[streaming][integration][speed]")
{
	SimulatedImageStreamerFixture fixture;
	cx::DataLocations::setTestMode();
	cx::reporter()->initialize();

	int numFrames = 100;
	JenkinsMeasurement jenkins;
	QStringList simulationTypes;
	simulationTypes << "Original data" << "CT to US" << "MR to US";
	for (int i = 0; i < 3; ++i)
	{
		cx::settings()->setValue("USsimulation/type", simulationTypes[i]);
		int simTime = fixture.simulateAndCheckUS(numFrames);
		jenkins.createOutput("Average time in ms per frame with simtype " + simulationTypes[i], QString::number(simTime));
	}

	cx::Reporter::shutdown();
}

}//namespace cxtest

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

#include <QFile>
#include <QString>
#include <vtkImageData.h>
#include "vtkForwardDeclarations.h"
#include "cxImage.h"
#include "cxDataLocations.h"
//#include "cxMHDImageStreamer.h"

#include "cxtestJenkinsMeasurement.h"
#include "cxReporter.h"
#include "cxSettings.h"
#include "cxTypeConversions.h"
#include "cxtestSimulatedImageStreamerFixture.h"

namespace cxtest
{

TEST_CASE("ImageSimulator: Constructor", "[streaming][unit]")
{
	SimulatedImageStreamerFixture::constructImageSimulatorVariable();
	SimulatedImageStreamerFixture::constructImageSimulatorBoostPtr();
}

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

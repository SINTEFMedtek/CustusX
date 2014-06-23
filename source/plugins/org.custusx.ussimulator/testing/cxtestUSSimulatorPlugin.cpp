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

#include "cxtestJenkinsMeasurement.h"
#include "cxReporter.h"
#include "cxSettings.h"
#include "cxTypeConversions.h"
#include "cxtestSimulatedImageStreamerFixture.h"

#include <ctkServiceTracker.h>
#include "cxStreamerService.h"
#include "cxPluginFramework.h"
#include "cxLogicManager.h"
#include "org_custusx_ussimulator_Export.h"

namespace {

cx::StreamerService* getStreamerService(QString name)
{
	cx::PluginFrameworkManagerPtr pluginFramework = cx::logicManager()->getPluginFramework();
	ctkPluginContext* context = pluginFramework->getPluginContext();

	ctkServiceTracker<cx::StreamerService*> tracker(context);
	tracker.open();

	QList<cx::StreamerService*> serviceList = tracker.getServices();

	for(int i = 0; i < serviceList.size(); ++i)
	{
		cx::StreamerService* service = serviceList.at(i);
		if (service->getName() == name)
		return service;
	}

	return NULL;
}

}

namespace cxtest
{

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
	//TODO get from simulatedImageStreamerService->getOptions()->getSomething()->getRange();
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

TEST_CASE("StreamerService: Service available", "[streaming][service][unit]")
{
	cx::DataLocations::setTestMode();
	cx::LogicManager::initialize();

	cx::PluginFrameworkManagerPtr pluginFramework = cx::logicManager()->getPluginFramework();
	ctkPluginContext* context = pluginFramework->getPluginContext();

	ctkServiceTracker<cx::StreamerService*> tracker(context);
	tracker.open();

	cx::StreamerService* service = tracker.getService();
	REQUIRE(service);

	QList<cx::StreamerService*> serviceList = tracker.getServices();
	REQUIRE(serviceList.size() > 0);

	cx::XmlOptionFile options = cx::XmlOptionFile(cx::DataLocations::getXmlSettingsFile(), "CustusX").descend("video");

	for(int i = 0; i < serviceList.size(); ++i)
	{
		cx::StreamerService* service = serviceList.at(i);
		INFO("Streamer: " + service->getName());
		QDomElement element = options.getElement("video", service->getName());
		cx::StreamerPtr streamer = service->createStreamer(element);
		REQUIRE(streamer);
	}

	cx::LogicManager::shutdown();
}

TEST_CASE("StreamerService: SimulatedImageStreamerService available", "[streaming][service][unit]")
{
	cx::DataLocations::setTestMode();
	cx::LogicManager::initialize();
	cx::StreamerService* service = getStreamerService("Simulator");
	REQUIRE(service);
	cx::LogicManager::shutdown();
}

}//namespace cxtest

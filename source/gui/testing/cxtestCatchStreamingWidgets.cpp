#include "catch.hpp"

#include <QTimer>
#include <QApplication>
#include "cxtestTestVideoConnectionWidget.h"
#include "cxtestQueuedSignalListener.h"
#include "cxDataLocations.h"

#include "sscMessageManager.h"
#include "sscDummyToolManager.h"
#include "sscDataManager.h"
#include "sscToolManager.h"
#include "cxVideoService.h"
#include "cxVideoServiceBackend.h"
#include "cxtestDummyDataManager.h"
#include "cxLogicManager.h"

namespace cxtest
{

TEST_CASE("VideoConnectionWidget can stream", "[unit][gui][widget][streaming]")
{
	cx::LogicManager::initialize();
//	cxtest::TestServicesPtr services = cxtest::TestServices::create();
//	cx::messageManager()->initialize();
//	cx::cxDataManager::getInstance()->initialize();

//	cx::cxToolManager::initializeObject();
//	cx::cxToolManager* tm = cx::cxToolManager::getInstance();
//	cx::TrackingServicePtr ts = services->trackingService();

//	cx::VideoService::initialize(cx::VideoServiceBackend::create(services->dataService(),
//																 services->trackingService(),
//																 services->spaceProvider()));
	cx::TrackingServicePtr ts = cx::logicManager()->getTrackingService();

	cx::DummyToolPtr tool = cx::DummyToolTestUtilities::createDummyTool(cx::DummyToolTestUtilities::createProbeDataLinear(), ts);
	ts->runDummyTool(tool);
	ts->setDominantTool(tool->getUid());
	waitForQueuedSignal(ts.get(), SIGNAL(trackingStarted()));

	QString filename = cx::DataLocations::getTestDataPath() + "/testing/TubeSegmentationFramework/Default.mhd";
	REQUIRE(QFile::exists(filename));

	TestVideoConnectionWidget* widget = new TestVideoConnectionWidget();
	REQUIRE(widget->canStream(filename, "MHDFile"));
	REQUIRE(widget->canStream(filename, "SimulatedImageStreamer"));
	delete widget;

//	DummyToolManager::getInstance()->shutdown();
//	cx::cxDataManager::getInstance()->shutdown();
//	cx::messageManager()->shutdown();
	ts.reset();
	cx::LogicManager::shutdown();
}

} //namespace cxtest

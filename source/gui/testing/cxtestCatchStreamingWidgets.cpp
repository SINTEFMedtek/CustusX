#include "catch.hpp"

#include <QTimer>
#include <QApplication>
#include "cxtestTestVideoConnectionWidget.h"
#include "cxtestQueuedSignalListener.h"
#include "cxDataLocations.h"

#include "sscMessageManager.h"
#include "sscDummyToolManager.h"
#include "cxDataManager.h"
#include "cxToolManager.h"
#include "cxVideoService.h"
#include "cxVideoServiceBackend.h"

namespace cxtest
{

TEST_CASE("VideoConnectionWidget can stream", "[unit][gui][widget][streaming]")
{
	cx::messageManager()->initialize();
	cx::cxDataManager::getInstance()->initialize();

	cx::cxToolManager::initializeObject();
	cx::cxToolManager* tm = cx::cxToolManager::getInstance();

	cx::VideoService::initialize(cx::VideoServiceBackend::create(cx::cxDataManager::getInstance(),
																 tm,
																 cx::SpaceProviderPtr()));

	cx::DummyToolPtr tool = cx::DummyToolTestUtilities::createDummyTool(cx::DummyToolTestUtilities::createProbeDataLinear(),tm);
	tm->runDummyTool(tool);
	tm->setDominantTool(tool->getUid());
	waitForQueuedSignal(tm, SIGNAL(trackingStarted()));

	QString filename = cx::DataLocations::getTestDataPath() + "/testing/TubeSegmentationFramework/Default.mhd";
	REQUIRE(QFile::exists(filename));

	TestVideoConnectionWidget* widget = new TestVideoConnectionWidget();
	REQUIRE(widget->canStream(filename, "MHDFile"));
	REQUIRE(widget->canStream(filename, "SimulatedImageStreamer"));
	delete widget;

//	DummyToolManager::getInstance()->shutdown();
	cx::cxDataManager::getInstance()->shutdown();
	cx::messageManager()->shutdown();
}

} //namespace cxtest

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

namespace cxtest
{

TEST_CASE("VideoConnectionWidget can stream", "[unit][gui][widget][streaming]")
{
	ssc::messageManager()->initialize();
	cx::cxDataManager::getInstance()->initialize();

	cx::cxToolManager::initializeObject();
	cx::cxToolManager* tm = cx::cxToolManager::getInstance();
	ssc::DummyToolPtr tool = ssc::DummyToolTestUtilities::createDummyTool(ssc::DummyToolTestUtilities::createProbeDataLinear(),tm);
	tm->runDummyTool(tool);
	tm->setDominantTool(tool->getUid());
	waitForQueuedSignal(tm, SIGNAL(trackingStarted()));

	QString filename = cx::DataLocations::getTestDataPath() + "/testing/TubeSegmentationFramework/Default.mhd";
	REQUIRE(QFile::exists(filename));

	TestVideoConnectionWidget* widget = new TestVideoConnectionWidget();
	REQUIRE(widget->canStream(filename, "MHDFile"));
	REQUIRE(widget->canStream(filename, "SimulatedImageStreamer"));
	delete widget;

//	ssc::DummyToolManager::getInstance()->shutdown();
	cx::cxDataManager::getInstance()->shutdown();
	ssc::messageManager()->shutdown();
}

} //namespace cxtest

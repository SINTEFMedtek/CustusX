#include "catch.hpp"

#include <QTimer>
#include <QApplication>
#include "cxtestTestVideoConnectionWidget.h"
#include "cxtestSignalListener.h"
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
	cx::DataManager::getInstance()->initialize();

	cx::ToolManager::initializeObject();
	cx::ToolManager* tm = cx::ToolManager::getInstance();
	ssc::DummyToolPtr tool = ssc::DummyToolTestUtilities::createDummyTool(ssc::DummyToolTestUtilities::createProbeDataLinear(),tm);
	tm->runDummyTool(tool);
	tm->setDominantTool(tool->getUid());
	waitForSignal(tm, SIGNAL(trackingStarted()));

	QString filename = cx::DataLocations::getTestDataPath() + "/testing/TubeSegmentationFramework/Default.mhd";
	REQUIRE(QFile::exists(filename));

	TestVideoConnectionWidget* widget = new TestVideoConnectionWidget();
	REQUIRE(widget->canStream(filename, "MHDFile"));
	REQUIRE(widget->canStream(filename, "SimulatedImageStreamer"));
	delete widget;

//	ssc::DummyToolManager::getInstance()->shutdown();
	cx::DataManager::getInstance()->shutdown();
	ssc::messageManager()->shutdown();
}

} //namespace cxtest

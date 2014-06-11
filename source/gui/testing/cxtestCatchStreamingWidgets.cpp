#include "catch.hpp"

#include <QTimer>
#include <QApplication>
#include "cxtestTestVideoConnectionWidget.h"
#include "cxtestQueuedSignalListener.h"
#include "cxDataLocations.h"

#include "cxDummyToolManager.h"
#include "cxLogicManager.h"

namespace cxtest
{

//Disabled test of SimulatedImageStreamer
TEST_CASE("VideoConnectionWidget can stream", "[unit][gui][not_win32][widget][streaming]")
{
	cx::LogicManager::initialize();

	cx::TrackingServicePtr ts = cx::logicManager()->getTrackingService();

	cx::DummyToolPtr tool = cx::DummyToolTestUtilities::createDummyTool(cx::DummyToolTestUtilities::createProbeDataLinear(), ts);
	ts->runDummyTool(tool);
	ts->setDominantTool(tool->getUid());
	waitForQueuedSignal(ts.get(), SIGNAL(trackingStarted()));

	QString filename = cx::DataLocations::getTestDataPath() + "/testing/TubeSegmentationFramework/Default.mhd";
	REQUIRE(QFile::exists(filename));

	TestVideoConnectionWidget* widget = new TestVideoConnectionWidget();
	REQUIRE(widget->canStream(filename, "MHDFile"));
//	REQUIRE(widget->canStream(filename, "SimulatedImageStreamer"));
	delete widget;

	ts.reset();
	cx::LogicManager::shutdown();
}

} //namespace cxtest

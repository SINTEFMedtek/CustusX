#include "catch.hpp"

#include <QTimer>
#include <QApplication>
#include "sscMessageManager.h"
#include "cxtestTestVideoConnectionWidget.h"
#include "cxLogicManager.h"
#include "cxDataLocations.h"

namespace cxtest
{

TEST_CASE("StreamingWidget construction", "[unit][gui][widget][streaming]")
{
	cx::LogicManager::initialize();

	QString filename = cx::DataLocations::getTestDataPath() + "/testing/TubeSegmentationFramework/Default.mhd";

	TestVideoConnectionWidget* widget = new TestVideoConnectionWidget();
	REQUIRE(widget);
	CHECK(widget->canStream(filename));
	delete widget;

	QTimer::singleShot(1000, qApp, SLOT(quit()));
}

} //namespace cxtest

#include "catch.hpp"

#include "sscMessageManager.h"
#include "cxtestTestVideoConnectionWidget.h"
#include "cxLogicManager.h"

namespace cxtest
{

TEST_CASE("StreamingWidget construction", "[unit][gui][widget][streaming]")
{
	cx::LogicManager::initialize();

	TestVideoConnection* widget = new TestVideoConnection();
	REQUIRE(widget);
	CHECK(widget->startStopServer());
	delete widget;

	QTimer::singleShot(1000, qApp, SLOT(quit()));
}

} //namespace cxtest

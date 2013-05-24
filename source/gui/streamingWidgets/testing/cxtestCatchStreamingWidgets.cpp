#include "catch.hpp"

#include "cxVideoConnectionWidget.h"
#include "cxToolManager.h"

namespace cxtest
{

TEST_CASE("StreamingWidget construction", "[unit][gui][widget][streaming]")
{
	cx::ToolManager::initializeObject();

	cx::QTestVideoConnection* widget = new cx::QTestVideoConnection();
	REQUIRE(widget);
	REQUIRE(widget->startServer());

	delete widget;
}

} //namespace cxtest

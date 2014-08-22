#include "catch.hpp"

#include <QDomElement>
#include <ctkServiceTracker.h>
#include <ctkPluginContext.h>
#include "cxLogicManager.h"
#include "cxDataLocations.h"
#include "cxStreamerService.h"
#include "cxXmlOptionItem.h"
#include "cxPluginFramework.h"

namespace cxtest
{
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
		INFO("Streamer: " + service->getName().toStdString());
		QDomElement element = options.getElement("video", service->getName());
		cx::StreamerPtr streamer = service->createStreamer(element);
		REQUIRE(streamer);
	}

	cx::LogicManager::shutdown();
}
}//namespace cxtest

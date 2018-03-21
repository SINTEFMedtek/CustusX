/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include <QDomElement>
#include <ctkServiceTracker.h>
#include <ctkPluginContext.h>
#include "cxLogicManager.h"
#include "cxProfile.h"
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

	ctkPluginContext* context = cx::logicManager()->getPluginContext();

	ctkServiceTracker<cx::StreamerService*> tracker(context);
	tracker.open();

	cx::StreamerService* service = tracker.getService();
	REQUIRE(service);

	QList<cx::StreamerService*> serviceList = tracker.getServices();
	REQUIRE(serviceList.size() > 0);

	cx::XmlOptionFile options = cx::profile()->getXmlSettings().descend("video");

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

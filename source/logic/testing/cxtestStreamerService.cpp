/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

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
//Made this test integration, as it constructs/destructs the GEStreamer.
//The GEStremer destructor sometimes fail to stop a ISB_GE thread, and this may Catch to crash
TEST_CASE("StreamerService: Service available", "[streaming][service][integration][broken]")
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

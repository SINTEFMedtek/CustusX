/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include "cxtestQueuedSignalListener.h"
#include "cxReporter.h"
#include "cxMessageListener.h"

namespace cx
{

TEST_CASE("Reporter can catch cout", "[unit]")
{
	cx::Reporter::initialize();

	{
		// given
		cx::MessageListenerPtr listener = cx::MessageListener::create();
		listener->setMessageQueueMaxSize(100);
		CHECK(listener->getMessages().size()==0);
		QString testString = "<test string>";

		// when
		std::cout << testString << std::endl;

		// then
		REQUIRE(cxtest::waitForQueuedSignal(listener.get(), SIGNAL(newMessage(Message)), 200, true));

		CHECK(!listener->getMessages().isEmpty());
		if (!listener->getMessages().isEmpty())
			CHECK(listener->getMessages().front().getText().contains(testString));
	}

	cx::Reporter::shutdown();
}

TEST_CASE("Reporter can be run multiple times", "[unit]")
{
	unsigned numberOfRuns = 3;
	for (unsigned i=0; i<numberOfRuns; ++i)
	{
		cx::Reporter::initialize();
		std::cout << "<test string>" << std::endl;
		cx::Reporter::shutdown();
	}
	CHECK(true);
}

TEST_CASE("Reporter can be run nested", "[unit]")
{
	cx::Reporter::initialize();
	cx::Reporter::initialize();

	CX_LOG_CHANNEL_INFO("report test") << "test string";
	CHECK(true);

	cx::Reporter::shutdown();
	cx::Reporter::shutdown();
}

TEST_CASE("Reporter: MessageListener receives messages", "[unit]")
{
	cx::Reporter::initialize();

	{
		// given
		cx::MessageListenerPtr listener = cx::MessageListener::create();
		listener->setMessageQueueMaxSize(100);
		CHECK(listener->getMessages().size()==0);
		QString testString = "<test string>";

		// when
		CX_LOG_CHANNEL_INFO("report test") << testString;
		//	std::cout << testString << std::endl;

		// then
		REQUIRE(cxtest::waitForQueuedSignal(listener.get(), SIGNAL(newMessage(Message)), 200, true));

		CHECK(!listener->getMessages().isEmpty());
		if (!listener->getMessages().isEmpty())
			CHECK(listener->getMessages().front().getText().contains(testString));
	}

	cx::Reporter::shutdown();
}



} // namespace cx



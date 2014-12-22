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

#include "cxtestQueuedSignalListener.h"
#include "cxReporter.h"
#include "cxMessageListener.h"

namespace cx
{

TEST_CASE("Reporter can catch cout", "[unit]")
{
	cx::Reporter::initialize();

	{
		std::cout << "<test string>" << std::endl;
		CHECK(cxtest::waitForQueuedSignal(cx::reporter(), SIGNAL(emittedMessage(Message)), 200, true));
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



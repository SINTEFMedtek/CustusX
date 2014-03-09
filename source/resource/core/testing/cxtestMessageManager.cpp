// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "catch.hpp"
#include "sscMessageManager.h"
#include "cxtestDirectSignalListener.h"

namespace cx
{

TEST_CASE("MessageManager can catch cout", "[unit]")
{
	cx::MessageManager::initialize();

	cxtest::DirectSignalListener listener(cx::messageManager(), SIGNAL(emittedMessage(Message)));
	std::cout << "<test string>" << std::endl;
	CHECK(listener.isReceived());

	cx::MessageManager::shutdown();
}

TEST_CASE("MessageManager can be run multiple times", "[unit]")
{
	unsigned numberOfRuns = 3;
	for (unsigned i=0; i<numberOfRuns; ++i)
	{
		cx::MessageManager::initialize();
		std::cout << "<test string>" << std::endl;
		cx::MessageManager::shutdown();
	}
	CHECK(true);
}

TEST_CASE("MessageManager can be run nested", "[unit]")
{
	cx::MessageManager::initialize();
	cx::MessageManager::initialize();
	std::cout << "<test string>" << std::endl;
	cx::MessageManager::shutdown();
	cx::MessageManager::shutdown();

	CHECK(true);
}


} // namespace cx



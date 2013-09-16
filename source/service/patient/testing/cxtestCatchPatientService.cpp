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
#include "sscDataManagerImpl.h"

TEST_CASE("DataManagerImpl setup/shutdown works multiple times", "[unit]")
{
    for (unsigned i=0; i<2; ++i)
    {
		cx::DataManagerImpl::initialize();
		CHECK(cx::dataManager());

		cx::DataManagerImpl::shutdown();
        //REQUIRE_FALSE(dataManager()); //todo: should work
    }
}

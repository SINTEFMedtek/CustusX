
/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include "cxDummyToolManager.h"


TEST_CASE("DummyToolManager setup/shutdown works multiple times", "[unit][org.custus.core.tracking]")
{
    for (unsigned i=0; i<2; ++i)
    {
		cx::TrackingServicePtr service = cx::DummyToolManager::create();
		REQUIRE(service);
		CHECK(service.unique());
		service.reset();
    }
}

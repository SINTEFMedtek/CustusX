/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include "cxtestVisServices.h"

#include "cxMessageListener.h"

namespace cx
{

TEST_CASE("Core test services correctly contructed/destructed", "[unit]")
{
	cx::MessageListenerPtr messageListener = cx::MessageListener::createWithQueue();

	cxtest::TestVisServicesPtr services = cxtest::TestVisServices::create();
	services.reset();

	CHECK(!messageListener->containsErrors());
}

} // namespace cx



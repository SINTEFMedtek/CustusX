/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "catch.hpp"

#include "cxtestUtilities.h"
#include "cxtest_org_custusx_webserver_export.h"

namespace
{
EXPORT_DUMMY_CLASS_FOR_LINKING_ON_WINDOWS_IN_LIB_WITHOUT_EXPORTED_CLASS(CXTEST_ORG_CUSTUSX_WEBSERVER_EXPORT)
}

TEST_CASE("ExamplePlugin: Check nothing", "[unit][plugins][org.custusx.example][hide]")
{
	CHECK(true);
}

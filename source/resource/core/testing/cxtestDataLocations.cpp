/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include "cxDataLocations.h"

namespace cx
{

TEST_CASE("Website url can be set", "[unit][datalocations]")
{
    CHECK(DataLocations::getWebsiteURL().isEmpty());
    cx::DataLocations::setWebsiteURL("custusx.pages.sintef.no/CustusX/");
    CHECK(!DataLocations::getWebsiteURL().isEmpty());
}

TEST_CASE("User documentation url is set", "[unit][datalocations]")
{
    CHECK(!DataLocations::getWebsiteUserDocumentationURL().isEmpty());
}

TEST_CASE("Get user home path", "[unit][datalocations]")
{
	CHECK(!DataLocations::getUserHomePath().isEmpty());
}

} // namespace cx


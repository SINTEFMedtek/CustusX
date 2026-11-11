/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include <QString>
#include "cxConfig.h"

TEST_CASE("The version string is set by CMake", "[unit][resource][core]")
{
	QString version = QString("%1").arg(CustusX_VERSION_STRING);
	CHECK_FALSE(version.isEmpty());
	CHECK_FALSE(version.contains("@"));
	CHECK(version == cx::getVersionString());
}

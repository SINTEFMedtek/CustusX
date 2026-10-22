/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "catch.hpp"

#include "cxStatusBar.h"

namespace cxtest
{

TEST_CASE("StatusBar: tool button style shows tracking status", "[unit][gui]")
{
	CHECK(cx::StatusBar::getToolStyle(true, false, false, false) == "background-color: silver");
	CHECK(cx::StatusBar::getToolStyle(false, true, false, false) == "background-color: orangered");
	CHECK(cx::StatusBar::getToolStyle(true, true, false, false) == "background-color: green");
	CHECK(cx::StatusBar::getToolStyle(true, true, true, false) == "background-color: lime");
}

TEST_CASE("StatusBar: tool button is orange when a visible tool has a position warning", "[unit][gui]")
{
	CHECK(cx::StatusBar::getToolStyle(true, true, false, true) == "background-color: orange");
	CHECK(cx::StatusBar::getToolStyle(true, true, true, true) == "background-color: #ffc04d");
	CHECK(cx::StatusBar::getToolStyle(false, true, false, true) == "background-color: orangered");
	CHECK(cx::StatusBar::getToolStyle(true, false, false, true) == "background-color: silver");
}

} // namespace cxtest

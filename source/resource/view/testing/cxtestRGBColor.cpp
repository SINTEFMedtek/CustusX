/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "catch.hpp"
#include "cxVtkHelperClasses.h"
#include <QColor>

namespace cxtest
{

namespace
{
void checkColor(const cx::RGBColor& color, double r, double g, double b)
{
	CHECK(color[0] == Approx(r));
	CHECK(color[1] == Approx(g));
	CHECK(color[2] == Approx(b));
}
}

TEST_CASE("RGBColor: Constructors set all components", "[unit][resource][visualization]")
{
	const double rgb[3] = {0.1, 0.2, 0.3};
	checkColor(cx::RGBColor(0.4, 0.5, 0.6), 0.4, 0.5, 0.6);
	checkColor(cx::RGBColor(rgb), 0.1, 0.2, 0.3);
	checkColor(cx::RGBColor(QColor::fromRgbF(1.0, 0.5, 0.0)), 1.0, 0.5, 0.0);
}

TEST_CASE("RGBColor: Copy and assignment copy all components", "[unit][resource][visualization]")
{
	const cx::RGBColor original(0.4, 0.5, 0.6);
	const cx::RGBColor copy(original);
	checkColor(copy, 0.4, 0.5, 0.6);

	cx::RGBColor assigned(0.0, 0.0, 0.0);
	assigned = original;
	checkColor(assigned, 0.4, 0.5, 0.6);

	assigned = QColor::fromRgbF(0.0, 1.0, 0.5);
	checkColor(assigned, 0.0, 1.0, 0.5);
}

} // namespace cxtest

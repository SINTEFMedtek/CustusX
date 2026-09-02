/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "catch.hpp"

#include "cxLiverSegmentationWidget.h"
#include "cxLiverVisibilityWidget.h"
#include "cxLiverWidget.h"
#include "cxVisServices.h"

TEST_CASE("LiverPlugin: Construct widgets with null services", "[unit][plugins][org.custusx.liver]")
{
	cx::VisServicesPtr services = cx::VisServices::getNullObjects();

	cx::LiverSegmentationWidget segmentationWidget(services);
	cx::LiverVisibilityWidget visibilityWidget(services);
	cx::LiverWidget liverWidget(services);

	CHECK(true);
}

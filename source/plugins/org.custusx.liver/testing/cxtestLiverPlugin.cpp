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

TEST_CASE("LiverPlugin: filterLabel() is non-empty for every filter", "[unit][plugins][org.custusx.liver]")
{
	using cx::LiverSegmentationWidget;
	CHECK_FALSE(LiverSegmentationWidget::filterLabel(LiverSegmentationWidget::fkLiverPancreas).isEmpty());
	CHECK_FALSE(LiverSegmentationWidget::filterLabel(LiverSegmentationWidget::fkLiverVessels).isEmpty());
	CHECK_FALSE(LiverSegmentationWidget::filterLabel(LiverSegmentationWidget::fkLiverLesions).isEmpty());
	CHECK_FALSE(LiverSegmentationWidget::filterLabel(LiverSegmentationWidget::fkLiverSegments).isEmpty());
}

TEST_CASE("LiverPlugin: iniFileNameFor() has no MR ini for Liver Vessels only", "[unit][plugins][org.custusx.liver]")
{
	using cx::LiverSegmentationWidget;

	// Every CT filter has an ini file.
	CHECK_FALSE(LiverSegmentationWidget::iniFileNameFor(LiverSegmentationWidget::fkLiverPancreas, cx::imCT).isEmpty());
	CHECK_FALSE(LiverSegmentationWidget::iniFileNameFor(LiverSegmentationWidget::fkLiverVessels, cx::imCT).isEmpty());
	CHECK_FALSE(LiverSegmentationWidget::iniFileNameFor(LiverSegmentationWidget::fkLiverLesions, cx::imCT).isEmpty());
	CHECK_FALSE(LiverSegmentationWidget::iniFileNameFor(LiverSegmentationWidget::fkLiverSegments, cx::imCT).isEmpty());

	// Vessels has no MR-capable TotalSegmentator model - every other MR filter does.
	CHECK(LiverSegmentationWidget::iniFileNameFor(LiverSegmentationWidget::fkLiverVessels, cx::imMR).isEmpty());
	CHECK_FALSE(LiverSegmentationWidget::iniFileNameFor(LiverSegmentationWidget::fkLiverPancreas, cx::imMR).isEmpty());
	CHECK_FALSE(LiverSegmentationWidget::iniFileNameFor(LiverSegmentationWidget::fkLiverLesions, cx::imMR).isEmpty());
	CHECK_FALSE(LiverSegmentationWidget::iniFileNameFor(LiverSegmentationWidget::fkLiverSegments, cx::imMR).isEmpty());

	// CT and MR ini files for the same filter must differ.
	CHECK(LiverSegmentationWidget::iniFileNameFor(LiverSegmentationWidget::fkLiverPancreas, cx::imCT)
	      != LiverSegmentationWidget::iniFileNameFor(LiverSegmentationWidget::fkLiverPancreas, cx::imMR));

	// An unhandled modality yields no ini file for any filter.
	CHECK(LiverSegmentationWidget::iniFileNameFor(LiverSegmentationWidget::fkLiverPancreas, cx::imUS).isEmpty());
}

TEST_CASE("LiverPlugin: needsPreparation() is scoped to the heaviest-smoothing filters", "[unit][plugins][org.custusx.liver]")
{
	using cx::LiverSegmentationWidget;
	CHECK(LiverSegmentationWidget::needsPreparation(LiverSegmentationWidget::fkLiverPancreas));
	CHECK(LiverSegmentationWidget::needsPreparation(LiverSegmentationWidget::fkLiverSegments));
	CHECK_FALSE(LiverSegmentationWidget::needsPreparation(LiverSegmentationWidget::fkLiverVessels));
	CHECK_FALSE(LiverSegmentationWidget::needsPreparation(LiverSegmentationWidget::fkLiverLesions));
}

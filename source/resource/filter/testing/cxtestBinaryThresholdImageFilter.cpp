/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include "cxtestfilter_export.h"
#include "cxBinaryThresholdImageFilter.h"
#include "cxtestVisServices.h"

namespace
{
typedef boost::shared_ptr<class BinaryThresholdImageFilterFixture> BinaryThresholdImageFilterFixturePtr;
class BinaryThresholdImageFilterFixture : public cx::BinaryThresholdImageFilter
{
public:
	BinaryThresholdImageFilterFixture(cx::VisServicesPtr services) :
		cx::BinaryThresholdImageFilter(services)
	{}
	void init()
	{
		this->getOptions();
		this->getInputTypes();
		this->getOutputTypes();
		this->setActive(true);
	}
	void callThresholdSlot()
	{
		this->thresholdSlot();
	}
	cx::ImagePtr getPreviewImage()
	{
		return mPreviewImage;
	}

};
}

TEST_CASE("BinaryThresholdImageFilter: update threshold with no image selected", "[unit]")
{
	cxtest::TestVisServicesPtr dummyservices = cxtest::TestVisServices::create();
	BinaryThresholdImageFilterFixturePtr filterFixture = BinaryThresholdImageFilterFixturePtr(new BinaryThresholdImageFilterFixture(dummyservices));
	REQUIRE(filterFixture);

	filterFixture->init();
	filterFixture->callThresholdSlot();
	REQUIRE_FALSE(filterFixture->getPreviewImage());
}

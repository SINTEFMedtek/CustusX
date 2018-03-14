/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "catch.hpp"
#include "cxtestVisServices.h"
#include "cxStream2DRep3D.h"
#include "cxTrackedStream.h"
#include "cxDummyTool.h"
#include "cxTestVideoSource.h"

TEST_CASE("Stream2DRep3D init", "[unit][resource]")
{
	cxtest::TestVisServicesPtr mServices = cxtest::TestVisServices::create();
	cx::Stream2DRep3DPtr rep = cx::Stream2DRep3D::New(mServices->spaceProvider());
	REQUIRE(rep);
}

TEST_CASE("Stream2DRep3D Set TrackedStream", "[unit][resource]")
{
	cxtest::TestVisServicesPtr mServices = cxtest::TestVisServices::create();
	cx::Stream2DRep3DPtr rep = cx::Stream2DRep3D::New(mServices->spaceProvider());
	REQUIRE(rep);

	cx::TrackedStreamPtr trackedStream = cx::TrackedStream::create("streamUid", "streamName");
	rep->setTrackedStream(trackedStream);
	REQUIRE_FALSE(rep->isReady());

	cx::DummyToolPtr dummyTool(new cx::DummyTool());
	cx::TestVideoSourcePtr testVideoSource(new cx::TestVideoSource("TestVideoSourceUid", "TestVideoSource" , 80, 40));

	trackedStream->setProbeTool(dummyTool);
	trackedStream->setVideoSource(testVideoSource);
	REQUIRE(rep->isReady());
}
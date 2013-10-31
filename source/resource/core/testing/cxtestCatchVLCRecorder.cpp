// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOsT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "catch.hpp"

#include "cxVLCRecorder.h"
#include "cxtestVLCRecorderFixture.h"

#ifdef CX_WINDOWS
#include <windows.h>
#endif

namespace cxtest
{

TEST_CASE("VLCRecorder can be constructed", "[unit][resource][core][VLCRecorder][VLC]")
{
	VLCRecorderFixture fix;
	REQUIRE(cx::vlc());
}

TEST_CASE("VLCRecorder can find VLC application", "[unit][resource][core][VLCRecorder][VLC]")
{
	VLCRecorderFixture fix;
	CHECK(cx::vlc()->hasVLCApplication());
}

TEST_CASE("VLCRecorder can record for 7 seconds", "[integration][resource][core][VLCRecorder][VLC][unstable]")
{
	VLCRecorderFixture vlc;

	vlc.checkThatVLCCanRecordTheScreen(7);

	vlc.checkIsMovieFilePlayable();
}

} //namespace cxtest

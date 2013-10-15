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

#include <QTimer>
#include "sscMessageManager.h"
#include "cxVLCRecorder.h"

TEST_CASE("VLCRecorder can be constructed", "[unit][resource][core][VLCRecorder][VLC]")
{
	cx::messageManager()->initialize();
	CHECK(cx::vlc());
	cx::messageManager()->shutdown();
}

TEST_CASE("VLCRecorder can find VLC application", "[unit][resource][core][VLCRecorder][VLC]")
{
	cx::messageManager()->initialize();
	CHECK(cx::vlc()->hasVLCApplication());
	cx::messageManager()->shutdown();
}

TEST_CASE("VLCRecorder can record", "[unit][resource][core][VLCRecorder][VLC][jb]")
{
	cx::messageManager()->initialize();

	CHECK_FALSE(cx::vlc()->isRecording());
	cx::vlc()->startRecording("/Users/jbake/Desktop/video/file.mp4");
	cx::vlc()->waitForStarted();

	CHECK(cx::vlc()->isRecording());
	sleep(3);
	cx::vlc()->stopRecording();

//	QTimer::singleShot(1000, cx::vlc(), SLOT(stopRecording()));
	cx::vlc()->waitForFinished();
	CHECK_FALSE(cx::vlc()->isRecording());

	cx::messageManager()->shutdown();
}

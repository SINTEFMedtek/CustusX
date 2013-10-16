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

#include <QDir>
#include <QTimer>
#include "sscMessageManager.h"
#include "cxDataLocations.h"
#include "cxVLCRecorder.h"

#ifdef CX_WINDOWS
#include <windows.h>
#endif

TEST_CASE("VLCRecorder can be constructed", "[unit][resource][core][VLCRecorder][VLC]")
{
	cx::messageManager()->initialize();
	CHECK(cx::vlc());
	cx::messageManager()->shutdown();
}

TEST_CASE("VLCRecorder can find VLC application", "[unit][resource][core][VLCRecorder][VLC][hide]")
{
	cx::messageManager()->initialize();
	CHECK(cx::vlc()->hasVLCApplication());
	cx::messageManager()->shutdown();
}

TEST_CASE("VLCRecorder can record", "[integration][resource][core][VLCRecorder][VLC][hide]")
{
	//TODO this test does not fail even if vlc fails.... needs to be fixed.

	cx::messageManager()->initialize();

	CHECK_FALSE(cx::vlc()->isRecording());
	QString path = cx::DataLocations::getTestDataPath() + "/testing/VLC/";
	QDir().mkpath(path);
	cx::vlc()->startRecording(path+"screen_video.mp4");
	cx::vlc()->waitForStarted();

	CHECK(cx::vlc()->isRecording());

#ifndef CX_WINDOWS
		sleep(15); //seconds
#else
		Sleep(15000); //milliseconds
#endif

	cx::vlc()->stopRecording();

	cx::vlc()->waitForFinished();
	CHECK_FALSE(cx::vlc()->isRecording());

	cx::messageManager()->shutdown();
}

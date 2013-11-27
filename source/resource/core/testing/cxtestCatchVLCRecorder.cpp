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

#include <QProcess>
#include "cxtestUtilities.h"
#include <QDebug>

namespace cxtest
{

TEST_CASE("VLCRecorder can be constructed", "[unit][resource][core][VLCRecorder][VLC][unstable]")
{
	VLCRecorderFixture fix;
	REQUIRE(cx::vlc());
}

TEST_CASE("VLCRecorder can find VLC application", "[unit][resource][core][VLCRecorder][VLC][unstable]")
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


TEST_CASE("QProcess and VLC can use cli for starting and stopping screen capture.", "[VLC][proof_of_concept][hide][not_linux][not_win32][not_win64][not_apple]")
{
	QProcess* p = new QProcess();
	p->setProcessChannelMode(QProcess::MergedChannels);
	p->setReadChannel(QProcess::StandardOutput);

	QString pathToVLC = "/Applications/VLC.app/Contents/MacOS/VLC";
	QString saveFile = "/Users/jbake/Desktop/video/file.mp4";

	p->start(pathToVLC+" -I cli screen:// \":sout=#transcode{vcodec=h264,vb=800,fps=10,scale=1,acodec=none}:duplicate{dst=standard{access=file,mux=mp4,dst="+saveFile+"}}\"");
	REQUIRE(p->waitForStarted());

	//How long the thread needs too sleep (startup time) depends on several things, cpu load is one of them...
	Utilities::sleep_sec(9);

	std::cout << p->write("quit\n") << std::endl;

	REQUIRE(p->waitForFinished());

	qDebug() << p->readAllStandardOutput();
}



} //namespace cxtest

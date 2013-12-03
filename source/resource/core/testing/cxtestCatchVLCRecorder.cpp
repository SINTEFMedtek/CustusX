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
#include <QDir>
#include <QTcpSocket>

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

TEST_CASE("VLCRecorder can record for 15 seconds", "[integration][resource][core][VLCRecorder][VLC][unstable]")
{
	VLCRecorderFixture vlc;

	vlc.checkThatVLCCanRecordTheScreen(15);

	vlc.checkIsMovieFileOfValidSize();
}


TEST_CASE("QProcess and VLC can use cli for starting and stopping screen capture.", "[jb][VLC][proof_of_concept][hide][not_linux][not_win32][not_win64][not_apple]")
{
	QProcess* p = new QProcess();
	p->setProcessChannelMode(QProcess::MergedChannels);
	p->setReadChannel(QProcess::StandardOutput);

#ifdef CX_APPLE
	QString pathToVLC = "/Applications/VLC.app/Contents/MacOS/VLC";
	QString saveFile = "/Users/jbake/Desktop/video/file.mp4";

	p->start(pathToVLC+" -I cli screen:// \":sout=#transcode{vcodec=h264,vb=800,fps=10,scale=1,acodec=none}:duplicate{dst=standard{access=file,mux=mp4,dst="+saveFile+"}}\"");
#endif
#ifdef CX_WINDOWS
	QString pathToVLC = "\"C:/Program Files (x86)/VideoLAN/VLC/vlc.exe\"";
	QString saveFile = "\"C:/Users/Dev/Desktop/test.mp4\"";

	QDir vlcDir("C:/Program Files (x86)/VideoLAN/VLC/");
	qDebug() << "Working dir: " << vlcDir.absolutePath();
	p->setWorkingDirectory(vlcDir.absolutePath());

	QStringList env = QProcess::systemEnvironment();
	env.replaceInStrings(QRegExp("^PATH=(.*)", Qt::CaseInsensitive), "PATH=\\1;C:\\Program Files (x86)\\VideoLAN\\VLC");
	p->setEnvironment(env);
	//qDebug() << "Env: " << p->environment();

	//p->start(pathToVLC+" -I luacli screen:// :screen-fps=10.000000 :live-caching=300 :sout=#transcode{vcodec=h264,acodec=none}:file{dst="+saveFile+"} :sout-keep");
	//p->start(pathToVLC+" -I luacli");
	p->start(pathToVLC, QStringList() <<"-I" << "luacli" << "-vvv");
	//p->start("vlc.exe -I luacli");
#endif

	REQUIRE(p->waitForStarted());

	//How long the thread needs too sleep (startup time) depends on several things, cpu load is one of them...
	Utilities::sleep_sec(3);

	qDebug() << "PID: " << QString::number((qint64)p->pid());
	//qDebug() << "Program: " << p->program();
	qDebug() << "ErrorString: " << p->errorString();
	qDebug() << "isOpen: " << p->isOpen();
	qDebug() << "isReadable: " << p->isReadable();
	qDebug() << "isWritable: " << p->isWritable();
	qDebug() << "workingDirectory: " << p->workingDirectory();
	//qDebug() << "" << p->
	//qDebug() << "" << p->
	//qDebug() << "" << p->
	qDebug() << "State: " << p->state();

#ifdef CX_APPLE
	QString quit("quit");
	std::cout << p->write((quit + "\n").toLocal8Bit()) << std::endl;
#endif
#ifdef CX_WINDOWS
	QTcpSocket* tcp = new QTcpSocket();
	tcp->connectToHost("localhost", 4212);
	//REQUIRE(p->waitForFinished());
	qDebug() << tcp->write("quit\r\n");
	//REQUIRE(p->waitForBytesWritten());
	//p->closeWriteChannel();
#endif

	CHECK(p->waitForFinished());

	qDebug() << p->readAllStandardOutput();

	delete p;
}



} //namespace cxtest

/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include <cxVLCRecorder.h>

#include <QFileInfo>

#include <QProcess>
#include "cxLogger.h"

namespace cx
{

VLCRecorder* VLCRecorder::mTheInstance = NULL;
VLCRecorder* vlc(){ return VLCRecorder::getInstance(); }

VLCRecorder* VLCRecorder::getInstance()
{
  if(mTheInstance == NULL)
  {
    mTheInstance = new VLCRecorder();
  }
  return mTheInstance;
}

void VLCRecorder::initialize()
{
	VLCRecorder::getInstance();
}

void VLCRecorder::shutdown()
{
  delete mTheInstance;
  mTheInstance = NULL;
}

VLCRecorder::VLCRecorder() :
		mCommandLine(new ProcessWrapper("VLC")), mVLCPath("")
{
	connect(mCommandLine->getProcess(), &QProcess::stateChanged, this, &VLCRecorder::stateChanged);
	this->findVLCApplication();
}

VLCRecorder::~VLCRecorder()
{}

bool VLCRecorder::hasVLCApplication()
{
	return QFile::exists(mVLCPath);
}

void VLCRecorder::findVLCApplication(QStringList suggestedVLCLocations)
{
	suggestedVLCLocations.push_front(this->getVLCDefaultLocation());
	foreach(QString path, suggestedVLCLocations)
	{
		if(this->isValidVLC(path))
		{
			this->setVLCPath(path);
			return;
		}
	}
}

bool VLCRecorder::isRecording()
{
	return mCommandLine->isRunning();
}

bool VLCRecorder::waitForStarted(int msecs)
{
	return mCommandLine->waitForStarted(msecs);
}

bool VLCRecorder::waitForFinished(int msecs)
{
	return mCommandLine->waitForFinished(msecs);
}

QString VLCRecorder::getVLCPath()
{
	return mVLCPath;
}

void VLCRecorder::startRecording(QString saveFile)
{
	if(this->hasVLCApplication())
		mCommandLine->launch("\""+mVLCPath+"\""+this->getVLCDefaultRecorderArguments(saveFile));
	else
		reportError("VLC not found.");
}

void VLCRecorder::stopRecording()
{
	QString quit = "quit\n";
	mCommandLine->write(quit.toStdString().c_str());
}

void VLCRecorder::setVLCPath(QString path)
{
	mVLCPath = path;
	report("Found valid VLC application: "+mVLCPath);
}

bool VLCRecorder::isValidVLC(QString vlcPath)
{
	QFileInfo info(vlcPath);
	if(info.exists() && !QString::compare(info.baseName(), "vlc", Qt::CaseInsensitive))
		return true;
	return false;
}

QString VLCRecorder::getVLCDefaultLocation()
{
	QString defaultLocation("");
#ifdef CX_WINDOWS
	defaultLocation = "C:/Program Files (x86)/VideoLAN/VLC/vlc.exe";
#endif
#ifdef CX_APPLE
	defaultLocation = "/Applications/VLC.app/Contents/MacOS/VLC";
#endif
#ifdef CX_LINUX
	defaultLocation = "/usr/bin/vlc";
#endif
	return defaultLocation;
}

QString VLCRecorder::getVLCDefaultRecorderArguments(QString saveFile)
{
	QString defaultArguements("");
#ifdef CX_WINDOWS
	saveFile = saveFile.replace("/", "\\");
	defaultArguements = " -I luacli screen:// :screen-fps=10.000000 :live-caching=300 :sout=#transcode{vcodec=h264,acodec=none}:file{dst="+saveFile+"} :sout-keep ";
#endif
#ifdef CX_APPLE
    CX_LOG_WARNING("VLC 2.2.1 fails on Mac. VLC version 2.1.2 works.");
	defaultArguements = " -I cli screen:// \":sout=#transcode{vcodec=h264,vb=800,fps=10,scale=1,acodec=none}:duplicate{dst=standard{access=file,mux=mp4,dst="+saveFile+"}}\"";
#endif
#ifdef CX_LINUX
	defaultArguements = " -I cli screen:// :screen-fps=10.000000 :live-caching=300 \":sout=#transcode{vcodec=h264,vb=0,fps=10,scale=0,acodec=none}:file{dst="+saveFile+"}\" :sout-keep";
#endif
	return defaultArguements;
}

} /* namespace cx */

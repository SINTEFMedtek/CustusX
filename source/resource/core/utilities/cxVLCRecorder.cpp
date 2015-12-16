/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

#include <cxVLCRecorder.h>

#include <QFileInfo>
#include "sscMessageManager.h"

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

VLCRecorder::VLCRecorder() :
		mCommandLine(new ProcessWrapper("VLC")), mVLCPath("")
{
	this->findVLCApplication();
}

VLCRecorder::~VLCRecorder()
{}

bool VLCRecorder::hasVLCApplication()
{
	return QFile::exists(mVLCPath);
}

void VLCRecorder::findVLCApplication(QStringList searchPaths)
{
	searchPaths.push_front(this->getVLCDefaultLocation());
	foreach(QString path, searchPaths)
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

void VLCRecorder::waitForStarted()
{
	mCommandLine->getProcess()->waitForStarted();
}

void VLCRecorder::waitForFinished()
{
	mCommandLine->getProcess()->waitForFinished();
}

void VLCRecorder::startRecording(QString saveFile)
{
	QStringList args = this->getVLCArguements(saveFile);
	if(this->hasVLCApplication())
		mCommandLine->launch(mVLCPath, args);
}

void VLCRecorder::stopRecording()
{
	mCommandLine->requestTerminateSlot();
}

void VLCRecorder::setVLCPath(QString path)
{
	mVLCPath = path;
	messageManager()->sendInfo("Found valid VLC application: "+mVLCPath);
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
	defaultLocation = "";
#endif
#ifdef CX_APPLE
	defaultLocation = "/Applications/VLC.app/Contents/MacOS/VLC";
#endif
#ifdef CX_LINUX
	defaultLocation = "/usr/bin/vlc";
#endif
	return defaultLocation;
}

QStringList VLCRecorder::getVLCArguements(QString saveFile)
{
	QStringList list;
	list << "--intf";
	list << "hotkeys";
	list << "-vvv";
	list << "screen://";
	list << "\':sout=#transcode{vcodec=h264,vb=800,fps=10,scale=1,acodec=none}:standard{access=file,mux=mp4,dst=/Users/jbake/Desktop/video/file.mp4}\'";
//	list << "--sout-standard-access='file'";
//	list << "--sout-standard-mux='mp4'";
//	list << "--sout-standard-dst='/Users/jbake/Desktop/video/file.mp4'";
//	list << "--sout-transcode-vcodec='h264'";
//	list << "--sout-transcode-vb=800";
//	list << "--sout-transcode-fps=10";
//	list << "--sout-transcode-scale=1";
//	list << "--sout-transcode-acodec='none'";

	return list;
}


} /* namespace cx */

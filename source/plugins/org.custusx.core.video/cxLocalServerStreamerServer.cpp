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
#include "cxLocalServerStreamerServer.h"

#include "cxStringProperty.h"
#include "cxDoubleProperty.h"
#include "cxBoolProperty.h"
#include "cxIGTLinkClientStreamer.h"
#include "cxImageStreamerOpenCV.h"
#include "cxUtilHelpers.h"
#include "cxLogger.h"
#include "QApplication"
#include <QDir>
#include "cxDataLocations.h"
#include "cxTypeConversions.h"
#include "cxFilePathProperty.h"


namespace cx
{


std::vector<PropertyPtr> LocalServerStreamerArguments::getSettings(QDomElement root)
{
	std::vector<PropertyPtr> retval;
	retval.push_back(this->getRunLocalServerOption(root));
	retval.push_back(this->getLocalServerNameOption(root));
	return retval;
}

BoolPropertyBasePtr LocalServerStreamerArguments::getRunLocalServerOption(QDomElement root)
{
	BoolPropertyPtr retval;

#ifdef __APPLE__
	bool defaultValue = true; // problems with opencv on mac - cannot close
#else
	bool defaultValue = false;
#endif

	retval = BoolProperty::initialize("runlocalserver", "Run as separate process",
											"Run streamer in a separate process",
											defaultValue, root);
	retval->setAdvanced(false);
	retval->setGroup("Connection");
	return retval;
}

FilePathPropertyPtr LocalServerStreamerArguments::getLocalServerNameOption(QDomElement root)
{
	QString filename = "OpenIGTLinkServer";
#ifdef WIN32
	filename += ".exe";
#endif

	QStringList paths = QStringList() << qApp->applicationDirPath();
#ifdef __APPLE__
	paths << QString("%1/%2.app/Contents/MacOS").arg(DataLocations::getBundlePath()).arg(filename);
#endif

	FilePathPropertyPtr retval;
	retval = FilePathProperty::initialize("localservername", "Server Name",
										  "Name of server executable, used only if Run Local Server is set.",
										  filename,
										  paths,
										  root);
	retval->setAdvanced(false);
	retval->setGroup("Connection");
	return retval;
}


///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

StreamerPtr LocalServerStreamer::createStreamerIfEnabled(QDomElement root, StringMap args)
{
	bool useLocalServer = LocalServerStreamerArguments().getRunLocalServerOption(root)->getValue();

	if (!useLocalServer)
		return StreamerPtr();

	QStringList cmdlineArguments;
	for (StringMap::iterator i=args.begin(); i!=args.end(); ++i)
		cmdlineArguments << i->first << i->second;

	QString localServer = LocalServerStreamerArguments().getLocalServerNameOption(root)->getValue();
	boost::shared_ptr<LocalServerStreamer> streamer;
	streamer.reset(new LocalServerStreamer(localServer, cmdlineArguments.join(" ")));

	return streamer;
}

LocalServerStreamer::LocalServerStreamer(QString serverName, QString serverArguments) :
	mServerName(serverName),
	mServerArguments(serverArguments)
{
	mLocalVideoServerProcess.reset(new ProcessWrapper(QString("Local Video Server: %1").arg(mServerName)));

	boost::shared_ptr<IGTLinkClientStreamer> igtLinkStreamer(new IGTLinkClientStreamer());
	int defaultport = 18333;
	igtLinkStreamer->setAddress("Localhost", defaultport);
	mBase = igtLinkStreamer;
}

LocalServerStreamer::~LocalServerStreamer()
{

}

bool LocalServerStreamer::startStreaming(SenderPtr sender)
{
	mLocalVideoServerProcess->launchWithRelativePath(mServerName, mServerArguments.split(" "));

	this->waitForServerStart();
	if (!this->localVideoServerIsRunning())
	{
		reportError("Local server failed to start");
		return false;
	}

	return mBase->startStreaming(sender);
}

void LocalServerStreamer::waitForServerStart()
{
	int waitTime = 5000;
	while (waitTime > 0)
	{
		qApp->processEvents();
		if (this->localVideoServerIsRunning())
			return;
		int interval = 50;
		sleep_ms(interval);
		waitTime -= interval;
	}
}

void LocalServerStreamer::stopStreaming()
{
	mBase->stopStreaming();

	if (mLocalVideoServerProcess->getProcess())
		mLocalVideoServerProcess->getProcess()->close();
}

bool LocalServerStreamer::localVideoServerIsRunning()
{
	if (!mLocalVideoServerProcess->getProcess())
		return false;
	return this->mLocalVideoServerProcess->getProcess()->state() == QProcess::Running;
}


} // namespace cx

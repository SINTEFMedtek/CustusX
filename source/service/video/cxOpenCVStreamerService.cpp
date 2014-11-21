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
#include "cxOpenCVStreamerService.h"

#include "cxStringDataAdapterXml.h"
#include "cxDoubleDataAdapterXml.h"
#include "cxBoolDataAdapterXml.h"
#include "cxIGTLinkClientStreamer.h"
#include "cxImageStreamerOpenCV.h"
#include "cxUtilHelpers.h"
#include "cxReporter.h"
#include "QApplication"

namespace cx
{


LocalServerStreamer::LocalServerStreamer(QString serverName, QString serverArguments) :
	mServerName(serverName),
	mServerArguments(serverArguments)
{
	mReconnectInterval = 400;
	mConnectWhenLocalServerRunning = 0;

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

	return this->attemptStartStreaming(sender);
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

bool LocalServerStreamer::attemptStartStreaming(SenderPtr sender)
{
	// hold here until all attempts are finished
	int numberOfConnectionAttempts = 5;
	for (int i=0; i<numberOfConnectionAttempts; ++i)
	{
		if (i>0)
			report(QString("Attempt %1 to connect to streamer %2").arg(i+1).arg(mBase->getType()));
		if (mBase->startStreaming(sender))
			return true;
		sleep_ms(mReconnectInterval);
	}
	return false;
}

void LocalServerStreamer::stopStreaming()
{
	mBase->stopStreaming();

	if (mLocalVideoServerProcess->getProcess())
		mLocalVideoServerProcess->getProcess()->close();
}

QString LocalServerStreamer::getType()
{
	return "LocalServer";
}

bool LocalServerStreamer::localVideoServerIsRunning()
{
	if (!mLocalVideoServerProcess->getProcess())
		return false;
	return this->mLocalVideoServerProcess->getProcess()->state() == QProcess::Running;
}



///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------


QString OpenCVStreamerService::getName()
{
	return "OpenCV_new";
}

std::vector<DataAdapterPtr> OpenCVStreamerService::getSettings(QDomElement root)
{
	std::vector<DataAdapterPtr> retval;
	std::vector<DataAdapterPtr> opencvArgs = ImageStreamerOpenCVArguments().getSettings(root);
	std::copy(opencvArgs.begin(), opencvArgs.end(), back_inserter(retval));
	retval.push_back(this->getRunLocalServerOption(root));
	retval.push_back(this->getLocalServerNameOption(root));
	return retval;
}

BoolDataAdapterPtr OpenCVStreamerService::getRunLocalServerOption(QDomElement root)
{
	BoolDataAdapterXmlPtr retval;
	bool defaultValue = false;
	retval = BoolDataAdapterXml::initialize("runlocalserver", "Run Local Server",
											"Run streamer in a separate process",
											defaultValue, root);
	retval->setAdvanced(false);
	retval->setGroup("Connection");
	return retval;
}
StringDataAdapterPtr OpenCVStreamerService::getLocalServerNameOption(QDomElement root)
{
	StringDataAdapterXmlPtr retval;
	QString defaultValue = "OpenIGTLinkServer";
	retval = StringDataAdapterXml::initialize("localservername", "Server Name",
											  "Name of server executable, used only if Run Local Server is set.",
											  defaultValue, root);
	retval->setAdvanced(false);
	retval->setGroup("Connection");
	return retval;
}

StreamerPtr OpenCVStreamerService::createStreamer(QDomElement root)
{
	bool useLocalServer = this->getRunLocalServerOption(root)->getValue();
	StringMap args = ImageStreamerOpenCVArguments().convertToCommandLineArguments(root);

	if (useLocalServer)
	{
		QStringList cmdlineArguments;
		for (StringMap::iterator i=args.begin(); i!=args.end(); ++i)
			cmdlineArguments << i->first << i->second;

		QString localServer = this->getLocalServerNameOption(root)->getValue();
		boost::shared_ptr<LocalServerStreamer> streamer;
		streamer.reset(new LocalServerStreamer(localServer, cmdlineArguments.join(" ")));

		return streamer;
	}
	else
	{
		boost::shared_ptr<ImageStreamerOpenCV> streamer(new ImageStreamerOpenCV());
		streamer->initialize(args);
		return streamer;
	}
}

ReceiverPtr OpenCVStreamerService::createReceiver(QDomElement root)
{
	return ReceiverPtr();
}

} // namespace cx

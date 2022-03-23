/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
	return std::move(retval);//Fixed copy bug on old compilers
}

FilePathPropertyPtr LocalServerStreamerArguments::getLocalServerNameOption(QDomElement root)
{
	QString filename = "OpenIGTLinkServer";
#ifdef WIN32
	filename += ".exe";
#endif

	QStringList paths = QStringList() << qApp->applicationDirPath();
#ifdef __APPLE__
    // special case for running from the build tree, server built as bundle.
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

	FilePathPropertyPtr localServerProp = LocalServerStreamerArguments().getLocalServerNameOption(root);
	QString localServer = localServerProp->getEmbeddedPath().getAbsoluteFilepath();
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

void LocalServerStreamer::startStreaming(SenderPtr sender)
{
    mSender = sender;
    connect(mLocalVideoServerProcess.get(), &ProcessWrapper::stateChanged, this, &LocalServerStreamer::processStateChanged);
    mLocalVideoServerProcess->launchWithRelativePath(mServerName, mServerArguments.split(" "));
}

void LocalServerStreamer::processStateChanged()
{
	if(mLocalVideoServerProcess->isRunning())
		mBase->startStreaming(mSender);
}

void LocalServerStreamer::stopStreaming()
{
	mBase->stopStreaming();

	if (mLocalVideoServerProcess->getProcess())
	{
		mLocalVideoServerProcess->getProcess()->close();
		mLocalVideoServerProcess.reset();
	}
}

bool LocalServerStreamer::isStreaming()
{
	return localVideoServerIsRunning();
}

bool LocalServerStreamer::localVideoServerIsRunning()
{
	if (!mLocalVideoServerProcess || !mLocalVideoServerProcess->getProcess())
		return false;
	return this->mLocalVideoServerProcess->isRunning();
}


} // namespace cx

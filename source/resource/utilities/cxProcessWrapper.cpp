// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include <cxProcessWrapper.h>
#include <QtCore>
#include "cxDataLocations.h"
#include "sscMessageManager.h"
#include "sscTypeConversions.h"

namespace cx
{

ProcessWrapper::ProcessWrapper(QString name, QObject* parent) : QObject(parent), mName(name)
{
	mProcess = new QProcess(this);
	connect(mProcess, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(serverProcessStateChanged(QProcess::ProcessState)));
	connect(mProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(serverProcessError(QProcess::ProcessError)));

	connect(mProcess, SIGNAL(readyRead()), this, SLOT(serverProcessReadyRead()));
	mProcess->setProcessChannelMode(QProcess::MergedChannels);
	mProcess->setReadChannel(QProcess::StandardOutput);
}

ProcessWrapper::~ProcessWrapper()
{
	disconnect(mProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(serverProcessError(QProcess::ProcessError)));
	mProcess->close();
}

QProcess* ProcessWrapper::getProcess()
{
	return mProcess;
}

void ProcessWrapper::launch(QString executable, QStringList arguments)
{
	if (executable.isEmpty())
		return;
	if (mProcess->state() != QProcess::NotRunning)
		return;

	if (!QFileInfo(executable).isAbsolute())
		executable = DataLocations::getBundlePath() + "/" + executable;

	executable = executable.trimmed();
	executable = QDir::cleanPath(executable);

	if (!QFileInfo(executable).exists())
	{
		ssc::messageManager()->sendError(QString("Cannot find %1 [%2]").arg(mName).arg(executable));
		return;
	}

	ssc::messageManager()->sendInfo(QString("Launching %1 %2 with arguments %3").arg(mName).arg(executable).arg(arguments.join(", ")));

	if (mProcess->state() == QProcess::NotRunning)
		mProcess->start(executable, arguments);
}

void ProcessWrapper::serverProcessError(QProcess::ProcessError error)
{
	QString msg;
	msg += QString("%1 reported an error: ").arg(mName);

	switch (error)
	{
	case QProcess::FailedToStart:
		msg += "Failed to start";
		break;
	case QProcess::Crashed:
		msg += "Crashed";
		break;
	case QProcess::Timedout:
		msg += "Timed out";
		break;
	case QProcess::WriteError:
		msg += "Write Error";
		break;
	case QProcess::ReadError:
		msg += "Read Error";
		break;
	case QProcess::UnknownError:
		msg += "Unknown Error";
		break;
	default:
		msg += "Invalid error";
	}

	ssc::messageManager()->sendError(msg);
}

void ProcessWrapper::serverProcessStateChanged(QProcess::ProcessState newState)
{
	if (newState == QProcess::Running)
	{
		ssc::messageManager()->sendInfo(QString("%1 running.").arg(mName));
//		this->delayedAutoConnectServer();
	}
	if (newState == QProcess::NotRunning)
	{
		ssc::messageManager()->sendInfo(QString("%1 not running.").arg(mName));
	}
	if (newState == QProcess::Starting)
	{
		ssc::messageManager()->sendInfo(QString("%1 starting.").arg(mName));
	}
}

void ProcessWrapper::serverProcessReadyRead()
{
	ssc::messageManager()->sendInfo(QString(mProcess->readAllStandardOutput()));
}




}

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

#include "cxProcessWrapper.h"
#include <QDir>
#include <QFileInfo>
#include "cxDataLocations.h"
#include "sscMessageManager.h"
#include "sscTypeConversions.h"

namespace cx
{

ProcessWrapper::ProcessWrapper(QString name, QObject* parent) :
		QObject(parent), mName(name), mLastExecutablePath("")
{
	mProcess = new QProcess(this);
	connect(mProcess, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(processStateChanged(QProcess::ProcessState)));
	connect(mProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));
	connect(mProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
	connect(mProcess, SIGNAL(readyRead()), this, SLOT(processReadyRead()));

	mProcess->setProcessChannelMode(QProcess::MergedChannels);
	mProcess->setReadChannel(QProcess::StandardOutput);
}

ProcessWrapper::~ProcessWrapper()
{
	disconnect(mProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));
	mProcess->close();
}

QProcess* ProcessWrapper::getProcess()
{
	return mProcess;
}

void ProcessWrapper::launchWithRelativePath(QString executable, QStringList arguments)
{
	QString absolutePathToExe = this->getExecutableInBundlesAbsolutePath(executable);

	this->launch(absolutePathToExe, arguments);
}

void ProcessWrapper::launch(QString executable, QStringList arguments)
{
	if (executable.isEmpty() || this->isRunning())
		return;

	this->internalLaunch(executable, arguments);
}

bool ProcessWrapper::isRunning()
{
	return mProcess->pid() != 0;
}

void ProcessWrapper::requestTerminateSlot()
{
	mProcess->terminate();
	messageManager()->sendInfo(QString("Requesting termination of %1 %2").arg(mName).arg(mLastExecutablePath));
	mProcess->waitForFinished();
}

void ProcessWrapper::processReadyRead()
{
	messageManager()->sendInfo(QString(mProcess->readAllStandardOutput()));
}

void ProcessWrapper::processStateChanged(QProcess::ProcessState newState)
{
	if (newState == QProcess::Running)
	{
		messageManager()->sendInfo(QString("%1 running.").arg(mName));
	}
	if (newState == QProcess::NotRunning)
	{
		messageManager()->sendInfo(QString("%1 not running.").arg(mName));
	}
	if (newState == QProcess::Starting)
	{
		messageManager()->sendInfo(QString("%1 starting.").arg(mName));
	}
}

void ProcessWrapper::processError(QProcess::ProcessError error)
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

	messageManager()->sendError(msg);
}

void ProcessWrapper::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	QString msg = QString("%1 %2 exited with exit status %3. (%1 last exit code was %4.)").arg(mName).arg(mLastExecutablePath).arg(exitStatus).arg(exitCode);
	if(exitStatus == 0)
		messageManager()->sendSuccess(msg);
	else
		messageManager()->sendError(msg);
}

QString ProcessWrapper::getExecutableInBundlesAbsolutePath(QString exeInBundle)
{
	QString absolutePathToExe = exeInBundle;

	if (!QFileInfo(absolutePathToExe).isAbsolute())
		absolutePathToExe = DataLocations::getBundlePath() + "/" + absolutePathToExe;

	absolutePathToExe = absolutePathToExe.trimmed();
	absolutePathToExe = QDir::cleanPath(absolutePathToExe);

	if (!QFileInfo(absolutePathToExe).exists())
		messageManager()->sendError(QString("Cannot find %1 [%2]").arg(mName).arg(absolutePathToExe));

	return absolutePathToExe;
}

void ProcessWrapper::internalLaunch(QString executable, QStringList arguments)
{
	if(this->isRunning())
		return;

	messageManager()->sendInfo(QString("Launching %1 %2 with arguments: %3").arg(mName).arg(executable).arg(arguments.join(" ")));

	if(arguments.isEmpty())
		mProcess->start(executable);
	else
		mProcess->start(executable, arguments);

	mLastExecutablePath = executable;
}

}

/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxProcessReporter.h"


#include "cxLogger.h"


namespace cx
{
ProcessReporter::ProcessReporter(QProcess* process, QString name) :
		mName(name)
{
	CX_ASSERT(process);
	mProcess = process;

	connect(mProcess, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(processStateChanged(QProcess::ProcessState)));
	connect(mProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));
	connect(mProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
	connect(mProcess, SIGNAL(readyRead()), this, SLOT(processReadyRead()));
}

ProcessReporter::~ProcessReporter()
{
	disconnect(mProcess, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(processStateChanged(QProcess::ProcessState)));
	disconnect(mProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));
	disconnect(mProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
	disconnect(mProcess, SIGNAL(readyRead()), this, SLOT(processReadyRead()));
}

void ProcessReporter::processReadyRead()
{
	report(QString(mProcess->readAllStandardOutput()));
}

void ProcessReporter::processStateChanged(QProcess::ProcessState newState)
{
	if (newState == QProcess::Running)
	{
		report(QString("%1 started.").arg(mName));
	}
	if (newState == QProcess::NotRunning)
	{
		report(QString("%1 stopped.").arg(mName));
	}
	if (newState == QProcess::Starting)
	{
		report(QString("%1 starting.").arg(mName));
	}
}

void ProcessReporter::processError(QProcess::ProcessError error)
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

	reportError(msg);
}

void ProcessReporter::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	QString msg = QString("%1 exited with exit status %3. (%1 last exit code was %4.)").arg(mName).arg(exitStatus).arg(exitCode);
	if(exitStatus == 0)
		reportSuccess(msg);
	else
		reportError(msg);
}

} /* namespace cx */

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

#include "cxProcessReporter.h"

#include "cxLogger.h"
#include "cxReporter.h"

namespace cx
{
ProcessReporter::ProcessReporter(QProcess* process, QString name) :
		mName(name)
{
	SSC_ASSERT(process);
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
		report(QString("%1 running.").arg(mName));
	}
	if (newState == QProcess::NotRunning)
	{
		report(QString("%1 not running.").arg(mName));
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

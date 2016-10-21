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

#include "cxProcessWrapper.h"
#include <QDir>
#include <QFileInfo>
#include "cxDataLocations.h"
#include "cxLogger.h"
#include "cxTypeConversions.h"
#include <iostream>

namespace cx
{

ProcessWrapper::ProcessWrapper(QString name, QObject* parent) :
		QObject(parent), mName(name), mLastExecutablePath("")
{
	mProcess = new QProcess(this);
	mReporter = ProcessReporterPtr(new ProcessReporter(mProcess, mName));

	mProcess->setProcessChannelMode(QProcess::MergedChannels);
	mProcess->setReadChannel(QProcess::StandardOutput);

	connect(mProcess.data(), &QProcess::stateChanged, this, &ProcessWrapper::stateChanged);
}

ProcessWrapper::~ProcessWrapper()
{
	mProcess->terminate();//terminate gives the process a chance to shutdown
	this->waitForFinished();
}

QProcess* ProcessWrapper::getProcess()
{
	return mProcess.data();
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
	return mProcess->state() == QProcess::Running;
}

qint64 ProcessWrapper::write(const char * data)
{
	return mProcess->write(data);
}

bool ProcessWrapper::waitForStarted(int msecs)
{
	return mProcess->waitForStarted(msecs);
}

bool ProcessWrapper::waitForFinished(int msecs)
{
	return mProcess->waitForFinished(msecs);
}

QString ProcessWrapper::getExecutableInBundlesAbsolutePath(QString exeInBundle)
{
	QString absolutePathToExe = exeInBundle;

	if (!QFileInfo(absolutePathToExe).isAbsolute())
		absolutePathToExe = DataLocations::getBundlePath() + "/" + absolutePathToExe;

	absolutePathToExe = absolutePathToExe.trimmed();
	absolutePathToExe = QDir::cleanPath(absolutePathToExe);

	if (!QFileInfo(absolutePathToExe).exists())
		reportError(QString("Cannot find %1 [%2]").arg(mName).arg(absolutePathToExe));

	return absolutePathToExe;
}

void ProcessWrapper::internalLaunch(QString executable, QStringList arguments)
{
	if(this->isRunning())
		return;

	report(QString("Launching %1: [%2 %3]").arg(mName).arg(executable).arg(arguments.join(" ")));

	if(arguments.isEmpty())
		mProcess->start(executable);
	else
		mProcess->start(executable, arguments);

	mLastExecutablePath = executable;
}
}

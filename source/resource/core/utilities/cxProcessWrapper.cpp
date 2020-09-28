/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

bool ProcessWrapper::launch(QString executable, QStringList arguments)
{
	if (executable.isEmpty() || this->isRunning())
		return false;

	return this->internalLaunch(executable, arguments);
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

void ProcessWrapper::turnOffReporting()
{
	mReporter.reset();
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

bool ProcessWrapper::internalLaunch(QString executable, QStringList arguments)
{
	if(this->isRunning())
		return false;

	report(QString("Launching %1: [%2 %3]").arg(mName).arg(executable).arg(arguments.join(" ")));

	if(arguments.isEmpty())
		mProcess->start(executable);
	else
		mProcess->start(executable, arguments);

	mLastExecutablePath = executable;
	return true;
}
}

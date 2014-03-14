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
#include "cxReporter.h"
#include "cxTypeConversions.h"

namespace cx
{

ProcessWrapper::ProcessWrapper(QString name, QObject* parent) :
		QObject(parent), mName(name), mLastExecutablePath("")
{
	mProcess = new QProcess(this);
	mReporter = ProcessReporterPtr(new ProcessReporter(mProcess, mName));

	mProcess->setProcessChannelMode(QProcess::MergedChannels);
	mProcess->setReadChannel(QProcess::StandardOutput);
}

ProcessWrapper::~ProcessWrapper()
{
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

	report(QString("Launching %1 %2 with arguments: %3").arg(mName).arg(executable).arg(arguments.join(" ")));

	if(arguments.isEmpty())
		mProcess->start(executable);
	else
		mProcess->start(executable, arguments);

	mLastExecutablePath = executable;
}
}

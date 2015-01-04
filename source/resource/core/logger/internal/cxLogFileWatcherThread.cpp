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

#include "cxLogFileWatcherThread.h"
#include "cxLogFileWatcher.h"
#include "cxLogger.h"
#include <QtGlobal>
#include <iostream>
#include "boost/bind.hpp"
#include "boost/shared_ptr.hpp"
#include <QString>
#include <QMutex>
#include <QSound>
#include <QDir>
#include <QTextStream>
#include <QTimer>
#include "cxTypeConversions.h"
#include "cxDefinitionStrings.h"
#include "cxTime.h"
#include "cxDataLocations.h"
#include "cxMessageListener.h"
#include "cxLogFile.h"

#include "cxReporterMessageRepository.h"
#include "cxTime.h"

namespace cx
{

LogFileWatcherThread::LogFileWatcherThread(QObject *parent) :
	LogThread(parent)
{
	qRegisterMetaType<Message>("Message");

	mRepository = MessageRepository::create();

	connect(&mWatcher, &QFileSystemWatcher::directoryChanged, this, &LogFileWatcherThread::onDirectoryChanged);
	connect(&mWatcher, &QFileSystemWatcher::fileChanged, this, &LogFileWatcherThread::onFileChanged);

	QString isoDateFormat("yyyy-MM-dd");
	QString isoDate = QDateTime::currentDateTime().toString(isoDateFormat);
	this->setLoggingFolder(DataLocations::getRootConfigPath()+"/Logs/"+isoDate);
}

LogFileWatcherThread::~LogFileWatcherThread()
{
}

void LogFileWatcherThread::onDirectoryChanged(const QString& path)
{
//	std::cout << "modified DIR: " << path << std::endl;
	QDir info(mLogPath);
	QStringList nameFilters;
	nameFilters << "org.custusx.*";
	QStringList current = info.entryList(nameFilters, QDir::Files);
	current.removeAll("org.custusx.log.all.txt");
	current.sort();

	if (current==mInitializedFiles)
		return;

	if (!mWatcher.files().isEmpty())
		mWatcher.removePaths(mWatcher.files());

	mInitializedFiles = current;
	for (int i=0; i<mInitializedFiles.size(); ++i)
	{
		QString filename = info.absoluteFilePath(mInitializedFiles[i]);
		mWatcher.addPath(filename);
		this->onFileChanged(filename);
	}
}

void LogFileWatcherThread::onFileChanged(const QString& path)
{
	if (!mFiles.count(path))
		mFiles[path] = LogFile::fromFilename(path);

	std::vector<Message> messages = mFiles[path].readMessages();
	for (unsigned i=0; i<messages.size(); ++i)
		this->processMessage(messages[i]);
}

void LogFileWatcherThread::setLoggingFolder(QString absoluteLoggingFolderPath)
{
	QMutexLocker sentry(&mActionsMutex);
	PendingActionType action = boost::bind(&LogFileWatcherThread::executeSetLoggingFolder, this, absoluteLoggingFolderPath);
	mPendingActions.push_back(action);
	sentry.unlock();

	this->invokePendingAction();
}

void LogFileWatcherThread::executeSetLoggingFolder(QString absoluteLoggingFolderPath)
{
	mLogPath = absoluteLoggingFolderPath;
	mWatcher.addPath(mLogPath);
}

int LogFileWatcherThread::getDefaultTimeout(MESSAGE_LEVEL messageLevel) const
{
	switch(messageLevel)
	{
	case mlDEBUG: return 0;
	case mlINFO: return 1500;
	case mlSUCCESS: return 1500;
	case mlWARNING: return 3000;
	case mlERROR: return 0;
	case mlVOLATILE: return 5000;
	default: return 0;
	}
}

void LogFileWatcherThread::processMessage(Message message)
{
	message = this->cleanupMessage(message);

	emit emittedMessage(message);

	mRepository->setMessage(message);
}

Message LogFileWatcherThread::cleanupMessage(Message message)
{
	if (message.mTimeoutTime<0)
		message.mTimeoutTime = this->getDefaultTimeout(message.mMessageLevel);

	if (message.mChannel.isEmpty())
		message.mChannel = "console";

	if (!message.mSourceFile.isEmpty())
	{
		message.mSourceFile = message.mSourceFile.split("CustusX/").back();
	}

	return message;
}

void LogFileWatcherThread::installObserver(MessageObserverPtr observer, bool resend)
{
	QMutexLocker sentry(&mActionsMutex);
	PendingActionType action = boost::bind(&MessageRepository::install, mRepository.get(), observer, resend);
	mPendingActions.push_back(action);
	sentry.unlock();

	this->invokePendingAction();
}

void LogFileWatcherThread::uninstallObserver(MessageObserverPtr observer)
{
	QMutexLocker sentry(&mActionsMutex);
	PendingActionType action = boost::bind(&MessageRepository::uninstall, mRepository.get(), observer);
	mPendingActions.push_back(action);
	sentry.unlock();

	this->invokePendingAction();
}

void LogFileWatcherThread::invokePendingAction()
{
	QMetaObject::invokeMethod(this, "pendingAction", Qt::QueuedConnection);
}

void LogFileWatcherThread::pendingAction()
{
	while (this->executeAction());
}

bool LogFileWatcherThread::executeAction()
{
	PendingActionType action = this->popAction();
	if (!action)
		return false;

	action();
	return true;
}

LogFileWatcherThread::PendingActionType LogFileWatcherThread::popAction()
{
	QMutexLocker sentry(&mActionsMutex);
	PendingActionType action;

	if (mPendingActions.isEmpty())
		return action;

	action = mPendingActions.front();
	mPendingActions.pop_front();
	return action;
}


} //End namespace cx

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

	connect(&mWatcher, &QFileSystemWatcher::directoryChanged, this, &LogFileWatcherThread::onDirectoryChanged);
	connect(&mWatcher, &QFileSystemWatcher::fileChanged, this, &LogFileWatcherThread::onFileChanged);
}

LogFileWatcherThread::~LogFileWatcherThread()
{
}

void LogFileWatcherThread::onDirectoryChanged(const QString& path)
{
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
	std::multimap<QDateTime, Message> messages;
	for (int i=0; i<mInitializedFiles.size(); ++i)
	{
		QString filename = info.absoluteFilePath(mInitializedFiles[i]);
		mWatcher.addPath(filename);
		std::vector<Message> msg = this->readMessages(filename);
		for (unsigned i=0; i<msg.size(); ++i)
			messages.insert(std::make_pair(msg[i].mTimeStamp, msg[i]));
		this->onFileChanged(filename);
	}

	for (std::multimap<QDateTime, Message>::iterator i=messages.begin(); i!=messages.end(); ++i)
		this->processMessage(i->second);
}

void LogFileWatcherThread::onFileChanged(const QString& path)
{
//	if (!mFiles.count(path))
//		mFiles[path] = LogFile::fromFilename(path);

//	std::vector<Message> messages = mFiles[path].readMessages();
	std::vector<Message> messages = this->readMessages(path);
	for (unsigned i=0; i<messages.size(); ++i)
		this->processMessage(messages[i]);
}

std::vector<Message> LogFileWatcherThread::readMessages(const QString& path)
{
	if (!mFiles.count(path))
		mFiles[path] = LogFile::fromFilename(path);

	std::vector<Message> messages = mFiles[path].readMessages();
	return messages;
}

void LogFileWatcherThread::executeSetLoggingFolder(QString absoluteLoggingFolderPath)
{
	if (mLogPath == absoluteLoggingFolderPath)
		return;

	mLogPath = absoluteLoggingFolderPath;

	mInitializedFiles.clear();
	mRepository->clearQueue();

	mWatcher.addPath(mLogPath);

	if (!mWatcher.directories().isEmpty())
		mWatcher.removePaths(mWatcher.directories());
	if (!mWatcher.files().isEmpty())
		mWatcher.removePaths(mWatcher.files());
	this->onDirectoryChanged(mLogPath);
}


} //End namespace cx

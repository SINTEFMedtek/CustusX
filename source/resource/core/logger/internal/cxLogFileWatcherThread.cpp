/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

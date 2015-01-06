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

#include "cxReporterThread.h"
#include "cxReporter.h"
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

#include "cxLogQDebugRedirecter.h"
#include "cxLogIOStreamRedirecter.h"
#include "cxReporterMessageRepository.h"
#include "cxTime.h"
#include "cxLogFile.h"

namespace cx
{

ReporterThread::ReporterThread(QObject *parent) :
	LogThread(parent)
{
	qInstallMessageHandler(convertQtMessagesToCxMessages);
	qRegisterMetaType<Message>("Message");

	mRepository = MessageRepository::create();

	// make sure streams are closed properly before reconnecting.
	mCout.reset();
	mCerr.reset();

	mCout.reset(new SingleStreamerImpl(std::cout, mlCOUT));
	mCerr.reset(new SingleStreamerImpl(std::cerr, mlCERR));

//	QString isoDateFormat("yyyy-MM-dd");
//	QString isoDate = QDateTime::currentDateTime().toString(isoDateFormat);
//	this->setLoggingFolder(DataLocations::getRootConfigPath()+"/Logs/"+isoDate);
}

ReporterThread::~ReporterThread()
{
	qInstallMessageHandler(0);
	mCout.reset();
	mCerr.reset();
}

bool ReporterThread::initializeLogFile(LogFile file)
{
	QString filename = file.getFilename();
	if (mInitializedFiles.contains(filename))
		return true;

	mInitializedFiles << filename;

	file.writeHeader();

	if (!file.isWritable())
	{
		this->processMessage(Message("Failed to open log file " + filename, mlERROR));
		return false;
	}

	return true;
}

void ReporterThread::setLoggingFolder(QString absoluteLoggingFolderPath)
{
	QMutexLocker sentry(&mActionsMutex);
	PendingActionType action = boost::bind(&ReporterThread::executeSetLoggingFolder, this, absoluteLoggingFolderPath);
	mPendingActions.push_back(action);
	sentry.unlock();

	this->invokePendingAction();
}


void ReporterThread::executeSetLoggingFolder(QString absoluteLoggingFolderPath)
{
	mLogPath = absoluteLoggingFolderPath;

	QFileInfo(mLogPath+"/").absoluteDir().mkpath(".");

//	this->initializeLogFile(this->getFilenameForChannel("console"));
//	this->initializeLogFile(this->getFilenameForChannel("all"));

	this->initializeLogFile(LogFile::fromChannel(mLogPath, "console"));
	this->initializeLogFile(LogFile::fromChannel(mLogPath, "all"));
}

int ReporterThread::getDefaultTimeout(MESSAGE_LEVEL messageLevel) const
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

void ReporterThread::logMessage(Message msg)
{
	QMetaObject::invokeMethod(this, "processMessage",
							  Qt::QueuedConnection,
							  Q_ARG(Message, msg));
}

void ReporterThread::processMessage(Message message)
{
	message = this->cleanupMessage(message);

	this->sendToCout(message);
	this->sendToFile(message);

	emit emittedMessage(message);

	mRepository->setMessage(message);
}

void ReporterThread::sendToFile(Message message)
{
	if (message.getMessageLevel()==mlVOLATILE)
		return;

//	QString channelFile = this->getFilenameForChannel(message.mChannel);
	LogFile channelLog = LogFile::fromChannel(mLogPath, message.mChannel);
	LogFile allLog = LogFile::fromChannel(mLogPath, "all");

	this->initializeLogFile(channelLog);

	channelLog.write(message);
	allLog.write(message);
}

void ReporterThread::sendToCout(Message message)
{
//	if (!mCout)
//		return;
//	if (message.getMessageLevel()==mlVOLATILE)
//		return;
//	if (( message.getMessageLevel() == mlCOUT )||( message.getMessageLevel() == mlCERR ))
//		return;

//	mCout->sendUnredirected(message.getPrintableMessage()+"\n");
}

Message ReporterThread::cleanupMessage(Message message)
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

void ReporterThread::installObserver(MessageObserverPtr observer, bool resend)
{
	QMutexLocker sentry(&mActionsMutex);
	PendingActionType action = boost::bind(&MessageRepository::install, mRepository.get(), observer, resend);
	mPendingActions.push_back(action);
	sentry.unlock();

	this->invokePendingAction();
}

void ReporterThread::uninstallObserver(MessageObserverPtr observer)
{
	QMutexLocker sentry(&mActionsMutex);
	PendingActionType action = boost::bind(&MessageRepository::uninstall, mRepository.get(), observer);
	mPendingActions.push_back(action);
	sentry.unlock();

	this->invokePendingAction();
}

void ReporterThread::invokePendingAction()
{
	QMetaObject::invokeMethod(this, "pendingAction", Qt::QueuedConnection);
}

void ReporterThread::pendingAction()
{
	while (this->executeAction());
}

bool ReporterThread::executeAction()
{
	PendingActionType action = this->popAction();
	if (!action)
		return false;

	action();
	return true;
}

ReporterThread::PendingActionType ReporterThread::popAction()
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

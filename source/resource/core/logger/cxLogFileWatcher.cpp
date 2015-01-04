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

#include "cxLogFileWatcher.h"
#include "cxLogger.h"
#include <QtGlobal>
#include <iostream>
#include "boost/shared_ptr.hpp"
#include <QString>
#include <QMutex>
#include <QSound>
#include <QDir>
#include <QTextStream>
#include "cxTypeConversions.h"
#include "cxDefinitionStrings.h"
#include "cxTime.h"
#include "cxDataLocations.h"
#include "cxMessageListener.h"

#include "internal/cxLogFileWatcherThread.h"

namespace cx
{


Log::Log()
{
}

Log::~Log()
{
	this->stopThread();
}

void Log::initializeObject()
{
	this->stopThread();
	this->startThread();
}

void Log::startThread()
{
	if (mThread)
		return;

	mThread.reset(new QThread());
	mThread->setObjectName("org.custusx.resource.core.logger");

	mWorker = this->createWorker();
	mWorker->moveToThread(mThread.get());
	if (!mLogPath.isEmpty())
		mWorker->setLoggingFolder(mLogPath);
	connect(mWorker.get(), &LogThread::emittedMessage, this, &Log::onEmittedMessage);

	mThread->start();
}

void Log::stopThread()
{
	if (!mThread)
		return;

	disconnect(mWorker.get(), &LogThread::emittedMessage, this, &Log::onEmittedMessage);
	LogThreadPtr tempWorker = mWorker;
	mWorker.reset();

	mThread->quit();
	mThread->wait(); // forever or until dead thread

	mThread.reset();
	tempWorker.reset();
}

void Log::setLoggingFolder(QString absoluteLoggingFolderPath)
{
	mLogPath = absoluteLoggingFolderPath;
	if (mWorker)
		mWorker->setLoggingFolder(mLogPath);
}

void Log::installObserver(MessageObserverPtr observer, bool resend)
{
	if (mWorker)
		mWorker->installObserver(observer, resend);
}

void Log::uninstallObserver(MessageObserverPtr observer)
{
	if (mWorker)
		mWorker->uninstallObserver(observer);
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

LogFileWatcherPtr LogFileWatcher::create()
{
	LogFileWatcherPtr retval(new LogFileWatcher());
	retval->initializeObject();
	return retval;
}

LogThreadPtr LogFileWatcher::createWorker()
{
	return LogThreadPtr(new LogFileWatcherThread());
}

} //End namespace cx

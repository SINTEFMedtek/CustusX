/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxLog.h"

#include "cxLogger.h"
#include <QtGlobal>
#include <QThread>
#include <iostream>
#include "boost/shared_ptr.hpp"
#include <QString>
#include <QMutex>
#include <QSound>
#include <QDir>
#include <QTextStream>
#include "cxTypeConversions.h"
#include "cxTime.h"
#include "cxProfile.h"
#include "cxMessageListener.h"
#include "internal/cxLogThread.h"
#include "QApplication"

namespace cx
{

class EventProcessingThread : public QThread
{
public:
	EventProcessingThread()
	{
	}
	virtual ~EventProcessingThread()
	{

	}

	virtual void run()
	{
		this->exec();
		qApp->processEvents(); // exec() docs doesn't guarantee that the posted events are processed. - do that here.
	}
};


Log::Log()
{
	mLogPath = this->getDefaultLogPath();
}

Log::~Log()
{
	this->stopThread();
}

QString Log::getDefaultLogPath() const
{
	QString isoDateFormat("yyyy-MM-dd");
	QString isoDate = QDateTime::currentDateTime().toString(isoDateFormat);
	QString retval = ProfileManager::getInstance()->getSettingsPath()+"/Logs/"+isoDate;
	return retval;
}

void Log::initializeObject()
{
	if (mThread)
		return;

	this->stopThread();
	this->startThread();
}

void Log::startThread()
{
	if (mThread)
		return;

	mThread.reset(new EventProcessingThread());
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
	emit loggingFolderChanged();
}

QString Log::getLoggingFolder() const
{
	return mLogPath;
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


} //End namespace cx

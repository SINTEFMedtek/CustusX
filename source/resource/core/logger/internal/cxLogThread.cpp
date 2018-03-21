/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxLogThread.h"
#include "cxReporterMessageRepository.h"
#include "boost/bind.hpp"
#include <iostream>

namespace cx
{

ThreadMethodInvoker::ThreadMethodInvoker(QObject* parent) :
    QObject(parent)
{
}

void ThreadMethodInvoker::callInLogThread(ActionType action)
{
    QMutexLocker sentry(&mActionsMutex);
    mPendingActions.push_back(action);
    sentry.unlock();

    this->invokePendingAction();
}

void ThreadMethodInvoker::invokePendingAction()
{
    QMetaObject::invokeMethod(this, "pendingAction", Qt::QueuedConnection);
}

void ThreadMethodInvoker::pendingAction()
{
    while (this->executeAction());
}

bool ThreadMethodInvoker::executeAction()
{
    ActionType action = this->popAction();
    if (!action)
        return false;

    action();
    return true;
}

ThreadMethodInvoker::ActionType ThreadMethodInvoker::popAction()
{
    QMutexLocker sentry(&mActionsMutex);
    ActionType action;

    if (mPendingActions.isEmpty())
        return action;

    action = mPendingActions.front();
    mPendingActions.pop_front();
    return action;
}

} // namespace cx




namespace cx
{

LogThread::LogThread(QObject* parent) :
	QObject(parent)
{
    mQueue = new ThreadMethodInvoker(this);
	mRepository = MessageRepository::create();
}

void LogThread::installObserver(MessageObserverPtr observer, bool resend)
{
	ActionType action = boost::bind(&MessageRepository::install, mRepository, observer, resend);
	this->callInLogThread(action);
}

void LogThread::uninstallObserver(MessageObserverPtr observer)
{
	ActionType action = boost::bind(&MessageRepository::uninstall, mRepository, observer);
	this->callInLogThread(action);
}

void LogThread::setLoggingFolder(QString absoluteLoggingFolderPath)
{
	ActionType action = boost::bind(&LogThread::executeSetLoggingFolder, this, absoluteLoggingFolderPath);
	this->callInLogThread(action);
}

void LogThread::callInLogThread(ThreadMethodInvoker::ActionType action)
{
    mQueue->callInLogThread(action);
}

void LogThread::processMessage(Message message)
{
	message = this->cleanupMessage(message);

	emit emittedMessage(message);

	mRepository->setMessage(message);
}


Message LogThread::cleanupMessage(Message message)
{
	if (message.mTimeoutTime<0)
		message.mTimeoutTime = this->getDefaultTimeout(message.mMessageLevel);

	if (message.mChannel.isEmpty())
		message.mChannel = "console";

	if (!message.mSourceFile.isEmpty())
	{
		message.mSourceFile = message.mSourceFile.split("CustusX/").back();
		message.mSourceFile = message.mSourceFile.split("CX/").back();
	}

	return message;
}

int LogThread::getDefaultTimeout(MESSAGE_LEVEL messageLevel) const
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



} //End namespace cx

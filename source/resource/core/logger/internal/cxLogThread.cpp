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

#include "cxLogThread.h"
#include "cxReporterMessageRepository.h"
#include "boost/bind.hpp"
#include <iostream>

namespace cx
{

LogThread::LogThread(QObject* parent) :
	QObject(parent)
{
	mRepository = MessageRepository::create();
}

void LogThread::installObserver(MessageObserverPtr observer, bool resend)
{
	PendingActionType action = boost::bind(&MessageRepository::install, mRepository, observer, resend);
	this->callInLogThread(action);
}

void LogThread::uninstallObserver(MessageObserverPtr observer)
{
	PendingActionType action = boost::bind(&MessageRepository::uninstall, mRepository, observer);
	this->callInLogThread(action);
}

void LogThread::setLoggingFolder(QString absoluteLoggingFolderPath)
{
	PendingActionType action = boost::bind(&LogThread::executeSetLoggingFolder, this, absoluteLoggingFolderPath);
	this->callInLogThread(action);
}

void LogThread::callInLogThread(PendingActionType& action)
{
	QMutexLocker sentry(&mActionsMutex);
	mPendingActions.push_back(action);
	sentry.unlock();

	this->invokePendingAction();
}

void LogThread::invokePendingAction()
{
	QMetaObject::invokeMethod(this, "pendingAction", Qt::QueuedConnection);
}

void LogThread::pendingAction()
{
	while (this->executeAction());
}

bool LogThread::executeAction()
{
	PendingActionType action = this->popAction();
	if (!action)
		return false;

	action();
	return true;
}

LogThread::PendingActionType LogThread::popAction()
{
	QMutexLocker sentry(&mActionsMutex);
	PendingActionType action;

	if (mPendingActions.isEmpty())
		return action;

	action = mPendingActions.front();
	mPendingActions.pop_front();
	return action;
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

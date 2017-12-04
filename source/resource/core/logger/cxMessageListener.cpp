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
#include "cxMessageListener.h"
#include <iostream>
#include <QThread>
#include "internal/cxReporterMessageRepository.h"
#include "cxLogFileWatcher.h"
#include "cxNullDeleter.h"
#include "cxReporter.h"
#include <QApplication>

namespace cx
{

MessageListenerPtr MessageListener::create(LogPtr log)
{
	return MessageListenerPtr(new MessageListener(log));
}

MessageListenerPtr MessageListener::createWithQueue(LogPtr log, int size)
{
	MessageListenerPtr retval(new MessageListener(log));
	retval->setMessageQueueMaxSize(size);
	return retval;
}

MessageListenerPtr MessageListener::clone()
{
	MessageListenerPtr retval(new MessageListener(this->mManager));
	retval->mMessages = this->mMessages;
	retval->mManager = this->mManager;
	retval->mMessageHistoryMaxSize = this->mMessageHistoryMaxSize;
	return retval;
}


MessageListener::MessageListener(LogPtr log) :
	mManager(log),
	mMessageHistoryMaxSize(0)
{
	mManager = log;
	if (!mManager)
		mManager = reporter();

	mObserver.reset(new MessageObserver());
	connect(mObserver.get(), &MessageObserver::newMessage, this, &MessageListener::messageReceived);
	connect(mObserver.get(), &MessageObserver::newChannel, this, &MessageListener::newChannel);

	mManager->installObserver(mObserver, false);
}

MessageListener::~MessageListener()
{
	if (mManager)
	{
		mManager->uninstallObserver(mObserver);
	}
}

void MessageListener::messageReceived(Message message)
{
	mMessages.push_back(message);
	this->limitQueueSize();
	emit newMessage(message);
}

void MessageListener::limitQueueSize()
{
	if (mMessageHistoryMaxSize<0)
		return;

	while (mMessages.size() > mMessageHistoryMaxSize)
	{
		mMessages.pop_front();
	}
}

bool MessageListener::isError(MESSAGE_LEVEL level) const
{
	return ((level==mlERROR )||( level==mlCERR ));
}

bool MessageListener::containsErrors() const
{
	QApplication::processEvents();

	for (QList<Message>::const_iterator i=mMessages.begin(); i!=mMessages.end(); ++i)
	{
		if (this->isError(i->getMessageLevel()))
		{
			std::cout << "******* MessageListener::containsErrors() Found Error: " << i->getText() <<"\n";
			return true;
		}
	}
	return false;
}

bool MessageListener::containsText(const QString text) const
{
    QApplication::processEvents();
    for (QList<Message>::const_iterator i=mMessages.begin(); i!=mMessages.end(); ++i)
    {
        QString message = i->getText();
        if(i->getText().contains(text, Qt::CaseInsensitive))
            return true;
    }
    return false;
}

void MessageListener::restart()
{
	mManager->installObserver(mObserver, true);
}

void MessageListener::installFilter(MessageFilterPtr filter)
{
	mObserver->installFilter(filter);
	mManager->installObserver(mObserver, false);
}

void MessageListener::setMessageQueueMaxSize(int count)
{
	mMessageHistoryMaxSize = count;
}

int MessageListener::getMessageQueueMaxSize() const
{
	return mMessageHistoryMaxSize;
}

QList<Message> MessageListener::getMessages() const
{
	return mMessages;
}


} // namespace cx


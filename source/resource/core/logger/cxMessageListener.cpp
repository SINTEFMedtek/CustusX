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

namespace cx
{


MessageListenerPtr MessageListener::create()
{
	return MessageListenerPtr(new MessageListener);
}

MessageListenerPtr MessageListener::clone()
{
	MessageListenerPtr retval(new MessageListener());
	retval->mMessages = this->mMessages;
	retval->mManager = this->mManager;
	retval->mMessageHistoryMaxSize = this->mMessageHistoryMaxSize;
//	retval->mFilter = this->mFilter;
	return retval;
}


MessageListener::MessageListener() :
	mManager(NULL),
	mMessageHistoryMaxSize(1000)
{
	mManager = reporter();

	mObserver.reset(new MessageObserver());
	connect(mObserver.get(), &MessageObserver::newMessage, this, &MessageListener::messageReceived);

	mManager->installObserver(mObserver, true);
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
		mMessages.pop_front();
}

bool MessageListener::isError(MESSAGE_LEVEL level) const
{
	return ((level==mlERROR )||( level==mlCERR ));
}

bool MessageListener::containsErrors() const
{
	for (QList<Message>::const_iterator i=mMessages.begin(); i!=mMessages.end(); ++i)
		if (this->isError(i->getMessageLevel()))
			return true;
	return false;
}

void MessageListener::restart()
{
	mManager->installObserver(mObserver, true);

//	for (QList<Message>::iterator i=mMessages.begin(); i!=mMessages.end(); ++i)
//	{
//		this->emitThroughFilter(*i);
//	}
}

//bool MessageListener::testFilter(const Message& msg) const
//{
//	if (!mFilter)
//		return true; // always succeed with no filter.
//	return (*mFilter)(msg);
//}

void MessageListener::installFilter(MessageFilterPtr filter)
{
//	mFilter = filter;
	mObserver->installFilter(filter);
	mManager->installObserver(mObserver, true);
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

//---------------------------------------------------------



















MessageRepositoryPtr MessageRepository::create()
{
	return MessageRepositoryPtr(new MessageRepository);
}

MessageRepository::MessageRepository() :
	mMessageHistoryMaxSize(3000)
{
}

MessageRepository::~MessageRepository()
{
}

void MessageRepository::setMessage(Message message)
{
	mMessages.push_back(message);
	this->limitQueueSize();
	this->emitThroughFilter(message);
}

void MessageRepository::limitQueueSize()
{
	if (mMessageHistoryMaxSize<0)
		return;

	while (mMessages.size() > mMessageHistoryMaxSize)
		mMessages.pop_front();
}

void MessageRepository::emitThroughFilter(const Message& message)
{
	for (unsigned i=0; i<mObservers.size(); ++i)
	{
		this->emitThroughFilter(mObservers[i], message);
	}
}

void MessageRepository::emitThroughFilter(MessageObserverPtr observer, const Message& message)
{
	if (observer->testFilter(message))
		observer->sendMessage(message);
}

void MessageRepository::install(MessageObserverPtr observer, bool resend)
{
	this->uninstall(observer);

	mObservers.push_back(observer);

	if (resend)
	{
		for (QList<Message>::iterator i=mMessages.begin(); i!=mMessages.end(); ++i)
			this->emitThroughFilter(observer, *i);
	}
}

void MessageRepository::uninstall(MessageObserverPtr observer)
{
	if (!std::count(mObservers.begin(), mObservers.end(), observer))
			return;
	mObservers.erase(std::find(mObservers.begin(), mObservers.end(), observer));
}

void MessageRepository::setMessageQueueMaxSize(int count)
{
	mMessageHistoryMaxSize = count;
}

int MessageRepository::getMessageQueueMaxSize() const
{
	return mMessageHistoryMaxSize;
}


} // namespace cx


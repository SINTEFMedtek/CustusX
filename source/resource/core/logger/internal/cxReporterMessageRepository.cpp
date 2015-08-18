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

#include "cxReporterMessageRepository.h"
#include "cxMessageListener.h"
#include <iostream>
#include <QThread>
#include "cxLogMessageFilter.h"

namespace cx
{

void MessageObserver::sendMessage(const Message& message)
{
	QMutexLocker locker(&mMutex);
	if (!mChannels.contains(message.mChannel))
	{
		mChannels.append(message.mChannel);
		locker.unlock();
		emit newChannel(message.mChannel);
		locker.relock();
	}

	if (this->testFilter(message))
	{
		locker.unlock();
		emit newMessage(message);
		locker.relock();
	}
}

bool MessageObserver::testFilter(const Message &msg) const
{
	if (!mFilter)
		return true; // always succeed with no filter.
	return (*mFilter)(msg);
}

void MessageObserver::installFilter(MessageFilterPtr filter)
{
	QMutexLocker locker(&mMutex);
	// Clone to ensure filter is standalone
	// and safely can be passed to the reporter thread.
	mFilter = filter->clone();
}


///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------


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
		mObservers[i]->sendMessage(message);
	}
}

void MessageRepository::install(MessageObserverPtr observer, bool resend)
{
	if(!this->exists(observer))
		mObservers.push_back(observer);

	if (resend)
	{
		for (QList<Message>::iterator i=mMessages.begin(); i!=mMessages.end(); ++i)
			observer->sendMessage(*i);
	}
}

bool MessageRepository::exists(MessageObserverPtr observer)
{
	if (std::count(mObservers.begin(), mObservers.end(), observer))
		return true;
	return false;
}

void MessageRepository::uninstall(MessageObserverPtr observer)
{
	if (!this->exists(observer))
			return;
	mObservers.erase(std::find(mObservers.begin(), mObservers.end(), observer));
}

void MessageRepository::setMessageQueueMaxSize(int count)
{
	mMessageHistoryMaxSize = count;
	this->limitQueueSize();
}

int MessageRepository::getMessageQueueMaxSize() const
{
	return mMessageHistoryMaxSize;
}

void MessageRepository::clearQueue()
{
	mMessages.clear();
}



} // namespace cx


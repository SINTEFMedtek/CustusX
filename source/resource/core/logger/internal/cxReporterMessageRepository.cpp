/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
		for (unsigned i = 0; i < mMessages.size(); ++i)
			observer->sendMessage(mMessages[i]);

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


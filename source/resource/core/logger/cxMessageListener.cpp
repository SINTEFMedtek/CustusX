/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
			std::cout << "MessageListener::containsErrors() Found Error: " << i->getText() <<"\n";
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


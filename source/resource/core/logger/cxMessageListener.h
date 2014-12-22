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
#ifndef CXMESSAGELISTENER_H
#define CXMESSAGELISTENER_H

#include "cxResourceExport.h"
#include "cxReporter.h"


#include <vector>
#include <QPointer>

#include "cxEnumConverter.h"

namespace cx
{

static LOG_SEVERITY level2severity(MESSAGE_LEVEL level)
{
	switch (level)
	{
	// level 1
	case mlCERR :
	case mlERROR : return msERROR;

	// level 2
	case mlSUCCESS :
	case mlWARNING : return msWARNING;

	// level 3
	case mlINFO : return msINFO;

	// level 4
	case mlCOUT :
	case mlDEBUG : return msDEBUG;

	default: return msCOUNT;
	}
}

typedef boost::shared_ptr<class MessageFilter> MessageFilterPtr;

class MessageFilter
{
public:
	~MessageFilter() {}
	virtual bool operator()(const Message& msg) const = 0;
	virtual MessageFilterPtr clone() = 0;

};

class MessageFilterConsole : public MessageFilter
{
public:
	virtual bool operator()(const Message& msg) const
	{
		if (!isActiveChannel(msg))
			return false;
		if (!isActiveSeverity(msg))
			return false;
		return true;
	}

	virtual MessageFilterPtr clone()
	{
		return MessageFilterPtr(new MessageFilterConsole(*this));
	}


	bool isActiveSeverity(const Message& msg) const
	{
		LOG_SEVERITY severity = level2severity(msg.getMessageLevel());
		return severity <= mLowestSeverity;
	}

	bool isActiveChannel(const Message& msg) const
	{
		if (mChannel == "all")
			return true;
		if (msg.mChannel == mChannel)
			return true;
		return false;
	}

	void setActiveChannel(QString uid)
	{
		mChannel = uid;
	}

	void clearSeverity()
	{
		mLowestSeverity = msERROR;
	}
	void activateSeverity(LOG_SEVERITY severity)
	{
		mLowestSeverity = std::max(mLowestSeverity, severity);
	}
	void setLowestSeverity(LOG_SEVERITY severity)
	{
		mLowestSeverity = severity;
	}
	LOG_SEVERITY getLowestSeverity() const
	{
		return mLowestSeverity;
	}

private:
	QString mChannel;
	LOG_SEVERITY mLowestSeverity;
};

/**
 * Mediator between MessageRepository and MessageListener.
 *
 * MessageListener install the MessageObserver into the
 * MessageRepository, which in turn sends messages that
 * passes the filter.
 */
class MessageObserver : public QObject
{
	Q_OBJECT
public:
	/** Required by MessageRepository
	 *  return true if msg is accepted by filter.
	 */
	bool testFilter(const Message &msg) const
	{
		if (!mFilter)
			return true; // always succeed with no filter.
		return (*mFilter)(msg);
	}
	void sendMessage(Message message)
	{
		emit newMessage(message);
	}

	/** Install a filter for use in the reporter.
	 *  The filter will be cloned, i.e. call after every modification of filter.
	 *
	 *  Required by MessageListener
	 */
	void installFilter(MessageFilterPtr filter)
	{
		// Clone to ensure filter is standalone
		// and safely can be passed to the reporter thread.
		mFilter = filter->clone();
	}

signals:
	void newMessage(Message message);
private:
	MessageFilterPtr mFilter;
};

typedef boost::shared_ptr<class MessageObserver> MessageObserverPtr;

typedef boost::shared_ptr<class MessageListener> MessageListenerPtr;

/** Utility for listening to the Reporter
  * and storing messages from it.
  *
  * Messages are passed throught MessageFilter before being
  * emitted from this class.
  *
 * \ingroup cx_resource_core_logger
  * \date 2014-03-09
  * \author christiana
  */
class cxResource_EXPORT MessageListener : public QObject
{
	Q_OBJECT
public:
	static MessageListenerPtr create();
	MessageListenerPtr clone();
	MessageListener();
	~MessageListener();
	bool containsErrors() const;
	QList<Message> getMessages() const;

	void restart(); // emit all messages in queue, then continue emitting new incoming messages

	void installFilter(MessageFilterPtr);
	void setMessageQueueMaxSize(int count);
	int getMessageQueueMaxSize() const; // <0 means infinite

signals:
	void newMessage(Message message);

private slots:
	void messageReceived(Message message);
private:
	void emitThroughFilter(const Message& message);
	bool isError(MESSAGE_LEVEL level) const;
	void limitQueueSize();
	QList<Message> mMessages;
	QPointer<Reporter> mManager;
	int mMessageHistoryMaxSize;

	MessageObserverPtr mObserver;
};

typedef boost::shared_ptr<class MessageRepository> MessageRepositoryPtr;

/** Utility for listening to the Reporter
  * and storing messages from it.
  *
  * Messages are passed throught MessageFilter before being
  * emitted from this class.
  *
 * \ingroup cx_resource_core_logger
  * \date 2014-03-09
  * \author christiana
  */
class cxResource_EXPORT MessageRepository : public QObject
{
public:
	static MessageRepositoryPtr create();
	MessageRepository();
	~MessageRepository();
	void setMessage(Message message);

	void install(MessageObserverPtr observer, bool resend);
	void uninstall(MessageObserverPtr observer);

	void setMessageQueueMaxSize(int count);
	int getMessageQueueMaxSize() const; // <0 means infinite
private:
	void limitQueueSize();
	void emitThroughFilter(const Message& message);
	void emitThroughFilter(MessageObserverPtr observer, const Message& message);
	QList<Message> mMessages;
	std::vector<MessageObserverPtr> mObservers;
	int mMessageHistoryMaxSize;
};


} // namespace cx


#endif // CXMESSAGELISTENER_H

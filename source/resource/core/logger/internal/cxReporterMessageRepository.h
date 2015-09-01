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
#ifndef CXREPORTERMESSAGEREPOSITORY_H
#define CXREPORTERMESSAGEREPOSITORY_H

#include "cxResourceExport.h"
#include "cxReporter.h"
#include "cxMessageListener.h"


#include <vector>
#include <QPointer>

#include "cxEnumConverter.h"

namespace cx
{

/**
 * Mediator between MessageRepository and MessageListener.
 *
 * MessageListener install the MessageObserver into the
 * MessageRepository, which in turn sends messages that
 * passes the filter.
 *
 * This class is threadsafe
 */
class MessageObserver : public QObject
{
	Q_OBJECT
public:
	/** Required by MessageRepository
	 *  Send message/channel changes to listeners.
	 */
	void sendMessage(const Message& message);
	/** Install a filter for use in the reporter.
	 *  The filter will be cloned, i.e. call after every modification of filter.
	 *
	 *  Required by MessageListener
	 */
	void installFilter(MessageFilterPtr filter);

signals:
	void newMessage(Message message);
	void newChannel(QString channel);
private:
	bool testFilter(const Message &msg) const;

	MessageFilterPtr mFilter;
	QStringList mChannels;
	QMutex mMutex;
};

typedef boost::shared_ptr<class MessageObserver> MessageObserverPtr;
typedef boost::shared_ptr<class MessageListener> MessageListenerPtr;
typedef boost::shared_ptr<class MessageRepository> MessageRepositoryPtr;

/** Utility for listening to the Reporter
  * and storing messages from it.
  *
  * Messages are passed through MessageFilter before being
  * emitted from this class.
  *
 * \ingroup cx_resource_core_logger
  * \date 2014-03-09
  * \author christiana
  */
class MessageRepository : public QObject
{
public:
	static MessageRepositoryPtr create();
	~MessageRepository();
	void setMessage(Message message);

	void install(MessageObserverPtr observer, bool resend);
	void uninstall(MessageObserverPtr observer);

	void setMessageQueueMaxSize(int count);
	int getMessageQueueMaxSize() const; // <0 means infinite
	void clearQueue();

private:
	MessageRepository();
	void limitQueueSize();
	void emitThroughFilter(const Message& message);
	QList<Message> mMessages;
	std::vector<MessageObserverPtr> mObservers;
	int mMessageHistoryMaxSize;
	bool exists(MessageObserverPtr observer);
};


} // namespace cx

#endif // CXREPORTERMESSAGEREPOSITORY_H

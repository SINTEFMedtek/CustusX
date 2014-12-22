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

typedef boost::shared_ptr<class MessageFilter> MessageFilterPtr;
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
	static MessageListenerPtr createWithQueue(int size=1000);
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


} // namespace cx


#endif // CXMESSAGELISTENER_H

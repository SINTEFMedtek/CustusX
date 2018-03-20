/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
typedef boost::shared_ptr<class Log> LogPtr;

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
	static MessageListenerPtr create(LogPtr log=LogPtr());
	static MessageListenerPtr createWithQueue(LogPtr log=LogPtr(), int size=1000);
	MessageListenerPtr clone();
	~MessageListener();
	bool containsErrors() const;
    bool containsText(const QString text) const;
	QList<Message> getMessages() const;

	void restart(); // emit all messages in queue, then continue emitting new incoming messages

	void installFilter(MessageFilterPtr);
	void setMessageQueueMaxSize(int count);
	int getMessageQueueMaxSize() const; // <0 means infinite

signals:
	void newMessage(Message message);
	void newChannel(QString channel);

private slots:
	void messageReceived(Message message);
private:
	MessageListener(LogPtr log);
	bool isError(MESSAGE_LEVEL level) const;
	void limitQueueSize();
	QList<Message> mMessages;
	LogPtr mManager;
	int mMessageHistoryMaxSize;

	MessageObserverPtr mObserver;
};


} // namespace cx


#endif // CXMESSAGELISTENER_H

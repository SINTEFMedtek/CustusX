/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXREPORTERMESSAGEREPOSITORY_H
#define CXREPORTERMESSAGEREPOSITORY_H

#include "cxResourceExport.h"
#include "cxReporter.h"
#include "cxMessageListener.h"


#include <vector>
#include <QPointer>

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

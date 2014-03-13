// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.
#ifndef CXMESSAGELISTENER_H
#define CXMESSAGELISTENER_H

#include "cxMessageManager.h"
#include <vector>
#include <QPointer>

namespace cx
{

typedef boost::shared_ptr<class MessageListener> MessageListenerPtr;

/** Utility for listening to the MessageManager
  * and storing messages from it.
  *
 * \ingroup cx_resource_core_logger
  * \date 2014-03-09
  * \author christiana
  */
class MessageListener : public QObject
{
	Q_OBJECT
public:
	static MessageListenerPtr create();
	MessageListener();
	~MessageListener();
	bool containsErrors() const;

private slots:
	void messageReceived(Message message);
private:
	bool isError(MESSAGE_LEVEL level) const;
	std::vector<Message> mMessages;
	QPointer<MessageManager> mManager;
};


} // namespace cx


#endif // CXMESSAGELISTENER_H

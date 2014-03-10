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
#include "cxMessageListener.h"

namespace cx
{


MessageListenerPtr MessageListener::create()
{
	return MessageListenerPtr(new MessageListener);
}

MessageListener::MessageListener() : mManager(NULL)
{
	mManager = messageManager();
	connect(mManager, SIGNAL(emittedMessage(Message)), this, SLOT(messageReceived(Message)));
}

MessageListener::~MessageListener()
{
	if (mManager)
	{
		disconnect(mManager, SIGNAL(emittedMessage(Message)), this, SLOT(messageReceived(Message)));
	}
}

void MessageListener::messageReceived(Message message)
{
	mMessages.push_back(message);
}

bool MessageListener::isError(MESSAGE_LEVEL level) const
{
	return ((level==mlERROR )||( level==mlCERR ));
}

bool MessageListener::containsErrors() const
{
	for (unsigned i=0; i<mMessages.size(); ++i)
		if (this->isError(mMessages[i].getMessageLevel()))
			return true;
	return false;
}

} // namespace cx


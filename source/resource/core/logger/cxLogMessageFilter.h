/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXLOGMESSAGEFILTER_H
#define CXLOGMESSAGEFILTER_H

#include "cxResourceExport.h"
#include "cxReporter.h"

#include <vector>
#include <QPointer>

#include "cxEnumConverter.h"
#include <iostream>
#include "cxTypeConversions.h"

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
	MessageFilterConsole() : mChannel("") {}
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


} // namespace cx

#endif // CXLOGMESSAGEFILTER_H

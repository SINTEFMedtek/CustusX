/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include <QFileInfo>

#include "cxLogger.h"
#include "cxReporter.h"

namespace cx
{

class MessageLoggerInternalData
{
public:
	std::stringstream stream;
	Message msg;
};

MessageLogger::MessageLogger(const char *file, int line, const char *function, const QString& channel, MESSAGE_LEVEL severity)
{
	Message msg;
	msg.mChannel = channel;
	msg.mMessageLevel = severity;
	msg.mTimeStamp = QDateTime::currentDateTime();
	msg.mTimeoutTime = 3000;
	msg.mSourceFile = file;
	msg.mSourceLine = line;
	msg.mSourceFunction = function;

	mInternalData.reset(new MessageLoggerInternalData);
	mInternalData->msg = msg;
}
MessageLogger::~MessageLogger()
{
	if (mInternalData.use_count()==1)
	{
		mInternalData->msg.mText = QString::fromStdString(mInternalData->stream.str());
		reporter()->sendMessage(mInternalData->msg);
	}
}

MessageLogger MessageLogger::logger() const
{
	return MessageLogger(*this);
}

MessageLogger MessageLogger::logger(QString text) const
{
	return MessageLogger(*this) << text;
}

std::stringstream& MessageLogger::getStream()
{
	return mInternalData->stream;
}



void reportDebug(QString msg) { reporter()->sendDebug(msg); }
void report(QString msg) { reporter()->sendInfo(msg); }
void reportWarning(QString msg) { reporter()->sendWarning(msg); }
void reportError(QString msg) { reporter()->sendError(msg); }
void reportSuccess(QString msg) { reporter()->sendSuccess(msg); }
void reportVolatile(QString msg) { reporter()->sendVolatile(msg); }


}

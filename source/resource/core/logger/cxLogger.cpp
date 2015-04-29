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

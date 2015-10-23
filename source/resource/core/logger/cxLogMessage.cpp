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

#include "cxLogMessage.h"

#include "cxReporter.h"
#include "cxLogger.h"
#include <QtGlobal>
#include <iostream>
#include "boost/shared_ptr.hpp"
#include <QString>
#include <QMutex>
#include <QSound>
#include <QDir>
#include <QTextStream>
#include <QThread>
#include <QApplication>
#include "cxTypeConversions.h"
#include "cxDefinitionStrings.h"
#include "cxTime.h"
#include "cxMessageListener.h"

namespace cx
{

Message::Message(QString text, MESSAGE_LEVEL messageLevel, int timeoutTime) :
  mText(text),
  mMessageLevel(messageLevel),
  mTimeoutTime(timeoutTime),
  mTimeStamp(QDateTime::currentDateTime()),
  mMuted(true),
  mChannel("console")
{
	this->setThreadName();
}

Message::~Message()
{

}

void Message::setThreadName()
{
	if (QApplication::instance()->thread() == QThread::currentThread())
		mThread = "main";
	else
	{
		mThread = QThread::currentThread()->objectName();
		if (mThread.isEmpty())
			mThread = "0x"+QString::number(reinterpret_cast<long>(QThread::currentThreadId()), 16);
	}
}

QString Message::getPrintableMessage() const
{
	QString source = this->getSourceLocation();

	QString printableMessage;
	if(mMessageLevel == mlRAW)
		printableMessage = mText;
	else
		printableMessage = QString("[%1][%2] %3")
				.arg(mTimeStamp.toString("hh:mm:ss.zzz"))
				.arg(qstring_cast(mMessageLevel))
				.arg(mText);
//		printableMessage = QString("[%1][%2][%3][%4] %5")
//				.arg(mTimeStamp.toString("hh:mm:ss.zzz"))
//				.arg(source)
//				.arg(mThread)
//				.arg(qstring_cast(mMessageLevel))
//				.arg(mText);

	return printableMessage;
}

MESSAGE_LEVEL Message::getMessageLevel() const
{
  return mMessageLevel;
}

QString Message::getText() const
{
  return mText;
}

QDateTime Message::getTimeStamp() const
{
  return mTimeStamp;
}
int Message::getTimeout() const
{
  return mTimeoutTime;
}
QString Message::getSourceLocation() const
{
	if (mSourceFile.isEmpty() && mSourceFunction.isEmpty())
		return "";
	return QString("%1:%2/%3").arg(mSourceFile).arg(mSourceLine).arg(mSourceFunction);
}


} //End namespace cx

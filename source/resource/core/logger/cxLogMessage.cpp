/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

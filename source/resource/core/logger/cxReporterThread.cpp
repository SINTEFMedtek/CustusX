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

#include "cxReporterThread.h"
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
#include <QTimer>
#include "cxTypeConversions.h"
#include "cxDefinitionStrings.h"
#include "cxTime.h"
#include "cxDataLocations.h"
#include "cxMessageListener.h"

#include "cxLogQDebugRedirecter.h"
#include "cxLogIOStreamRedirecter.h"

namespace cx
{

ReporterThread::ReporterThread(QObject *parent) :
	QObject(parent)
{
	std::cout << "ReporterThread::ReporterThread threadid=" << QThread::currentThreadId() << std::endl;

	qInstallMessageHandler(convertQtMessagesToCxMessages);
	qRegisterMetaType<Message>("Message");

	this->initialize();
}

ReporterThread::~ReporterThread()
{
	qInstallMessageHandler(0);
	mCout.reset();
	mCerr.reset();
}

void ReporterThread::initialize()
{
	mRepository.reset(new MessageRepository());

	mCout.reset();
	mCerr.reset();

//	mCout.reset(new SingleStreamerImpl(std::cout, mlCOUT));
//	mCerr.reset(new SingleStreamerImpl(std::cerr, mlCERR));

	this->setLoggingFolder(DataLocations::getRootConfigPath()+"/Logs");
}

void ReporterThread::setFormat(Format format)
{
	mFormat = format;
}

bool ReporterThread::initializeLogFile(QString filename)
{
	QString timestamp = QDateTime::currentDateTime().toString(timestampMilliSecondsFormatNice());
	QString text = QString("-------> Logging initialized %1\n").arg(timestamp);
	bool success = this->appendToLogfile(filename, text);
	if (!success)
	{
		this->sendMessage(Message("Failed to open log file " + filename, mlERROR));
	}
	return success;
}

QString ReporterThread::getFilenameForChannel(QString channel) const
{
	return QString("%1/org.custusx.log.%2.txt").arg(mLogPath).arg(channel);
}

void ReporterThread::setLoggingFolder(QString absoluteLoggingFolderPath)
{
	mLogPath = absoluteLoggingFolderPath;

	QFileInfo(mLogPath+"/").absoluteDir().mkpath(".");

	this->initializeLogFile(this->getFilenameForChannel("console"));
	this->initializeLogFile(this->getFilenameForChannel("all"));
}

int ReporterThread::getDefaultTimeout(MESSAGE_LEVEL messageLevel) const
{
	switch(messageLevel)
	{
	case mlDEBUG: return 0;
	case mlINFO: return 1500;
	case mlSUCCESS: return 1500;
	case mlWARNING: return 3000;
	case mlERROR: return 0;
	case mlVOLATILE: return 5000;
	default: return 0;
	}
}

void ReporterThread::logMessage(Message msg)
{
	QMetaObject::invokeMethod(this, "sendMessage",
							  Qt::QueuedConnection,
							  Q_ARG(Message, msg));
}

void ReporterThread::sendMessage(Message message)
{
	if (message.mTimeoutTime<0)
		message.mTimeoutTime = this->getDefaultTimeout(message.mMessageLevel);

	if (message.mChannel.isEmpty())
		message.mChannel = "console";

	if (!message.mSourceFile.isEmpty())
	{
		message.mSourceFile = message.mSourceFile.split("CustusX/").back();
	}

	if (message.getMessageLevel()!=mlVOLATILE)
	{
		if (mCout)
		{
			if (message.getMessageLevel() != mlCOUT && message.getMessageLevel() != mlCERR)
				mCout->sendUnredirected(message.getPrintableMessage()+"\n");
		}

		this->appendToLogfile(this->getFilenameForChannel(message.mChannel), this->formatMessage(message) + "\n");
		this->appendToLogfile(this->getFilenameForChannel("all"), this->formatMessage(message) + "\n");
	}

	emit emittedMessage(message);

	this->sendMessageToRepository(message);
}


QString ReporterThread::formatMessage(Message msg)
{
	QString retval;

	QString bra = (mFormat.mShowBrackets ? "[" : "");
	QString ket = (mFormat.mShowBrackets ? "]" : "");

	// timestamp in front
	retval += bra + msg.getTimeStamp().toString("hh:mm:ss.zzz") + ket;

	// show source location
	if (!msg.getSourceLocation().isEmpty())
		retval += " " + bra + msg.getSourceLocation() + ket;

	// show level if set, or anyway if one of error/warning/success
	if (mFormat.mShowLevel
			|| msg.getMessageLevel() == mlERROR
			|| msg.getMessageLevel() == mlWARNING
			|| msg.getMessageLevel() == mlSUCCESS)
		retval += " " + bra + qstring_cast(msg.getMessageLevel()) + ket;

	// add message text at end.
	retval += " " + msg.getText();

	return retval;
}

/** Open the logfile and append the input text to it
 */
bool ReporterThread::appendToLogfile(QString filename, QString text)
{
	if (filename.isEmpty())
		return false;

	QFile file(filename);
	QTextStream stream;

	if (!file.open(QFile::WriteOnly | QFile::Append))
	{
		return false;
	}

	//note: writing to cout here causes recursion: disable cout redirection first.
	//	std::cout << "writing: " << text << " to " << mLogFile << std::endl;

	stream.setDevice(&file);
	stream << text;
	stream << flush;

	return true;
}

ReporterThread::Format::Format() :
	mShowBrackets(true),
	mShowLevel(true),
	mShowSourceLocation(true)
{}

void ReporterThread::installObserver(MessageObserverPtr observer, bool resend)
{
	if (!mRepository)
		return;
	QMutexLocker sentry(&mRepositoryMutex);
	mRepository->install(observer, resend);
}

void ReporterThread::uninstallObserver(MessageObserverPtr observer)
{
	if (!mRepository)
		return;
	QMutexLocker sentry(&mRepositoryMutex);
	mRepository->uninstall(observer);
}

void ReporterThread::sendMessageToRepository(const Message& message)
{
	if (!mRepository)
		return;
	QMutexLocker sentry(&mRepositoryMutex);
	mRepository->setMessage(message);
}


} //End namespace cx

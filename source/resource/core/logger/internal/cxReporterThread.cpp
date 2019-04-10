/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxReporterThread.h"
#include "cxReporter.h"
#include "cxLogger.h"
#include <QtGlobal>
#include <iostream>
#include "boost/bind.hpp"
#include "boost/shared_ptr.hpp"
#include <QString>
#include <QMutex>
#include <QSound>
#include <QDir>
#include <QTextStream>
#include <QTimer>
#include "cxTime.h"
#include "cxMessageListener.h"

#include "cxLogQDebugRedirecter.h"
#include "cxLogIOStreamRedirecter.h"
#include "cxReporterMessageRepository.h"
#include "cxTime.h"
#include "cxLogFile.h"

namespace cx
{

ReporterThread::ReporterThread(QObject *parent) :
	LogThread(parent)
{
	qInstallMessageHandler(convertQtMessagesToCxMessages);
	qRegisterMetaType<Message>("Message");

	connect(this, &LogThread::emittedMessage, this, &ReporterThread::onMessageEmitted);

	// make sure streams are closed properly before reconnecting.
	mCout.reset();
	mCerr.reset();

	mCout.reset(new SingleStreamerImpl(std::cout, mlCOUT));
	mCerr.reset(new SingleStreamerImpl(std::cerr, mlCERR));
}

ReporterThread::~ReporterThread()
{
	qInstallMessageHandler(0);
	mCout.reset();
	mCerr.reset();
}

bool ReporterThread::initializeLogFile(LogFile file)
{
	QString filename = file.getFilename();
	if (mInitializedFiles.contains(filename))
		return true;

	mInitializedFiles << filename;

	file.writeHeader();

	if (!file.isWritable())
	{
		this->processMessage(Message("Failed to open log file " + filename, mlERROR));
		return false;
	}

	return true;
}

void ReporterThread::executeSetLoggingFolder(QString absoluteLoggingFolderPath)
{
	mLogPath = absoluteLoggingFolderPath;

	QFileInfo(mLogPath+"/").absoluteDir().mkpath(".");

//	this->initializeLogFile(this->getFilenameForChannel("console"));
//	this->initializeLogFile(this->getFilenameForChannel("all"));

	this->initializeLogFile(LogFile::fromChannel(mLogPath, "console"));
	this->initializeLogFile(LogFile::fromChannel(mLogPath, "all"));
}

void ReporterThread::logMessage(Message msg)
{
	// send in calling thread- this helps if the app
	// is about to crash and we need debug info.
	this->sendToCout(msg);

	QMetaObject::invokeMethod(this, "processMessage",
							  Qt::QueuedConnection,
							  Q_ARG(Message, msg));
}

void ReporterThread::onMessageEmitted(Message msg)
{
	//	this->sendToCout(message);
	this->sendToFile(msg);
}

void ReporterThread::sendToFile(Message message)
{
	if (message.getMessageLevel()==mlVOLATILE)
		return;

//	QString channelFile = this->getFilenameForChannel(message.mChannel);
	LogFile channelLog = LogFile::fromChannel(mLogPath, message.mChannel);
	LogFile allLog = LogFile::fromChannel(mLogPath, "all");

	this->initializeLogFile(channelLog);

	channelLog.write(message);
	allLog.write(message);
}

void ReporterThread::sendToCout(Message message)
{
	if (!mCout)
		return;
	if (message.getMessageLevel()==mlVOLATILE)
		return;
	if (( message.getMessageLevel() == mlCOUT )||( message.getMessageLevel() == mlCERR ))
		return;

	mCout->sendUnredirected(message.getPrintableMessage()+"\n");
}

} //End namespace cx

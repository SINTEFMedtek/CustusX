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

#ifndef CXREPORTERTHREAD_H
#define CXREPORTERTHREAD_H

#include "cxResourceExport.h"

//#define SSC_PRINT_CALLER_INFO

#include <QMetaType>
#include <QObject>
#include <QMutex>
#include <QDateTime>
#include <QFile>
#include "boost/shared_ptr.hpp"
#include "boost/function.hpp"
#include "cxDefinitions.h"
#include "cxAudio.h"
#include <sstream>
#include "cxTypeConversions.h"
#include "cxLogger.h"
#include "cxLogMessage.h"
#include <QList>
#include <QThread>
#include "cxLogThread.h"

class QString;
class QDomNode;
class QDomDocument;
class QFile;
class QTextStream;

/**
 * \file
 * \addtogroup cx_resource_core_logger
 * @{
 */

namespace cx
{

/**\brief Thread for log handling. Used inside Reporter.
 *
 * \addtogroup cx_resource_core_logger
 */
class ReporterThread : public LogThread
{
	Q_OBJECT

public:
	ReporterThread(QObject* parent = NULL);
	virtual ~ReporterThread();
	virtual void setLoggingFolder(QString absoluteLoggingFolderPath); ///< call during startup, will fail if called when running

	virtual void installObserver(MessageObserverPtr observer, bool resend);
	virtual void uninstallObserver(MessageObserverPtr observer);

public slots:
	virtual void logMessage(Message msg);
	void pendingAction();

signals:
	void emittedMessage(Message message); ///< emitted for each new message, in addition to writing to file.

private slots:
	void processMessage(Message msg);
private:
	QMutex mActionsMutex;

	typedef boost::function<void()> PendingActionType;
	QList<PendingActionType> mPendingActions;
	bool executeAction();
	PendingActionType popAction();
	void invokePendingAction();
	void executeSetLoggingFolder(QString absoluteLoggingFolderPath);

	int getDefaultTimeout(MESSAGE_LEVEL messageLevel) const;

	bool initializeLogFile(LogFile file);

	void sendToFile(Message message);
	void sendToCout(Message message);
	Message cleanupMessage(Message message);

	typedef boost::shared_ptr<class SingleStreamerImpl> SingleStreamerImplPtr;
	SingleStreamerImplPtr mCout;
	SingleStreamerImplPtr mCerr;

	QString mLogPath;
	MessageRepositoryPtr mRepository;
	QStringList mInitializedFiles;

};

} //namespace cx

/**
 * @}
 */

#endif // CXREPORTERTHREAD_H

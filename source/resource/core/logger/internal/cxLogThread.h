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

#ifndef CXLOGTHREAD_H
#define CXLOGTHREAD_H

#include "cxResourceExport.h"

#include <QMetaType>
#include <QObject>
#include <QMutex>
#include <QDateTime>
#include <QFile>
#include <QPointer>
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
#include <QFileSystemWatcher>

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
typedef boost::shared_ptr<class MessageObserver> MessageObserverPtr;
typedef boost::shared_ptr<class MessageRepository> MessageRepositoryPtr;
class LogFile;


/** Call methods in the object's thread, via the thread message queue.
 *
 * \addtogroup cx_resource_core_logger
 */
class ThreadMethodInvoker : public QObject
{
    Q_OBJECT

public:
    typedef boost::function<void()> ActionType;

    ThreadMethodInvoker(QObject* parent);
    void callInLogThread(ActionType action);

private slots:
    void pendingAction();

private:
    QMutex mActionsMutex;
    QList<ActionType> mPendingActions;

    bool executeAction();
    ActionType popAction();
    void invokePendingAction();
};

/** Thread for log handling. Used inside Log.
 *
 * \addtogroup cx_resource_core_logger
 */
class LogThread : public QObject
{
	Q_OBJECT
    typedef boost::function<void()> ActionType;

public:
	LogThread(QObject* parent = NULL);
	virtual ~LogThread() {}
	virtual void setLoggingFolder(QString absoluteLoggingFolderPath); ///< call during startup, will fail if called when running
	virtual void installObserver(MessageObserverPtr observer, bool resend);
	virtual void uninstallObserver(MessageObserverPtr observer);

signals:
	void emittedMessage(Message message); ///< emitted for each new message, in addition to writing to observer.
public slots:
	virtual void logMessage(Message msg) {} // default impl do nothing (should be removed)

protected:
	virtual void executeSetLoggingFolder(QString absoluteLoggingFolderPath) = 0;
    void callInLogThread(ThreadMethodInvoker::ActionType action);
	Message cleanupMessage(Message message);
	MessageRepositoryPtr mRepository;

protected slots:
	void processMessage(Message msg);

private:
    QPointer<ThreadMethodInvoker> mQueue;

	int getDefaultTimeout(MESSAGE_LEVEL messageLevel) const;
};

} //namespace cx

/**
 * @}
 */

#endif // CXLOGTHREAD_H

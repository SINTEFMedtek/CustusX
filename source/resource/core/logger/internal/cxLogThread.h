/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

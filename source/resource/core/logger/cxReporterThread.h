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
#include "cxDefinitions.h"
#include "cxAudio.h"
#include <sstream>
#include "cxTypeConversions.h"
#include "cxLogger.h"
#include "cxLogMessage.h"
#include <QList>
#include <QThread>

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

/**\brief Thread for log handling. Used inside Reporter.
 *
 * \addtogroup cx_resource_core_logger
 */
class ReporterThread : public QThread
{
	Q_OBJECT

public:
	ReporterThread(QObject* parent = NULL);
	virtual ~ReporterThread();
	void setLoggingFolder(QString absoluteLoggingFolderPath); ///< call during startup, will fail if called when running

	void installObserver(MessageObserverPtr observer, bool resend);
	void uninstallObserver(MessageObserverPtr observer);

public slots:
	void logMessage(Message msg);

signals:
	void emittedMessage(Message message); ///< emitted for each new message, in addition to writing to file.

protected:
	virtual void run();

private slots:
	void onTimeout();
private:
	QMutex mMutex;
	QMutex mRepositoryMutex;

	struct Format
	{
		Format();
		bool mShowBrackets;
		bool mShowLevel;
		bool mShowSourceLocation;
	};
	void processMessageQueue();
	bool popMessageQueue();

	void sendMessage(Message msg);
	void setFormat(Format format); ///< fine-tune messaging format
	void initialize();
	void sendMessageToRepository(const Message& message);

	bool appendToLogfile(QString filename, QString text);
	QString formatMessage(Message msg);
	int getDefaultTimeout(MESSAGE_LEVEL messageLevel) const;

	bool initializeLogFile(QString filename);
	QString getFilenameForChannel(QString channel) const;

	typedef boost::shared_ptr<class SingleStreamerImpl> SingleStreamerImplPtr;
	SingleStreamerImplPtr mCout;
	SingleStreamerImplPtr mCerr;

	Format mFormat;
	QString mLogPath;
	QList<Message> mMessageQueue;
	MessageRepositoryPtr mRepository;

	//  MessageListenerPtr mListener;
};

} //namespace cx

/**
 * @}
 */

#endif // CXREPORTERTHREAD_H

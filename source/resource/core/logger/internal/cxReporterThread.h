/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

public slots:
	virtual void logMessage(Message msg);

signals:
	void emittedMessage(Message message); ///< emitted for each new message, in addition to writing to file.

protected:
	virtual void executeSetLoggingFolder(QString absoluteLoggingFolderPath);

private slots:
	void onMessageEmitted(Message msg);
private:
	bool initializeLogFile(LogFile file);

	void sendToFile(Message message);
	void sendToCout(Message message);

	typedef boost::shared_ptr<class SingleStreamerImpl> SingleStreamerImplPtr;
	SingleStreamerImplPtr mCout;
	SingleStreamerImplPtr mCerr;

	QString mLogPath;
	QStringList mInitializedFiles;

};

} //namespace cx

/**
 * @}
 */

#endif // CXREPORTERTHREAD_H

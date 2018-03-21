/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXLOGFILEWATCHERTHREAD_H
#define CXLOGFILEWATCHERTHREAD_H

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
#include <QFileSystemWatcher>
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
class LogFile;

/**\brief Thread for log handling. Used inside LogFileWatcher.
 *
 * \addtogroup cx_resource_core_logger
 */
class LogFileWatcherThread : public LogThread
{
	Q_OBJECT

public:
	LogFileWatcherThread(QObject* parent = NULL);
	virtual ~LogFileWatcherThread();

private slots:
	void onDirectoryChanged(const QString& path);
	void onFileChanged(const QString& path);
private:
	virtual void executeSetLoggingFolder(QString absoluteLoggingFolderPath);

	std::vector<Message> readMessages(const QString& path);

	QFileSystemWatcher mWatcher;
	QString mLogPath;
	QStringList mInitializedFiles;
	std::map<QString, LogFile> mFiles;
};

} //namespace cx

/**
 * @}
 */

#endif // CXLOGFILEWATCHERTHREAD_H

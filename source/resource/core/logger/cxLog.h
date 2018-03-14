/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXLOG_H
#define CXLOG_H

#include "cxResourceExport.h"

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
typedef boost::shared_ptr<class MessageListener> MessageListenerPtr;
typedef boost::shared_ptr<class LogThread> LogThreadPtr;
typedef boost::shared_ptr<class Log> LogPtr;

/**
 * \author Christian Askeland, SINTEF
 * \date 2014-12-28
 *
 * \addtogroup cx_resource_core_logger
 */
class cxResource_EXPORT Log : public QObject
{
  Q_OBJECT

public:
  virtual ~Log();

  QString getLoggingFolder() const;
  void setLoggingFolder(QString absoluteLoggingFolderPath);

  void installObserver(MessageObserverPtr observer, bool resend);
  void uninstallObserver(MessageObserverPtr observer);

signals:
  void loggingFolderChanged();

protected:
  virtual LogThreadPtr createWorker() = 0;

protected slots:
  virtual void onEmittedMessage(Message message) {}

protected:
  Log();

  void initializeObject();
  void startThread();
  void stopThread();

  QString getDefaultLogPath() const;
  QString mLogPath;
  boost::shared_ptr<class QThread> mThread;
  LogThreadPtr mWorker;
private:
  Log(const Log&);
  Log& operator=(const Log&);
};


} //namespace cx

/**
 * @}
 */

#endif // CXLOG_H

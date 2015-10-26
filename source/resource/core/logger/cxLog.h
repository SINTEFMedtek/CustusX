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

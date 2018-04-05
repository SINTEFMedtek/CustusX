/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXLOGFILEWATCHER_H
#define CXLOGFILEWATCHER_H

#include "cxResourceExport.h"

#include "cxLog.h"

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

typedef boost::shared_ptr<class LogFileWatcher> LogFileWatcherPtr;

/**
 * \author Christian Askeland, SINTEF
 * \date 2014-12-28
 *
 * \addtogroup cx_resource_core_logger
 */
class cxResource_EXPORT LogFileWatcher : public Log
{
  Q_OBJECT

public:
  static LogFileWatcherPtr create();
  virtual ~LogFileWatcher() {}

protected:
  virtual LogThreadPtr createWorker();
private:
  LogFileWatcher() {}
  LogFileWatcher(const LogFileWatcher&);
  LogFileWatcher& operator=(const LogFileWatcher&);
};


} //namespace cx

/**
 * @}
 */

#endif // CXLOGFILEWATCHER_H

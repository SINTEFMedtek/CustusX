/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxLogFileWatcher.h"
#include "cxLogger.h"
#include <QtGlobal>
#include <iostream>
#include "boost/shared_ptr.hpp"
#include <QString>
#include <QMutex>
#include <QSound>
#include <QDir>
#include <QTextStream>
#include "cxTypeConversions.h"
#include "cxDefinitionStrings.h"
#include "cxTime.h"
#include "cxMessageListener.h"

#include "internal/cxLogFileWatcherThread.h"

namespace cx
{

LogFileWatcherPtr LogFileWatcher::create()
{
	LogFileWatcherPtr retval(new LogFileWatcher());
	retval->initializeObject();
	return retval;
}

LogThreadPtr LogFileWatcher::createWorker()
{
	return LogThreadPtr(new LogFileWatcherThread());
}

} //End namespace cx

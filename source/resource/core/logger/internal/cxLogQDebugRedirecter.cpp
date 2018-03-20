/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxLogQDebugRedirecter.h"

#include <QtGlobal>
#include "cxReporter.h"

namespace cx
{

namespace {
bool isBogusQtWarning(QString msg)
{
	// appears a lot on mac
	// seems to be a bug in qt:
	// http://stackoverflow.com/questions/33545006/qt5-attempt-to-set-a-screen-on-a-child-window-many-runtime-warning-messages
	if (msg.contains("QScreen(") &&
			msg.contains("): Attempt to set a screen on a child window."))
		return true;
	return false;
}
}

void convertQtMessagesToCxMessages(QtMsgType type, const QMessageLogContext &, const QString &msg)
{
	MESSAGE_LEVEL level;// = mlINFO;
	switch (type)
	{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
    case QtInfoMsg:
		level = mlINFO;
		break;
#endif
    case QtDebugMsg:
		level = mlDEBUG;
		break;
	case QtWarningMsg:
		if (isBogusQtWarning(msg))
			return;
		level = mlWARNING;
		break;
	case QtCriticalMsg:
		level = mlERROR;
		break;
	case QtFatalMsg:
		level = mlERROR;
		//abort(); here we hope for the best instead of aborting...
	default:
		level = mlINFO;
	}

	Message message("[QT] "+msg, level);
	message.mChannel = "qdebug";
	reporter()->sendMessage(message);
}


} //End namespace cx

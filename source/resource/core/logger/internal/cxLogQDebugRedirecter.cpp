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

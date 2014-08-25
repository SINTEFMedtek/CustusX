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

#include "cxReporter.h"
#include <QFileInfo>
#include "cxLogger.h"

#define MAX_LEN_LOG_LINE 500


namespace // unnamed
{
	QString mergeSourceInfo(const char *file, int line, const char *function)
	{
		QString filename = QFileInfo(file).fileName();
		return QString("%1:%2/%3").arg(filename).arg(line).arg(function);
	}
}


void SSC_Log( const char *file, int line, const char *function, const char *format, ... )
{
	if (!cx::reporter()->isEnabled())
		return;

	va_list ap;
	char buf[MAX_LEN_LOG_LINE];
	va_start( ap, format );
	vsnprintf(buf, MAX_LEN_LOG_LINE, format, ap);
	va_end(ap);

	cx::reporter()->sendMessage(buf, cx::mlINFO, 0, false, mergeSourceInfo(file, line, function));
}

void SSC_Error( const char *file, int line, const char *function, const char *format, ... )
{
	if (!cx::reporter()->isEnabled())
		return;

	va_list ap;
	char buf[MAX_LEN_LOG_LINE];
	va_start( ap, format );
	vsnprintf(buf, MAX_LEN_LOG_LINE, format, ap);
	va_end( ap );

	cx::reporter()->sendMessage(buf, cx::mlERROR, 0, false, mergeSourceInfo(file, line, function));
}

void SSC_Warning( const char *file, int line, const char *function, const char *format, ... )
{
	if (!cx::reporter()->isEnabled())
		return;

	va_list ap;
	char buf[MAX_LEN_LOG_LINE];
	va_start( ap, format );
	vsnprintf(buf, MAX_LEN_LOG_LINE, format, ap);
	va_end( ap );

	cx::reporter()->sendMessage(buf, cx::mlWARNING, 0, false, mergeSourceInfo(file, line, function));
}


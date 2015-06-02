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

#ifndef CXIMAGESENDERFACTORY_H_
#define CXIMAGESENDERFACTORY_H_

#include "cxGrabberExport.h"

#include <QObject>
#include <QStringList>
#include <map>
#include <vector>
#include "cxStreamer.h"

class QTcpSocket;

namespace cx
{

typedef std::map<QString, QString> StringMap;
cxGrabber_EXPORT StringMap extractCommandlineOptions(QStringList cmdline);


/**
 * \brief Factory class for creating streamer objects.
 *
 * \ingroup cx_resource_videoserver
 * \author Christian Askeland, SINTEF
 * \author Janne Beate Bakeng, SINTEF
 * \date Aug 9, 2011
 */
class cxGrabber_EXPORT CommandlineImageStreamerFactory
{
public:
	CommandlineImageStreamerFactory();
	QString getDefaultSenderType() const;
	QStringList getSenderTypes() const; ///< all available sender types
	QStringList getArgumentDescription(QString type) const; ///< arguments for one streamer
	CommandLineStreamerPtr getImageSender(QString type);
	StreamerPtr getFromArguments(StringMap args);

private:
	std::vector<CommandLineStreamerPtr> mCommandLineStreamers;
};

}

#endif /* CXIMAGESENDERFACTORY_H_ */

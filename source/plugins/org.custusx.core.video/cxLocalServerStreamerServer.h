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
#ifndef CXLOCALSERVERSTREAMERSERVER_H
#define CXLOCALSERVERSTREAMERSERVER_H

#include "org_custusx_core_video_Export.h"
#include "cxStreamerService.h"
#include "cxStreamer.h"
#include "cxProcessWrapper.h"

class ctkPluginContext;

namespace cx
{
typedef boost::shared_ptr<class StringPropertyBase> StringPropertyBasePtr;
typedef boost::shared_ptr<class DoublePropertyBase> DoublePropertyBasePtr;
typedef boost::shared_ptr<class Property> PropertyPtr;
typedef boost::shared_ptr<class FilePathProperty> FilePathPropertyPtr;
typedef boost::shared_ptr<class BoolPropertyBase> BoolPropertyBasePtr;


/** Options for LocalServerStreamer
 *
 * \ingroup org_custusx_core_video
 *
 * \date 2014-11-21
 * \author Christian Askeland, SINTEF
 */
class org_custusx_core_video_EXPORT LocalServerStreamerArguments
{
public:
	std::vector<PropertyPtr> getSettings(QDomElement root);

	BoolPropertyBasePtr getRunLocalServerOption(QDomElement root);
	FilePathPropertyPtr getLocalServerNameOption(QDomElement root);
};

/** Streamer wrapping another Streamer, but also runs an executable as a local process.
 *
 * \ingroup org_custusx_core_video
 *
 * \date 2014-11-21
 * \author Christian Askeland, SINTEF
 */
class org_custusx_core_video_EXPORT LocalServerStreamer: public Streamer
{
Q_OBJECT

public:
	LocalServerStreamer(QString serverName, QString serverArguments);
	virtual ~LocalServerStreamer();
	virtual bool startStreaming(SenderPtr sender);
	virtual void stopStreaming();

	static StreamerPtr createStreamerIfEnabled(QDomElement root, StringMap args);

private slots:
	virtual void streamSlot() {}

private:
	bool localVideoServerIsRunning();
	void waitForServerStart();
	ProcessWrapperPtr mLocalVideoServerProcess;

	StreamerPtr mBase;
	QString mServerName;
	QString mServerArguments;
};
typedef boost::shared_ptr<class LocalServerStreamer> LocalServerStreamerPtr;

} //end namespace cx


#endif // CXLOCALSERVERSTREAMERSERVER_H

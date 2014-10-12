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

#ifndef CXVIDEOSERVICE_H
#define CXVIDEOSERVICE_H

#include <QObject>
#include <boost/shared_ptr.hpp>

#define VideoService_iid "cx::VideoService"

namespace cx
{
class StreamerService;

typedef boost::shared_ptr<class VideoService> VideoServicePtr;

/** \brief Video services
 *
 *  \ingroup cx_resource_video
 *  \date 2014-09-19
 *  \author Ole Vegard Solberg, SINTEF
 */
class VideoService : public QObject
{
	Q_OBJECT
public:
	virtual StreamerService *getStreamerService(QString service) = 0;
	virtual QList<StreamerService *> getStreamerServices() = 0;

	virtual bool isNull() = 0;
	static VideoServicePtr getNullObject();

signals:
	void StreamerServiceAdded(StreamerService* service);
	void StreamerServiceRemoved(StreamerService* service);

public slots:

};

} //cx
Q_DECLARE_INTERFACE(cx::VideoService, VideoService_iid)

#endif // CXVIDEOSERVICE_H
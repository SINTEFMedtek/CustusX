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
#ifndef CXHTTPREQUESTHANDLER_H
#define CXHTTPREQUESTHANDLER_H

#include <QObject>
#include "cxVisServices.h"

class QHttpRequest;
class QHttpResponse;

namespace cx
{
typedef boost::shared_ptr<class RemoteAPI> RemoteAPIPtr;
class ScreenVideoProvider;

/**
 *
 */
class HttpRequestHandler : public QObject
{
	Q_OBJECT
public:
	HttpRequestHandler(RemoteAPIPtr api);
public slots:
	void handle_request(QHttpRequest *req, QHttpResponse *resp);

private slots:
	void handle_view3D(QHttpRequest *req, QHttpResponse *resp);
	void handle_default(QHttpRequest *req, QHttpResponse *resp);
	void handle_screenshot(QHttpRequest *req, QHttpResponse *resp);
	void handle_viewshot(QHttpRequest *req, QHttpResponse *resp);

//	void handle_stream(QHttpRequest *req, QHttpResponse *resp);
	void handle_helloworld(QHttpRequest *req, QHttpResponse *resp);
	void handle_users_reply(QHttpRequest *req, QHttpResponse *resp);

private:
	RemoteAPIPtr mApi;
	ScreenVideoProvider* mScreenVideo;
};

} // namespace cx

#endif // CXHTTPREQUESTHANDLER_H

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

#include "org_custusx_webserver_Export.h"

class QHttpRequest;
class QHttpResponse;

namespace cx
{
typedef boost::shared_ptr<class RemoteAPI> RemoteAPIPtr;

/**
 *
 */
class org_custusx_webserver_EXPORT HttpRequestHandler : public QObject
{
	Q_OBJECT
public:
	HttpRequestHandler(RemoteAPIPtr api);
public slots:
	void handle_request(QHttpRequest *req, QHttpResponse *resp);

protected:
	void handle_complete_request(QHttpRequest *req, QHttpResponse *resp);
	void process_mainpage(QHttpRequest *req, QHttpResponse *resp);

    void handle_screen(QHttpRequest *req, QHttpResponse *resp);
    void process_screen(QHttpRequest *req, QHttpResponse *resp);

    void handle_layout(QHttpRequest *req, QHttpResponse *resp);
    void process_display(QHttpRequest *req, QHttpResponse *resp);
    void process_stream(QHttpRequest *req, QHttpResponse *resp);
    void process_layout(QHttpRequest *req, QHttpResponse *resp);

    void reply_mainpage(QHttpResponse *resp);
    void reply_screenshot(QHttpResponse *resp);
    void reply_notfound(QHttpResponse *resp);
    void reply_method_not_allowed(QHttpResponse *resp);
    void reply_layout_list(QHttpResponse *resp);
    void get_display_image(QHttpResponse *resp);
    void create_display(QHttpRequest *req, QHttpResponse *resp);
    void delete_display(QHttpResponse *resp);
    virtual void create_stream(QHttpRequest *req, QHttpResponse *resp);
    virtual void delete_stream(QHttpResponse *resp);

	virtual QString getAdditionalMainPageDescription() const { return ""; }

protected:
	RemoteAPIPtr mApi;

private slots:
	void onRequestSuccessful();
private:
	struct RequestType
	{
		RequestType(QHttpRequest *req_, QHttpResponse *resp_) : req(req_), resp(resp_) {}
		QHttpRequest *req;
		QHttpResponse *resp;
	};
	QList<RequestType> mRequests;


    QByteArray generatePNGEncoding(QImage image);

};

} // namespace cx

#endif // CXHTTPREQUESTHANDLER_H

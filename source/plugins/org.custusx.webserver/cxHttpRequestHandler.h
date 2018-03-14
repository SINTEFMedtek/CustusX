/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

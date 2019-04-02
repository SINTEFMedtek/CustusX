/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXWEBSERVERWIDGET_H_
#define CXWEBSERVERWIDGET_H_

#include <QWidget>
#include "boost/shared_ptr.hpp"
#include "cxVisServices.h"

class QVBoxLayout;
class QPushButton;

class QHttpServer;
class QHttpRequest;
class QHttpResponse;

namespace cx
{

typedef boost::shared_ptr<class RemoteAPI> RemoteAPIPtr;
typedef boost::shared_ptr<class HttpRequestHandler> HttpRequestHandlerPtr;

/**
 * Widget for use by the Webserver plugin
 *
 * \ingroup org_custusx_webserver
 *
 * \date 2019-03-29
 * \author Ole Vegard Solberg
 */
class WebServerWidget : public QWidget
{
	Q_OBJECT
public:
	WebServerWidget(cx::VisServicesPtr services, QWidget* parent = 0);
	virtual ~WebServerWidget();

private slots:
	void startStopSlot(bool checked);
private:
	VisServicesPtr mVisServices;
	HttpRequestHandlerPtr mRequestHandler;
	RemoteAPIPtr mAPI;
	QHttpServer *mServer;
	quint16 mPort;

	QString defaultWhatsThis() const;
	QVBoxLayout*  mVerticalLayout;
	QPushButton* mStartStopButton;

	void initServer();
	void startServer();
	void stopServer();
	void shutdownServer();
};

} /* namespace cx */

#endif /* CXWEBSERVERWIDGET_H_ */

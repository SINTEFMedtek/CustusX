/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXPLUSCONNECTWIDGET_H
#define CXPLUSCONNECTWIDGET_H

#include <QProcess>
#include "cxTabbedWidget.h"

class QPushButton;
class QDomElement;

namespace cx
{
typedef boost::shared_ptr<class VisServices> VisServicesPtr;
typedef boost::shared_ptr<class OpenIGTLinkStreamerService> OpenIGTLinkStreamerServicePtr;
typedef boost::shared_ptr<class StreamerService> StreamerServicePtr;

class PlusConnectWidget : public BaseWidget
{
public:
	PlusConnectWidget(VisServicesPtr services, QWidget *parent);
private slots:
	void connectButtonClickedSlot();

private:
	VisServicesPtr mServices;
	QPushButton* mConnectButton;
	bool mPlusRunning;
	QProcess* mExternalProcess;

	void turnOnStartTrackingInOpenIGTLinkStreamer(StreamerServicePtr streamerService);
	StreamerServicePtr getStreamerService();
	bool startPlus();
	bool stopPlus();
	QDomElement getXmlVideoElement();
};

}//namespace cx
#endif // CXPLUSCONNECTWIDGET_H

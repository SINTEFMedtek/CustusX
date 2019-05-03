/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXWEBSERVERWIDGETBASE_H
#define CXWEBSERVERWIDGETBASE_H

#include "cxResourceWidgetsExport.h"

#include <QWidget>
#include "cxVisServices.h"

class QPushButton;
class QVBoxLayout;

namespace cx
{
/**
 * Base Widget WebServer widgets
 *
 * \ingroup org_custusx_webserver
 *
 * \date 2019-04-03
 * \author Ole Vegard Solberg
 */
class cxResourceWidgets_EXPORT WebServerWidgetBase : public QWidget
{
	Q_OBJECT
public:
	WebServerWidgetBase(cx::VisServicesPtr services, QWidget* parent = 0);
	virtual ~WebServerWidgetBase();

protected slots:
	virtual void startStopSlot(bool checked);

protected:
	VisServicesPtr mServices;
	QPushButton* mStartStopButton;
	QVBoxLayout*  mVerticalLayout;

	virtual void startServer() = 0;
	virtual void stopServer() = 0;
	virtual void shutdownServer() {};//May be removed after testing and fixing shutdown issues
};

}//cx

#endif // CXWEBSERVERWIDGETBASE_H

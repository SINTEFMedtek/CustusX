/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXREMOTEAPI_H
#define CXREMOTEAPI_H

#include <QObject>
#include "cxVisServices.h"

#include "org_custusx_webserver_Export.h"

namespace cx
{
typedef boost::shared_ptr<class RemoteAPI> RemoteAPIPtr;
typedef boost::shared_ptr<class LayoutVideoSource> LayoutVideoSourcePtr;
class ScreenVideoProvider;

/**
 * API indended to be callable from external applications,
 * e.g. over http or igtl.
 */
class org_custusx_webserver_EXPORT RemoteAPI : public QObject
{
	Q_OBJECT
public:
	explicit RemoteAPI(VisServicesPtr services);

	QStringList getAvailableLayouts() const;
	void createLayoutWidget(QSize size, QString layout);
    void closeLayoutWidget();
    LayoutVideoSourcePtr startStreaming(); ///< stop streaming by destroying the returned object
    QImage grabLayout();
    QImage grabScreen();

	VisServicesPtr getServices() { return mServices; }

private:
	VisServicesPtr mServices;
    ScreenVideoProvider* mScreenVideo;
};

} // namespace cx


#endif // CXREMOTEAPI_H

/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXNDITRACKINGPLUGINACTIVATOR_H_
#define CXNDITRACKINGPLUGINACTIVATOR_H_

#include <ctkPluginActivator.h>
#include "boost/shared_ptr.hpp"

namespace cx
{

/**
 * \defgroup org_custusx_core_tracking_system_ndi
 * \ingroup cx_plugins
 *
 * See \ref cx::TrackingSystemNDIService.
 */

typedef boost::shared_ptr<class RegisteredService> RegisteredServicePtr;

/**
 * Activator for the NDI Tracking plugin
 *
 * \ingroup org_custusx_core_tracking_system_ndi
 */
class NDITrackingPluginActivator : public QObject, public ctkPluginActivator
{
	Q_OBJECT
	Q_INTERFACES(ctkPluginActivator)
	Q_PLUGIN_METADATA(IID "org_custusx_core_tracking_system_ndi")

public:
	NDITrackingPluginActivator();
	~NDITrackingPluginActivator();

	void start(ctkPluginContext* context);
	void stop(ctkPluginContext* context);

private:
	RegisteredServicePtr mRegistration;
};

} // namespace cx

#endif /* CXNDITRACKINGPLUGINACTIVATOR_H_ */

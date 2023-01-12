/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTRACKINGSHAPEPLUGINACTIVATOR_H_
#define CXTRACKINGSHAPEPLUGINACTIVATOR_H_

#include <ctkPluginActivator.h>
#include "boost/shared_ptr.hpp"

namespace cx
{

typedef boost::shared_ptr<class TrackingShapePGUIExtenderService> TrackingShapePGUIExtenderServicePtr;
typedef boost::shared_ptr<class RegisteredService> RegisteredServicePtr;

/**
 * Activator for the tracking_shape plugin
 *
 * \ingroup org_custusx_core_tracking_shape
 *
 * \date 2022-11-08
 * \author Ole Vegard Solberg
 */
class TrackingShapePluginActivator :  public QObject, public ctkPluginActivator
{
	Q_OBJECT
	Q_INTERFACES(ctkPluginActivator)
	Q_PLUGIN_METADATA(IID "org_custusx_core_tracking_shape")

public:

	TrackingShapePluginActivator();
	~TrackingShapePluginActivator();

	void start(ctkPluginContext* context);
	void stop(ctkPluginContext* context);

private:
	RegisteredServicePtr mRegistration;
};

} // namespace cx

#endif /* CXTRACKINGSHAPEPLUGINACTIVATOR_H_ */

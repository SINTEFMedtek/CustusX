/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXFILTERCLIPMESHPLUGINACTIVATOR_H
#define CXFILTERCLIPMESHPLUGINACTIVATOR_H

#include <ctkPluginActivator.h>
#include "boost/shared_ptr.hpp"

namespace cx
{

typedef boost::shared_ptr<class RegisteredService> RegisteredServicePtr;

class FilterClipMeshPluginActivator : public QObject, public ctkPluginActivator
{
	Q_OBJECT
	Q_INTERFACES(ctkPluginActivator)
	Q_PLUGIN_METADATA(IID "org_custusx_filter_clipmesh")

public:
	FilterClipMeshPluginActivator();
	~FilterClipMeshPluginActivator();

	void start(ctkPluginContext* context);
	void stop(ctkPluginContext* context);

private:
	RegisteredServicePtr mRegistration;
};

} // namespace cx

#endif // CXFILTERCLIPMESHPLUGINACTIVATOR_H

/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXFILTERCPDPLUGINACTIVATOR_H
#define CXFILTERCPDPLUGINACTIVATOR_H

#include <ctkPluginActivator.h>
#include <memory>

namespace cx
{

typedef std::shared_ptr<class RegisteredService> RegisteredServicePtr;

class FilterCPDPluginActivator : public QObject, public ctkPluginActivator
{
	Q_OBJECT
	Q_INTERFACES(ctkPluginActivator)
	Q_PLUGIN_METADATA(IID "org_custusx_filter_cpd")

public:
	FilterCPDPluginActivator();
	~FilterCPDPluginActivator();

	void start(ctkPluginContext* context);
	void stop(ctkPluginContext* context);

private:
	RegisteredServicePtr mRegistration;
};

} // namespace cx

#endif // CXFILTERCPDPLUGINACTIVATOR_H

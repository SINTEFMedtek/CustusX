/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXREGISTEREDSERVICE_H
#define CXREGISTEREDSERVICE_H

#include "cxResourcePluginUtilitiesExport.h"

#include "boost/shared_ptr.hpp"
#include <ctkPluginActivator.h>

typedef boost::shared_ptr<class QObject> QObjectPtr;

namespace cx
{
typedef boost::shared_ptr<class RegisteredService> RegisteredServicePtr;

/**
 * Activator for the registration plugin
 *
 * \ingroup org_custusx_registration
 *
 *  \date 2014-09-03
 *  \author Ole Vegard Solberg, SINTEF
 *  \author Christian Askeland, SINTEF
 */
class cxResourcePluginUtilities_EXPORT RegisteredService
{
public:
	template <class SERVICE_TYPE>
	static RegisteredServicePtr create(ctkPluginContext* context, QString iid)
	{
		return RegisteredServicePtr(new RegisteredService(context, new SERVICE_TYPE(context), iid));
	}
	template <class SERVICE_TYPE>
	static RegisteredServicePtr create(ctkPluginContext* context, SERVICE_TYPE* service, QString iid)
	{
		return RegisteredServicePtr(new RegisteredService(context, service, iid));
	}

	explicit RegisteredService(ctkPluginContext* context, QObject* instance, QString iid);
	~RegisteredService();
	template<class SERVICE_TYPE>
	SERVICE_TYPE* get() const { return dynamic_cast<SERVICE_TYPE*>(mService.get()); }
	QObjectPtr mService;
	ctkServiceRegistration mReference;
};

} // namespace cx

#endif // CXREGISTEREDSERVICE_H

/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
	 this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
	 this list of conditions and the following disclaimer in the documentation
	 and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
	 may be used to endorse or promote products derived from this software
	 without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

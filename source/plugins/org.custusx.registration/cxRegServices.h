/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXREGISTRATIONMETHODSERVICES_H
#define CXREGISTRATIONMETHODSERVICES_H

#include "org_custusx_registration_Export.h"

#include "cxVisServices.h"

namespace cx
{
typedef boost::shared_ptr<class RegServices> RegServicesPtr;
typedef boost::shared_ptr<class RegistrationService> RegistrationServicePtr;
typedef boost::shared_ptr<class ViewService> ViewServicePtr;
typedef boost::shared_ptr<class AcquisitionService> AcquisitionServicePtr;

/**
 * Convenience class combining all services used by registration methods.
 *
 * \ingroup org_custusx_registration
 *
 * \date Nov 14 2014
 * \author Ole Vegard Solberg, SINTEF
 */
class org_custusx_registration_EXPORT RegServices : public VisServices
{
public:
	static RegServicesPtr create(ctkPluginContext* context);
	RegServices(ctkPluginContext* context);
	static RegServicesPtr getNullObjects();

	RegistrationServicePtr registration() { return registrationService; }
	AcquisitionServicePtr acquisition() { return acquisitionService; }

protected:
	RegServices();
	RegistrationServicePtr registrationService;
	AcquisitionServicePtr acquisitionService;
};

}

#endif // CXREGISTRATIONMETHODSERVICES_H

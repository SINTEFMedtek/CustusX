/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXREGISTRATIONMETHODCENTERLINEPLUGINACTIVATOR_H_
#define CXREGISTRATIONMETHODCENTERLINEPLUGINACTIVATOR_H_

#include <ctkPluginActivator.h>
#include "boost/shared_ptr.hpp"
#include "org_custusx_registration_method_centerline_Export.h"

namespace cx
{
/**
 * \defgroup org_custusx_registration_method_centerline
 * \ingroup cx_plugins
 *
 * \see cx::RegistrationMethodCenterlineImageToPatientService.
 *
 */

typedef boost::shared_ptr<class RegisteredService> RegisteredServicePtr;

/**
 * Activator for Registration method Centerline service
 *
 * \ingroup org_custusx_registration_method_centerline
 *
 * \date 2017-02-16
 * \author Erlend Fagertun Hofstad, SINTEF
 */
class org_custusx_registration_method_centerline_EXPORT RegistrationMethodCenterlinePluginActivator :  public QObject, public ctkPluginActivator
{
	Q_OBJECT
	Q_INTERFACES(ctkPluginActivator)
    Q_PLUGIN_METADATA(IID "org_custusx_registration_method_centerline")

public:

    RegistrationMethodCenterlinePluginActivator();
    ~RegistrationMethodCenterlinePluginActivator();

	void start(ctkPluginContext* context);
	void stop(ctkPluginContext* context);

private:
	RegisteredServicePtr mRegistrationImageToPatient;
};

} // namespace cx

#endif /* CXREGISTRATIONMETHODCENTERLINEPLUGINACTIVATOR_H_ */

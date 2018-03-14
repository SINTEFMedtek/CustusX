/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXREGISTRATIONMETHODBRONCHOSCOPYPLUGINACTIVATOR_H_
#define CXREGISTRATIONMETHODBRONCHOSCOPYPLUGINACTIVATOR_H_

#include <ctkPluginActivator.h>
#include "boost/shared_ptr.hpp"

namespace cx
{
/**
 * \defgroup org_custusx_registration_method_bronchoscopy
 * \ingroup cx_plugins
 *
 * \see cx::RegistrationMethodBronchoscopyImageToPatientService.
 *
 */

typedef boost::shared_ptr<class RegisteredService> RegisteredServicePtr;

/**
 * Activator for Registration method Bronchoscopy service
 *
 * \ingroup org_custusx_registration_method_bronchoscopy
 *
 * \date 2014-10-01
 * \author Ole Vegard Solberg, SINTEF
 */
class RegistrationMethodBronchoscopyPluginActivator :  public QObject, public ctkPluginActivator
{
	Q_OBJECT
	Q_INTERFACES(ctkPluginActivator)
	Q_PLUGIN_METADATA(IID "org_custusx_registration_method_bronchoscopy")

public:

	RegistrationMethodBronchoscopyPluginActivator();
	~RegistrationMethodBronchoscopyPluginActivator();

	void start(ctkPluginContext* context);
	void stop(ctkPluginContext* context);

private:
	RegisteredServicePtr mRegistrationImageToPatient;
    RegisteredServicePtr mRegistrationImageToImage;
};

} // namespace cx

#endif /* CXREGISTRATIONMETHODBRONCHOSCOPYPLUGINACTIVATOR_H_ */

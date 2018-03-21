/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXREGISTRATIONMETHODPLATEPLUGINACTIVATOR_H_
#define CXREGISTRATIONMETHODPLATEPLUGINACTIVATOR_H_

#include <ctkPluginActivator.h>
#include "boost/shared_ptr.hpp"

namespace cx
{
/**
 * \defgroup org_custusx_registration_method_plate
 * \ingroup cx_plugins
 *
 * \see cx::RegistrationMethodPlateImageToPatientService
 *
 */

//typedef boost::shared_ptr<class RegistrationMethodPlateService> RegistrationMethodPlateServicePtr;
typedef boost::shared_ptr<class RegisteredService> RegisteredServicePtr;

/**
 * Activator for Registration method Plate service
 *
 * \ingroup org_custusx_registration_method_plate
 *
 * \date 2014-10-01
 * \author Ole Vegard Solberg, SINTEF
 */
class RegistrationMethodPlatePluginActivator :  public QObject, public ctkPluginActivator
{
	Q_OBJECT
	Q_INTERFACES(ctkPluginActivator)
	Q_PLUGIN_METADATA(IID "org_custusx_registration_method_plate")

public:

	RegistrationMethodPlatePluginActivator();
	~RegistrationMethodPlatePluginActivator();

	void start(ctkPluginContext* context);
	void stop(ctkPluginContext* context);

private:
	RegisteredServicePtr mRegistrationImageToPatient;
};

} // namespace cx

#endif /* CXREGISTRATIONMETHODPLATEPLUGINACTIVATOR_H_ */

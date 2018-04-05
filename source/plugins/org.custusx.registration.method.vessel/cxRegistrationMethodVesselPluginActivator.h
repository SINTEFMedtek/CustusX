/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXREGISTRATIONMETHODVESSELPLUGINACTIVATOR_H_
#define CXREGISTRATIONMETHODVESSELPLUGINACTIVATOR_H_

#include <ctkPluginActivator.h>
#include "boost/shared_ptr.hpp"

namespace cx
{
/**
 * \defgroup org_custusx_registration_method_vessel
 * \ingroup cx_plugins
 *
 * \see cx::RegistrationMethodVesselImageToImageService
 *
 */

typedef boost::shared_ptr<class RegisteredService> RegisteredServicePtr;

/**
 * Activator for Registration method Vessel service
 *
 * \ingroup org_custusx_registration_method_vessel
 *
 * \date 2014-10-01
 * \author Ole Vegard Solberg, SINTEF
 */
class RegistrationMethodVesselPluginActivator :  public QObject, public ctkPluginActivator
{
	Q_OBJECT
	Q_INTERFACES(ctkPluginActivator)
	Q_PLUGIN_METADATA(IID "org_custusx_registration_method_vessel")

public:

	RegistrationMethodVesselPluginActivator();
	~RegistrationMethodVesselPluginActivator();

	void start(ctkPluginContext* context);
	void stop(ctkPluginContext* context);

private:
	RegisteredServicePtr mRegistrationImageToImage;
};

} // namespace cx

#endif /* CXREGISTRATIONMETHODVESSELPLUGINACTIVATOR_H_ */

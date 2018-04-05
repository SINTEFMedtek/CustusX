/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXREGISTRATIONMETHODMANUALPLUGINACTIVATOR_H_
#define CXREGISTRATIONMETHODMANUALPLUGINACTIVATOR_H_

#include <ctkPluginActivator.h>
#include "boost/shared_ptr.hpp"

namespace cx
{
/**
 * \defgroup org_custusx_registration_method_manual
 * \ingroup cx_plugins
 *
 * \see cx::RegistrationMethodManualPatientOrientationService
 * \see cx::RegistrationMethodManualImageTransformService
 * \see cx::RegistrationMethodManualImageToPatientService
 * \see cx::RegistrationMethodManualImageToImageService
 *
 */

typedef boost::shared_ptr<class RegisteredService> RegisteredServicePtr;

/**
 * Activator for the example plugin
 *
 * \ingroup org_custusx_registration_method_manual
 *
 * \date Sep 08, 2014
 * \author Ole Vegard Solberg, SINTEF
 * \author Geir Arne Tangen, SINTEF
 */
class RegistrationMethodManualPluginActivator :  public QObject, public ctkPluginActivator
{
	Q_OBJECT
	Q_INTERFACES(ctkPluginActivator)
	Q_PLUGIN_METADATA(IID "org_custusx_registration_method_manual")

public:

	RegistrationMethodManualPluginActivator();
	~RegistrationMethodManualPluginActivator();

	void start(ctkPluginContext* context);
	void stop(ctkPluginContext* context);

private:
	RegisteredServicePtr mRegistrationImageToImage;
	RegisteredServicePtr mRegistrationImageToPatient;
	RegisteredServicePtr mRegistrationImageTransform;
	RegisteredServicePtr mRegistrationPatientOrientation;
};

} // namespace cx

#endif /* CXREGISTRATIONMETHODMANUALPLUGINACTIVATOR_H_ */

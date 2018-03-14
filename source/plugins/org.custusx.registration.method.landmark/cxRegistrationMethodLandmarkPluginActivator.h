/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXREGISTRATIONMETHODLANDMARKPLUGINACTIVATOR_H_
#define CXREGISTRATIONMETHODLANDMARKPLUGINACTIVATOR_H_

#include <ctkPluginActivator.h>
#include "boost/shared_ptr.hpp"

namespace cx
{
/**
 * \defgroup org_custusx_registration_method_landmark
 * \ingroup cx_plugins
 *
 * \see cx::RegistrationMethodLandmarkImageToImageService
 * \see cx::RegistrationMethodLandmarkImageToPatientService
 * \see cx::RegistrationMethodFastLandmarkImageToPatientService
 *
 */

//typedef boost::shared_ptr<class RegistrationMethodLandmarkService> RegistrationMethodLandmarkServicePtr;
typedef boost::shared_ptr<class RegisteredService> RegisteredServicePtr;

/**
 * Activator for Registration method landmark service
 *
 * \ingroup org_custusx_registration_method_landmark
 *
 * \date 2014-10-01
 * \author Ole Vegard Solberg, SINTEF
 */
class RegistrationMethodLandmarkPluginActivator :  public QObject, public ctkPluginActivator
{
	Q_OBJECT
	Q_INTERFACES(ctkPluginActivator)
	Q_PLUGIN_METADATA(IID "org_custusx_registration_method_landmark")

public:

	RegistrationMethodLandmarkPluginActivator();
	~RegistrationMethodLandmarkPluginActivator();

	void start(ctkPluginContext* context);
	void stop(ctkPluginContext* context);

private:
	RegisteredServicePtr mRegistrationImageToImage;
	RegisteredServicePtr mRegistrationImageToPatient;
	RegisteredServicePtr mRegistrationFastImageToPatient;
};

} // namespace cx

#endif /* CXREGISTRATIONMETHODLANDMARKPLUGINACTIVATOR_H_ */

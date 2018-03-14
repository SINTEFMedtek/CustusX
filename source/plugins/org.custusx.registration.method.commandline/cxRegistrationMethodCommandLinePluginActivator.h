/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXREGISTRATIONMETHODCOMMANDLINEPLUGINACTIVATOR_H_
#define CXREGISTRATIONMETHODCOMMANDLINEPLUGINACTIVATOR_H_

#include <ctkPluginActivator.h>
#include "boost/shared_ptr.hpp"

namespace cx
{
/**
 * \defgroup org_custusx_registration_method_commandline
 * \ingroup cx_plugins
 *
 * \see cx::RegistrationMethodCommandLineService.
 *
 */

typedef boost::shared_ptr<class RegisteredService> RegisteredServicePtr;

/**
 * Activator for Registration method commandline service
 *
 * \ingroup org_custusx_registration_method_commandline
 *
 * \date 2014-10-09
 * \author Christian Askeland
 */
class RegistrationMethodCommandLinePluginActivator :  public QObject, public ctkPluginActivator
{
	Q_OBJECT
	Q_INTERFACES(ctkPluginActivator)
	Q_PLUGIN_METADATA(IID "org_custusx_registration_method_commandline")

public:

	RegistrationMethodCommandLinePluginActivator();
	~RegistrationMethodCommandLinePluginActivator();

	void start(ctkPluginContext* context);
	void stop(ctkPluginContext* context);

private:
	  RegisteredServicePtr mRegistrationCommandLine;
};

} // namespace cx
#endif /* CXREGISTRATIONMETHODCOMMANDLINEPLUGINACTIVATOR_H_ */

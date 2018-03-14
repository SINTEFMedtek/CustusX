/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXREGISTRATIONGUIPLUGINACTIVATOR_H_
#define CXREGISTRATIONGUIPLUGINACTIVATOR_H_

#include <ctkPluginActivator.h>
#include "boost/shared_ptr.hpp"

namespace cx
{
/**
 * \defgroup org_custusx_registration_gui
 * \ingroup cx_plugins
 *
 * \see cx::RegistrationGUIExtenderService.
 *
 */

typedef boost::shared_ptr<class RegisteredService> RegisteredServicePtr;

/**
 * Activator for the Registration GUI plugin
 *
 * \ingroup org_custusx_registration_gui
 *
 * \date Sep 08 2014
 * \author Ole Vegard Solberg, SINTEF
 */
class RegistrationGUIPluginActivator :  public QObject, public ctkPluginActivator
{
	Q_OBJECT
	Q_INTERFACES(ctkPluginActivator)
	Q_PLUGIN_METADATA(IID "org_custusx_registration_gui")

public:

	RegistrationGUIPluginActivator();
	~RegistrationGUIPluginActivator();

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);

private:
  RegisteredServicePtr mRegistration;
  RegisteredServicePtr mRegistrationHistory;
};

} // namespace cx

#endif /* CXREGISTRATIONGUIPLUGINACTIVATOR_H_ */

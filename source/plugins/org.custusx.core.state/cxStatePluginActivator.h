/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXSTATEPLUGINACTIVATOR_H
#define CXSTATEPLUGINACTIVATOR_H

#include <ctkPluginActivator.h>
#include "boost/shared_ptr.hpp"

namespace cx
{
/**
 * \defgroup org_custusx_core_state
 * \ingroup cx_plugins
 *
 * \see cx::StateServiceImpl.
 *
 */

typedef boost::shared_ptr<class RegisteredService> RegisteredServicePtr;

/**
 * Activator for the State plugin
 *
 * \ingroup org_custusx_core_state
 * \date 2014-11-24
 * \author Christian Askeland
 */
class StatePluginActivator :  public QObject, public ctkPluginActivator
{
  Q_OBJECT
  Q_INTERFACES(ctkPluginActivator)
	Q_PLUGIN_METADATA(IID "org_custusx_core_state")

public:

  StatePluginActivator();
  ~StatePluginActivator();

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);

private:
	RegisteredServicePtr mStateService;
};

} // namespace cx

#endif // CXSTATEPLUGINACTIVATOR_H

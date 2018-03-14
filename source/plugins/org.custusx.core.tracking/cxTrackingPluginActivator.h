/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTRACKINGPLUGINACTIVATOR_H_
#define CXTRACKINGPLUGINACTIVATOR_H_

#include <ctkPluginActivator.h>
#include "boost/shared_ptr.hpp"

namespace cx
{

/**
 * \defgroup org_custusx_core_tracking
 * \ingroup cx_plugins
 *
 *
 * See \ref cx::TrackingImplService.
 *
 */


typedef boost::shared_ptr<class RegisteredService> RegisteredServicePtr;

/**
 * Activator for the Tracking plugin
 *
 * \ingroup org_custusx_core_tracking
 * \date 2014-09-19
 * \author Ole Vegard Solberg
 */
class TrackingPluginActivator :  public QObject, public ctkPluginActivator
{
  Q_OBJECT
  Q_INTERFACES(ctkPluginActivator)
	Q_PLUGIN_METADATA(IID "org_custusx_core_tracking")

public:

  TrackingPluginActivator();
  ~TrackingPluginActivator();

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);

private:
	RegisteredServicePtr mRegistration;
};

} // namespace cx

#endif /* CXTRACKINGPLUGINACTIVATOR_H_ */

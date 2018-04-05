/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVIEWPLUGINACTIVATOR_H_
#define CXVIEWPLUGINACTIVATOR_H_

#include <ctkPluginActivator.h>
#include "boost/shared_ptr.hpp"

namespace cx
{
/**
 * \defgroup org_custusx_core_view
 * \ingroup cx_plugins
 *
 * \see cx::ViewImplService.
 *
 */

typedef boost::shared_ptr<class RegisteredService> RegisteredServicePtr;

/**
 * Activator for the Visualization plugin
 *
 * \ingroup org_custusx_core_view
 * \date 2014-09-19
 * \author Ole Vegard Solberg
 */
class VisualizationPluginActivator :  public QObject, public ctkPluginActivator
{
	Q_OBJECT
	Q_INTERFACES(ctkPluginActivator)
	Q_PLUGIN_METADATA(IID "org_custusx_core_view")

public:

  VisualizationPluginActivator();
  ~VisualizationPluginActivator();

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);

private:
	RegisteredServicePtr mRegistration;
};

} // namespace cx

#endif /* CXVIEWPLUGINACTIVATOR_H_ */

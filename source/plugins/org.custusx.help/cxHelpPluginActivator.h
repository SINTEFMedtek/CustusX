/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXHELPPLUGINACTIVATOR_H_
#define CXHELPPLUGINACTIVATOR_H_

#include <ctkPluginActivator.h>
#include "boost/shared_ptr.hpp"

namespace cx
{
/**
 * \defgroup org_custusx_help
 * \ingroup cx_plugins
 *
 * \see cx::HelpGUIExtenderService.
 *
 */

typedef boost::shared_ptr<class RegisteredService> RegisteredServicePtr;
typedef boost::shared_ptr<class HelpEngine> HelpEnginePtr;

/**
 * Activator for the help plugin
 *
 * \ingroup org_custusx_help
 *
 * \date 2014-09-11
 * \author Christian Askeland
 */
class HelpPluginActivator :  public QObject, public ctkPluginActivator
{
  Q_OBJECT
  Q_INTERFACES(ctkPluginActivator)
  Q_PLUGIN_METADATA(IID "org_custusx_help")

public:

  HelpPluginActivator();
  ~HelpPluginActivator();

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);

private:
  RegisteredServicePtr mGUIExtender;
  HelpEnginePtr mEngine;
};

} // namespace cx

#endif /* CXHELPPLUGINACTIVATOR_H_ */

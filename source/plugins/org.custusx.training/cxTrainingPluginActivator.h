/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTRAININGPLUGINACTIVATOR_H_
#define CXTRAININGPLUGINACTIVATOR_H_

#include <ctkPluginActivator.h>
#include "boost/shared_ptr.hpp"

namespace cx
{
/**
 * \defgroup org_custusx_training
 * \ingroup cx_plugins
 *
 *
 */

typedef boost::shared_ptr<class RegisteredService> RegisteredServicePtr;
typedef boost::shared_ptr<class HelpEngine> HelpEnginePtr;

/**
 * Activator for the training plugin
 *
 * \ingroup org_custusx_training
 *
 * \date 2016-03-14
 * \author Christian Askeland
 */
class TrainingPluginActivator :  public QObject, public ctkPluginActivator
{
  Q_OBJECT
  Q_INTERFACES(ctkPluginActivator)
  Q_PLUGIN_METADATA(IID "org_custusx_training")

public:

  TrainingPluginActivator();
  ~TrainingPluginActivator();

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);

private:
  RegisteredServicePtr mGUIExtender;
//  HelpEnginePtr mEngine;
};

} // namespace cx

#endif /* CXTRAININGPLUGINACTIVATOR_H_ */

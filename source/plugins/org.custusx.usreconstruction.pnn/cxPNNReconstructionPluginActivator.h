/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXPNNRECONSTRUCTIONPLUGINACTIVATOR_H_
#define CXPNNRECONSTRUCTIONPLUGINACTIVATOR_H_

#include <ctkPluginActivator.h>
#include "boost/shared_ptr.hpp"

namespace cx
{
/**
 * \defgroup org_custusx_usreconstruction_pnn
 * \ingroup cx_plugins
 *
 * \see cx::PNNReconstructionMethodService
 *
 */

typedef boost::shared_ptr<class RegisteredService> RegisteredServicePtr;

/**
 * Activator for the PNN reconstruction plugin
 *
 * \ingroup org_custusx_usreconstruction_pnn
 *
 * \date 2014-06-12
 * \author Janne Beate Bakeng, SINTEF
 * \author Ole Vegard Solberg, SINTEF
 */
class PNNReconstructionPluginActivator :  public QObject, public ctkPluginActivator
{
  Q_OBJECT
  Q_INTERFACES(ctkPluginActivator)
  Q_PLUGIN_METADATA(IID "org_custusx_usreconstruction_pnn")

public:

  PNNReconstructionPluginActivator();
  ~PNNReconstructionPluginActivator();

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);

private:
	RegisteredServicePtr mRegistration;
};

} // namespace cx

#endif /* CXPNNRECONSTRUCTIONPLUGINACTIVATOR_H_ */

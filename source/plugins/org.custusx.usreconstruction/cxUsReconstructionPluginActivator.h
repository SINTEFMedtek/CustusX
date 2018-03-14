/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXUSRECONSTRUCTIONPLUGINACTIVATOR_H_
#define CXUSRECONSTRUCTIONPLUGINACTIVATOR_H_

#include <ctkPluginActivator.h>
#include "boost/shared_ptr.hpp"

namespace cx
{
/**
 * \defgroup org_custusx_usreconstruction
 * \ingroup cx_plugins
 *
 * \see cx::UsReconstructionGUIExtenderService
 *
 */

typedef boost::shared_ptr<class RegisteredService> RegisteredServicePtr;

/**
 * Activator for the UsReconstruction GUI plugin
 *
 * \ingroup org_custusx_usreconstruction
 *
 * \date Oct 15 2014
 * \author Ole Vegard Solberg, SINTEF
 */
class UsReconstructionPluginActivator :  public QObject, public ctkPluginActivator
{
	Q_OBJECT
	Q_INTERFACES(ctkPluginActivator)
	Q_PLUGIN_METADATA(IID "org_custusx_usreconstruction")

public:

	UsReconstructionPluginActivator();
	~UsReconstructionPluginActivator();

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);

private:
  RegisteredServicePtr mRegisteredGui;
  RegisteredServicePtr mUsReconstruction;
};

} // namespace cx

#endif /* CXUSRECONSTRUCTIONPLUGINACTIVATOR_H_ */

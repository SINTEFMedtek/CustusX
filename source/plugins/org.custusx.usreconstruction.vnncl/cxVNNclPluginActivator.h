/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVNNCLRECONSTRUCTIONSERVICEPLUGINACTIVATOR_H_
#define CXVNNCLRECONSTRUCTIONSERVICEPLUGINACTIVATOR_H_

#include <ctkPluginActivator.h>
#include "boost/shared_ptr.hpp"

namespace cx
{
/**
 * \defgroup org_custusx_usreconstruction_vnncl
 * \ingroup cx_plugins
 *
 * \see cx::VNNclReconstructionMethodService
 *
 */

typedef boost::shared_ptr<class RegisteredService> RegisteredServicePtr;

/**
 * Activator for the Tord Øygard reconstruction service
 *
 * \ingroup org_custusx_usreconstruction_vnncl
 *
 * \date 2014-05-09
 * \author Janne Beate Bakeng
 */
class VNNclPluginActivator :  public QObject, public ctkPluginActivator
{
  Q_OBJECT
  Q_INTERFACES(ctkPluginActivator)
  Q_PLUGIN_METADATA(IID "org_custusx_vnnclreconstruction")

public:

  VNNclPluginActivator();
  ~VNNclPluginActivator();

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);

private:
	RegisteredServicePtr mRegistration;
};

} // namespace cx

#endif /* CXVNNCLRECONSTRUCTIONSERVICEPLUGINACTIVATOR_H_ */

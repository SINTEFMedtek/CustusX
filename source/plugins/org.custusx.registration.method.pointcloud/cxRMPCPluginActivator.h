/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXRMPCPLUGINACTIVATOR_H
#define CXRMPCPLUGINACTIVATOR_H

#include <ctkPluginActivator.h>
#include "boost/shared_ptr.hpp"

namespace cx
{
/**
 * \defgroup org_custusx_registration_method_pointcloud
 * \ingroup cx_plugins
 *
 */

typedef boost::shared_ptr<class DicomGUIExtenderService> DicomGUIExtenderServicePtr;
typedef boost::shared_ptr<class RegisteredService> RegisteredServicePtr;

/**
 * Activator for the pointcloud registration plugin
 *
 * \ingroup org_custusx_registration_method_pointcloud
 *
 * \date 2015-09-06
 * \author Christian Askeland
 */
class RMPCPluginActivator :  public QObject, public ctkPluginActivator
{
  Q_OBJECT
  Q_INTERFACES(ctkPluginActivator)
  Q_PLUGIN_METADATA(IID "org_custusx_registration_method_pointcloud")

public:

  RMPCPluginActivator();
  ~RMPCPluginActivator();

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);

private:
  RegisteredServicePtr mPointCloud;
  RegisteredServicePtr mPointerToSurface;
};

} // namespace cx

#endif // CXRMPCPLUGINACTIVATOR_H

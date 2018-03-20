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
 * \defgroup org_custusx_core_video
 * \ingroup cx_plugins
 *
 * See \ref cx::VideoImplService.
 *
 */

typedef boost::shared_ptr<class RegisteredService> RegisteredServicePtr;

/**
 * Activator for the Video plugin
 *
 * \ingroup org_custusx_core_video
 * \date 2014-09-19
 * \author Ole Vegard Solberg
 */
class VideoPluginActivator :  public QObject, public ctkPluginActivator
{
  Q_OBJECT
  Q_INTERFACES(ctkPluginActivator)
	Q_PLUGIN_METADATA(IID "org_custusx_core_video")

public:

  VideoPluginActivator();
  ~VideoPluginActivator();

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);

private:
	RegisteredServicePtr mRegistration;
	RegisteredServicePtr mIGTLinkStreamer;
	RegisteredServicePtr mOpenCVStreamer;
	RegisteredServicePtr mImageFileStreamer;
};

} // namespace cx

#endif /* CXTRACKINGPLUGINACTIVATOR_H_ */

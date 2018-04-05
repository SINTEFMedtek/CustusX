/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVBPLUGINACTIVATOR_H_
#define CXVBPLUGINACTIVATOR_H_

#include <ctkPluginActivator.h>
#include "boost/shared_ptr.hpp"

namespace cx
{
/**
 * \defgroup org_custusx_virtualbronchoscopy
 * \ingroup cx_plugins
 *
 * \see cx::VirtualBronchoscopyService
 *
 */

typedef boost::shared_ptr<class VBGUIExtenderService> VBGUIExtenderServicePtr;
typedef boost::shared_ptr<class RegisteredService> RegisteredServicePtr;

/**
 * Activator for the virtual bronchoscopy module plugin
 *
 * \ingroup org_custusx_virtualbronchoscopy
 *
 * \date Aug 26, 2015
 * \author Geir Arne Tangen, SINTEF
 */
class VBPluginActivator :  public QObject, public ctkPluginActivator
{
	Q_OBJECT
	Q_INTERFACES(ctkPluginActivator)
	Q_PLUGIN_METADATA(IID "org_custusx_virtualbronchoscopy")

public:

        VBPluginActivator();
        ~VBPluginActivator();

	void start(ctkPluginContext* context);
	void stop(ctkPluginContext* context);

private:
        RegisteredServicePtr mVBregistration;
};

} // namespace cx

#endif /* VBPLUGINACTIVATOR_H_ */

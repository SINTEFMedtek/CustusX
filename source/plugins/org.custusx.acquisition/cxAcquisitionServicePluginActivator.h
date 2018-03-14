/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXACQUISITIONSERVICEPLUGINACTIVATOR_H
#define CXACQUISITIONSERVICEPLUGINACTIVATOR_H
#include <ctkPluginActivator.h>
#include "boost/shared_ptr.hpp"

namespace cx
{

/**
 * \defgroup org_custusx_acquisition
 * \ingroup cx_plugins
 * \brief Handles the us acquisition process.
 *
 * \see cx::AcquisitionService.
 * \see cx::AcquisitionPlugin
 *
 */


typedef boost::shared_ptr<class RegisteredService> RegisteredServicePtr;

/**
 * Activator for the acquisition plugin
 *
 * \ingroup org_custusx_acquisition
 *
 *  \date 2014-12-02
 *  \author Ole Vegard Solberg, SINTEF
 */
class AcquisitionServicePluginActivator :  public QObject, public ctkPluginActivator
{
	Q_OBJECT
	Q_INTERFACES(ctkPluginActivator)
	Q_PLUGIN_METADATA(IID "org_custusx_acquisition")
public:
	AcquisitionServicePluginActivator();
	~AcquisitionServicePluginActivator();

	void start(ctkPluginContext* context);
	void stop(ctkPluginContext* context);

private:
	RegisteredServicePtr mRegisteredAcquisitionService;
	RegisteredServicePtr mRegisteredAcquisitionGUIService;
};

} // cx
#endif // CXACQUISITIONSERVICEPLUGINACTIVATOR_H

/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXCALIBRATIONPLUGINACTIVATOR_H_
#define CXCALIBRATIONPLUGINACTIVATOR_H_

#include <ctkPluginActivator.h>
#include "boost/shared_ptr.hpp"

namespace cx
{

/**
 * \defgroup org_custusx_calibration
 * \ingroup cx_plugins
 *
 * \see cx::CalibrationGUIExtenderService.
 *
 */


typedef boost::shared_ptr<class CalibrationGUIExtenderService> CalibrationGUIExtenderServicePtr;
typedef boost::shared_ptr<class RegisteredService> RegisteredServicePtr;

/**
 * Activator for the calibration plugin
 *
 * \ingroup org_custusx_calibration
 *
 * \date 2015-02-24
 * \author Christian Askeland
 */
class CalibrationPluginActivator :  public QObject, public ctkPluginActivator
{
	Q_OBJECT
	Q_INTERFACES(ctkPluginActivator)
	Q_PLUGIN_METADATA(IID "org_custusx_calibration")

public:

	CalibrationPluginActivator();
	~CalibrationPluginActivator();

	void start(ctkPluginContext* context);
	void stop(ctkPluginContext* context);

private:
	RegisteredServicePtr mGUIExtender;
};

} // namespace cx

#endif /* CXCALIBRATIONPLUGINACTIVATOR_H_ */

/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXCALIBRATIONGUIEXTENDERSERVICE_H_
#define CXCALIBRATIONGUIEXTENDERSERVICE_H_

#include "cxGUIExtenderService.h"
#include "org_custusx_calibration_Export.h"
class ctkPluginContext;

namespace cx
{

/**
 * Implementation of Calibration service.
 *
 * \ingroup org_custusx_calibration
 *
 * \date 2015-02-24
 * \author Christian Askeland
 */
class org_custusx_calibration_EXPORT CalibrationGUIExtenderService : public GUIExtenderService
{
	Q_INTERFACES(cx::GUIExtenderService)
public:
	CalibrationGUIExtenderService(ctkPluginContext *context);
	virtual ~CalibrationGUIExtenderService() {};

	std::vector<CategorizedWidget> createWidgets() const;

private:
  ctkPluginContext* mContext;

};
typedef boost::shared_ptr<CalibrationGUIExtenderService> CalibrationGUIExtenderServicePtr;

} /* namespace cx */

#endif /* CXCALIBRATIONGUIEXTENDERSERVICE_H_ */


/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXRMPCSERVICE_H
#define CXRMPCSERVICE_H

#include "cxRegistrationMethodService.h"
#include "org_custusx_registration_method_pointcloud_Export.h"

namespace cx
{

/**
 * Registration method: Point cloud to surface.
 *
 * \ingroup org_custusx_registration_method_pointcloud
 *
 * \date 2015-09-06
 * \author Christian Askeland, SINTEF
 */
class org_custusx_registration_method_pointcloud_EXPORT RMPCImageToPatientService : public RegistrationMethodService
{
	Q_INTERFACES(cx::RegistrationMethodService)
public:
	RMPCImageToPatientService(ctkPluginContext* context) :
		RegistrationMethodService(RegServices::create(context)) {}
	virtual ~RMPCImageToPatientService() {}
	virtual QString getRegistrationType() {return QString("ImageToPatient");}
	virtual QString getRegistrationMethod() {return QString("PointCloud");}
	virtual QString getWidgetName() {return QString("org_custusx_registration_method_pointcloud_widget");}
	virtual QWidget* createWidget();
};

} /* namespace cx */

#endif // CXRMPCSERVICE_H

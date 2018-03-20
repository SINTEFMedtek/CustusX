/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXRMPCFROMPOINTERSERVICE_H
#define CXRMPCFROMPOINTERSERVICE_H

#include "cxRegistrationMethodService.h"
#include "org_custusx_registration_method_pointcloud_Export.h"

namespace cx
{

/**
 * Registration method: Pointer to surface.
 * The pointer generates a point cloud that is registered to the surface using ICP.
 *
 * \ingroup org_custusx_registration_method_pointcloud
 *
 * \date 2015-09-06
 * \author Christian Askeland, SINTEF
 */
class org_custusx_registration_method_pointcloud_EXPORT RMPCFromPointerImageToPatientService : public RegistrationMethodService
{
	Q_INTERFACES(cx::RegistrationMethodService)
public:
	RMPCFromPointerImageToPatientService(ctkPluginContext* context) :
		RegistrationMethodService(RegServices::create(context)) {}
	virtual ~RMPCFromPointerImageToPatientService() {}
	virtual QString getRegistrationType() {return QString("ImageToPatient");}
	virtual QString getRegistrationMethod() {return QString("Contour From Tool");}
	virtual QString getWidgetName() {return QString("org_custusx_registration_method_pointcloud_frompointer_widget");}
	virtual QWidget* createWidget();
};

} /* namespace cx */

#endif // CXRMPCFROMPOINTERSERVICE_H

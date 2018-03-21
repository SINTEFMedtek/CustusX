/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXREGISTRATIONMETHODMANUALIMAGETRANSFORMSERVICE_H
#define CXREGISTRATIONMETHODMANUALIMAGETRANSFORMSERVICE_H

#include "cxRegistrationMethodManualService.h"

namespace cx
{

/**
 * \ingroup org_custusx_registration_method_manual
 */
class org_custusx_registration_method_manual_EXPORT RegistrationMethodManualImageTransformService : public RegistrationMethodManualService
{
	Q_INTERFACES(cx::RegistrationMethodService)
public:
	RegistrationMethodManualImageTransformService(RegServicesPtr services);
	virtual ~RegistrationMethodManualImageTransformService() {}
	virtual QString getRegistrationType() {return QString("ImageTransform");}
	virtual QString getRegistrationMethod() {return QString("Manual Image Transform");}
	virtual QWidget* createWidget();
	virtual QString getWidgetName() {return QString("org_custusx_registration_method_manual_image_transform");}
};
} // cx
#endif // CXREGISTRATIONMETHODMANUALIMAGETRANSFORMSERVICE_H

/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXREGISTRATIONMETHODMANUALIMAGETOPATIENTSERVICE_H
#define CXREGISTRATIONMETHODMANUALIMAGETOPATIENTSERVICE_H

#include "cxRegistrationMethodManualService.h"

namespace cx
{
/**
 * Implementation of registration method manual service: Image to Patient
 *
 * \ingroup org_custusx_registration_method_manual
 *
 * \date Sep 09, 2014
 * \author Ole Vegard Solberg, SINTEF
 */
class org_custusx_registration_method_manual_EXPORT RegistrationMethodManualImageToPatientService : public RegistrationMethodManualService
{
	Q_INTERFACES(cx::RegistrationMethodService)
public:
	RegistrationMethodManualImageToPatientService(RegServicesPtr services);
	virtual ~RegistrationMethodManualImageToPatientService() {}
	virtual QString getRegistrationType() {return QString("ImageToPatient");}
	virtual QString getRegistrationMethod() {return QString("Manual");}
	virtual QWidget* createWidget();
	virtual QString getWidgetName() {return QString("org_custusx_registration_method_manual_image_to_patient");}
};

} //cx

#endif // CXREGISTRATIONMETHODMANUALIMAGETOPATIENTSERVICE_H

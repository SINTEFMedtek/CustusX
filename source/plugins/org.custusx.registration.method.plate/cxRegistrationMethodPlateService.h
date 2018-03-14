/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXREGISTRATIONMETHODPLATESERVICE_H_
#define CXREGISTRATIONMETHODPLATESERVICE_H_

#include "cxRegistrationMethodService.h"
#include "org_custusx_registration_method_plate_Export.h"

namespace cx
{

/**
 * Registration method: Plate image to patient service implementation
 *
 * \ingroup org_custusx_registration_method_plate
 *
 * \date 2014-10-09
 * \author Ole Vegard Solberg, SINTEF
 */
class org_custusx_registration_method_plate_EXPORT RegistrationMethodPlateImageToPatientService : public RegistrationMethodService
{
	Q_INTERFACES(cx::RegistrationMethodService)
public:
	RegistrationMethodPlateImageToPatientService(RegServicesPtr services) :
		RegistrationMethodService(services) {}
	virtual ~RegistrationMethodPlateImageToPatientService() {}
	virtual QString getRegistrationType() {return QString("ImageToPatient");}
	virtual QString getRegistrationMethod() {return QString("Plate Landmark");}
	virtual QString getWidgetName() {return QString("org_custusx_registration_method_plate_widget");}
	virtual QWidget* createWidget();
};

} /* namespace cx */

#endif /* CXREGISTRATIONMETHODPLATESERVICE_H_ */


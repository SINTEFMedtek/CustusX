/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXREGISTRATIONMETHODCENTERLINESERVICE_H_
#define CXREGISTRATIONMETHODCENTERLINESERVICE_H_

#include "cxRegistrationMethodService.h"
#include "org_custusx_registration_method_centerline_Export.h"

namespace cx
{

/**
 * Registration method: Centerline from image to tracking data service implementation
 *
 * \ingroup org_custusx_registration_method_centerline
 *
 * \date 2017-02-16
 * \author Erlend Fagertun Hofstad, SINTEF
 */
class org_custusx_registration_method_centerline_EXPORT RegistrationMethodCenterlineImageToPatientService : public RegistrationMethodService
{
	Q_INTERFACES(cx::RegistrationMethodService)
public:
    RegistrationMethodCenterlineImageToPatientService(RegServicesPtr services) :
		RegistrationMethodService(services) {}
    virtual ~RegistrationMethodCenterlineImageToPatientService() {}
	virtual QString getRegistrationType() {return QString("ImageToPatient");}
    virtual QString getRegistrationMethod() {return QString("Centerline");}
    virtual QString getWidgetName() {return QString("org_custusx_registration_method_centerline_widget");}
	virtual QWidget* createWidget();
};

} /* namespace cx */

#endif /* CXREGISTRATIONMETHODCENTERLINESERVICE_H_ */


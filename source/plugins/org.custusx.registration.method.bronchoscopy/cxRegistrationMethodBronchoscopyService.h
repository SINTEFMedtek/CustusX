/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXREGISTRATIONMETHODBRONCHOSCOPYSERVICE_H_
#define CXREGISTRATIONMETHODBRONCHOSCOPYSERVICE_H_

#include "cxRegistrationMethodService.h"
#include "org_custusx_registration_method_bronchoscopy_Export.h"

namespace cx
{

/**
 * Registration method: Bronchoscopy image to patient service implementation
 *
 * \ingroup org_custusx_registration_method_bronchoscopy
 *
 * \date 2014-10-07
 * \author Ole Vegard Solberg, SINTEF
 */
class org_custusx_registration_method_bronchoscopy_EXPORT RegistrationMethodBronchoscopyImageToPatientService : public RegistrationMethodService
{
	Q_INTERFACES(cx::RegistrationMethodService)
public:
	RegistrationMethodBronchoscopyImageToPatientService(RegServicesPtr services) :
		RegistrationMethodService(services) {}
	virtual ~RegistrationMethodBronchoscopyImageToPatientService() {}
	virtual QString getRegistrationType() {return QString("ImageToPatient");}
	virtual QString getRegistrationMethod() {return QString("Bronchoscopy");}
	virtual QString getWidgetName() {return QString("org_custusx_registration_method_bronchoscopy_widget");}
	virtual QWidget* createWidget();
};

/**
 * Registration method: Bronchoscopy image to image service implementation
 *
 * \ingroup org_custusx_registration_method_bronchoscopy
 *
 * \date 2017-MAR-15
 * \author Erlend F Hofstad, SINTEF
 */
class org_custusx_registration_method_bronchoscopy_EXPORT RegistrationMethodBronchoscopyImageToImageService : public RegistrationMethodService
{
    Q_INTERFACES(cx::RegistrationMethodService)
public:
    RegistrationMethodBronchoscopyImageToImageService(RegServicesPtr services) :
        RegistrationMethodService(services) {}
    virtual ~RegistrationMethodBronchoscopyImageToImageService() {}
    virtual QString getRegistrationType() {return QString("ImageToImage");}
    virtual QString getRegistrationMethod() {return QString("Bronchoscopy");}
    virtual QString getWidgetName() {return QString("org_custusx_registration_method_bronchoscopy_image2image_widget");}
    virtual QWidget* createWidget();
};

} /* namespace cx */

#endif /* CXREGISTRATIONMETHODBRONCHOSCOPYSERVICE_H_ */


/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXREGISTRATIONMETHODLANDMARKSERVICE_H_
#define CXREGISTRATIONMETHODLANDMARKSERVICE_H_

#include "cxRegistrationMethodService.h"
#include "org_custusx_registration_method_landmark_Export.h"

namespace cx
{

/**
 * Registration method: Landmark image to image service implementation
 *
 * \ingroup org_custusx_registration_method_landmark
 *
 * \date 2014-10-01
 * \author Ole Vegard Solberg, SINTEF
 */
class org_custusx_registration_method_landmark_EXPORT RegistrationMethodLandmarkImageToImageService : public RegistrationMethodService
{
	Q_INTERFACES(cx::RegistrationMethodService)
public:
	RegistrationMethodLandmarkImageToImageService(RegServicesPtr services) :
		RegistrationMethodService(services) {}
	virtual ~RegistrationMethodLandmarkImageToImageService() {}
	virtual QString getRegistrationType() {return QString("ImageToImage");}
	virtual QString getRegistrationMethod() {return QString("Landmark");}
	virtual QString getWidgetName() {return QString("org_custusx_registration_method_landmark_image_to_image_widget");}
	virtual QWidget* createWidget();
};

/**
 * Registration method: Landmark image to patient service implementation
 *
 * \ingroup org_custusx_registration_method_landmark
 *
 * \date 2014-10-01
 * \author Ole Vegard Solberg, SINTEF
 */
class org_custusx_registration_method_landmark_EXPORT RegistrationMethodLandmarkImageToPatientService : public RegistrationMethodService
{
	Q_INTERFACES(cx::RegistrationMethodService)
public:
	RegistrationMethodLandmarkImageToPatientService(RegServicesPtr services) :
		RegistrationMethodService(services) {}
	virtual ~RegistrationMethodLandmarkImageToPatientService() {}
	virtual QString getRegistrationType() {return QString("ImageToPatient");}
	virtual QString getRegistrationMethod() {return QString("Landmark");}
	virtual QString getWidgetName() {return QString("org_custusx_registration_method_landmark_image_to_patient_widget");}
	virtual QWidget* createWidget();
};

/**
 * Registration method: Fast Landmark image to patient service implementation
 *
 * \ingroup org_custusx_registration_method_landmark
 *
 * \date 2014-10-06
 * \author Ole Vegard Solberg, SINTEF
 */
class org_custusx_registration_method_landmark_EXPORT RegistrationMethodFastLandmarkImageToPatientService : public RegistrationMethodService
{
	Q_INTERFACES(cx::RegistrationMethodService)
public:
	RegistrationMethodFastLandmarkImageToPatientService(RegServicesPtr services) :
		RegistrationMethodService(services) {}
	virtual ~RegistrationMethodFastLandmarkImageToPatientService() {}
	virtual QString getRegistrationType() {return QString("ImageToPatient");}
	virtual QString getRegistrationMethod() {return QString("Fast");}
	virtual QString getWidgetName() {return QString("org_custusx_registration_method_fast_landmark_image_to_patient_widget");}
	virtual QWidget* createWidget();
};

} /* namespace cx */

#endif /* CXREGISTRATIONMETHODLANDMARKSERVICE_H_ */


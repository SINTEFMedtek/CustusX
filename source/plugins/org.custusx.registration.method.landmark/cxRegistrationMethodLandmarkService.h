/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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


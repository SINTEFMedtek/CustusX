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
#include "cxRegistrationProperties.h"
#include "cxTypeConversions.h"
#include "cxRegistrationService.h"
#include "cxPatientModelService.h"
#include "cxData.h"

namespace cx
{


StringPropertyRegistrationFixedImage::StringPropertyRegistrationFixedImage(RegistrationServicePtr registrationService, PatientModelServicePtr patientModelService) :
	SelectDataStringPropertyBase(patientModelService),
	mRegistrationService(registrationService),
	mPatientModelService(patientModelService)
{
  mValueName = "Fixed Data";
  mHelp = "Select the fixed registration data";
	connect(mRegistrationService.get(), SIGNAL(fixedDataChanged(QString)), this, SIGNAL(changed()));
}

bool StringPropertyRegistrationFixedImage::setValue(const QString& value)
{
	DataPtr newImage = mPatientModelService->getData(value);
	if (newImage == mRegistrationService->getFixedData())
    return false;
	mRegistrationService->setFixedData(newImage);
  return true;
}
QString StringPropertyRegistrationFixedImage::getValue() const
{
	DataPtr image = mRegistrationService->getFixedData();
  if (!image)
    return "";
  return qstring_cast(image->getUid());
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

StringPropertyRegistrationMovingImage::StringPropertyRegistrationMovingImage(RegistrationServicePtr registrationService, PatientModelServicePtr patientModelService) :
	SelectDataStringPropertyBase(patientModelService),
	mRegistrationService(registrationService),
	mPatientModelService(patientModelService)
{
	mValueName = "Moving Data";
	mHelp = "Select the moving registration data";
	connect(mRegistrationService.get(), SIGNAL(movingDataChanged(QString)), this, SIGNAL(changed()));
}

bool StringPropertyRegistrationMovingImage::setValue(const QString& value)
{
	DataPtr newImage = mPatientModelService->getData(value);
	if (newImage == mRegistrationService->getMovingData())
    return false;
	mRegistrationService->setMovingData(newImage);
  return true;
}

QString StringPropertyRegistrationMovingImage::getValue() const
{
	DataPtr image = mRegistrationService->getMovingData();
  if (!image)
    return "";
  return qstring_cast(image->getUid());
}

}

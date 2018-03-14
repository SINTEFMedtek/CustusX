/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

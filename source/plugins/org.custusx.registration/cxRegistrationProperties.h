/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXREGISTRATIONPROPERTIES_H_
#define CXREGISTRATIONPROPERTIES_H_

#include "cxSelectDataStringPropertyBase.h"
#include "org_custusx_registration_Export.h"

namespace cx
{
/**
 * \file
 * \addtogroup org_custusx_registration
 * @{
 */

typedef boost::shared_ptr<class RegistrationService> RegistrationServicePtr;
typedef boost::shared_ptr<class PatientModelService> PatientModelServicePtr;

typedef boost::shared_ptr<class StringPropertyRegistrationFixedImage> StringPropertyRegistrationFixedImagePtr;
/** Adapter that connects to the fixed image in the registration manager.
 */
class org_custusx_registration_EXPORT StringPropertyRegistrationFixedImage : public SelectDataStringPropertyBase
{
  Q_OBJECT
public:
	StringPropertyRegistrationFixedImage(RegistrationServicePtr registrationService, PatientModelServicePtr patientModelService);
  virtual ~StringPropertyRegistrationFixedImage() {}

public: // basic methods
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

private:
	RegistrationServicePtr mRegistrationService;
	PatientModelServicePtr mPatientModelService;
};


typedef boost::shared_ptr<class StringPropertyRegistrationMovingImage> StringPropertyRegistrationMovingImagePtr;
/** Adapter that connects to the fixed image in the registration manager.
 */
class org_custusx_registration_EXPORT StringPropertyRegistrationMovingImage : public SelectDataStringPropertyBase
{
  Q_OBJECT
public:
	StringPropertyRegistrationMovingImage(RegistrationServicePtr registrationService, PatientModelServicePtr patientModelService);
  virtual ~StringPropertyRegistrationMovingImage() {}

public: // basic methods
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

private:
	RegistrationServicePtr mRegistrationService;
	PatientModelServicePtr mPatientModelService;
};

/**
 * @}
 */
}

#endif /* CXREGISTRATIONPROPERTIES_H_ */

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

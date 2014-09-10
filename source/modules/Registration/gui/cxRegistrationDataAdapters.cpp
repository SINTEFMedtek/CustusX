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
#include "cxRegistrationDataAdapters.h"
#include <ctkPluginContext.h>
#include "cxTypeConversions.h"
#include "cxRegistrationServiceProxy.h"
#include "cxPatientModelServiceProxy.h"
#include "cxData.h"

namespace cx
{


RegistrationFixedImageStringDataAdapter::RegistrationFixedImageStringDataAdapter(ctkPluginContext *pluginContext) :
	mRegistrationService(new cx::RegistrationServiceProxy(pluginContext)),
	mPatientModelService(new cx::PatientModelServiceProxy(pluginContext))
{
  mValueName = "Fixed Volume";
  mHelp = "Select the fixed registration data";
	connect(mRegistrationService.get(), SIGNAL(fixedDataChanged(QString)), this, SIGNAL(changed()));
}

bool RegistrationFixedImageStringDataAdapter::setValue(const QString& value)
{
	DataPtr newImage = mPatientModelService->getData(value);
	if (newImage == mRegistrationService->getFixedData())
    return false;
	mRegistrationService->setFixedData(newImage);
  return true;
}
QString RegistrationFixedImageStringDataAdapter::getValue() const
{
	DataPtr image = mRegistrationService->getFixedData();
  if (!image)
    return "";
  return qstring_cast(image->getUid());
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

RegistrationMovingImageStringDataAdapter::RegistrationMovingImageStringDataAdapter(ctkPluginContext *pluginContext) :
	mRegistrationService(new cx::RegistrationServiceProxy(pluginContext)),
	mPatientModelService(new cx::PatientModelServiceProxy(pluginContext))
{
	mValueName = "Moving Volume";
	mHelp = "Select the moving registration data";
	connect(mRegistrationService.get(), SIGNAL(movingDataChanged(QString)), this, SIGNAL(changed()));
}

bool RegistrationMovingImageStringDataAdapter::setValue(const QString& value)
{
	DataPtr newImage = mPatientModelService->getData(value);
	if (newImage == mRegistrationService->getMovingData())
    return false;
	mRegistrationService->setMovingData(newImage);
  return true;
}

QString RegistrationMovingImageStringDataAdapter::getValue() const
{
	DataPtr image = mRegistrationService->getMovingData();
  if (!image)
    return "";
  return qstring_cast(image->getUid());
}

}

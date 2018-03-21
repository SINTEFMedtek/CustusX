/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxRegistrationService.h"
#include "cxRegistrationServiceNull.h"
#include "cxNullDeleter.h"

#include "cxData.h"

namespace cx
{
RegistrationServicePtr RegistrationService::getNullObject()
{
	static RegistrationServicePtr mNull;
	if (!mNull)
		mNull.reset(new RegistrationServiceNull, null_deleter());
	return mNull;
}

QString RegistrationService::getFixedDataUid()
{
	if (!this->getFixedData())
		return "";
	else
		return this->getFixedData()->getUid();
}

QString RegistrationService::getMovingDataUid()
{
	if(!this->getMovingData())
		return "";
	else
		return this->getMovingData()->getUid();
}
}

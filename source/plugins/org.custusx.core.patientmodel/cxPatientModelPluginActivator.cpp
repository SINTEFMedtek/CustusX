/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxPatientModelPluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxPatientModelImplService.h"
#include "cxSessionStorageServiceImpl.h"
#include "cxRegisteredService.h"

namespace cx
{

PatientModelPluginActivator::PatientModelPluginActivator()
{
}

PatientModelPluginActivator::~PatientModelPluginActivator()
{
}

void PatientModelPluginActivator::start(ctkPluginContext* context)
{
	mSessionStorage = RegisteredService::create<SessionStorageServiceImpl>(context, SessionStorageService_iid);
	mPatientModel = RegisteredService::create<PatientModelImplService>(context, PatientModelService_iid);
}

void PatientModelPluginActivator::stop(ctkPluginContext* context)
{
	mPatientModel.reset();
	mSessionStorage.reset();
	Q_UNUSED(context);
}

} // namespace cx




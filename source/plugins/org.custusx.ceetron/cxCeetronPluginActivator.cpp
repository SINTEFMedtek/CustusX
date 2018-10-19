/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxCeetronPluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxRegisteredService.h"
#include "cxCgeoReaderWriter.h"
#include "cxPatientModelServiceProxy.h"

namespace cx
{

CeetronPluginActivator::CeetronPluginActivator()
{
}

CeetronPluginActivator::~CeetronPluginActivator()
{}

void CeetronPluginActivator::start(ctkPluginContext* context)
{
	PatientModelServicePtr patientModelService = PatientModelServiceProxy::create(context);
	mRegistration = RegisteredService::create<CgeoReaderWriter>(context, new CgeoReaderWriter(patientModelService), FileReaderWriterService_iid);
}

void CeetronPluginActivator::stop(ctkPluginContext* context)
{
	mRegistration.reset();
	Q_UNUSED(context);
}

} // namespace cx




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

#include "cxPatientModelServiceProxy.h"

#include <boost/bind.hpp>
#include <ctkPluginContext.h>
#include "cxNullDeleter.h"
#include "cxReporter.h"

namespace cx
{

PatientModelServiceProxy::PatientModelServiceProxy(ctkPluginContext *context) :
	mPluginContext(context),
	mPatientModelService(PatientModelService::getNullObject())
{
	this->initServiceListener();
}

void PatientModelServiceProxy::initServiceListener()
{
	mServiceListener.reset(new ServiceTrackerListener<PatientModelService>(
								 mPluginContext,
								 boost::bind(&PatientModelServiceProxy::onServiceAdded, this, _1),
								 boost::function<void (PatientModelService*)>(),
								 boost::bind(&PatientModelServiceProxy::onServiceRemoved, this, _1)
								 ));
	mServiceListener->open();
}
void PatientModelServiceProxy::onServiceAdded(PatientModelService* service)
{
	mPatientModelService.reset(service, null_deleter());
	if(mPatientModelService->isNull())
		reportWarning("ManualImageRegistrationWidget::onServiceAdded mRegistrationService->isNull()");
}

void PatientModelServiceProxy::onServiceRemoved(PatientModelService *service)
{
	mPatientModelService = PatientModelService::getNullObject();
}

void PatientModelServiceProxy::insertData(DataPtr data)
{
	mPatientModelService->insertData(data);
}

void PatientModelServiceProxy::updateRegistration_rMpr(const QDateTime& oldTime, const RegistrationTransform& newTransform)
{
	mPatientModelService->updateRegistration_rMpr(oldTime, newTransform);
}

std::map<QString, DataPtr> PatientModelServiceProxy::getData() const
{
	return mPatientModelService->getData();
}

DataPtr PatientModelServiceProxy::getData(const QString& uid) const
{
	return mPatientModelService->getData(uid);
}

void PatientModelServiceProxy::autoSave()
{
	mPatientModelService->autoSave();
}

bool PatientModelServiceProxy::isNull()
{
	return mPatientModelService->isNull();
}

} //cx

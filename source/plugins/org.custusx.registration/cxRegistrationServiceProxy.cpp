/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxRegistrationServiceProxy.h"

#include <boost/bind.hpp>
#include <ctkPluginContext.h>
#include "cxNullDeleter.h"
#include "cxLogger.h"
#include <QDateTime>

namespace cx
{

RegistrationServiceProxy::RegistrationServiceProxy(ctkPluginContext *context) :
	mPluginContext(context),
	mRegistrationService(RegistrationService::getNullObject())
{
	this->initServiceListener();
}

void RegistrationServiceProxy::initServiceListener()
{
	mServiceListener.reset(new ServiceTrackerListener<RegistrationService>(
								 mPluginContext,
								 boost::bind(&RegistrationServiceProxy::onServiceAdded, this, _1),
								 boost::function<void (RegistrationService*)>(),
								 boost::bind(&RegistrationServiceProxy::onServiceRemoved, this, _1)
								 ));
	mServiceListener->open();
}
void RegistrationServiceProxy::onServiceAdded(RegistrationService* service)
{
	mRegistrationService.reset(service, null_deleter());
	connect(mRegistrationService.get(), SIGNAL(fixedDataChanged(QString)), this, SIGNAL(fixedDataChanged(QString)));
	connect(mRegistrationService.get(), SIGNAL(movingDataChanged(QString)), this, SIGNAL(movingDataChanged(QString)));
	if(mRegistrationService->isNull())
		reportWarning("RegistrationServiceProxy::onServiceAdded mRegistrationService->isNull()");

	emit fixedDataChanged(mRegistrationService->getFixedDataUid());
	emit movingDataChanged(mRegistrationService->getMovingDataUid());
}

void RegistrationServiceProxy::onServiceRemoved(RegistrationService *service)
{
	disconnect(service, SIGNAL(fixedDataChanged(QString)), this, SIGNAL(fixedDataChanged(QString)));
	disconnect(service, SIGNAL(movingDataChanged(QString)), this, SIGNAL(movingDataChanged(QString)));
	mRegistrationService = RegistrationService::getNullObject();

	emit fixedDataChanged("");
	emit movingDataChanged("");
}

void RegistrationServiceProxy::setMovingData(DataPtr data)
{
	mRegistrationService->setMovingData(data);
}


void RegistrationServiceProxy::setFixedData(DataPtr data)
{
	mRegistrationService->setFixedData(data);
}

DataPtr RegistrationServiceProxy::getMovingData()
{
	return mRegistrationService->getMovingData();
}

DataPtr RegistrationServiceProxy::getFixedData()
{
	return mRegistrationService->getFixedData();
}

void RegistrationServiceProxy::doPatientRegistration()
{
	return mRegistrationService->doPatientRegistration();
}

void RegistrationServiceProxy::doFastRegistration_Translation()
{
	return mRegistrationService->doFastRegistration_Translation();
}

void RegistrationServiceProxy::doFastRegistration_Orientation(const Transform3D &tMtm, const Transform3D &prMt)
{
	return mRegistrationService->doFastRegistration_Orientation(tMtm, prMt);
}

void RegistrationServiceProxy::doImageRegistration(bool translationOnly)
{
	return mRegistrationService->doImageRegistration(translationOnly);
}

void RegistrationServiceProxy::addImage2ImageRegistration(Transform3D delta_pre_rMd, QString description)
{
	mRegistrationService->addImage2ImageRegistration(delta_pre_rMd, description);
}

void RegistrationServiceProxy::updateImage2ImageRegistration(Transform3D delta_pre_rMd, QString description)
{
	mRegistrationService->updateImage2ImageRegistration(delta_pre_rMd, description);
}

void RegistrationServiceProxy::addPatientRegistration(Transform3D rMpr_new, QString description)
{
	mRegistrationService->addPatientRegistration( rMpr_new, description);
}

void RegistrationServiceProxy::updatePatientRegistration(Transform3D rMpr_new, QString description)
{
	mRegistrationService->updatePatientRegistration( rMpr_new, description);
}

void RegistrationServiceProxy::applyPatientOrientation(const Transform3D &tMtm, const Transform3D &prMt)
{
	mRegistrationService->applyPatientOrientation(tMtm, prMt);
}

QDateTime RegistrationServiceProxy::getLastRegistrationTime()
{
	return mRegistrationService->getLastRegistrationTime();
}

void RegistrationServiceProxy::setLastRegistrationTime(QDateTime time)
{
	mRegistrationService->setLastRegistrationTime(time);
}

bool RegistrationServiceProxy::isNull()
{
	return mRegistrationService->isNull();
}

} //cx

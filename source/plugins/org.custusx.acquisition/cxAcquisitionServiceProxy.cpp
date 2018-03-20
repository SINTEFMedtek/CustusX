/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxAcquisitionServiceProxy.h"

#include <boost/bind.hpp>
#include <ctkPluginContext.h>
#include "cxNullDeleter.h"
#include "cxLogger.h"

namespace cx
{

//AcquisitionServicePtr AcquisitionServiceProxy::create(ctkPluginContext *pluginContext)
//{
//	return AcquisitionServicePtr(new AcquisitionServiceProxy(pluginContext));
//}

AcquisitionServiceProxy::AcquisitionServiceProxy(ctkPluginContext *context) :
	mPluginContext(context),
	mAcquisitionService(AcquisitionService::getNullObject())
{
	this->initServiceListener();
}

void AcquisitionServiceProxy::initServiceListener()
{
	mServiceListener.reset(new ServiceTrackerListener<AcquisitionService>(
								 mPluginContext,
								 boost::bind(&AcquisitionServiceProxy::onServiceAdded, this, _1),
								 boost::function<void (AcquisitionService*)>(),
								 boost::bind(&AcquisitionServiceProxy::onServiceRemoved, this, _1)
								 ));
	mServiceListener->open();
}
void AcquisitionServiceProxy::onServiceAdded(AcquisitionService* service)
{
	mAcquisitionService.reset(service, null_deleter());

	connect(service, &AcquisitionService::started, this, &AcquisitionService::started);
	connect(service, &AcquisitionService::cancelled, this, &AcquisitionService::cancelled);
	connect(service, &AcquisitionService::stateChanged, this, &AcquisitionService::stateChanged);
	connect(service, &AcquisitionService::usReadinessChanged, this, &AcquisitionService::usReadinessChanged);
	connect(service, &AcquisitionService::acquisitionStopped, this, &AcquisitionService::acquisitionStopped);
	connect(service, &AcquisitionService::recordedSessionsChanged, this, &AcquisitionService::recordedSessionsChanged);

	connect(service, &AcquisitionService::acquisitionDataReady, this, &AcquisitionService::acquisitionDataReady);
	connect(service, &AcquisitionService::saveDataCompleted, this, &AcquisitionService::saveDataCompleted);

	if(mAcquisitionService->isNull())
		reportWarning("AcquisitionServiceProxy::onServiceAdded mAcquisitionService->isNull()");

	emit stateChanged();
	emit usReadinessChanged();
	emit recordedSessionsChanged();
}

void AcquisitionServiceProxy::onServiceRemoved(AcquisitionService *service)
{
	disconnect(service, &AcquisitionService::started, this, &AcquisitionService::started);
	disconnect(service, &AcquisitionService::cancelled, this, &AcquisitionService::cancelled);
	disconnect(service, &AcquisitionService::stateChanged, this, &AcquisitionService::stateChanged);
	disconnect(service, &AcquisitionService::usReadinessChanged, this, &AcquisitionService::usReadinessChanged);
	disconnect(service, &AcquisitionService::acquisitionStopped, this, &AcquisitionService::acquisitionStopped);
	disconnect(service, &AcquisitionService::recordedSessionsChanged, this, &AcquisitionService::recordedSessionsChanged);

	disconnect(service, &AcquisitionService::acquisitionDataReady, this, &AcquisitionService::acquisitionDataReady);
	disconnect(service, &AcquisitionService::saveDataCompleted, this, &AcquisitionService::saveDataCompleted);

	mAcquisitionService = AcquisitionService::getNullObject();

	emit stateChanged();
	emit usReadinessChanged();
	emit recordedSessionsChanged();
}

bool AcquisitionServiceProxy::isNull()
{
	return mAcquisitionService->isNull();
}

RecordSessionPtr AcquisitionServiceProxy::getLatestSession()
{
	return mAcquisitionService->getLatestSession();
}

std::vector<RecordSessionPtr> AcquisitionServiceProxy::getSessions()
{
	return mAcquisitionService->getSessions();
}

bool AcquisitionServiceProxy::isReady(TYPES context) const
{
	return mAcquisitionService->isReady(context);
}

QString AcquisitionServiceProxy::getInfoText(TYPES context) const
{
	return mAcquisitionService->getInfoText(context);
}

AcquisitionService::STATE AcquisitionServiceProxy::getState() const
{
	return mAcquisitionService->getState();
}

void AcquisitionServiceProxy::startRecord(TYPES context, QString category, RecordSessionPtr session)
{
	mAcquisitionService->startRecord(context, category, session);
}

void AcquisitionServiceProxy::stopRecord()
{
	mAcquisitionService->stopRecord();
}

void AcquisitionServiceProxy::cancelRecord()
{
	mAcquisitionService->cancelRecord();
}

void AcquisitionServiceProxy::startPostProcessing()
{
	mAcquisitionService->startPostProcessing();
}

void AcquisitionServiceProxy::stopPostProcessing()
{
	mAcquisitionService->stopPostProcessing();
}

int AcquisitionServiceProxy::getNumberOfSavingThreads() const
{
	return mAcquisitionService->getNumberOfSavingThreads();
}

} //cx

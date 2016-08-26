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
#include <QDomElement>
#include <ctkPluginContext.h>
#include "cxNullDeleter.h"
#include "cxLogger.h"
#include "cxLandmark.h"
#include "cxData.h"

namespace cx
{
PatientModelServicePtr PatientModelServiceProxy::create(ctkPluginContext *pluginContext)
{
	return PatientModelServicePtr(new PatientModelServiceProxy(pluginContext));
}

PatientModelServiceProxy::PatientModelServiceProxy(ctkPluginContext *context) :
	mPluginContext(context),
	mPatientModelService(PatientModelService::getNullObject())
{
	this->initServiceListener();
}

PatientModelServiceProxy::~PatientModelServiceProxy()
{
//	mServiceListener.reset();//Needed?
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

	connect(service, &PatientModelService::centerChanged, this, &PatientModelService::centerChanged);
    connect(service, &PatientModelService::operatingTableChanged, this, &PatientModelService::operatingTableChanged);
	connect(service, &PatientModelService::dataAddedOrRemoved, this, &PatientModelService::dataAddedOrRemoved);
	connect(service, &PatientModelService::landmarkPropertiesChanged, this, &PatientModelService::landmarkPropertiesChanged);
	connect(service, &PatientModelService::clinicalApplicationChanged, this, &PatientModelService::clinicalApplicationChanged);
	connect(service, &PatientModelService::rMprChanged, this, &PatientModelService::rMprChanged);
	connect(service, &PatientModelService::streamLoaded, this, &PatientModelService::streamLoaded);
	connect(service, &PatientModelService::patientChanged, this, &PatientModelService::patientChanged);
	connect(service, &PatientModelService::videoAddedToTrackedStream, this, &PatientModelService::videoAddedToTrackedStream);

	if(mPatientModelService->isNull())
		reportWarning("PatientModelServiceProxy::onServiceAdded mPatientModelService->isNull()");

	emit dataAddedOrRemoved();
	emit landmarkPropertiesChanged();
	emit clinicalApplicationChanged();
	emit rMprChanged();
	emit patientChanged();
}

void PatientModelServiceProxy::onServiceRemoved(PatientModelService *service)
{
	disconnect(service, &PatientModelService::centerChanged, this, &PatientModelService::centerChanged);
    disconnect(service, &PatientModelService::operatingTableChanged, this, &PatientModelService::operatingTableChanged);
	disconnect(service, &PatientModelService::dataAddedOrRemoved, this, &PatientModelService::dataAddedOrRemoved);
	disconnect(service, &PatientModelService::landmarkPropertiesChanged, this, &PatientModelService::landmarkPropertiesChanged);
	disconnect(service, &PatientModelService::clinicalApplicationChanged, this, &PatientModelService::clinicalApplicationChanged);
	disconnect(service, &PatientModelService::rMprChanged, this, &PatientModelService::rMprChanged);
	disconnect(service, &PatientModelService::streamLoaded, this, &PatientModelService::streamLoaded);
	disconnect(service, &PatientModelService::patientChanged, this, &PatientModelService::patientChanged);
	disconnect(service, &PatientModelService::videoAddedToTrackedStream, this, &PatientModelService::videoAddedToTrackedStream);

	mPatientModelService = PatientModelService::getNullObject();

	emit dataAddedOrRemoved();
	emit landmarkPropertiesChanged();
	emit clinicalApplicationChanged();
	emit rMprChanged();
	emit patientChanged();
}

void PatientModelServiceProxy::insertData(DataPtr data)
{
	mPatientModelService->insertData(data);
}

DataPtr PatientModelServiceProxy::createData(QString type, QString uid, QString name)
{
	return mPatientModelService->createData(type, uid, name);
}

std::map<QString, DataPtr> PatientModelServiceProxy::getDatas(DataFilter filter) const
{
	return mPatientModelService->getDatas(filter);
}

DataPtr PatientModelServiceProxy::getData(const QString& uid) const
{
	return mPatientModelService->getData(uid);
}

LandmarksPtr PatientModelServiceProxy::getPatientLandmarks() const
{
	return mPatientModelService->getPatientLandmarks();
}

std::map<QString, LandmarkProperty> PatientModelServiceProxy::getLandmarkProperties() const
{
	return mPatientModelService->getLandmarkProperties();
}

void PatientModelServiceProxy::setLandmarkName(QString uid, QString name)
{
	mPatientModelService->setLandmarkName(uid, name);
}

void PatientModelServiceProxy::autoSave()
{
	mPatientModelService->autoSave();
}

bool PatientModelServiceProxy::isNull()
{
	return mPatientModelService->isNull();
}

void PatientModelServiceProxy::makeAvailable(const QString &uid, bool available)
{
	mPatientModelService->makeAvailable(uid, available);
}

std::map<QString, VideoSourcePtr> PatientModelServiceProxy::getStreams() const
{
	return mPatientModelService->getStreams();
}

QString PatientModelServiceProxy::getActivePatientFolder() const
{
	return mPatientModelService->getActivePatientFolder();
}

bool PatientModelServiceProxy::isPatientValid() const
{
	return mPatientModelService->isPatientValid();
}

DataPtr PatientModelServiceProxy::importData(QString fileName, QString &infoText)
{
	return mPatientModelService->importData(fileName, infoText);
}

void PatientModelServiceProxy::exportPatient(PATIENT_COORDINATE_SYSTEM externalSpace)
{
	return mPatientModelService->exportPatient(externalSpace);
}

void PatientModelServiceProxy::removeData(QString uid)
{
	return mPatientModelService->removeData(uid);
}

PresetTransferFunctions3DPtr PatientModelServiceProxy::getPresetTransferFunctions3D() const
{
	return mPatientModelService->getPresetTransferFunctions3D();
}

void PatientModelServiceProxy::setCenter(const Vector3D &center)
{
	mPatientModelService->setCenter(center);
}

Vector3D PatientModelServiceProxy::getCenter() const
{
    return mPatientModelService->getCenter();
}

void PatientModelServiceProxy::setOperatingTable(const OperatingTable &ot)
{
    mPatientModelService->setOperatingTable(ot);
}

OperatingTable PatientModelServiceProxy::getOperatingTable() const
{
    return mPatientModelService->getOperatingTable();
}

QString PatientModelServiceProxy::addLandmark()
{
	return mPatientModelService->addLandmark();
}

void PatientModelServiceProxy::setLandmarkActive(QString uid, bool active)
{
	mPatientModelService->setLandmarkActive(uid, active);
}

RegistrationHistoryPtr PatientModelServiceProxy::get_rMpr_History() const
{
	return mPatientModelService->get_rMpr_History();
}

ActiveDataPtr PatientModelServiceProxy::getActiveData() const
{
	return mPatientModelService->getActiveData();
}

CLINICAL_VIEW PatientModelServiceProxy::getClinicalApplication() const
{
	return mPatientModelService->getClinicalApplication();
}

void PatientModelServiceProxy::setClinicalApplication(CLINICAL_VIEW application)
{
	mPatientModelService->setClinicalApplication(application);
}

} //cx

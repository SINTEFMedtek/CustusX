/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

void PatientModelServiceProxy::insertData(DataPtr data, bool overWrite)
{
	mPatientModelService->insertData(data, overWrite);
}

DataPtr PatientModelServiceProxy::createData(QString type, QString uid, QString name)
{
	return mPatientModelService->createData(type, uid, name);
}

std::map<QString, DataPtr> PatientModelServiceProxy::getDatas(DataFilter filter) const
{
	return mPatientModelService->getDatas(filter);
}

std::map<QString, DataPtr> PatientModelServiceProxy::getChildren(QString parent_uid, QString of_type) const
{
	return mPatientModelService->getChildren(parent_uid, of_type);
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

void PatientModelServiceProxy::deleteLandmarks()
{
	return mPatientModelService->deleteLandmarks();
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

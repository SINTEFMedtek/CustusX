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

#include "cxPatientModelImplService.h"

#include <ctkPluginContext.h>
#include "cxData.h"

#include "cxPatientData.h"
#include "cxRegistrationTransform.h"
#include "cxDataFactory.h"
#include "cxDataManagerImpl.h"
#include "cxSharedPointerChecker.h"
#include "cxTrackingServiceProxy.h"
#include "cxPatientModelServiceProxy.h"
#include "cxSpaceProviderImpl.h"
#include "cxSessionStorageServiceProxy.h"

namespace cx
{

PatientModelImplService::PatientModelImplService(ctkPluginContext *context) :
	mContext(context )
{
	this->createInterconnectedDataAndSpace();

	connect(this->dataService().get(), &DataManager::dataAddedOrRemoved, this, &PatientModelService::dataAddedOrRemoved);
	connect(this->dataService().get(), &DataManager::activeImageChanged, this, &PatientModelService::activeImageChanged);
	connect(this->dataService().get(), &DataManager::debugModeChanged, this, &PatientModelService::debugModeChanged);
	connect(this->dataService().get(), &DataManager::rMprChanged, this, &PatientModelService::rMprChanged);
	connect(this->dataService().get(), &DataManager::streamLoaded, this, &PatientModelService::streamLoaded);
	connect(this->dataService().get(), &DataManager::clinicalApplicationChanged, this, &PatientModelService::clinicalApplicationChanged);

	connect(this->dataService().get(), &DataManager::centerChanged, this, &PatientModelService::centerChanged);
	connect(this->dataService().get(), &DataManager::landmarkPropertiesChanged, this, &PatientModelService::landmarkPropertiesChanged);

	connect(this->patientData().get(), &PatientData::patientChanged, this, &PatientModelService::patientChanged);
}

void PatientModelImplService::createInterconnectedDataAndSpace()
{
	// prerequisites:
	TrackingServicePtr trackingService = TrackingServiceProxy::create(mContext);

	// build object(s):
	PatientModelServicePtr patientModelService = PatientModelServiceProxy::create(mContext);

	mDataService = DataManagerImpl::create();

	SpaceProviderPtr spaceProvider(new cx::SpaceProviderImpl(trackingService, patientModelService));
	mDataService->setSpaceProvider(spaceProvider);

	mDataFactory.reset(new DataFactory(patientModelService, spaceProvider));
	mDataService->setDataFactory(mDataFactory);

//	mPatientServiceOld = PatientService::create(mDataService);
//	mPatientService = PatientService::create(mDataService);
	SessionStorageServicePtr session = SessionStorageServiceProxy::create(mContext);

	mPatientData.reset(new PatientData(mDataService, session));
}

void PatientModelImplService::shutdownInterconnectedDataAndSpace()
{
	requireUnique(mPatientData, "PatientData");
	mPatientData.reset();

	// [HACK] break loop by removing connection to DataFactory and SpaceProvider
	mDataService->setSpaceProvider(SpaceProviderPtr());
	mDataService->setDataFactory(DataFactoryPtr());
	mDataService->clear();

	requireUnique(mDataFactory, "DataFactory");
	mDataFactory.reset();

	requireUnique(mDataService, "DataService");
	mDataService.reset();


}


PatientModelImplService::~PatientModelImplService()
{
	if(dataService())
	{
		disconnect(this->dataService().get(), &DataManager::dataAddedOrRemoved, this, &PatientModelService::dataAddedOrRemoved);
		disconnect(this->dataService().get(), &DataManager::activeImageChanged, this, &PatientModelService::activeImageChanged);
		disconnect(this->dataService().get(), &DataManager::debugModeChanged, this, &PatientModelService::debugModeChanged);
		disconnect(this->dataService().get(), &DataManager::rMprChanged, this, &PatientModelService::rMprChanged);
		disconnect(this->dataService().get(), &DataManager::streamLoaded, this, &PatientModelService::streamLoaded);
		disconnect(this->dataService().get(), &DataManager::clinicalApplicationChanged, this, &PatientModelService::clinicalApplicationChanged);

		disconnect(this->patientData().get(), &PatientData::patientChanged, this, &PatientModelService::patientChanged);
	}

	this->shutdownInterconnectedDataAndSpace();
}

void PatientModelImplService::insertData(DataPtr data)
{
	QString outputBasePath = this->patientData()->getActivePatientFolder();

	this->dataService()->loadData(data);
	this->dataService()->saveData(data, outputBasePath);
}

DataPtr PatientModelImplService::createData(QString type, QString uid, QString name)
{
	return dataService()->getDataFactory()->create(type, uid, name);
}

std::map<QString, DataPtr> PatientModelImplService::getData() const
{
	return dataService()->getData();
}

DataPtr PatientModelImplService::getData(const QString& uid) const
{
	std::map<QString, DataPtr> dataMap = this->getData();
	std::map<QString, DataPtr>::const_iterator iter = dataMap.find(uid);
	if (iter == dataMap.end())
		return DataPtr();
	return iter->second;
}

LandmarksPtr PatientModelImplService::getPatientLandmarks() const
{
	return dataService()->getPatientLandmarks();
}

std::map<QString, LandmarkProperty> PatientModelImplService::getLandmarkProperties() const
{
	return dataService()->getLandmarkProperties();
}

void PatientModelImplService::setLandmarkName(QString uid, QString name)
{
	dataService()->setLandmarkName(uid, name);
}

Transform3D PatientModelImplService::get_rMpr() const
{
	return dataService()->get_rMpr();
}

void PatientModelImplService::autoSave()
{
	this->patientData()->autoSave();
}

bool PatientModelImplService::isNull()
{
	return false;
}

bool PatientModelImplService::getDebugMode() const
{
	return dataService()->getDebugMode();
}
void PatientModelImplService::setDebugMode(bool on)
{
	dataService()->setDebugMode(on);
}

ImagePtr PatientModelImplService::getActiveImage() const
{
	return dataService()->getActiveImage();
}

void PatientModelImplService::setActiveImage(ImagePtr activeImage)
{
	dataService()->setActiveImage(activeImage);
}

CLINICAL_VIEW PatientModelImplService::getClinicalApplication() const
{
	return dataService()->getClinicalApplication();
}

void PatientModelImplService::setClinicalApplication(CLINICAL_VIEW application)
{
	dataService()->setClinicalApplication(application);
}

cx::ImagePtr PatientModelImplService::createDerivedImage(vtkImageDataPtr data, QString uid, QString name, cx::ImagePtr parentImage, QString filePath)
{
	return dataService()->createDerivedImage(data, uid, name, parentImage, filePath);
}

MeshPtr PatientModelImplService::createMesh(vtkPolyDataPtr data, QString uidBase, QString nameBase, QString filePath)
{
	return dataService()->createMesh(data, uidBase, nameBase, filePath);
}

ImagePtr PatientModelImplService::createImage(vtkImageDataPtr data, QString uidBase, QString nameBase, QString filePath)
{
	return dataService()->createImage(data, uidBase, nameBase, filePath);
}

void PatientModelImplService::loadData(DataPtr data)
{
	dataService()->loadData(data);
}

void PatientModelImplService::saveData(DataPtr data, const QString &basePath)
{
	dataService()->saveData(data, basePath);
}

void PatientModelImplService::saveImage(ImagePtr image, const QString &basePath)
{
	dataService()->saveImage(image, basePath);
}

void PatientModelImplService::saveMesh(MeshPtr mesh, const QString &basePath)
{
	dataService()->saveMesh(mesh, basePath);
}

std::map<QString, VideoSourcePtr> PatientModelImplService::getStreams() const
{
	return dataService()->getStreams();
}

QString PatientModelImplService::getActivePatientFolder() const
{
	return this->patientData()->getActivePatientFolder();
}

bool PatientModelImplService::isPatientValid() const
{
	return this->patientData()->isPatientValid();
}

DataPtr PatientModelImplService::importData(QString fileName, QString &infoText)
{
	return this->patientData()->importData(fileName, infoText);
}

void PatientModelImplService::exportPatient(bool niftiFormat)
{
	this->patientData()->exportPatient(niftiFormat);
}

void PatientModelImplService::removeData(QString uid)
{
	this->patientData()->removeData(uid);
}

PresetTransferFunctions3DPtr PatientModelImplService::getPresetTransferFunctions3D() const
{
	return dataService()->getPresetTransferFunctions3D();
}

void PatientModelImplService::setCenter(const Vector3D &center)
{
	this->dataService()->setCenter(center);
}

Vector3D PatientModelImplService::getCenter() const
{
	return this->dataService()->getCenter();
}


QString PatientModelImplService::addLandmark()
{
	return dataService()->addLandmark();
}

void PatientModelImplService::setLandmarkActive(QString uid, bool active)
{
	dataService()->setLandmarkActive(uid, active);
}

RegistrationHistoryPtr PatientModelImplService::get_rMpr_History() const
{
	return this->dataService()->get_rMpr_History();
}


DataManagerImplPtr PatientModelImplService::dataService() const
{
	return mDataService;
}

PatientDataPtr PatientModelImplService::patientData() const
{
	return mPatientData;
}

} /* namespace cx */

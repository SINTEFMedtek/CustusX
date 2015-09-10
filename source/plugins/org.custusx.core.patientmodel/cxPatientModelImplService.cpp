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
#include <vtkImageData.h>

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
#include "cxTrackedStream.h"
#include "cxReporter.h"
#include "cxVideoSource.h"

namespace cx
{

PatientModelImplService::PatientModelImplService(ctkPluginContext *context) :
	mContext(context )
{
	this->createInterconnectedDataAndSpace();

	connect(this->dataService().get(), &DataManager::dataAddedOrRemoved, this, &PatientModelService::dataAddedOrRemoved);
	connect(this->dataService().get(), &DataManager::activeImageChanged, this, &PatientModelService::activeImageChanged);
	connect(this->dataService().get(), &DataManager::rMprChanged, this, &PatientModelService::rMprChanged);
	connect(this->dataService().get(), &DataManager::streamLoaded, this, &PatientModelService::streamLoaded);
	connect(this->dataService().get(), &DataManager::clinicalApplicationChanged, this, &PatientModelService::clinicalApplicationChanged);

	connect(this->dataService().get(), &DataManager::centerChanged, this, &PatientModelService::centerChanged);
	connect(this->dataService().get(), &DataManager::landmarkPropertiesChanged, this, &PatientModelService::landmarkPropertiesChanged);

	connect(this->patientData().get(), &PatientData::patientChanged, this, &PatientModelService::patientChanged);

	connect(mTrackingService.get(), &TrackingService::newProbe, this, &PatientModelImplService::newProbe);
}

void PatientModelImplService::createInterconnectedDataAndSpace()
{
	// prerequisites:
	mTrackingService = TrackingServiceProxy::create(mContext);

	// build object(s):
	PatientModelServicePtr patientModelService = PatientModelServiceProxy::create(mContext);

	mDataService = DataManagerImpl::create();

	SpaceProviderPtr spaceProvider(new cx::SpaceProviderImpl(mTrackingService, patientModelService));
	mDataService->setSpaceProvider(spaceProvider);

	mDataFactory.reset(new DataFactory(patientModelService, spaceProvider));
	mDataService->setDataFactory(mDataFactory);

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
	data->save(outputBasePath);
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

void PatientModelImplService::loadData(DataPtr data)
{
	dataService()->loadData(data);
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

void PatientModelImplService::exportPatient(PATIENT_COORDINATE_SYSTEM externalSpace)
{
	this->patientData()->exportPatient(externalSpace);
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

void PatientModelImplService::newProbe(const ToolPtr tool)
{
	ProbePtr probe = tool->getProbe();
	if(!probe)
	{
		reportWarning("PatientModelImplService::newProbe: Tool is not a probe");
		return;
	}
	//Move mProbeTools to DataManager?
	mProbeTools[tool->getUid()] = tool;

	connect(probe.get(), &Probe::videoSourceAdded, this, &PatientModelImplService::videoSourceAdded);
}

void PatientModelImplService::videoSourceAdded(VideoSourcePtr source)
{

	ToolPtr tool = this->getProbeTool(source->getUid());
	if(!tool)
		return;

	//Temporary code turning off generation of TrackedStream for video sources that are not 3D
	if (!source || !source->getVtkImageData() || source->getVtkImageData()->GetDataDimension() != 3)
		return;

	QString uid = source->getUid() + tool->getUid();
	QString name = source->getName() + " - " + tool->getName();
	TrackedStreamPtr trackedStream = this->dataService()->getTrackedStream(uid);
	if (!trackedStream)
		trackedStream = this->createSpecificData<TrackedStream>(uid, name);
	trackedStream->setProbeTool(tool);
	trackedStream->setVideoSource(source);
	trackedStream->setSpaceProvider(mDataService->getSpaceProvider());

	//Only load trackedStream, don't save it
	this->dataService()->loadData(trackedStream);
	emit videoAddedToTrackedStream();
}

ToolPtr PatientModelImplService::getProbeTool(QString videoSourceUid)
{
	for (std::map<QString, ToolPtr>::const_iterator iter = mProbeTools.begin(); iter != mProbeTools.end(); ++iter)
	{
		ToolPtr tool = iter->second;
		ProbePtr probe = tool->getProbe();
		if(probe && probe->getAvailableVideoSources().contains(videoSourceUid))
			return tool;
	}
	reportWarning("Found no probe for stream" + videoSourceUid);
	return ToolPtr();
}

} /* namespace cx */

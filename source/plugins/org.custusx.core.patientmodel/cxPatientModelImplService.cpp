/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
#include "cxActiveData.h"
#include "cxVideoServiceProxy.h"

namespace cx
{

PatientModelImplService::PatientModelImplService(ctkPluginContext *context) :
	mContext(context)
{
	this->createInterconnectedDataAndSpace();

	connect(this->dataService().get(), &DataManager::dataAddedOrRemoved, this, &PatientModelService::dataAddedOrRemoved);
	connect(this->dataService().get(), &DataManager::rMprChanged, this, &PatientModelService::rMprChanged);
	connect(this->dataService().get(), &DataManager::streamLoaded, this, &PatientModelService::streamLoaded);
	connect(this->dataService().get(), &DataManager::clinicalApplicationChanged, this, &PatientModelService::clinicalApplicationChanged);

	connect(this->dataService().get(), &DataManager::centerChanged, this, &PatientModelService::centerChanged);
    connect(this->dataService().get(), &DataManager::operatingTableChanged, this, &PatientModelService::operatingTableChanged);
	connect(this->dataService().get(), &DataManager::landmarkPropertiesChanged, this, &PatientModelService::landmarkPropertiesChanged);

	connect(this->patientData().get(), &PatientData::patientChanged, this, &PatientModelService::patientChanged);

	connect(mTrackingService.get(), &TrackingService::stateChanged, this, &PatientModelImplService::probesChanged);

	mUnavailableData.clear();
}

void PatientModelImplService::createInterconnectedDataAndSpace()
{
	// prerequisites:
	mTrackingService = TrackingServiceProxy::create(mContext);

	mVideoService = VideoServiceProxy::create(mContext);

	// build object(s):
	PatientModelServicePtr patientModelService = PatientModelServiceProxy::create(mContext);
	SessionStorageServicePtr session = SessionStorageServiceProxy::create(mContext);

	mActiveData.reset(new ActiveData(patientModelService, session));
	mDataService = DataManagerImpl::create(mActiveData);

	SpaceProviderPtr spaceProvider(new cx::SpaceProviderImpl(mTrackingService, patientModelService));
	mDataService->setSpaceProvider(spaceProvider);

	mDataFactory.reset(new DataFactory(patientModelService, spaceProvider));
	mDataService->setDataFactory(mDataFactory);

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

std::map<QString, DataPtr> PatientModelImplService::getDatas(DataFilter filter) const
{
	std::map<QString, DataPtr> retval = dataService()->getData();

	if (filter == HideUnavailable)
	{
		for(int i = 0; i < mUnavailableData.size(); ++i)
		{
			if (retval.count(mUnavailableData[i]))
				retval.erase(mUnavailableData[i]);
		}
	}

	return retval;
}

DataPtr PatientModelImplService::getData(const QString& uid) const
{
	std::map<QString, DataPtr> dataMap = this->getDatas(AllData);
	std::map<QString, DataPtr>::const_iterator iter = dataMap.find(uid);
	if (iter == dataMap.end())
		return DataPtr();
	return iter->second;
}

std::map<QString, DataPtr> PatientModelImplService::getChildren(QString parent_uid, QString of_type) const
{
	std::map<QString, DataPtr> retval;
	std::map<QString, DataPtr> dataMap = this->getDatas(AllData);
	std::map<QString, DataPtr>::const_iterator iter = dataMap.begin();
	for ( ; iter != dataMap.end(); ++iter)
	{
		if((iter->second->getParentSpace() == parent_uid))
			if((of_type == "") || (iter->second->getType() == of_type))
				retval[iter->first] =  iter->second;
	}

	return retval;
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

void PatientModelImplService::makeAvailable(const QString &uid, bool available)
{
	if(!available)
		mUnavailableData.push_back(uid);
	else
	{
		std::vector<QString>::iterator iter = std::find(mUnavailableData.begin(), mUnavailableData.end(), uid);
		if(iter != mUnavailableData.end())
			mUnavailableData.erase(iter);
	}
}

CLINICAL_VIEW PatientModelImplService::getClinicalApplication() const
{
	return dataService()->getClinicalApplication();
}

void PatientModelImplService::setClinicalApplication(CLINICAL_VIEW application)
{
	dataService()->setClinicalApplication(application);
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

void PatientModelImplService::setOperatingTable(const OperatingTable &ot)
{
    this->dataService()->setOperatingTable(ot);
}

OperatingTable PatientModelImplService::getOperatingTable() const
{
    return this->dataService()->getOperatingTable();
}

QString PatientModelImplService::addLandmark()
{
	return dataService()->addLandmark();
}

void PatientModelImplService::deleteLandmarks()
{
	return dataService()->deleteLandmarks();
}

void PatientModelImplService::setLandmarkActive(QString uid, bool active)
{
	dataService()->setLandmarkActive(uid, active);
}

RegistrationHistoryPtr PatientModelImplService::get_rMpr_History() const
{
	return this->dataService()->get_rMpr_History();
}

ActiveDataPtr PatientModelImplService::getActiveData() const
{
	return mActiveData;
}

DataServicePtr PatientModelImplService::dataService() const
{
	return mDataService;
}

PatientDataPtr PatientModelImplService::patientData() const
{
	return mPatientData;
}

void PatientModelImplService::probesChanged()
{
	this->disconnectProbes();
	mProbeTools.clear();
	this->connectProbes();
}

void PatientModelImplService::disconnectProbes()
{
	for (std::map<QString, ToolPtr>::const_iterator iter = mProbeTools.begin(); iter != mProbeTools.end(); ++iter)
	{
		ProbePtr probe = iter->second->getProbe();
		if(probe)
			disconnect(probe.get(), &Probe::videoSourceAdded, this, &PatientModelImplService::videoSourceAdded);
		else
			CX_LOG_WARNING() << "PatientModelImplService::disconnectProbes: Tool is not a probe: " << iter->second->getUid();
	}
}

void PatientModelImplService::connectProbes()
{
	ToolMap tools = mTrackingService->getTools();
	for (ToolMap::const_iterator iter = tools.begin(); iter != tools.end(); ++iter)
	{
		ToolPtr tool = iter->second;
		ProbePtr probe = tool->getProbe();
		if(tool->getProbe())
		{
			mProbeTools[iter->first] = tool;
			connect(probe.get(), &Probe::videoSourceAdded, this, &PatientModelImplService::videoSourceAdded);
		}
	}
}

void PatientModelImplService::videoSourceAdded(VideoSourcePtr source)
{
	ToolPtr tool = this->getProbeTool(source->getUid());
	if(!tool)
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
	this->reEmitActiveTrackedStream(trackedStream);
}

void PatientModelImplService::reEmitActiveTrackedStream(TrackedStreamPtr trackedStream)
{
	if(mActiveData->getActive<TrackedStream>() == trackedStream)
		mActiveData->setActive(trackedStream);
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

/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxUsReconstructionImplService.h"


#include <boost/bind.hpp>
#include <ctkPluginContext.h>
#include "cxLogger.h"
#include "cxStringProperty.h"
#include "cxDoubleProperty.h"
#include "cxBoolProperty.h"
#include "cxCompositeTimedAlgorithm.h"
#include "cxReconstructThreads.h"
#include "cxUSFrameData.h"
#include "cxUsReconstructionFileReader.h"
#include "cxReconstructPreprocessor.h"
#include "cxReconstructParams.h"
#include "cxReconstructionMethodService.h"
#include "cxServiceTrackerListener.h"
#include "cxReconstructionExecuter.h"
#include "cxPatientModelService.h"

//Windows fix
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace cx
{


UsReconstructionImplService::UsReconstructionImplService(ctkPluginContext *pluginContext, PatientModelServicePtr patientModelService, ViewServicePtr viewService, XmlOptionFile settings) :
	mPatientModelService(patientModelService),
	mViewService(viewService)
{
	mSettings = settings;
	mSettings.getElement("algorithms");

	mParams.reset(new ReconstructParams(patientModelService, settings));
	connect(mParams.get(), SIGNAL(changedInputSettings()), this, SLOT(setSettings()));
	connect(patientModelService.get(), &PatientModelService::patientChanged, this, &UsReconstructionImplService::patientChangedSlot);

	mServiceListener = boost::shared_ptr<ServiceTrackerListener<ReconstructionMethodService> >(new ServiceTrackerListener<ReconstructionMethodService>(
			pluginContext,
			boost::bind(&UsReconstructionImplService::onServiceAdded, this, _1),
			boost::bind(&UsReconstructionImplService::onServiceModified, this, _1),
			boost::bind(&UsReconstructionImplService::onServiceRemoved, this, _1)
	));

	mServiceListener->open();
}

UsReconstructionImplService::~UsReconstructionImplService()
{
}

bool UsReconstructionImplService::isNull()
{
	return false;
}

void UsReconstructionImplService::patientChangedSlot()
{
	this->selectData(mPatientModelService->getActivePatientFolder() + "/US_Acq/");
	emit newInputDataPath(this->getSelectedFileData().mFilename);
}

ReconstructionMethodService *UsReconstructionImplService::createAlgorithm()
{
	QString name = mParams->getParameter("Algorithm")->getValueAsVariant().toString();
	if(name.isEmpty())
		return NULL;
	return mServiceListener->getServiceFromName(name);
}

void UsReconstructionImplService::setSettings()
{
	mAlgoOptions.clear();
	this->updateFromOriginalFileData();
	emit paramsChanged();
	emit algorithmChanged();
}

void UsReconstructionImplService::startReconstruction()
{
	if(!mOutputVolumeParams.isValid())
	{
		reportError("Cannot reconstruct from invalid ultrasound data");
		return;
	}
	ReconstructionMethodService* algo = this->createAlgorithm();
	ReconstructCore::InputParams par = this->createCoreParameters();
	USReconstructInputData fileData = mOriginalFileData;
	fileData.mUsRaw = mOriginalFileData.mUsRaw->copy();

	ReconstructionExecuterPtr executer(new ReconstructionExecuter(mPatientModelService, mViewService));
	connect(executer.get(), SIGNAL(reconstructAboutToStart()), this, SIGNAL(reconstructAboutToStart()));
	connect(executer.get(), SIGNAL(reconstructStarted()), this, SIGNAL(reconstructStarted()));
	connect(executer.get(), SIGNAL(reconstructFinished()), this, SIGNAL(reconstructFinished()));
	connect(executer.get(), SIGNAL(reconstructFinished()), this, SLOT(reconstructFinishedSlot()));
	mExecuters.push_back(executer);

	executer->startReconstruction(algo, par, fileData, mParams->getCreateBModeWhenAngio()->getValue());
}

std::set<cx::TimedAlgorithmPtr> UsReconstructionImplService::getThreadedReconstruction()
{
	std::set<cx::TimedAlgorithmPtr> retval;
	for (unsigned i=0; i<mExecuters.size(); ++i)
		retval.insert(mExecuters[i]->getThread());
	return retval;
}

void UsReconstructionImplService::reconstructFinishedSlot()
{
	mOriginalFileData.mUsRaw->purgeAll();

	for (unsigned i=0; i<mExecuters.size(); ++i)
	{
		if (mExecuters[i]->getThread()->isFinished())
		{
			ReconstructionExecuterPtr executer = mExecuters[i];
			disconnect(executer.get(), SIGNAL(reconstructAboutToStart()), this, SIGNAL(reconstructAboutToStart()));
			disconnect(executer.get(), SIGNAL(reconstructStarted()), this, SIGNAL(reconstructStarted()));
			disconnect(executer.get(), SIGNAL(reconstructFinished()), this, SIGNAL(reconstructFinished()));
			disconnect(executer.get(), SIGNAL(reconstructFinished()), this, SLOT(reconstructFinishedSlot()));

			mExecuters.erase(mExecuters.begin()+i);
			i=0;
		}
	}
}

void UsReconstructionImplService::clearAll()
{
	mOriginalFileData = USReconstructInputData();
	mOutputVolumeParams = OutputVolumeParams();
}

OutputVolumeParams UsReconstructionImplService::getOutputVolumeParams() const
{
	return mOutputVolumeParams;
}

void UsReconstructionImplService::setOutputVolumeParams(const OutputVolumeParams& par)
{
	mOutputVolumeParams = par;
	this->setSettings();
}

std::vector<PropertyPtr> UsReconstructionImplService::getAlgoOptions()
{
	if (mAlgoOptions.empty())
	{
		ReconstructionMethodService* algo = this->createAlgorithm();
		if (algo)
		{
			QDomElement element = mSettings.getElement("algorithms", algo->getName());
			mAlgoOptions = algo->getSettings(element);
		}
	}

	return mAlgoOptions;
}

XmlOptionFile UsReconstructionImplService::getSettings()
{
	return mSettings;
}

QString UsReconstructionImplService::getSelectedFilename() const
{
	return mOriginalFileData.mFilename;
}

USReconstructInputData UsReconstructionImplService::getSelectedFileData()
{
	return mOriginalFileData;
}

PropertyPtr UsReconstructionImplService::getParam(QString uid)
{
	return mParams->getParameter(uid);
}

void UsReconstructionImplService::selectData(QString filename, QString calFilesPath)
{
	if (filename.isEmpty())
	{
		reportWarning("no file selected");
		return;
	}

	cx::UsReconstructionFileReaderPtr fileReader(new cx::UsReconstructionFileReader());
	USReconstructInputData fileData = fileReader->readAllFiles(filename, calFilesPath);
	fileData.mFilename = filename;
	this->selectData(fileData);
}

void UsReconstructionImplService::selectData(USReconstructInputData fileData)
{
	this->clearAll();
	mOriginalFileData = fileData;
	this->updateFromOriginalFileData();
	emit inputDataSelected(fileData.mFilename);
}

void UsReconstructionImplService::updateFromOriginalFileData()
{
	if (!mOriginalFileData.isValid())
		return;

	ReconstructPreprocessorPtr preprocessor(new ReconstructPreprocessor(mPatientModelService));
	preprocessor->initialize(this->createCoreParameters(), mOriginalFileData);

	if (preprocessor->getOutputVolumeParams().isValid())
		mOutputVolumeParams = preprocessor->getOutputVolumeParams();
	else
	{
		reportError("Input ultrasound data not valid for reconstruction");
		return;
	}

	emit paramsChanged();
}

ReconstructCore::InputParams UsReconstructionImplService::createCoreParameters()
{
	ReconstructCore::InputParams par;
	par.mAlgorithmUid = mParams->getAlgorithmAdapter()->getValue();
	par.mAlgoSettings = mSettings.getElement("algorithms", par.mAlgorithmUid).cloneNode(true).toElement();
	par.mShaderPath = mShaderPath;
	par.mAngio = mParams->getAngioAdapter()->getValue();
	par.mTransferFunctionPreset = mParams->getPresetTFAdapter()->getValue();
	par.mMaxOutputVolumeSize = mParams->getMaxVolumeSize()->getValue();
	par.mExtraTimeCalibration = mParams->getTimeCalibration()->getValue();
	par.mAlignTimestamps = mParams->getAlignTimestamps()->getValue();
    par.mPositionThinning = mParams->getPositionThinning()->getValue();
    par.mPosFilterStrength = mParams->getPosFilterStrength()->getValue().toDouble();;
	par.mMaskReduce = mParams->getMaskReduce()->getValue().toDouble();
	par.mOrientation = mParams->getOrientationAdapter()->getValue();
	return par;
}

void UsReconstructionImplService::onServiceAdded(ReconstructionMethodService* service)
{
	QStringList range = mParams->getAlgorithmAdapter()->getValueRange();
	range << service->getName();
	mParams->getAlgorithmAdapter()->setValueRange(range);

	// select algo if none selected
	PropertyPtr algoName = mParams->getParameter("Algorithm");
	if (algoName->getValueAsVariant().value<QString>().isEmpty())
		algoName->setValueFromVariant(service->getName());
}

void UsReconstructionImplService::onServiceModified(ReconstructionMethodService* service)
{
	reportWarning("ReconstructionMethodService modified... Do not know what to do. Contact developer.");
}

void UsReconstructionImplService::onServiceRemoved(ReconstructionMethodService* service)
{
	QStringList range = mParams->getAlgorithmAdapter()->getValueRange();
	range.removeAll(service->getName());
	mParams->getAlgorithmAdapter()->setValueRange(range);

	QString algoname = mParams->getParameter("Algorithm")->getValueAsVariant().toString();
	if (algoname==service->getName())
		this->setSettings();
}

void UsReconstructionImplService::newDataOnDisk(QString mhdFilename)
{
	emit newInputDataAvailable(mhdFilename);
}

} //cx

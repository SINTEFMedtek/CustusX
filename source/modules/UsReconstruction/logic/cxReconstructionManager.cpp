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


#include "cxReconstructionManager.h"

#include <boost/bind.hpp>

#include "cxStringDataAdapterXml.h"
#include "cxDoubleDataAdapterXml.h"
#include "cxBoolDataAdapterXml.h"
#include "cxCompositeTimedAlgorithm.h"
#include "cxReconstructThreads.h"
#include "cxUSFrameData.h"
#include "cxUsReconstructionFileReader.h"
#include "cxReconstructPreprocessor.h"
#include "cxReconstructParams.h"
#include "cxReconstructionService.h"
#include "cxServiceTrackerListener.h"
#include "cxLogicManager.h"
#include "cxPluginFramework.h"
#include "cxReconstructionExecuter.h"

//Windows fix
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace cx
{


ReconstructionManager::ReconstructionManager(XmlOptionFile settings, QString shaderPath)
{
	mSettings = settings;
	mSettings.getElement("algorithms");

	mParams.reset(new ReconstructParams(settings));
	connect(mParams.get(), SIGNAL(changedInputSettings()), this, SLOT(setSettings()));
	connect(mParams.get(), SIGNAL(transferFunctionChanged()), this, SLOT(transferFunctionChangedSlot()));

	mServiceListener = boost::shared_ptr<ServiceTrackerListener<ReconstructionService> >(new ServiceTrackerListener<ReconstructionService>(
					LogicManager::getInstance()->getPluginContext(),//Should get pluginContext in constructor to make it independant of LogicManager
	        boost::bind(&ReconstructionManager::onServiceAdded, this, _1),
	        boost::bind(&ReconstructionManager::onServiceModified, this, _1),
	        boost::bind(&ReconstructionManager::onServiceRemoved, this, _1)
	));

	mServiceListener->open();
	this->initAlgorithm();
}

ReconstructionManager::~ReconstructionManager()
{
}

ReconstructionServicePtr ReconstructionManager::createAlgorithm()
{
	QString name = mParams->mAlgorithmAdapter->getValue();

	if(name.isEmpty())
		return ReconstructionServicePtr();

	return mServiceListener->getService(name);
}

void ReconstructionManager::initAlgorithm()
{
	ReconstructionService* algo = this->createAlgorithm();

	// generate settings for new algo
	if (algo)
	{
		QDomElement element = mSettings.getElement("algorithms", algo->getName());
		mAlgoOptions = algo->getSettings(element);
	}
	else
	{
		mAlgoOptions.clear();
	}

	emit algorithmChanged();
}

void ReconstructionManager::setSettings()
{
	this->initAlgorithm();
	this->updateFromOriginalFileData();
	emit paramsChanged();
}

void ReconstructionManager::transferFunctionChangedSlot()
{
	//Use angio reconstruction also if only transfer function is set to angio
	if(mParams->mPresetTFAdapter->getValue() == "US Angio")
	{
		reportDebug("Reconstructing angio (Because of angio transfer function)");
		mParams->mAngioAdapter->setValue(true);
	}
	else if(mParams->mPresetTFAdapter->getValue() == "US B-Mode" && mParams->mAngioAdapter->getValue())
	{
		reportDebug("Not reconstructing angio (Because of B-Mode transfer function)");
		mParams->mAngioAdapter->setValue(false);
	}
}

void ReconstructionManager::startReconstruction()
{
	if(!mOutputVolumeParams.isValid())
	{
		reportError("Cannot reconstruct from invalid ultrasound data");
		return;
	}
	ReconstructionService* algo = this->createAlgorithm();
	ReconstructCore::InputParams par = this->createCoreParameters();
	USReconstructInputData fileData = mOriginalFileData;
	fileData.mUsRaw = mOriginalFileData.mUsRaw->copy();

	ReconstructionExecuterPtr executer(new ReconstructionExecuter());
	connect(executer.get(), SIGNAL(reconstructAboutToStart()), this, SIGNAL(reconstructAboutToStart()));
	connect(executer.get(), SIGNAL(reconstructStarted()), this, SIGNAL(reconstructStarted()));
	connect(executer.get(), SIGNAL(reconstructFinished()), this, SIGNAL(reconstructFinished()));
	connect(executer.get(), SIGNAL(reconstructFinished()), this, SLOT(reconstructFinishedSlot()));
	mExecuters.push_back(executer);

	executer->startReconstruction(algo, par, fileData, mParams->mCreateBModeWhenAngio->getValue());
}

std::set<cx::TimedAlgorithmPtr> ReconstructionManager::getThreadedReconstruction()
{
	std::set<cx::TimedAlgorithmPtr> retval;
	for (unsigned i=0; i<mExecuters.size(); ++i)
		retval.insert(mExecuters[i]->getThread());
	return retval;
}

void ReconstructionManager::reconstructFinishedSlot()
{
	mOriginalFileData.mUsRaw->purgeAll();

	std::set<cx::TimedAlgorithmPtr> retval;
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

void ReconstructionManager::clearAll()
{
	mOriginalFileData = USReconstructInputData();
	mOutputVolumeParams = OutputVolumeParams();
}

OutputVolumeParams ReconstructionManager::getOutputVolumeParams() const
{
	return mOutputVolumeParams;
}

void ReconstructionManager::setOutputVolumeParams(const OutputVolumeParams& par)
{
	mOutputVolumeParams = par;
	this->setSettings();
}

void ReconstructionManager::setOutputRelativePath(QString path)
{
	mOutputRelativePath = path;
}

void ReconstructionManager::setOutputBasePath(QString path)
{
	mOutputBasePath = path;
}

ReconstructParamsPtr ReconstructionManager::getParams()
{
	return mParams;
}

std::vector<DataAdapterPtr> ReconstructionManager::getAlgoOptions()
{
	return mAlgoOptions;
}

XmlOptionFile ReconstructionManager::getSettings()
{
	return mSettings;
}

QString ReconstructionManager::getSelectedFilename() const
{
	return mOriginalFileData.mFilename;
}

USReconstructInputData ReconstructionManager::getSelectedFileData()
{
	return mOriginalFileData;
}

void ReconstructionManager::selectData(QString filename, QString calFilesPath)
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

void ReconstructionManager::selectData(USReconstructInputData fileData)
{
	this->clearAll();
	mOriginalFileData = fileData;
	this->updateFromOriginalFileData();
	emit inputDataSelected(fileData.mFilename);
}

void ReconstructionManager::updateFromOriginalFileData()
{
	if (!mOriginalFileData.isValid())
		return;

	ReconstructPreprocessorPtr preprocessor(new ReconstructPreprocessor());
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

ReconstructCore::InputParams ReconstructionManager::createCoreParameters()
{
	ReconstructCore::InputParams par;
	par.mAlgorithmUid = mParams->mAlgorithmAdapter->getValue();
	par.mAlgoSettings = mSettings.getElement("algorithms", par.mAlgorithmUid).cloneNode(true).toElement();
	par.mOutputBasePath = mOutputBasePath;
	par.mOutputRelativePath = mOutputRelativePath;
	par.mShaderPath = mShaderPath;
	par.mAngio = mParams->mAngioAdapter->getValue();
	par.mTransferFunctionPreset = mParams->mPresetTFAdapter->getValue();
	par.mMaxOutputVolumeSize = mParams->mMaxVolumeSize->getValue();
	par.mExtraTimeCalibration = mParams->mTimeCalibration->getValue();
	par.mAlignTimestamps = mParams->mAlignTimestamps->getValue();
	par.mMaskReduce = mParams->mMaskReduce->getValue().toDouble();
	par.mOrientation = mParams->mOrientationAdapter->getValue();
	return par;
}

void ReconstructionManager::onServiceAdded(ReconstructionService* service)
{
    QStringList range = mParams->mAlgorithmAdapter->getValueRange();
    range << service->getName();
    mParams->mAlgorithmAdapter->setValueRange(range);
}

void ReconstructionManager::onServiceModified(ReconstructionService* service)
{
	reportWarning("ReconstructionService modified... Do not know what to do. Contact developer.");
}

void ReconstructionManager::onServiceRemoved(ReconstructionService* service)
{
    QStringList range = mParams->mAlgorithmAdapter->getValueRange();
    range.removeAll(service->getName());
    mParams->mAlgorithmAdapter->setValueRange(range);
}

}

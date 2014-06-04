// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

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

#ifdef CX_USE_OPENCL_UTILITY
	#include "TordReconstruct/TordTest.h"
#endif // CX_USE_OPENCL_UTILITY
#include "cxPNNReconstructAlgorithm.h"

//Windows fix
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace cx
{


ReconstructionManagerImpl::ReconstructionManagerImpl(XmlOptionFile settings, QString shaderPath) :
		ReconstructionManager(settings, shaderPath), mOutputRelativePath(""), mOutputBasePath(""), mShaderPath(shaderPath)
{
	mExecuter.reset(new ReconstructionExecuter());
	connect(mExecuter.get(), SIGNAL(reconstructAboutToStart()), this, SIGNAL(reconstructAboutToStart()));
	connect(mExecuter.get(), SIGNAL(reconstructFinished()), this, SLOT(reconstructFinishedSlot()));


	mSettings = settings;
	mSettings.getElement("algorithms");

	mParams.reset(new ReconstructParams(settings));
	connect(mParams.get(), SIGNAL(changedInputSettings()), this, SLOT(setSettings()));
	connect(mParams.get(), SIGNAL(transferFunctionChanged()), this, SLOT(transferFunctionChangedSlot()));

	mServiceListener = boost::shared_ptr<ServiceTrackerListener<ReconstructionService> >(new ServiceTrackerListener<ReconstructionService>(
	        LogicManager::getInstance()->getPluginFramework(),
	        boost::bind(&ReconstructionManagerImpl::onServiceAdded, this, _1),
	        boost::bind(&ReconstructionManagerImpl::onServiceModified, this, _1),
	        boost::bind(&ReconstructionManagerImpl::onServiceRemoved, this, _1)
	));

	this->initAlgorithm();
}

ReconstructionManagerImpl::~ReconstructionManagerImpl()
{
}

void ReconstructionManagerImpl::init()
{
	mServiceListener->open();
}

namespace
{
struct null_deleter
{
	void operator()(void const *) const {}
};
}

ReconstructionServicePtr ReconstructionManagerImpl::createAlgorithm()
{
	QString name = mParams->mAlgorithmAdapter->getValue();

	ReconstructionServicePtr algo;
	if(name == "TordReconstructionService")
	{
		ReconstructionServicePtr pointer(ReconstructionServicePtr(mServiceListener->getService(name), null_deleter()));
		algo = pointer;
	}
	else
	{
		if (name == "PNN")
			algo = ReconstructionServicePtr(new PNNReconstructAlgorithm());
	#ifdef CX_USE_OPENCL_UTILITY
		else if (name == "TordTest")
		{
			algo = ReconstructionServicePtr(new TordTest());
		}
	#endif // CX_USE_OPENCL_UTILITY
	}
	return algo;
}

void ReconstructionManagerImpl::initAlgorithm()
{
    ReconstructionServicePtr algo = this->createAlgorithm();

	// generate settings for new algo
	if (algo)
	{
		QDomElement element = mSettings.getElement("algorithms", algo->getName());
		mAlgoOptions = algo->getSettings(element);
		emit algorithmChanged();
	}
}

void ReconstructionManagerImpl::setSettings()
{
	this->initAlgorithm();
	this->updateFromOriginalFileData();
	emit paramsChanged();
}

void ReconstructionManagerImpl::transferFunctionChangedSlot()
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

std::vector<ReconstructCorePtr> ReconstructionManagerImpl::startReconstruction()
{
	ReconstructionServicePtr algo = this->createAlgorithm();
	ReconstructCore::InputParams par = this->createCoreParameters();
	USReconstructInputData fileData = mOriginalFileData;
	fileData.mUsRaw = mOriginalFileData.mUsRaw->copy();

	return mExecuter->startReconstruction(algo, par, fileData, mParams->mCreateBModeWhenAngio->getValue(), this->validInputData());
}

std::set<cx::TimedAlgorithmPtr> ReconstructionManagerImpl::getThreadedReconstruction()
{
	return mExecuter->getThreadedReconstruction();
}

void ReconstructionManagerImpl::reconstructFinishedSlot()
{
	mOriginalFileData.mUsRaw->purgeAll();
}


void ReconstructionManagerImpl::clearAll()
{
	mOriginalFileData = USReconstructInputData();
	mOutputVolumeParams = OutputVolumeParams();
}

OutputVolumeParams ReconstructionManagerImpl::getOutputVolumeParams() const
{
	return mOutputVolumeParams;
}

void ReconstructionManagerImpl::setOutputVolumeParams(const OutputVolumeParams& par)
{
	mOutputVolumeParams = par;
	this->setSettings();
}

void ReconstructionManagerImpl::setOutputRelativePath(QString path)
{
	mOutputRelativePath = path;
}

void ReconstructionManagerImpl::setOutputBasePath(QString path)
{
	mOutputBasePath = path;
}

bool ReconstructionManagerImpl::validInputData() const
{
	if (mOriginalFileData.mFrames.empty() || !mOriginalFileData.mUsRaw || mOriginalFileData.mPositions.empty())
		return false;
	if(mOriginalFileData.mUsRaw->is4D())
	{
		reportWarning("US reconstructer do not handle 4D US data");
		return false;
	}
	return true;
}

ReconstructParamsPtr ReconstructionManagerImpl::getParams()
{
	return mParams;
}

std::vector<DataAdapterPtr> ReconstructionManagerImpl::getAlgoOptions()
{
	return mAlgoOptions;
}

XmlOptionFile ReconstructionManagerImpl::getSettings()
{
	return mSettings;
}

QString ReconstructionManagerImpl::getSelectedFilename() const
{
	return mOriginalFileData.mFilename;
}

USReconstructInputData ReconstructionManagerImpl::getSelectedFileData()
{
	return mOriginalFileData;
}

void ReconstructionManagerImpl::selectData(QString filename, QString calFilesPath)
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

void ReconstructionManagerImpl::selectData(USReconstructInputData fileData)
{
	this->clearAll();
	mOriginalFileData = fileData;
	this->updateFromOriginalFileData();
	emit inputDataSelected(fileData.mFilename);
}

void ReconstructionManagerImpl::updateFromOriginalFileData()
{
	if (!this->validInputData())
		return;

	ReconstructCore::InputParams par;
	USReconstructInputData fileData = mOriginalFileData;
	fileData.mUsRaw = mOriginalFileData.mUsRaw->copy();
	ReconstructPreprocessorPtr preprocessor = mExecuter->createPreprocessor(this->createCoreParameters(), fileData, this->validInputData());
	mOutputVolumeParams = preprocessor->getOutputVolumeParams();

	emit paramsChanged();
}

ReconstructCore::InputParams ReconstructionManagerImpl::createCoreParameters()
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

void ReconstructionManagerImpl::onServiceAdded(ReconstructionService* service)
{
    QStringList range = mParams->mAlgorithmAdapter->getValueRange();
    range << service->getName();
    mParams->mAlgorithmAdapter->setValueRange(range);
}

void ReconstructionManagerImpl::onServiceModified(ReconstructionService* service)
{
	//TODO
}

void ReconstructionManagerImpl::onServiceRemoved(ReconstructionService* service)
{
    QStringList range = mParams->mAlgorithmAdapter->getValueRange();
    range.removeAll(service->getName());
    mParams->mAlgorithmAdapter->setValueRange(range);
}

}

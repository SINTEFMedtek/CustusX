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

//Windows fix
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace cx
{


ReconstructionManagerImpl::ReconstructionManagerImpl(XmlOptionFile settings, QString shaderPath) :
		ReconstructionManager(settings, shaderPath), mOutputRelativePath(""), mOutputBasePath(""), mShaderPath(shaderPath)
{

	mSettings = settings;
	mSettings.getElement("algorithms");

	mParams.reset(new ReconstructParams(settings));
	connect(mParams.get(), SIGNAL(changedInputSettings()), this, SLOT(setSettings()));
	connect(mParams.get(), SIGNAL(transferFunctionChanged()), this, SLOT(transferFunctionChangedSlot()));

	mServiceListener = boost::shared_ptr<ServiceTrackerListener<ReconstructionService> >(new ServiceTrackerListener<ReconstructionService>(
					LogicManager::getInstance()->getPluginFramework()->getPluginContext(),//Should get pluginContext in constructor to make it independant of LogicManager
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

	ReconstructCorePtr core; ///< in progress: algorithm part of class moved here.
	core.reset(new ReconstructCore());
	ReconstructionServicePtr algo;
	if(name == "TordReconstructionService")
	{
		ReconstructionServicePtr pointer(ReconstructionServicePtr(mServiceListener->getService(name), null_deleter()));
		algo = pointer;
	}
	else
	{
		algo = core->createAlgorithm(name);
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
	std::vector<ReconstructCorePtr> cores = this->createCores();

	if (cores.empty())
	{
		reportWarning("Failed to start reconstruction");
		return cores;
	}

	cx::CompositeTimedAlgorithmPtr algorithm = this->assembleReconstructionPipeline(cores);

	this->launch(algorithm);

	return cores;
}

std::set<cx::TimedAlgorithmPtr> ReconstructionManagerImpl::getThreadedReconstruction()
{
	return mThreadedReconstruction;
}

void ReconstructionManagerImpl::launch(cx::TimedAlgorithmPtr thread)
{
	mThreadedReconstruction.insert(thread);
	emit reconstructAboutToStart();
	connect(thread.get(), SIGNAL(finished()), this, SLOT(threadFinishedSlot())); // connect after emit, to allow listeners to get thread at finish
	thread->execute();
}

void ReconstructionManagerImpl::threadFinishedSlot()
{
	std::set<cx::TimedAlgorithmPtr>::iterator iter;
	for(iter=mThreadedReconstruction.begin(); iter!=mThreadedReconstruction.end(); )
	{
		if ((*iter)->isFinished())
		{
			mThreadedReconstruction.erase(iter);
			iter = mThreadedReconstruction.begin();
		}
		else
			++iter;
	}

	if (mThreadedReconstruction.empty())
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

cx::CompositeTimedAlgorithmPtr ReconstructionManagerImpl::assembleReconstructionPipeline(std::vector<ReconstructCorePtr> cores)
{
	cx::CompositeSerialTimedAlgorithmPtr pipeline(new cx::CompositeSerialTimedAlgorithm("US Reconstruction"));

	ReconstructPreprocessorPtr preprocessor = this->createPreprocessor();
	pipeline->append(ThreadedTimedReconstructPreprocessor::create(preprocessor, cores));

	cx::CompositeTimedAlgorithmPtr temp = pipeline;
	if(this->canCoresRunInParallel(cores))
	{
		cx::CompositeParallelTimedAlgorithmPtr parallel(new cx::CompositeParallelTimedAlgorithm());
		pipeline->append(parallel);
		temp = parallel;
		reportDebug("Running reconstruction cores in parallel.");
	}

	for (unsigned i=0; i<cores.size(); ++i)
		temp->append(ThreadedTimedReconstructCore::create(cores[i]));

	return pipeline;
}

bool ReconstructionManagerImpl::canCoresRunInParallel(std::vector<ReconstructCorePtr> cores)
{
	bool parallelizable = true;

	std::vector<ReconstructCorePtr>::iterator it;
	for(it = cores.begin(); it != cores.end(); ++it)
		parallelizable = parallelizable && (it->get()->getInputParams().mAlgorithmUid == "PNN");

	return parallelizable;
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

	ReconstructPreprocessorPtr preprocessor = this->createPreprocessor();
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

ReconstructPreprocessorPtr ReconstructionManagerImpl::createPreprocessor()
{
	if (!this->validInputData())
		return ReconstructPreprocessorPtr();

	ReconstructPreprocessorPtr retval(new ReconstructPreprocessor());

	ReconstructCore::InputParams par = this->createCoreParameters();

	USReconstructInputData fileData = mOriginalFileData;
	fileData.mUsRaw = mOriginalFileData.mUsRaw->copy();

	retval->initialize(par, fileData);

	return retval;
}

std::vector<ReconstructCorePtr> ReconstructionManagerImpl::createCores()
{
	std::vector<ReconstructCorePtr> retval;

	// create both
	if (mParams->mCreateBModeWhenAngio->getValue() && mParams->mAngioAdapter->getValue())
	{
		ReconstructCorePtr core = this->createBModeCore();
		if (core)
			retval.push_back(core);
		core = this->createCore();
		if (core)
			retval.push_back(core);
	}
	// only one thread
	else
	{
		ReconstructCorePtr core = this->createCore();
		if (core)
			retval.push_back(core);
	}

	return retval;
}

ReconstructCorePtr ReconstructionManagerImpl::createCore()
{
	if (!this->validInputData())
		return ReconstructCorePtr();

	ReconstructCorePtr retval(new ReconstructCore());

	ReconstructCore::InputParams par = this->createCoreParameters();
	retval->initialize(par);

	return retval;
}

ReconstructCorePtr ReconstructionManagerImpl::createBModeCore()
{
	if (!this->validInputData())
		return ReconstructCorePtr();

	ReconstructCorePtr retval(new ReconstructCore());

	ReconstructCore::InputParams par = this->createCoreParameters();
	par.mAngio = false;
	par.mTransferFunctionPreset = "US B-Mode";

	retval->initialize(par);

	return retval;
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

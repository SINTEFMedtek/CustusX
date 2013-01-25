/*
 * sscReconstructManager.cpp
 *
 *  \date Oct 4, 2011
 *      \author christiana
 */

#include <sscReconstructManager.h>

#include <algorithm>
#include <QtCore>
#include <vtkImageData.h>
#include "sscBoundingBox3D.h"
#include "sscDataManager.h"
#include "sscXmlOptionItem.h"
#include "sscStringDataAdapterXml.h"
#include "sscDoubleDataAdapterXml.h"
#include "sscToolManager.h"
#include "sscMessageManager.h"
#include "utils/sscReconstructHelper.h"
#include "sscTime.h"
#include "sscTypeConversions.h"
#include "sscRegistrationTransform.h"
#include "sscUtilHelpers.h"
#include "cxCreateProbeDataFromConfiguration.h"
#include "sscVolumeHelpers.h"
#include "cxUsReconstructionFileReader.h"
#include "sscPresetTransferFunctions3D.h"
#include "cxToolManager.h"
#include "sscManualTool.h"
#include "cxPatientService.h"
#include "cxPatientData.h"
#include "cxViewManager.h"
#include "cxCompositeTimedAlgorithm.h"
#include "sscReconstructThreads.h"

//Windows fix
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace ssc
{


ReconstructManager::ReconstructManager(XmlOptionFile settings, QString shaderPath) :
	mOutputRelativePath(""), mOutputBasePath(""), mShaderPath(shaderPath)
{
	mSettings = settings;
	mSettings.getElement("algorithms");

	mParams.reset(new ReconstructParams(settings));
	connect(mParams.get(), SIGNAL(changedInputSettings()), this, SLOT(setSettings()));
	connect(mParams.get(), SIGNAL(transferFunctionChanged()), this, SLOT(transferFunctionChangedSlot()));

	this->initAlgorithm();
}

ReconstructManager::~ReconstructManager()
{

}

ReconstructAlgorithmPtr ReconstructManager::createAlgorithm()
{
	QString name = mParams->mAlgorithmAdapter->getValue();

	ReconstructCorePtr core; ///< in progress: algorithm part of class moved here.
	core.reset(new ReconstructCore());
	ReconstructAlgorithmPtr algo = core->createAlgorithm(name);
	return algo;
}

void ReconstructManager::initAlgorithm()
{
	ReconstructAlgorithmPtr algo = this->createAlgorithm();

	// generate settings for new algo
	if (algo)
	{
		QDomElement element = mSettings.getElement("algorithms", algo->getName());
		mAlgoOptions = algo->getSettings(element);
		emit algorithmChanged();
	}
}

void ReconstructManager::setSettings()
{
	this->initAlgorithm();
	this->updateFromOriginalFileData();
	emit paramsChanged();
}

void ReconstructManager::transferFunctionChangedSlot()
{
	//Use angio reconstruction also if only transfer function is set to angio
	if(mParams->mPresetTFAdapter->getValue() == "US Angio")
	{
		ssc::messageManager()->sendDebug("Reconstructing angio (Because of angio transfer function)");
		mParams->mAngioAdapter->setValue(true);
	}
	else if(mParams->mPresetTFAdapter->getValue() == "US B-Mode" && mParams->mAngioAdapter->getValue())
	{
		ssc::messageManager()->sendDebug("Not reconstructing angio (Because of B-Mode transfer function)");
		mParams->mAngioAdapter->setValue(false);
	}
}

std::vector<ReconstructCorePtr> ReconstructManager::startReconstruction()
{
	cx::CompositeTimedAlgorithmPtr serial(new cx::CompositeTimedAlgorithm("US Reconstruction"));
	cx::CompositeParallelTimedAlgorithmPtr parallel(new cx::CompositeParallelTimedAlgorithm());

	ReconstructPreprocessorPtr preprocessor = this->createPreprocessor();
	std::vector<ReconstructCorePtr> cores = this->createCores();

	if (cores.empty())
	{
		ssc::messageManager()->sendWarning("Failed to start reconstruction");
		return cores;
	}

	serial->append(ThreadedTimedReconstructPreprocessor::create(preprocessor, cores));
	serial->append(parallel);
	for (unsigned i=0; i<cores.size(); ++i)
		parallel->append(ThreadedTimedReconstructCore::create(cores[i]));

	this->launch(serial);

	return cores;
}

void ReconstructManager::launch(cx::TimedAlgorithmPtr thread)
{
	mThreadedReconstruction.insert(thread);
	emit reconstructAboutToStart();
	connect(thread.get(), SIGNAL(finished()), this, SLOT(threadFinishedSlot())); // connect after emit, to allow listeners to get thread at finish
	thread->execute();
}

void ReconstructManager::threadFinishedSlot()
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
		{
			++iter;
		}
	}

	if (mThreadedReconstruction.empty())
		mOriginalFileData.mUsRaw->purgeAll();
}

void ReconstructManager::clearAll()
{
	mOriginalFileData = ssc::USReconstructInputData();
	mOutputVolumeParams = OutputVolumeParams();
}

OutputVolumeParams ReconstructManager::getOutputVolumeParams() const
{
	return mOutputVolumeParams;
}

void ReconstructManager::setOutputVolumeParams(const OutputVolumeParams& par)
{
	mOutputVolumeParams = par;
	this->setSettings();
}

void ReconstructManager::setOutputRelativePath(QString path)
{
	mOutputRelativePath = path;
}

void ReconstructManager::setOutputBasePath(QString path)
{
	mOutputBasePath = path;
}

bool ReconstructManager::validInputData() const
{
	if (mOriginalFileData.mFrames.empty() || !mOriginalFileData.mUsRaw || mOriginalFileData.mPositions.empty())
		return false;
	if(mOriginalFileData.mUsRaw->is4D())
	{
		ssc::messageManager()->sendWarning("US reconstructer do not handle 4D US data");
		return false;
	}
	return true;
}

ReconstructParamsPtr ReconstructManager::getParams()
{
	return mParams;
}

std::vector<DataAdapterPtr> ReconstructManager::getAlgoOptions()
{
	return mAlgoOptions;
}

QString ReconstructManager::getSelectedFilename() const
{
	return mOriginalFileData.mFilename;
}

void ReconstructManager::selectData(QString filename, QString calFilesPath)
{
	if (filename.isEmpty())
	{
		ssc::messageManager()->sendWarning("no file selected");
		return;
	}

	cx::UsReconstructionFileReaderPtr fileReader(new cx::UsReconstructionFileReader());
	ssc::USReconstructInputData fileData = fileReader->readAllFiles(filename, calFilesPath);
	fileData.mFilename = filename;
	this->selectData(fileData);
}

void ReconstructManager::selectData(ssc::USReconstructInputData fileData)
{
	this->clearAll();
	mOriginalFileData = fileData;
	this->updateFromOriginalFileData();
	emit inputDataSelected(fileData.mFilename);
}

void ReconstructManager::updateFromOriginalFileData()
{
	if (!this->validInputData())
		return;

	ReconstructPreprocessorPtr preprocessor = this->createPreprocessor();
	mOutputVolumeParams = preprocessor->getOutputVolumeParams();

	emit paramsChanged();
}

ReconstructCore::InputParams ReconstructManager::createCoreParameters()
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

ReconstructPreprocessorPtr ReconstructManager::createPreprocessor()
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

std::vector<ReconstructCorePtr> ReconstructManager::createCores()
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

ReconstructCorePtr ReconstructManager::createCore()
{
	if (!this->validInputData())
		return ReconstructCorePtr();

	ReconstructCorePtr retval(new ReconstructCore());

	ReconstructCore::InputParams par = this->createCoreParameters();
	retval->initialize(par);

	return retval;
}

ReconstructCorePtr ReconstructManager::createBModeCore()
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


}

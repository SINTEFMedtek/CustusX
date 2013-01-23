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
#include "sscReconstructer.h"
#include "cxPatientService.h"
#include "cxPatientData.h"
#include "cxViewManager.h"
#include "cxCompositeTimedAlgorithm.h"

//Windows fix
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace ssc
{


ReconstructManager::ReconstructManager(XmlOptionFile settings, QString shaderPath)
{
	mFileReader.reset(new cx::UsReconstructionFileReader());

	mReconstructer.reset(new Reconstructer(settings, shaderPath));

	connect(mReconstructer.get(), SIGNAL(paramsChanged()), this, SIGNAL(paramsChanged()));
	connect(mReconstructer.get(), SIGNAL(algorithmChanged()), this, SIGNAL(algorithmChanged()));
	connect(mReconstructer.get(), SIGNAL(inputDataSelected(QString)), this, SIGNAL(inputDataSelected(QString)));
}

ReconstructManager::~ReconstructManager()
{

}

std::vector<ReconstructCorePtr> ReconstructManager::startReconstruction()
{
	cx::CompositeTimedAlgorithmPtr serial(new cx::CompositeTimedAlgorithm("US Reconstruction"));
	cx::CompositeParallelTimedAlgorithmPtr parallel(new cx::CompositeParallelTimedAlgorithm());

	ReconstructPreprocessorPtr preprocessor = mReconstructer->createPreprocessor();
	std::vector<ReconstructCorePtr> cores = mReconstructer->createCores();

	if (cores.empty())
	{
		ssc::messageManager()->sendWarning("Failed to start reconstruction");
		return cores;
	}

	serial->append(ThreadedTimedReconstructerStep1::create(preprocessor, cores));
	serial->append(parallel);
	for (unsigned i=0; i<cores.size(); ++i)
		parallel->append(ThreadedTimedReconstructerStep2::create(cores[i]));

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


ReconstructParamsPtr ReconstructManager::getParams()
{
	return mReconstructer->mParams;
}


std::vector<DataAdapterPtr> ReconstructManager::getAlgoOptions()
{
	return mReconstructer->mAlgoOptions;
}

QString ReconstructManager::getSelectedData() const
{
	return mOriginalFileData.mFilename;
//	return mReconstructer->getSelectedData();
}


OutputVolumeParams ReconstructManager::getOutputVolumeParams() const
{
	return mReconstructer->getOutputVolumeParams();
}

void ReconstructManager::setOutputVolumeParams(const OutputVolumeParams& par)
{
	mReconstructer->setOutputVolumeParams(par);
}

void ReconstructManager::setOutputRelativePath(QString path)
{
	mReconstructer->setOutputRelativePath(path);
}

void ReconstructManager::setOutputBasePath(QString path)
{
	mReconstructer->setOutputBasePath(path);
}

bool ReconstructManager::validInputData() const
{
	if (mOriginalFileData.mFrames.empty() || !mOriginalFileData.mUsRaw || mOriginalFileData.mPositions.empty())
		return false;
	return true;
}

void ReconstructManager::clearAll()
{
	mOriginalFileData = ssc::USReconstructInputData();

	mReconstructer->clearAll();
}

void ReconstructManager::selectData(QString filename, QString calFilesPath)
{
	if (filename.isEmpty())
	{
		ssc::messageManager()->sendWarning("no file selected");
		return;
	}

	this->clearAll();
	this->readCoreFiles(filename, calFilesPath);
	mReconstructer->setInputData(mOriginalFileData);
}

void ReconstructManager::selectData(ssc::USReconstructInputData data)
{
	this->clearAll();

	mOriginalFileData = data;
	mCalFilesPath = "";

	mReconstructer->setInputData(mOriginalFileData);
}

/**Read from file into mOriginalFileData.
 * These data are not changed before clearAll() or this method is called again.
 */
void ReconstructManager::readCoreFiles(QString fileName, QString calFilesPath)
{
	mOriginalFileData.mFilename = fileName;
	mCalFilesPath = calFilesPath;

	ssc::USReconstructInputData temp = mFileReader->readAllFiles(fileName, calFilesPath);
	if (!temp.mUsRaw)
		return;

	mOriginalFileData = temp;
	mOriginalFileData.mFilename = fileName;
	mCalFilesPath = calFilesPath;
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


//ThreadedTimedReconstructer::ThreadedTimedReconstructer(ReconstructCorePtr reconstructer) :
//	cx::ThreadedTimedAlgorithm<void> ("US Reconstruction", 30)
//{
//	mReconstructer = reconstructer;
//}

//ThreadedTimedReconstructer::~ThreadedTimedReconstructer()
//{
//}

//void ThreadedTimedReconstructer::preProcessingSlot()
//{
//	mReconstructer->threadedPreReconstruct();
//}

//void ThreadedTimedReconstructer::calculate()
//{
//	mReconstructer->threadablePreReconstruct();
//	mReconstructer->threadedReconstruct();
//}

//void ThreadedTimedReconstructer::postProcessingSlot()
//{
//	mReconstructer->threadedPostReconstruct();

//	cx::patientService()->getPatientData()->autoSave();
//	cx::viewManager()->autoShowData(mReconstructer->getOutput());
//}


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


ThreadedTimedReconstructerStep1::ThreadedTimedReconstructerStep1(ReconstructPreprocessorPtr input, std::vector<ReconstructCorePtr> cores) :
	cx::ThreadedTimedAlgorithm<void> ("US PreReconstruction", 30)
{
	mInput = input;
	mUseDefaultMessages = false;
//	mReconstructer = reconstructer;
	mCores = cores;
}

ThreadedTimedReconstructerStep1::~ThreadedTimedReconstructerStep1()
{
}

void ThreadedTimedReconstructerStep1::preProcessingSlot()
{
//	mReconstructer->threadedPreReconstruct();
}

void ThreadedTimedReconstructerStep1::calculate()
{
	mInput->initializeCores(mCores);

//	ProcessedUSInputDataPtr bmode_in, angio_in;
//	mInput->getProcessedFileData().mUsRaw->initializeFrames(mBMode!=0, &bmode_in, mAngio!=0, &angio_in);

//	if (mBMode)
//		mBMode->initialize(bmode_in, mInput->getOutputVolumeParams());
//	if (mAngio)
//		mAngio->initialize(angio_in, mInput->getOutputVolumeParams());

//	mReconstructer->threadablePreReconstruct();
}

void ThreadedTimedReconstructerStep1::postProcessingSlot()
{
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


ThreadedTimedReconstructerStep2::ThreadedTimedReconstructerStep2(ReconstructCorePtr reconstructer) :
	cx::ThreadedTimedAlgorithm<void> ("US Reconstruction", 30)
{
	mUseDefaultMessages = false;
	mReconstructer = reconstructer;
}

ThreadedTimedReconstructerStep2::~ThreadedTimedReconstructerStep2()
{
}

void ThreadedTimedReconstructerStep2::preProcessingSlot()
{
	mReconstructer->threadedPreReconstruct();
}

void ThreadedTimedReconstructerStep2::calculate()
{
	mReconstructer->threadedReconstruct();
}

void ThreadedTimedReconstructerStep2::postProcessingSlot()
{
	mReconstructer->threadedPostReconstruct();

	cx::patientService()->getPatientData()->autoSave();
	if (cx::viewManager()) // might be called by auto test - no service
		cx::viewManager()->autoShowData(mReconstructer->getOutput());
}

}

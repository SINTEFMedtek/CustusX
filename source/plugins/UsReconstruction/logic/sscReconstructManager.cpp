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
#include "sscReconstructThreads.h"

//Windows fix
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace ssc
{


ReconstructManager::ReconstructManager(XmlOptionFile settings, QString shaderPath)
{
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
//	mCalFilesPath = "";

	mReconstructer->setInputData(mOriginalFileData);
}

/**Read from file into mOriginalFileData.
 * These data are not changed before clearAll() or this method is called again.
 */
void ReconstructManager::readCoreFiles(QString fileName, QString calFilesPath)
{
	mOriginalFileData.mFilename = fileName;
//	mCalFilesPath = calFilesPath;

	cx::UsReconstructionFileReaderPtr fileReader(new cx::UsReconstructionFileReader());
	ssc::USReconstructInputData temp = fileReader->readAllFiles(fileName, calFilesPath);
	if (!temp.mUsRaw)
		return;

	mOriginalFileData = temp;
	mOriginalFileData.mFilename = fileName;
//	mCalFilesPath = calFilesPath;
}


}

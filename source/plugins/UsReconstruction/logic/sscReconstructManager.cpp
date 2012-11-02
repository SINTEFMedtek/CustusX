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

void ReconstructManager::startReconstruction()
{
	ThreadedTimedReconstructerPtr thread(new ssc::ThreadedTimedReconstructer(mReconstructer->createCore()));
	this->launch(thread);

	if (mReconstructer->mParams->mCreateBModeWhenAngio->getValue() && mReconstructer->mParams->mAngioAdapter->getValue())
	{
		ReconstructCorePtr dualCore = mReconstructer->createDualCore();
		ThreadedTimedReconstructerPtr dual(new ssc::ThreadedTimedReconstructer(dualCore));
		this->launch(dual);
	}
}

void ReconstructManager::launch(ThreadedTimedReconstructerPtr thread)
{
	mThreadedReconstruction.insert(thread);
	emit reconstructAboutToStart();
	connect(thread.get(), SIGNAL(finished()), this, SLOT(threadFinishedSlot())); // connect after emit, to allow listeners to get thread at finish
	thread->start();
}

void ReconstructManager::threadFinishedSlot()
{
	std::set<ssc::ThreadedTimedReconstructerPtr>::iterator iter;
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


ThreadedTimedReconstructer::ThreadedTimedReconstructer(ReconstructCorePtr reconstructer) :
	cx::ThreadedTimedAlgorithm<void> ("US Reconstruction", 30)
{
	mReconstructer = reconstructer;
}

ThreadedTimedReconstructer::~ThreadedTimedReconstructer()
{
}

void ThreadedTimedReconstructer::start()
{
	mReconstructer->threadedPreReconstruct();
	this->generate();
}

void ThreadedTimedReconstructer::postProcessingSlot()
{
	mReconstructer->threadedPostReconstruct();
}

void ThreadedTimedReconstructer::calculate()
{
	mReconstructer->threadedReconstruct();
}


}

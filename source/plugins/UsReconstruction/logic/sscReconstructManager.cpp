/*
 * sscReconstructManager.cpp
 *
 *  Created on: Oct 4, 2011
 *      Author: christiana
 */

#include <sscReconstructManager.h>

#include <algorithm>
#include <QtCore>
#include <vtkImageData.h>
#include "matrixInterpolation.h"
#include "sscBoundingBox3D.h"
#include "sscDataManager.h"
#include "sscXmlOptionItem.h"
#include "sscStringDataAdapterXml.h"
#include "sscDoubleDataAdapterXml.h"
#include "sscToolManager.h"
#include "sscMessageManager.h"
//#include "sscThunderVNNReconstructAlgorithm.h"
//#include "sscPNNReconstructAlgorithm.h"
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
	//mOutputRelativePath(""), mOutputBasePath(""), mMaxTimeDiff(100)// TODO: Change default value for max allowed time difference between tracking and image time tags
{
	mFileReader.reset(new cx::UsReconstructionFileReader());

	mReconstructer.reset(new Reconstructer(settings, shaderPath));

	connect(mReconstructer.get(), SIGNAL(paramsChanged()), this, SIGNAL(paramsChanged()));
	connect(mReconstructer.get(), SIGNAL(algorithmChanged()), this, SIGNAL(algorithmChanged()));
	connect(mReconstructer.get(), SIGNAL(inputDataSelected(QString)), this, SIGNAL(inputDataSelected(QString)));
	connect(mReconstructer.get(), SIGNAL(reconstructFinished()), this, SIGNAL(reconstructFinished()));

//	mOrientationAdapter = mOrientationAdapter;
//	mPresetTFAdapter = mPresetTFAdapter;
//	mAlgorithmAdapter = mAlgorithmAdapter;
//	//	std::vector<DataAdapterPtr> mAlgoOptions;
//	mMaskReduce = mMaskReduce;//Reduce mask size in % in each direction
//	mAlignTimestamps = mAlignTimestamps; ///align track and frame timestamps to each other automatically
//	mTimeCalibration = mTimeCalibration; ///set a offset in the frame timestamps
//	mAngioAdapter = mAngioAdapter; ///US angio data is used as input
}

ReconstructManager::~ReconstructManager()
{

}

ReconstructParamsPtr ReconstructManager::getParams()
{
	return mReconstructer->mParams;
}


ReconstructAlgorithmPtr ReconstructManager::getAlgorithm()
{
	return mReconstructer->mAlgorithm;
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

void ReconstructManager::reconstruct()
{
	mReconstructer->reconstruct();
}


ImagePtr ReconstructManager::getOutput()
{
	return mReconstructer->getOutput();
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

//	emit inputDataSelected(filename);
}

/**Read from file into mOriginalFileData.
 * These data are not changed before clearAll() or this method is called again.
 */
void ReconstructManager::readCoreFiles(QString fileName, QString calFilesPath)
{
	mOriginalFileData.mFilename = fileName;
	mCalFilesPath = calFilesPath;

	ssc::USReconstructInputData temp = mFileReader->readAllFiles(fileName, calFilesPath,
		this->getParams()->mAngioAdapter->getValue());
	if (!temp.mUsRaw)
		return;

	mOriginalFileData = temp;
	mOriginalFileData.mFilename = fileName;
	mCalFilesPath = calFilesPath;
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


ThreadedReconstructer::ThreadedReconstructer(ReconstructManagerPtr reconstructer)
{
	mReconstructer = reconstructer;
	mReconstructer->getReconstructer()->threadedPreReconstruct();
	connect(this, SIGNAL(finished()), this, SLOT(postReconstructionSlot())); // ensure this slot is run before all other listeners.
}
void ThreadedReconstructer::run()
{
	mReconstructer->getReconstructer()->threadedReconstruct();
}
void ThreadedReconstructer::postReconstructionSlot()
{
	mReconstructer->getReconstructer()->threadedPostReconstruct();
}
}

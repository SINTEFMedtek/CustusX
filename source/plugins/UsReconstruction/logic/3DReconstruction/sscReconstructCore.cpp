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

#include "sscReconstructCore.h"

#include <vtkImageData.h>
#include "sscDataManager.h"
#include "sscToolManager.h"
#include "sscThunderVNNReconstructAlgorithm.h"
#include "sscPNNReconstructAlgorithm.h"
#include "sscTime.h"
#include "sscTypeConversions.h"
#include "sscRegistrationTransform.h"
#include "sscVolumeHelpers.h"
#include "sscTransferFunctions3DPresets.h"
#include "sscTimeKeeper.h"
#include "sscLogger.h"
#include "sscUSFrameData.h"

namespace ssc
{

ReconstructCore::ReconstructCore()
{
//	mMaxTimeDiff = 100; // TODO: Change default value for max allowed time difference between tracking and image time tags
	mSuccess = false;
}

ReconstructCore::~ReconstructCore()
{
}

void ReconstructCore::initialize(InputParams input)
{
	mInput = input;
}

void ReconstructCore::initialize(ProcessedUSInputDataPtr fileData, OutputVolumeParams outputVolumeParams)
{
	mOutputVolumeParams = outputVolumeParams;
	mFileData = fileData;
//	this->updateFromOriginalFileData();

	mAlgorithm = this->createAlgorithm(mInput.mAlgorithmUid);
}

ReconstructAlgorithmPtr ReconstructCore::createAlgorithm(QString name)
{
	ReconstructAlgorithmPtr retval;

	if (mAlgorithm && mAlgorithm->getName() == name)
		return mAlgorithm;

	// create new algo
	if (name == "ThunderVNN")
	{
		retval = ThunderVNNReconstructAlgorithm::create(mInput.mShaderPath);
//		retval = ReconstructAlgorithmPtr(new ThunderVNNReconstructAlgorithm(mInput.mShaderPath));
	}
	else if (name == "PNN")
		retval = ReconstructAlgorithmPtr(new PNNReconstructAlgorithm());
	else
		retval.reset();

	return retval;
}

ssc::ImagePtr ReconstructCore::reconstruct()
{
	this->threadedPreReconstruct();
//	this->threadablePreReconstruct();
	this->threadedReconstruct();
	this->threadedPostReconstruct();
	return mOutput;
}

/**The reconstruct part that must be fun pre-rec in the main thread.
 *
 */
void ReconstructCore::threadedPreReconstruct()
{
	if (!this->validInputData())
		return;
	mRawOutput = this->generateRawOutputVolume();
}

/**The reconstruct part that can be run in a separate thread.
 *
 */
void ReconstructCore::threadedReconstruct()
{
	if (!this->validInputData())
		return;
	SSC_ASSERT(mRawOutput);

	TimeKeeper timer;

	mSuccess = mAlgorithm->reconstruct(mFileData, mRawOutput, mInput.mAlgoSettings);

	timer.printElapsedSeconds("Reconstruct core time");
}

/**The reconstruct part that must be done post-rec in the main thread.
 *
 */
void ReconstructCore::threadedPostReconstruct()
{
	if (!this->validInputData())
		return;

	if (mSuccess)
	{
		mOutput = this->generateOutputVolume(mRawOutput);

		Eigen::Array3i outputDims(mRawOutput->GetDimensions());
		int total = outputDims[0] * outputDims[1] * outputDims[2];
		ssc::messageManager()->sendInfo(QString("US Reconstruction complete: %1Mb, output=%2, algo=%3, preset=%4, angio=%5")
										.arg(total/1024/1024)
										.arg(mOutput->getName())
										.arg(mAlgorithm->getName())
										.arg(mInput.mTransferFunctionPreset)
										.arg(mInput.mAngio));

		DataManager::getInstance()->loadData(mOutput);
		DataManager::getInstance()->saveImage(mOutput, mInput.mOutputBasePath);
	}
	else
	{
		ssc::messageManager()->sendError("Reconstruction failed");
	}
}

/**
 * Pre:  All data read, mExtent is calculated
 * Post: Output volume is initialized
 */
ImagePtr ReconstructCore::generateOutputVolume(vtkImageDataPtr rawOutput)
{
	//If no output path is selecetd, use the same path as the input
	QString filePath;
	if (mInput.mOutputBasePath.isEmpty() && mInput.mOutputRelativePath.isEmpty())
		filePath = qstring_cast(mFileData->getFilePath());
	else
		filePath = mInput.mOutputRelativePath;

	QString uid = this->generateOutputUid();
	QString name = this->generateImageName(uid);

	ImagePtr image = dataManager()->createImage(rawOutput, uid + "_%1", name + " %1", filePath);
	image->get_rMd_History()->setRegistration(mOutputVolumeParams.m_rMd);
	image->setModality("US");
	if (mInput.mAngio)
		image->setImageType("Angio");
	else
		image->setImageType("B-Mode");

	ssc::PresetTransferFunctions3DPtr presets = ssc::dataManager()->getPresetTransferFunctions3D();
	presets->load(mInput.mTransferFunctionPreset, image);

	return image;
}

/**
 * Pre:  All data read, mExtent is calculated
 * Post: Output volume is initialized
 */
vtkImageDataPtr ReconstructCore::generateRawOutputVolume()
{
	Eigen::Array3i dim = mOutputVolumeParams.getDim();
	ssc::Vector3D spacing = ssc::Vector3D(1, 1, 1) * mOutputVolumeParams.getSpacing();
	vtkImageDataPtr data = ssc::generateVtkImageData(dim, spacing, 0);
	return data;
}

/** Generate an output uid based on the assumption that input uid
 * is on the format "US-Acq_01_20001224T170000".
 * Change to "US_01_20001224T170000",
 * or add a "rec" postfix if a different name format is detected.
 */
QString ReconstructCore::generateOutputUid()
{
	QString base = mFileData->getUid();
	QString name = mFileData->getFilePath().split("/").back();
	name = name.split(".").front();

	QStringList split = name.split("_");
	QStringList prefix = split.front().split("-");
	if (prefix.size() == 2)
	{
		split[0] = prefix[0];
	}
	else
	{
		split[0] += "_rec";
	}

	return split.join("_");
}

/**Generate a pretty name for for volume based on the filename.
 * Assume filename has format US-Acq_01_20001224T170000 or similar.
 * Format: US <counter> <hh:mm>, for example US 3 15:34
 */
QString ReconstructCore::generateImageName(QString uid) const
{
	QString name = uid.split("/").back();
	name = name.split(".").front();
	QString prefix = name.split("_").front(); // retrieve US-Acq part
	prefix = prefix.split("-").front(); // retrieve US part.
	if (prefix.isEmpty())
		prefix = "US";

//	std::cout << "reconstruct input uid " << uid << std::endl;
	if (mInput.mAngio) // tag angio images as such
		prefix += "A";

	// retrieve  index counter from _99_
	QString counter = "";
	QRegExp countReg("_[0-9]{1,2}_");
	if (countReg.indexIn(name) > 0)
	{
		counter = countReg.cap(0).remove("_");
	}

	// retrieve timestamp as HH:MM
	QRegExp tsReg("[0-9]{8}T[0-9]{6}");
	if (tsReg.indexIn(name) > 0)
	{
		QDateTime datetime = QDateTime::fromString(tsReg.cap(0), timestampSecondsFormat());
		QString timestamp = datetime.toString("hh:mm");
		return prefix + " " + counter + " " + timestamp;
	}

	return name;
}

ImagePtr ReconstructCore::getOutput()
{
	return mOutput;
}

bool ReconstructCore::validInputData() const
{
	return mAlgorithm!=0;
}

} /* namespace ssc */

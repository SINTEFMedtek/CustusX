/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxReconstructCore.h"
#include <vtkImageData.h>
#include "cxTime.h"
#include "cxTypeConversions.h"
#include "cxRegistrationTransform.h"
#include "cxVolumeHelpers.h"
#include "cxTransferFunctions3DPresets.h"
#include "cxTimeKeeper.h"
#include "cxLogger.h"
#include "cxUSFrameData.h"
#include "cxReconstructionMethodService.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"
#include "cxPatientModelService.h"

namespace cx
{


ReconstructCore::ReconstructCore(cx::PatientModelServicePtr patientModelService) :
	mInput(InputParams()),
	mPatientModelService(patientModelService)
{
//	mMaxTimeDiff = 100; // TODO: Change default value for max allowed time difference between tracking and image time tags
	mSuccess = false;
}

ReconstructCore::~ReconstructCore()
{
}

void ReconstructCore::initialize(InputParams input, ReconstructionMethodService* algorithm)
{
	mInput = input;
	mAlgorithm = algorithm;
}

void ReconstructCore::initialize(ProcessedUSInputDataPtr fileData, OutputVolumeParams outputVolumeParams)
{
	mOutputVolumeParams = outputVolumeParams;
	mFileData = fileData;
}

ImagePtr ReconstructCore::reconstruct()
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
    CX_ASSERT(mRawOutput);

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
		report(QString("US Reconstruction complete: %1Mb, output=%2, algo=%3, preset=%4, angio=%5")
										.arg(total/1024/1024)
										.arg(mOutput->getName())
										.arg(mAlgorithm->getName())
										.arg(mInput.mTransferFunctionPreset)
										.arg(mInput.mAngio));

		mPatientModelService->insertData(mOutput);
	}
	else
	{
		reportError("Reconstruction failed");
	}
}

/**
 * Pre:  All data read, mExtent is calculated
 * Post: Output volume is initialized
 */
ImagePtr ReconstructCore::generateOutputVolume(vtkImageDataPtr rawOutput)
{
	setDeepModified(rawOutput);

//	//If no output path is selecetd, use the same path as the input
//	QString filePath;
//	if (mInput.mOutputBasePath.isEmpty() && mInput.mOutputRelativePath.isEmpty())
//		filePath = qstring_cast(mFileData->getFilePath());
//	else
//		filePath = mInput.mOutputRelativePath;

	QString uid = this->generateOutputUid();
	QString name = this->generateImageName(uid);

	ImagePtr image = mPatientModelService->createSpecificData<Image>(uid + "_%1", name + " %1");
	image->setVtkImageData(rawOutput);

//	ImagePtr image = mPatientModelService->createImage(rawOutput, uid + "_%1", name + " %1", filePath);
	image->get_rMd_History()->setRegistration(mOutputVolumeParams.get_rMd());
	image->setModality(imUS);
	if (mInput.mAngio)
		image->setImageType(istANGIO);
	else
		image->setImageType(istUSBMODE);

	PresetTransferFunctions3DPtr presets = mPatientModelService->getPresetTransferFunctions3D();
	presets->load(mInput.mTransferFunctionPreset, image, true, false);//Only apply to 2D, not 3D
	presets->load("US B-Mode", image, false, true);//Only apply to 3D, not 2D

	return image;
}

/**
 * Pre:  All data read, mExtent is calculated
 * Post: Output volume is initialized
 */
vtkImageDataPtr ReconstructCore::generateRawOutputVolume()
{
	Eigen::Array3i dim = mOutputVolumeParams.getDim();
	Vector3D spacing = Vector3D(1, 1, 1) * mOutputVolumeParams.getSpacing();
	vtkImageDataPtr data = generateVtkImageData(dim, spacing, 0);
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
 * Format may also be US-Acq_01_20001224T170000_Tissue or similar
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
		QString postfix = name.split("_").back();
		QDateTime datetime = QDateTime::fromString(tsReg.cap(0), timestampSecondsFormat());
		QString timestamp = datetime.toString("hh:mm");
		return prefix + " " + counter + " " + postfix + " " + timestamp;
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

} /* namespace cx */

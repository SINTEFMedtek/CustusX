/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXRECONSTRUCTCORE_H_
#define CXRECONSTRUCTCORE_H_

#include "org_custusx_usreconstruction_Export.h"
#include <memory>

#include "cxBoundingBox3D.h"
#include "cxForwardDeclarations.h"
#include "cxReconstructedOutputVolumeParams.h"

namespace cx
{
class ReconstructionMethodService;
//typedef class ReconstructionMethodService* ReconstructionMethodServicePtr;

typedef std::shared_ptr<class ReconstructCore> ReconstructCorePtr;

/** \brief Algorithm part of reconstruction -
 * no dependencies on parameter classes.
 *
 * \ingroup org_custusx_usreconstruction
 *  \date Oct 29, 2012
 *  \author christiana
 */
class org_custusx_usreconstruction_EXPORT ReconstructCore
{
public:
	/**All parameters required to control the reconstruction
	 *
	 */
	struct InputParams
	{
		InputParams() :
			mExtraTimeCalibration(0),
			mAlignTimestamps(false),
			mPositionThinning(false),
			mPosFilterStrength(0),
			mPosFilterType("Moving average"),
			mHoltAlpha(0.5),
			mHoltBeta(0.3),
			mHoltBothDirections(true),
			mHoltAddTrend(false),
			mMaskReduce(0),
			mAngio(false),
			mMaxOutputVolumeSize(1024*1024),
			mWriteProcessedPositions(false)
		{}
		double mExtraTimeCalibration;
		bool mAlignTimestamps;
		bool mPositionThinning;
		unsigned mPosFilterStrength; ///< Strength (half window length, in samples) of the moving average position filter. 0 = off.
		QString mPosFilterType; ///< "Off", "Moving average" (uses mPosFilterStrength) or "Holt" (uses the Holt parameters below)
		double mHoltAlpha; ///< Holt filter: smoothing of the level, [0,1]
		double mHoltBeta; ///< Holt filter: smoothing of the trend, [0,1]
		bool mHoltBothDirections; ///< Holt filter: filter forward and backward in time and use the mean, which gives no delay
		bool mHoltAddTrend; ///< Holt filter: output level + trend instead of the level
		double mMaskReduce;
		QString mOrientation;

		QString mAlgorithmUid;
		QDomElement mAlgoSettings;
//		QString mOutputRelativePath;///< Relative path to the output image
//		QString mOutputBasePath;///< Global path where the relative path starts, for the output image
		QString mShaderPath; ///< name of shader folder
		bool mAngio; ///< true for angio data, false is B-mode.
		QString mTransferFunctionPreset;
		double mMaxOutputVolumeSize;
		/** Write the positions as used by the reconstruction (after filtering, time lag and any correction by the
		 *  algorithm) next to the input data, as <name>.tp.corr and <name>.tts.corr. */
		bool mWriteProcessedPositions;
	};

	ReconstructCore(PatientModelServicePtr patientModelService);
	virtual ~ReconstructCore();

	// used for reconstruction algo
	void initialize(InputParams input, ReconstructionMethodService* algorithm);
	void initialize(ProcessedUSInputDataPtr fileData, OutputVolumeParams outputVolumeParams);
	ImagePtr reconstruct();
	void threadedPreReconstruct();
	void threadedReconstruct();
	void threadedPostReconstruct();
	ImagePtr getOutput();

	// published helper methods, also needed for parameter display outside of reconstruction execution:
	InputParams getInputParams() { return mInput; }
	ReconstructionMethodService* getAlgorithm() { return mAlgorithm; }

private:
	bool validInputData() const;
	void writeProcessedPositions();
	QString generateOutputUid();
	QString generateImageName(QString uid) const;

	vtkImageDataPtr generateRawOutputVolume();
	ImagePtr generateOutputVolume(vtkImageDataPtr rawOutput);

	// input data
	InputParams mInput;
	ProcessedUSInputDataPtr mFileData;

	// generated data
	ReconstructionMethodService* mAlgorithm;///< The used reconstruction algorithm
	vtkImageDataPtr mRawOutput; ///< Output image, prior to conversion to Image.
	ImagePtr mOutput;///< Output image from reconstruction
	OutputVolumeParams mOutputVolumeParams;
	bool mSuccess;
	PatientModelServicePtr mPatientModelService;
};

} /* namespace cx */
#endif /* CXRECONSTRUCTCORE_H_ */

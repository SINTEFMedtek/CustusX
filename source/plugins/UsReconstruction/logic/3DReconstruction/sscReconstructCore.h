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
#ifndef SSCRECONSTRUCTCORE_H_
#define SSCRECONSTRUCTCORE_H_

#include "sscReconstructAlgorithm.h"
#include "sscBoundingBox3D.h"
#include "sscReconstructedOutputVolumeParams.h"

namespace ssc
{

typedef boost::shared_ptr<class ReconstructCore> ReconstructCorePtr;

/** \brief Algorithm part of reconstruction -
 * no dependencies on parameter classes.
 *
 *  \date Oct 29, 2012
 *  \author christiana
 */
class ReconstructCore
{
public:
	/**All parameters required to control the reconstruction
	 *
	 */
	struct InputParams
	{
		double mExtraTimeCalibration;
		bool mAlignTimestamps;
		double mMaskReduce;
		QString mOrientation;

		QString mAlgorithmUid;
		QDomElement mAlgoSettings;
		QString mOutputRelativePath;///< Relative path to the output image
		QString mOutputBasePath;///< Global path where the relative path starts, for the output image
		QString mShaderPath; ///< name of shader folder
		bool mAngio; ///< true for angio data, false is B-mode.
		QString mTransferFunctionPreset;
		double mMaxOutputVolumeSize;
	};

	ReconstructCore();
	virtual ~ReconstructCore();

	// used for reconstruction algo
	void initialize(InputParams input);
	void initialize(ProcessedUSInputDataPtr fileData, OutputVolumeParams outputVolumeParams);
	ssc::ImagePtr reconstruct();
	void threadedPreReconstruct();
	void threadedReconstruct();
	void threadedPostReconstruct();
	ImagePtr getOutput();

	// published helper methods, also needed for parameter display outside of reconstruction execution:
	ReconstructAlgorithmPtr createAlgorithm(QString uid);
	InputParams getInputParams() { return mInput; }

private:
	bool validInputData() const;
	QString generateOutputUid();
	QString generateImageName(QString uid) const;

	vtkImageDataPtr generateRawOutputVolume();
	ImagePtr generateOutputVolume(vtkImageDataPtr rawOutput);

	// input data
	InputParams mInput;
	ProcessedUSInputDataPtr mFileData;

	// generated data
	ReconstructAlgorithmPtr mAlgorithm;///< The used reconstruction algorithm
	vtkImageDataPtr mRawOutput; ///< Output image, prior to conversion to ssc::Image.
	ImagePtr mOutput;///< Output image from reconstruction
	OutputVolumeParams mOutputVolumeParams;
	bool mSuccess;
};

} /* namespace ssc */
#endif /* SSCRECONSTRUCTCORE_H_ */

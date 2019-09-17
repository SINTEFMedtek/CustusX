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

#include "cxBoundingBox3D.h"
#include "cxForwardDeclarations.h"
#include "cxReconstructedOutputVolumeParams.h"

namespace cx
{
class ReconstructionMethodService;
//typedef class ReconstructionMethodService* ReconstructionMethodServicePtr;

typedef boost::shared_ptr<class ReconstructCore> ReconstructCorePtr;

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
			mMaskReduce(0),
			mAngio(false),
			mMaxOutputVolumeSize(1024*1024)
		{}
		double mExtraTimeCalibration;
		bool mAlignTimestamps;
		bool mPositionThinning;
		unsigned mPosFilterStrength;
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
	ReconstructionMethodService* mAlgorithm;///< The used reconstruction algorithm
	vtkImageDataPtr mRawOutput; ///< Output image, prior to conversion to Image.
	ImagePtr mOutput;///< Output image from reconstruction
	OutputVolumeParams mOutputVolumeParams;
	bool mSuccess;
	PatientModelServicePtr mPatientModelService;
};

} /* namespace cx */
#endif /* CXRECONSTRUCTCORE_H_ */

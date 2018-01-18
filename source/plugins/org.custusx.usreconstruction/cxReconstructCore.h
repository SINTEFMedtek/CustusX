/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
        double mPosFilterStrength;
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

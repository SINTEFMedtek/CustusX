/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXRECONSTRUCTPREPROCESSOR_H_
#define CXRECONSTRUCTPREPROCESSOR_H_

#include "org_custusx_usreconstruction_Export.h"

#include "cxBoundingBox3D.h"
#include "cxReconstructedOutputVolumeParams.h"
#include "cxReconstructCore.h"
#include "cxUSReconstructInputData.h"

namespace cx
{
typedef boost::shared_ptr<class ReconstructPreprocessor> ReconstructPreprocessorPtr;

/** \brief Algorithm part of reconstruction -
 * no dependencies on parameter classes.
 *
 * \ingroup org_custusx_usreconstruction
 *  \date Oct 29, 2012
 *  \author christiana
 */
class org_custusx_usreconstruction_EXPORT ReconstructPreprocessor
{
public:
	ReconstructPreprocessor(PatientModelServicePtr patientModelService);
	virtual ~ReconstructPreprocessor();

	// used for reconstruction algo
	void initialize(ReconstructCore::InputParams input, USReconstructInputData fileData);
	OutputVolumeParams getOutputVolumeParams() { return mOutputVolumeParams; }
	ReconstructCore::InputParams getInputParams() { return mInput; }

	std::vector<ProcessedUSInputDataPtr> createProcessedInput(std::vector<bool> angio);

private:
	void cropInputData();
	IntBoundingBox3D reduceCropboxToImageSize(IntBoundingBox3D cropbox, QSize size);
	void updateFromOriginalFileData();
	void findExtentAndOutputTransform();
	Transform3D applyOutputOrientation();
	std::vector<Vector3D> generateInputRectangle();
	void interpolatePositions();
	double timeToPosition(unsigned i_frame, unsigned i_pos);
	void filterPositions(); // Noise-supressing position filter, averaging filter, configurable length
	void positionThinning(); // If enabled, try to remove "suspect" data (large jumps etc.)
	void applyTimeCalibration();
	void alignTimeSeries();
	void calibrateTimeStamps(double offset, double scale);

	// input data
	ReconstructCore::InputParams mInput;
	USReconstructInputData mFileData;
	double mMaxTimeDiff;

	// generated data
	OutputVolumeParams mOutputVolumeParams;

	PatientModelServicePtr mPatientModelService;
	void convertToQuaternions(Eigen::ArrayXXd qPosArray, int nQuaternions, int nPositions, int filterStrength);
	Eigen::ArrayXXd filterQuaternionArray(Eigen::ArrayXXd qPosArray, int nPositions, int filterLength);
	void convertFromQuaternion(Eigen::ArrayXXd qPosFiltered);
};

} /* namespace cx */

#endif // CXRECONSTRUCTPREPROCESSOR_H_

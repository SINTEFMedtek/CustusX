/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXRECONSTRUCTPREPROCESSOR_H_
#define CXRECONSTRUCTPREPROCESSOR_H_

#include <map>
#include "org_custusx_usreconstruction_Export.h"
#include <memory>

#include "cxBoundingBox3D.h"
#include "cxReconstructedOutputVolumeParams.h"
#include "cxReconstructCore.h"
#include "cxUSReconstructInputData.h"

namespace cx
{
struct TimeLagEstimate;
struct RemoveDataType;
typedef std::shared_ptr<class ReconstructPreprocessor> ReconstructPreprocessorPtr;

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

	/**
	 * If the algorithm requests it, estimate the time lag between images and tracking data,
	 * and if the algorithm asks for it, apply the lag to the tracking data (overriding Extra Temporal Calib and
	 * Align timestamps), and recalculate positions, frames and output volume.
	 * Must be called before createProcessedInput() / getOutputVolumeParams() are used for the reconstruction.
	 * \return true if a lag was applied.
	 */
	bool estimateAndApplyTimeLag(ReconstructionMethodService* algorithm);

private:
	void cropInputData();
	IntBoundingBox3D reduceCropboxToImageSize(IntBoundingBox3D cropbox, QSize size);
	void updateFromOriginalFileData();
	void processPositions(bool reportRemovedFrames); ///< reportRemovedFrames: false when repeated with other settings
	TimeLagEstimate estimateTimeLag(ReconstructionMethodService* algorithm);
	void reportRemovedFrames(const std::map<unsigned, RemoveDataType>& removedData, unsigned startFrames);
	void findExtentAndOutputTransform();
	Transform3D applyOutputOrientation();
	std::vector<Vector3D> generateInputRectangle();
	void interpolatePositions(bool reportRemovedFrames);
	double timeToPosition(unsigned i_frame, unsigned i_pos);
	void filterPositions(); // Noise-supressing position filter, averaging filter, configurable length
	void positionThinning(); // If enabled, try to remove "suspect" data (large jumps etc.)
	void applyTimeCalibration();
	void alignTimeSeries();
	void calibrateTimeStamps(double offset, double scale);

	// input data
	ReconstructCore::InputParams mInput;
	USReconstructInputData mFileData;
	USReconstructInputData mCroppedFileData; ///< input after cropping, before any time calibration and position processing
	double mMaxTimeDiff;

	// generated data
	OutputVolumeParams mOutputVolumeParams;
	Transform3D mPrMd; ///< from output volume space to patient reference space, from findExtentAndOutputTransform()

	PatientModelServicePtr mPatientModelService;
	void convertToQuaternions(Eigen::ArrayXXd qPosArray, int nQuaternions, int nPositions, int filterStrength);
	Eigen::ArrayXXd filterQuaternionArray(Eigen::ArrayXXd qPosArray, int nPositions, int filterLength);
	void convertFromQuaternion(Eigen::ArrayXXd qPosFiltered);
};

} /* namespace cx */

#endif // CXRECONSTRUCTPREPROCESSOR_H_

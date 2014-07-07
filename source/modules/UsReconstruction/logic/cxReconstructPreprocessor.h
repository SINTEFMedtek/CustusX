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
#ifndef CXRECONSTRUCTPREPROCESSOR_H_
#define CXRECONSTRUCTPREPROCESSOR_H_


#include "cxBoundingBox3D.h"
#include "cxReconstructedOutputVolumeParams.h"
#include "cxReconstructCore.h"
#include "cxUSReconstructInputData.h"
#include "cxLegacySingletons.h"

namespace cx
{
typedef boost::shared_ptr<class ReconstructPreprocessor> ReconstructPreprocessorPtr;

/** \brief Algorithm part of reconstruction -
 * no dependencies on parameter classes.
 *
 *  \date Oct 29, 2012
 *  \author christiana
 */
class ReconstructPreprocessor
{
public:
    ReconstructPreprocessor();
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
    void applyTimeCalibration();
    void alignTimeSeries();
	void calibrateTimeStamps(double offset, double scale);

    // input data
    ReconstructCore::InputParams mInput;
    USReconstructInputData mFileData;
    double mMaxTimeDiff;

    // generated data
    OutputVolumeParams mOutputVolumeParams;
};

} /* namespace cx */

#endif // CXRECONSTRUCTPREPROCESSOR_H_

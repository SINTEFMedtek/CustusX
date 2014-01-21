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
#ifndef SSCRECONSTRUCTPREPROCESSOR_H
#define SSCRECONSTRUCTPREPROCESSOR_H


#include "sscBoundingBox3D.h"
#include "sscReconstructedOutputVolumeParams.h"
#include "sscReconstructCore.h"
#include "cxUSReconstructInputData.h"

namespace cx
{
typedef boost::shared_ptr<class ReconstructAlgorithm> ReconstructAlgorithmPtr;

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
    /** Return the filedata after preprocessing, ready to be sent to reconstruction
      */
    void initializeCores(std::vector<ReconstructCorePtr> cores); ///< fill input cores with relevant processed input
		std::vector<ProcessedUSInputDataPtr> createProcessedInput(std::vector<ReconstructCorePtr> cores);

private:
    void cropInputData();
    void updateFromOriginalFileData();
    void findExtentAndOutputTransform();
    Transform3D applyOutputOrientation();
		std::vector<Vector3D> generateInputRectangle();
		void interpolatePositions();
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

#endif // SSCRECONSTRUCTPREPROCESSOR_H

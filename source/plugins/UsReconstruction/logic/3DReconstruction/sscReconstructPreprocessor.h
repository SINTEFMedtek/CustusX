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

#include "sscReconstructAlgorithm.h"
#include "sscBoundingBox3D.h"
#include "sscReconstructedOutputVolumeParams.h"
#include "sscReconstructCore.h"

namespace ssc
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
//    ssc::ImagePtr reconstruct();
//    void threadedPreReconstruct();
//    void threadablePreReconstruct();
//    void threadedReconstruct();
//    void threadedPostReconstruct();
    OutputVolumeParams getOutputVolumeParams() { return mOutputVolumeParams; }
    ReconstructCore::InputParams getInputParams() { return mInput; }
    /** Return the filedata after preprocessing, ready to be sent to reconstruction
      */
//    USReconstructInputData getProcessedFileData();
    void initializeCores(std::vector<ReconstructCorePtr> cores); ///< fill input cores with relevant processed input

    static Transform3D slerpInterpolate(const Transform3D& a, const Transform3D& b, double t);

private:
    void cropInputData();
    void updateFromOriginalFileData();
    void findExtentAndOutputTransform();
    ssc::Transform3D applyOutputOrientation();
    std::vector<ssc::Vector3D> generateInputRectangle();
    void transformPositionsTo_prMu();
    void interpolatePositions();
    Transform3D interpolate(const Transform3D& a, const Transform3D& b, double t);
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

} /* namespace ssc */

#endif // SSCRECONSTRUCTPREPROCESSOR_H

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
};

} /* namespace cx */

#endif // CXRECONSTRUCTPREPROCESSOR_H_

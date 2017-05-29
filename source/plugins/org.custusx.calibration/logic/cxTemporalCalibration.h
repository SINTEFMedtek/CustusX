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
#ifndef CXTEMPORALCALIBRATION_H_
#define CXTEMPORALCALIBRATION_H_

#include "org_custusx_calibration_Export.h"

#include <sstream>
#include "cxVideoRecorder.h"
#include "cxTool.h"
#include "cxUSReconstructInputData.h"
#include "cxForwardDeclarations.h"

namespace cx
{
/**
 * \file
 * \addtogroup org_custusx_calibration
 * @{
 */

typedef std::map<double,double> TimeSeriesType;

/**Class for performing temporal calibration.
 *
 * Load a US Acquisition data set, and run calibrate().
 * The algorithm will extract the probe movement from
 * the tracking data and from the us images, and use
 * correlation to find the shift between them.
 *
 * The shift sign is given from:
 *   frames = tracking + shift
 *
 */
class org_custusx_calibration_EXPORT TemporalCalibration
{
public:
	TemporalCalibration();
  void selectData(QString filename);
  void setDebugFolder(QString path);
  double calibrate(bool* success);

private:
	vtkImageDataPtr extractLine_y(USFrameDataPtr data, int line_index_x, int frame);
  double findCorrelation(USFrameDataPtr data, int frame_a, int frame_b, double maxShift, double lastVal);
  std::vector<double> computeProbeMovement();
  std::vector<double> resample(std::vector<double> shift, std::vector<TimedPosition> time, double resolution);
  std::vector<double> computeTrackingMovement();
  double findCorrelationShift(std::vector<double> frames, std::vector<double> tracking, double resolution) const;
  double findLeastSquares(std::vector<double> frames, std::vector<double> tracking, int shift) const;
  double findLSShift(std::vector<double> frames, std::vector<double> tracking, double resolution) const;
  bool checkFrameMovementQuality(std::vector<double> pos);
  void writePositions(QString title, std::vector<double> pos, std::vector<TimedPosition> time, double shift);

  void saveDebugFile();

  USReconstructInputData mFileData; ///< original version of loaded data. Use as basis when recalculating due to changed params.
  std::vector<vtkImageDataPtr> mProcessedFrames; ///< frame data processed from the input mFileData.
  QString mDebugFolder;
  QString mFilename;
  mutable std::stringstream mDebugStream;
  bool mAddRawToDebug;
  vtkImageDataPtr mMask;

};


typedef boost::shared_ptr<TemporalCalibration> TemporalCalibrationPtr;

/**
 * @}
 */
}

#endif /* CXTEMPORALCALIBRATION_H_ */

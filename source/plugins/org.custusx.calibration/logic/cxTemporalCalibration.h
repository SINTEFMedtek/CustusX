/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
  void selectData(QString filename, FileManagerServicePtr filemanager);
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

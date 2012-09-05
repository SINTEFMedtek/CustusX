/*
 * cxTemporalCalibration.h
 *
 *  \date May 9, 2011
 *      \author christiana
 */

#ifndef CXTEMPORALCALIBRATION_H_
#define CXTEMPORALCALIBRATION_H_

#include <sstream>
#include "cxUsReconstructionFileReader.h"
#include "sscVideoRecorder.h"
#include "sscTool.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxPluginCalibration
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
class TemporalCalibration
{
public:
	TemporalCalibration();
  void selectData(QString filename);
  void setDebugFolder(QString path);
  double calibrate(bool* success);

private:
  vtkImageDataPtr extractLine_y(ssc::USFrameDataPtr data, int line_index_x, int frame);
  double findCorrelation(ssc::USFrameDataPtr data, int frame_a, int frame_b, double maxShift, double lastVal);
  std::vector<double> computeProbeMovement();
  std::vector<double> resample(std::vector<double> shift, std::vector<ssc::TimedPosition> time, double resolution);
  std::vector<double> computeTrackingMovement();
  double findCorrelationShift(std::vector<double> frames, std::vector<double> tracking, double resolution) const;
  double findLeastSquares(std::vector<double> frames, std::vector<double> tracking, int shift) const;
  double findLSShift(std::vector<double> frames, std::vector<double> tracking, double resolution) const;
  bool checkFrameMovementQuality(std::vector<double> pos);
  void writePositions(QString title, std::vector<double> pos, std::vector<ssc::TimedPosition> time, double shift);

  void saveDebugFile();

  ssc::USReconstructInputData mFileData; ///< original version of loaded data. Use as basis when recalculating due to changed params.
  QString mDebugFolder;
  QString mFilename;
  mutable std::stringstream mDebugStream;
  bool mAddRawToDebug;

};


typedef boost::shared_ptr<TemporalCalibration> TemporalCalibrationPtr;

/**
 * @}
 */
}

#endif /* CXTEMPORALCALIBRATION_H_ */

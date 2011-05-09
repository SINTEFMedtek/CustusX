/*
 * cxTemporalCalibration.h
 *
 *  Created on: May 9, 2011
 *      Author: christiana
 */

#ifndef CXTEMPORALCALIBRATION_H_
#define CXTEMPORALCALIBRATION_H_

#include <sstream>
#include "cxUsReconstructionFileReader.h"
#include "sscRTSourceRecorder.h"
#include "sscTool.h"

namespace cx
{

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

  void selectData(QString filename);
  void setDebugFile(QString filename);
  double calibrate();

private:
//  void computeTemporalCalibration(ssc::RTSourceRecorder::DataType volumes, ssc::TimedTransformMap tracking, ssc::ToolPtr probe);
  vtkImageDataPtr extractLine_y(ssc::USFrameDataPtr data, int line_index_x, int frame);
  double findCorrelation(ssc::USFrameDataPtr data, int frame_a, int frame_b, int maxShift);
  std::vector<double> computeProbeMovement();
  std::vector<double> resample(std::vector<double> shift, std::vector<ssc::TimedPosition> time, double resolution);
  std::vector<double> computeTrackingMovement();
  double findCorrelationShift(std::vector<double> frames, std::vector<double> tracking, double resolution);

  void saveDebugFile();

  struct FileData
  {
    ssc::USFrameDataPtr mUsRaw;///<All imported US data frames with pointers to each frame
    std::vector<ssc::TimedPosition> mFrames;
    std::vector<ssc::TimedPosition> mPositions;
  };
  FileData mFileData; ///< original version of loaded data. Use as basis when recalculating due to changed params.
  QString mDebugFile;
  QString mFilename;
  std::stringstream mDebugStream;

};

typedef boost::shared_ptr<TemporalCalibration> TemporalCalibrationPtr;

}

#endif /* CXTEMPORALCALIBRATION_H_ */

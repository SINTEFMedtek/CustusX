/*
 * cxTemporalCalibration.cpp
 *
 *  Created on: May 9, 2011
 *      Author: christiana
 */

#include <cxTemporalCalibration.h>


#include <QtGui>
#include <QVBoxLayout>
#include "boost/bind.hpp"
#include "sscToolManager.h"
#include <vtkDoubleArray.h>
#include <vtkPiecewiseFunction.h>
#include <vtkPointData.h>
#include <vtkImageData.h>
#include "sscTypeConversions.h"
#include "cxPatientData.h"
#include "cxStateMachineManager.h"
#include "cxSettings.h"
#include "cxToolDataAdapters.h"
#include "cxDoubleDataAdapterTemporalCalibration.h"
#include "sscUtilHelpers.h"
#include "sscVolumeHelpers.h"
#include "vtkImageCorrelation.h"
typedef vtkSmartPointer<vtkImageCorrelation> vtkImageCorrelationPtr;
typedef vtkSmartPointer<vtkDoubleArray> vtkDoubleArrayPtr;
#include "sscMessageManager.h"
#include "sscTime.h"

namespace cx
{

typedef unsigned char uchar; // for removing eclipse warnings

vtkImageDataPtr generateVtkImageDataDouble(ssc::Vector3D dim,
    ssc::Vector3D spacing,
    double initValue)
{
  vtkImageDataPtr data = vtkImageDataPtr::New();
  data->SetSpacing(spacing[0], spacing[1], spacing[2]);
  data->SetExtent(0, dim[0]-1, 0, dim[1]-1, 0, dim[2]-1);
  data->SetScalarTypeToDouble();
  data->SetNumberOfScalarComponents(1);

  int scalarSize = dim[0]*dim[1]*dim[2];

  double *rawchars = (double*)malloc((scalarSize+1)*8);
  std::fill(rawchars,rawchars+scalarSize, initValue);

  vtkDoubleArrayPtr array = vtkDoubleArrayPtr::New();
  array->SetNumberOfComponents(1);
  //TODO: Whithout the +1 the volume is black
  array->SetArray(rawchars, scalarSize+1, 0); // take ownership
  data->GetPointData()->SetScalars(array);

  // A trick to get a full LUT in ssc::Image (automatic LUT generation)
  // Can't seem to fix this by calling Image::resetTransferFunctions() after volume is modified
  rawchars[0] = 255;
  data->GetScalarRange();// Update internal data in vtkImageData. Seems like it is not possible to update this data after the volume has been changed.
  rawchars[0] = 0;


  return data;
}

/**
 * Found this on
 * http://paulbourke.net/miscellaneous/correlate/
 * Slightly modified.
 *
 */
void correlate(double* x, double* y, double* corr, int maxdelay, int n)
{
  int i, j;
  double mx, my, sx, sy, sxy, denom, r;
  int delay;

  /* Calculate the mean of the two series x[], y[] */
  mx = 0;
  my = 0;
  for (i = 0; i < n; i++)
  {
    mx += x[i];
    my += y[i];
  }
  mx /= n;
  my /= n;

  /* Calculate the denominator */
  sx = 0;
  sy = 0;
  for (i = 0; i < n; i++)
  {
    sx += (x[i] - mx) * (x[i] - mx);
    sy += (y[i] - my) * (y[i] - my);
  }
  denom = sqrt(sx * sy);

//  std::cout << "raw: " << std::endl;
  /* Calculate the correlation series */
  for (delay = -maxdelay; delay < maxdelay; delay++)
  {
    sxy = 0;
    for (i = 0; i < n; i++)
    {
      j = i + delay;
      if (j < 0 || j >= n)
        continue;
      else
        sxy += (x[i] - mx) * (y[j] - my);
      /* Or should it be (?)
       if (j < 0 || j >= n)
       sxy += (x[i] - mx) * (-my);
       else
       sxy += (x[i] - mx) * (y[j] - my);
       */
    }
    r = sxy / denom;
//    std::cout << sxy << "\t" << sxy/denom << std::endl;
    corr[delay+maxdelay] = r;//(sxy/denom+1) * 128;

    /* r is the correlation coefficient at "delay" */

  }

}

void TemporalCalibration::selectData(QString filename)
{
  mFilename = filename;
  mFileData = FileData();

  if (!QFileInfo(filename).exists())
    return;

  UsReconstructionFileReader fileReader;
  QString mhdFileName = ssc::changeExtension(filename, "mhd");

  //Read US images
  mFileData.mUsRaw = fileReader.readUsDataFile(mhdFileName);

  if (!mFileData.mUsRaw)
  {
    ssc::messageManager()->sendWarning("Failed to load data from " + filename);
    return;
  }

  mFileData.mFrames = fileReader.readFrameTimestamps(filename);
  mFileData.mPositions = fileReader.readPositions(filename);

  ssc::messageManager()->sendInfo("Temporal Calibration: Successfully loaded data from " + filename);
}

void TemporalCalibration::setDebugFile(QString filename)
{
  mDebugFile = filename;
}

void TemporalCalibration::saveDebugFile()
{
  if (mDebugFile.isEmpty())
    return;
//  std::stringstream mDebugStream;
//  QString fullname = QFileInfo(mFilename).dir().absolutePath()+"/"+mDebugFile;
//  QString fullname = "/home/christiana/christiana/"+mDebugFile;
  QFile file(mDebugFile);

  if (!file.open(QIODevice::ReadWrite))
  {
    ssc::messageManager()->sendError("Failed to write file " + file.fileName() + ".");
    return;
  }

  file.write(qstring_cast(mDebugStream.str()).toAscii());
  ssc::messageManager()->sendInfo("Saved temporal calibration details to " + file.fileName());
  file.close();
}


/**Find temporal calibration shift (frames = tracking + shift)
 * from the loaded data series.
 *
 *
 * - use correlation to find shifts between volumes
 *   - use the first image as ref for all others
 *   - as a first: use global maxima to create data
 * - compute the component of the shift in e_z in t space, s(t)
 * - compute tracking data along e_z in t space, q(t).
 * - use correlation to find shift between the two sequences.
 * - the shift is the difference between frame timestamps and tracking timestamps -> temporal calibration.
 * - add this as a delta to the current cal.
 *
 */
double TemporalCalibration::calibrate()
{
  mDebugStream.clear();

  if (!mFileData.mUsRaw)
  {
    ssc::messageManager()->sendWarning("No data loaded");
    return 0;
  }

  mDebugStream << "Temporal Calibration " << QDateTime::currentDateTime().toString(ssc::timestampSecondsFormatNice()) << std::endl;
  mDebugStream << "Loaded data: " << mFilename << std::endl;
  mDebugStream << "=======================================" << std::endl;

  std::vector<double> frameMovement = this->computeProbeMovement();
  std::vector<double> trackingMovement = this->computeTrackingMovement();

  // set a resolution, resample both tracking and frames to that
  double resolution = 10; // ms

  double offset = mFileData.mFrames.front().mTime - mFileData.mPositions.front().mTime;
  std::vector<double> frameMovementRegular = this->resample(frameMovement, mFileData.mFrames, resolution);
  std::vector<double> trackingMovementRegular = this->resample(trackingMovement, mFileData.mPositions, resolution);

  double shift = this->findCorrelationShift(frameMovementRegular, trackingMovementRegular, resolution);

  double totalShift = offset + shift;

  mDebugStream << "=======================================" << std::endl;
  mDebugStream << "Performed temporal calibration:" << std::endl;
  mDebugStream << "offset = " << offset << ", shift = " << shift << std::endl;
  mDebugStream << "Total temporal shift tf-tt = " << offset+shift << " ms" << std::endl;
  mDebugStream << "=======================================" << std::endl;

  this->saveDebugFile();
  return totalShift;
}

/** Find the correlation shift between the regularly spaces series frames and tracking,
 *  with a spacing of resolution.
 *
 *  the returned shift is shift frames-tracking: frame = tracking + shift.
 */
double TemporalCalibration::findCorrelationShift(std::vector<double> frames, std::vector<double> tracking, double resolution)
{
  int N = std::min(tracking.size(), frames.size());
  std::vector<double> result(N, 0);

  correlate(&*frames.begin(), &*tracking.begin(), &*result.begin(), N / 2, N);

  int top = std::distance(result.begin(), std::max_element(result.begin(), result.end()));
  double shift = (N/2-top) * resolution; // convert to shift in ms.

  mDebugStream << "=======================================" << std::endl;
  mDebugStream << "tracking vs frames correlation:" << std::endl;
  mDebugStream << "Temporal resolution " << resolution << " ms" << std::endl;
  mDebugStream << "#frames=" << frames.size() << ", #tracks=" << tracking.size() << std::endl;
  mDebugStream << std::endl;
  mDebugStream << "Frame pos" << "\t" << "Track pos" << "\t" << "correlation" << std::endl;
  for (int x = 0; x < N; ++x)
  {
    mDebugStream << frames[x] << "\t" << tracking[x] << "\t" << result[x] << std::endl;
  }

  mDebugStream << std::endl;
  mDebugStream << "corr top: " << top << ", = shift in ms: " << shift << std::endl;
  mDebugStream << "=======================================" << std::endl;

  return shift; // shift frames-tracking: frame = tracking + shift
}

/**resample the time+shift function onto a regular time series given by resolution.
 *
 *///  TemporalCalibration();
//  virtual ~TemporalCalibration();

std::vector<double> TemporalCalibration::resample(std::vector<double> shift, std::vector<ssc::TimedPosition> time, double resolution)
{
  if (shift.size()!=time.size())
  {
    ssc::messageManager()->sendError("Assert failure, shift and time different sizes");
  }

//  std::cout << "resample " << std::endl;
  vtkPiecewiseFunctionPtr frames = vtkPiecewiseFunctionPtr::New();
  for (unsigned i=0; i<shift.size(); ++i)
  {
//    std::cout << i << "\t" << time[i].mTime << "\t" << shift[i] << std::endl;

    frames->AddPoint(time[i].mTime, shift[i]);
  }
  double r0, r1;
  frames->GetRange(r0, r1);
  double range = r1-r0;
  int N_r = range/resolution;
//  std::cout << "range " << r0 << "\t" << r1 << "\t" << N_r << std::endl;

  std::vector<double> framesRegular;
  for (int i=0; i<N_r; ++i)
    framesRegular.push_back(frames->GetValue(r0 + i*resolution));

  return framesRegular;
}

std::vector<double> TemporalCalibration::computeTrackingMovement()
{
  std::vector<double> retval;
  ssc::Vector3D e_z(0,0,1);
  ssc::Vector3D origin(0,0,0);
  double zero = 0;

  for (unsigned i=0; i<mFileData.mPositions.size(); ++i)
  {
    ssc::Transform3D prMt = mFileData.mPositions[i].mPos;
    ssc::Vector3D p_pr = prMt.coord(origin);

//    std::cout << i << "\t" << p_pr << std::endl;

    ssc::Vector3D ez_pr = prMt.vector(e_z);
    double val = dot(ez_pr, p_pr);

    if (retval.empty())
      zero = val;

    retval.push_back(val-zero);
  }

  return retval;
}

/** Calculate offset values from the first frame for all frames.
 *
 */
std::vector<double> TemporalCalibration::computeProbeMovement()
{
  int N_frames = mFileData.mUsRaw->getDimensions()[2];

//  std::cout << "max correlation for each frame:" << std::endl;
  std::vector<double> retval;

  for (int i=0; i<N_frames; ++i)
  {
    double val = this->findCorrelation(mFileData.mUsRaw, 0, i, 1000);
//    std::cout << i << "\t" << val << std::endl;
    retval.push_back(val);
  }

//  int N_frames = mFileData.mUsRaw->getDimensions()[2];
//
//  //for (int frame=0; frame<N_frames; ++frame)
//  for (int frame=0; frame<2; ++frame)
//  {
//    vtkImageDataPtr line = this->extractLine_y(mFileData.mUsRaw, line_index_x, frame);
//  }
//  return TimeSeriesType();
  return retval;
}

double TemporalCalibration::findCorrelation(ssc::USFrameDataPtr data, int frame_a, int frame_b, int maxShift)
{
//  int N_frames = mFileData.mUsRaw->getDimensions()[2];
  int line_index_x = mFileData.mUsRaw->getDimensions()[0]/2;

//  std::vector<double>
  vtkImageDataPtr line1 = this->extractLine_y(mFileData.mUsRaw, line_index_x, frame_a);
  vtkImageDataPtr line2 = this->extractLine_y(mFileData.mUsRaw, line_index_x, frame_b);

//  vtkImageCorrelationPtr correlator = vtkImageCorrelationPtr::New();
//  correlator->SetDimensionality(1);
//  correlator->SetInput1(line1);
//  correlator->SetInput2(line2);
//  correlator->Update();
//  vtkImageDataPtr result = correlator->GetOutput();
//  std::cout << "got a result with dim " << ssc::Vector3D(result->GetDimensions()) << std::endl;

//  int N = result->GetDimensions()[0];
  int N = line1->GetDimensions()[0];
  std::vector<double> result(N, 0);
  double* line_a = static_cast<double*>(line1->GetScalarPointer());
  double* line_b = static_cast<double*>(line2->GetScalarPointer());
//  double* line_c = static_cast<double*>(result->GetScalarPointer());
  double* line_c = &*result.begin();

  correlate(line_a, line_b, line_c, N/2, N);

//  for (int x=0; x<N; ++x)
//  {
//    std::cout << line_a[x] << "\t" << line_b[x] << "\t" << line_c[x] << std::endl;
//  }

//  //for (int frame=0; frame<N_frames; ++frame)
//  for (int frame=0; frame<2; ++frame)
//  {
//    vtkImageDataPtr line = this->extractLine_y(mFileData.mUsRaw, line_index_x, frame);
//  }
  int top = std::distance(result.begin(), std::max_element(result.begin(), result.end()));
//  top = top - N_2;
  double hit = (N/2-top) * mFileData.mUsRaw->getSpacing()[1]; // convert to downwards movement in mm.
//  return 0;
  return hit;
}

/**extract the y-line with x-index line_index_x from frame ( data[line_index_x, y_varying, frame] )
 *
 */
vtkImageDataPtr TemporalCalibration::extractLine_y(ssc::USFrameDataPtr data, int line_index_x, int frame)
{
//  std::cout << "extract pre" << std::endl;
//  vtkImageDataPtr retval = vtkImageDataPtr::New();
//  int N = data->getDimensions()[1];
  int dimX = data->getDimensions()[0];
  int dimY = data->getDimensions()[1];

  vtkImageDataPtr retval = generateVtkImageDataDouble(ssc::Vector3D(dimY, 1, 1), ssc::Vector3D(1,1,1), 1);

  uchar* source = data->getFrame(frame);

//  int* dim(retval->GetDimensions());
  double* dest = static_cast<double*>(retval->GetScalarPointer());

//  std::cout << "extract begin" << std::endl;

  for (int y=0; y<dimY; ++y)
  {
    dest[y] = source[y*dimX + line_index_x];
//    std::cout << (int)dest[y] << std::endl;
  }

//  std::cout << "extract end" << std::endl;

  return retval;
}


//void TemporalCalibration::computeTemporalCalibration(ssc::RTSourceRecorder::DataType volumes, ssc::TimedTransformMap tracking, ssc::ToolPtr probe)
//{
//
//  ssc::RTSourceRecorder::DataType corrTemp;
//  ssc::RTSourceRecorder::DataType::iterator i = volumes.begin();
//  ssc::RTSourceRecorder::DataType::iterator j = i;
//  ++j;
//  for ( ; j!=volumes.end(); ++j, ++i)
//  {
//    vtkImageCorrelationPtr correlator = vtkImageCorrelationPtr::New();
//    correlator->SetInput1(i->second);
//    correlator->SetInput2(j->second);
//    correlator->Update();
//    vtkImageDataPtr result = correlator->GetOutput();
//    corrTemp[j->first] = result;
//  }
//
////  mFileMaker.reset(new UsReconstructionFileMaker(tracking, corrTemp, "corr_test", stateManager()->getPatientData()->getActivePatientFolder(), probe));
////  mFileMaker->write();
////  std::cout << "completed write of correlation results" << std::endl;
//}

///**Called when data is loaded into reconstructer.
// *
// */
//void ReconstructionWidget::inputDataSelected(QString mhdFileName)
//{
//  if(mReconstructer->getSelectedData().isEmpty())
//  {
//    return;
//  }
//
//  mFileSelectWidget->setFilename(mhdFileName);
//}

//void TemporalCalibrationWidget::fileMakerWriteFinished()
//{
//  QString targetFolder = mFileMakerFutureWatcher.future().result();
//  //stateManager()->getReconstructer()->selectData(mFileMaker->getMhdFilename(targetFolder));
//
//  mRTRecorder.reset(new ssc::RTSourceRecorder(mRTSource));
//
//  RecordSessionPtr session = stateManager()->getRecordSession(mLastSession);
//  ssc::RTSourceRecorder::DataType streamRecordedData = mRTRecorder->getRecording(session->getStartTime(), session->getStopTime());
//  ssc::TimedTransformMap trackerRecordedData = this->getRecording(session);
//  if(trackerRecordedData.empty())
//  {
//    ssc::messageManager()->sendError("Could not find any tracking data from session "+mLastSession+". Ignoring.");
//    return;
//  }
//  ssc::ToolPtr probe = this->getTool();
//
//  this->computeTemporalCalibration(streamRecordedData, trackerRecordedData, probe);
//}






}//namespace cx



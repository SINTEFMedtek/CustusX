/*
 * cxTemporalCalibration.cpp
 *
 *  \date May 9, 2011
 *      \author christiana
 */

#include <cxTemporalCalibration.h>


#include <QtGui>
#include <QVBoxLayout>
#include "boost/bind.hpp"
#include "sscToolManager.h"
#include <vtkPiecewiseFunction.h>
#include <vtkPointData.h>
#include <vtkImageData.h>
#include "sscTypeConversions.h"
#include "cxSettings.h"
#include "sscUtilHelpers.h"
#include "sscVolumeHelpers.h"
#include "vtkImageCorrelation.h"
typedef vtkSmartPointer<vtkImageCorrelation> vtkImageCorrelationPtr;

#include "sscMessageManager.h"
#include "sscTime.h"
#include <vtkImageMask.h>
typedef vtkSmartPointer<vtkImageMask> vtkImageMaskPtr;

namespace cx
{

typedef unsigned char uchar; // for removing eclipse warnings



/**
 * Found this on
 * http://paulbourke.net/miscellaneous/correlate/
 * Slightly modified.
 *
 * x: first input series, size n
 * y: second input series, size n
 * corr: correlation result, size maxdelay*2 (zero shif is found at corr[maxdelay]
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

TemporalCalibration::TemporalCalibration()
{
	mAddRawToDebug = false;
}

void TemporalCalibration::selectData(QString filename)
{
  mFilename = filename;
  mFileData = ssc::USReconstructInputData();

  if (!QFileInfo(filename).exists())
    return;

  UsReconstructionFileReader fileReader;
  mFileData = fileReader.readAllFiles(filename);

  if (!mFileData.mUsRaw)
  {
    ssc::messageManager()->sendWarning("Failed to load data from " + filename);
    return;
  }

  ssc::messageManager()->sendInfo("Temporal Calibration: Successfully loaded data from " + filename);
}

void TemporalCalibration::setDebugFolder(QString filename)
{
	mDebugFolder = filename;
}

void TemporalCalibration::saveDebugFile()
{
  if (mDebugFolder.isEmpty())
    return;

  QString dbFilename = mDebugFolder +"/"+ QFileInfo(mFilename).baseName() + "_temporal_calib.txt";
//  std::stringstream mDebugStream;
//  QString fullname = QFileInfo(mFilename).dir().absolutePath()+"/"+mDebugFile;
//  QString fullname = "/home/christiana/christiana/"+mDebugFile;
  QFile file(dbFilename);
  file.remove();

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
double TemporalCalibration::calibrate(bool* success)
{
  mDebugStream.str("");
  mDebugStream.clear();

  if (!mFileData.mUsRaw)
  {
    ssc::messageManager()->sendWarning("Temporal calib: No data loaded");
    return 0;
  }
  if (mFileData.mPositions.empty())
  {
    ssc::messageManager()->sendWarning("Temporal calib: Missing tracking data.");
    return 0;
  }

  mDebugStream << "Temporal Calibration " << QDateTime::currentDateTime().toString(ssc::timestampSecondsFormatNice()) << std::endl;
  mDebugStream << "Loaded data: " << mFilename << std::endl;
  mDebugStream << "=======================================" << std::endl;

  mFileData.mUsRaw->initializeFrames();

  std::vector<double> frameMovement = this->computeProbeMovement();

  if (!this->checkFrameMovementQuality(frameMovement))
  {
	  ssc::messageManager()->sendError("Failed to detect movement in images. Make sure that the first image is clear and visible.");
	  *success = false;
	  return 0;
  }


  std::vector<double> trackingMovement = this->computeTrackingMovement();

  // set a resolution, resample both tracking and frames to that
  double resolution = 5; // ms

  double offset = mFileData.mFrames.front().mTime - mFileData.mPositions.front().mTime;
  std::vector<double> frameMovementRegular = this->resample(frameMovement, mFileData.mFrames, resolution);
  std::vector<double> trackingMovementRegular = this->resample(trackingMovement, mFileData.mPositions, resolution);

//  double shift = this->findCorrelationShift(frameMovementRegular, trackingMovementRegular, resolution);
  double shift = this->findLSShift(frameMovementRegular, trackingMovementRegular, resolution);

  double totalShift = offset + shift;

  mDebugStream << "=======================================" << std::endl;
  mDebugStream << "Performed temporal calibration:" << std::endl;
  mDebugStream << "offset = " << offset << ", shift = " << shift << std::endl;
  mDebugStream << "Total temporal shift tf-tt = " << offset+shift << " ms" << std::endl;
  mDebugStream << "=======================================" << std::endl;

  double startTime = mFileData.mPositions.front().mTime;
  this->writePositions("Tracking", trackingMovement, mFileData.mPositions, startTime);
  this->writePositions("Frames", frameMovement, mFileData.mFrames, startTime);
  this->writePositions("Shifted Frames", frameMovement, mFileData.mFrames, startTime + totalShift);


  this->saveDebugFile();
  *success = true;
  return totalShift;
}

bool TemporalCalibration::checkFrameMovementQuality(std::vector<double> pos)
{
	int count = 0;
	for (unsigned i=0; i<pos.size(); ++i)
		if (ssc::similar(pos[i], 0))
			count++;

	// accept if less than 20% zeros.
	double error = double(count)/pos.size();
	if (error > 0.05)
		ssc::messageManager()->sendWarning(QString("Found %1 \% zeros in frame movement").arg(error*100));
	return error < 0.2;
}

/** shift tracking data with the input shift, then compute RMS value of function
 *
 */
double TemporalCalibration::findLeastSquares(std::vector<double> frames, std::vector<double> tracking, int shift) const
{
	int r0 = 0;
	int r1 = frames.size();

	r0 = std::max<int>(r0, - shift);
	r1 = std::min<int>(r1, tracking.size() - shift);
//	std::cout << "range " << r0 << " " << r1 << " shift="<< shift << std::endl;

	double value = 0;

	for (int i=r0; i<r1; ++i)
	{
		double core = pow(frames[i] - tracking[i+shift], 2.0);
		value += core;
	}
	value /= (r1-r0);
	value = sqrt(value);
	return value;
}

/** Find the correlation shift between the regularly spaces series frames and tracking,
 *  with a spacing of resolution.
 *
 *  the returned shift is shift frames-tracking: frame = tracking + shift.
 */
double TemporalCalibration::findLSShift(std::vector<double> frames, std::vector<double> tracking, double resolution) const
{
	double maxShift = 1000;
  int N = std::min(tracking.size(), frames.size());
  N = std::min<int>(N, 2*maxShift/resolution); // constrain search to 1 second in each direction
  std::vector<double> result(N, 0);
  int W = N/2;

//	std::cout << "0 " << this->findLeastSquares(frames, tracking, 0) << std::endl;
//	std::cout << "1 " << this->findLeastSquares(frames, tracking, 100) << std::endl;
//	std::cout << "2 " << this->findLeastSquares(frames, tracking, 200) << std::endl;
//	std::cout << "3 " << this->findLeastSquares(frames, tracking, 300) << std::endl;

  for (int i=-W; i<W; ++i)
  {
  	double rms = this->findLeastSquares(frames, tracking, i);
  	result[i+W] = rms;
  }

  int top = std::distance(result.begin(), std::min_element(result.begin(), result.end()));
  double shift = (W-top) * resolution; // convert to shift in ms.

//
//  correlate(&*frames.begin(), &*tracking.begin(), &*result.begin(), N / 2, N);
//
//  int top = std::distance(result.begin(), std::max_element(result.begin(), result.end()));
//  double shift = (N/2-top) * resolution; // convert to shift in ms.

  mDebugStream << "=======================================" << std::endl;
  mDebugStream << "tracking vs frames fit using least squares:" << std::endl;
  mDebugStream << "Temporal resolution " << resolution << " ms" << std::endl;
  mDebugStream << "Max shift " << maxShift << " ms" << std::endl;
  mDebugStream << "#frames=" << frames.size() << ", #tracks=" << tracking.size() << std::endl;
  mDebugStream << std::endl;
  mDebugStream << "Frame pos" << "\t" << "Track pos" << "\t" << "RMS(center=" << W << ")"	<< std::endl;
  for (int x = 0; x < std::min<int>(tracking.size(), frames.size()); ++x)
  {
    mDebugStream << frames[x] << "\t" << tracking[x];
    if (x<N)
    	mDebugStream << "\t" << result[x];
  	mDebugStream << std::endl;
  }

  mDebugStream << std::endl;
  mDebugStream << "minimal index: " << top << ", = shift in ms: " << shift << std::endl;
  mDebugStream << "=======================================" << std::endl;

  return shift; // shift frames-tracking: frame = tracking + shift
}

/** Find the correlation shift between the regularly spaces series frames and tracking,
 *  with a spacing of resolution.
 *
 *  the returned shift is shift frames-tracking: frame = tracking + shift.
 */
double TemporalCalibration::findCorrelationShift(std::vector<double> frames, std::vector<double> tracking, double resolution) const
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


void TemporalCalibration::writePositions(QString title, std::vector<double> pos, std::vector<ssc::TimedPosition> time, double shift)
{
	if (pos.size()!=time.size())
	{
		std::cout << "size mismatch" << std::endl;
		return;
	}

	mDebugStream << title << std::endl;
	mDebugStream << "time\t" << "pos\t" << std::endl;
	for (unsigned i=0; i<time.size(); ++i)
	{
		mDebugStream << time[i].mTime - shift << "\t" << pos[i] << std::endl;
	}
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
  ssc::Transform3D prM0t = mFileData.mPositions[0].mPos;
  ssc::Vector3D ez_pr = prM0t.vector(e_z);

  for (unsigned i=0; i<mFileData.mPositions.size(); ++i)
  {
    ssc::Transform3D prMt = mFileData.mPositions[i].mPos;
    ssc::Vector3D p_pr = prMt.coord(origin);

    double val = ssc::dot(ez_pr, p_pr);

    if (retval.empty())
      zero = val;

    retval.push_back(val-zero);
//    if (i<50)
//    	std::cout << i << "\t" << p_pr <<  "\t"  <<ez_pr << "\t" << val << "\t" << val-zero << std::endl;
  }

  if (mAddRawToDebug)
  {
		mDebugStream << "=======================================" << std::endl;
		mDebugStream << "tracking raw data:" << std::endl;
		mDebugStream << std::endl;
		mDebugStream << "timestamp" << "\t" << "pos" << std::endl;
		for (unsigned x = 0; x < mFileData.mPositions.size(); ++x)
		{
			mDebugStream << mFileData.mPositions[x].mTime << "\t" << retval[x] << std::endl;
		}
		mDebugStream << std::endl;
		mDebugStream << "=======================================" << std::endl;
//		std::cout << mDebugStream.str() << std::endl;
  }

  return retval;
}

/** Calculate offset values from the first frame for all frames.
 *
 */
std::vector<double> TemporalCalibration::computeProbeMovement()
{
  int N_frames = mFileData.mUsRaw->getDimensions()[2];

  std::vector<double> retval;

  double maxSingleStep = 5; // assume max 5mm movement per frame
  double currentMaxShift = 5;
  double lastVal = 0;

  for (int i=0; i<N_frames; ++i)
  {
    double val = this->findCorrelation(mFileData.mUsRaw, 0, i, maxSingleStep, lastVal);
    currentMaxShift =  fabs(val) + maxSingleStep;
    lastVal = val;
    retval.push_back(val);
  }

  if (mAddRawToDebug)
  {
		mDebugStream << "=======================================" << std::endl;
		mDebugStream << "frames raw data:" << std::endl;
		mDebugStream << std::endl;
		mDebugStream << "timestamp" << "\t" << "shift" << std::endl;
		for (int x = 0; x < N_frames; ++x)
		{
			mDebugStream << mFileData.mFrames[x].mTime << "\t" << retval[x] << std::endl;
		}
		mDebugStream << std::endl;
		mDebugStream << "=======================================" << std::endl;
  }

  return retval;
}

double TemporalCalibration::findCorrelation(ssc::USFrameDataPtr data, int frame_a, int frame_b, double maxShift, double lastVal)
{
	int maxShift_pix = maxShift / mFileData.mUsRaw->getSpacing()[1];
	int lastVal_pix = lastVal / mFileData.mUsRaw->getSpacing()[1];

//  int N_frames = mFileData.mUsRaw->getDimensions()[2];
//  int line_index_x = mFileData.mUsRaw->getDimensions()[0]/2;
  int line_index_x = mFileData.mProbeData.mData.getImage().mOrigin_p[0];

  int dimY = mFileData.mUsRaw->getDimensions()[1];

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
//  int N = dimY;
//  N = maxShift_pix*2; //result vector allocate space on both sides of zero
  int N = 2*dimY; //result vector allocate space on both sides of zero
//  N = 2*N; /// use double the space in order to accept more movement than half the interval.
  std::vector<double> result(N, 0);
  double* line_a = static_cast<double*>(line1->GetScalarPointer());
  double* line_b = static_cast<double*>(line2->GetScalarPointer());
  double* line_c = &*result.begin();

  correlate(line_a, line_b, line_c, N/2, dimY);

  // use the last found hit as a seed for looking for a local maximum
  int lastTop = N/2 - lastVal_pix;
  std::pair<int,int> range(lastTop - maxShift_pix, lastTop+maxShift_pix);
  range.first = std::max(0, range.first);
  range.second = std::min(N, range.second);

  // look for a max in the vicinity of the last hit
  int top = std::distance(result.begin(), std::max_element(result.begin()+range.first, result.begin()+range.second));

  double hit = (N/2-top) * mFileData.mUsRaw->getSpacing()[1]; // convert to downwards movement in mm.

//  std::cout << "frame_b=" << frame_b << " N=" << N << " lasttop=" << lastTop << " r0=" << range.first << " r1=" << range.second << " top=" << top << " hit=" << hit<< std::endl;
//
//  if (frame_b==46)
//  {
//		mDebugStream << "=======================================" << std::endl;
//		mDebugStream << "frame correlation raw data:" << std::endl;
//		mDebugStream << std::endl;
//		mDebugStream << "A=" << frame_a << "\t" << "B=" << frame_b << "\t" << "result (size=" << N << ")" << std::endl;
//		for (int x = 0; x < dimY; ++x)
//		{
//			mDebugStream << line_a[x] << "\t" << line_b[x];
//			if (int(x)<N)
//				mDebugStream << "\t" << result[x];
//			mDebugStream << std::endl;
//		}
//		mDebugStream << "top: " << top << std::endl;
//		mDebugStream << "hit: " << hit << std::endl;
//		std::cout << "hit: " << hit << std::endl;
//		mDebugStream << std::endl;
//		mDebugStream << "=======================================" << std::endl;
//  }

  return hit;
}

/**extract the y-line with x-index line_index_x from frame ( data[line_index_x, y_varying, frame] )
 *
 */
vtkImageDataPtr TemporalCalibration::extractLine_y(ssc::USFrameDataPtr data, int line_index_x, int frame)
{
  int dimX = data->getDimensions()[0];
  int dimY = data->getDimensions()[1];

  vtkImageDataPtr retval = ssc::generateVtkImageDataDouble(Eigen::Array3i(dimY, 1, 1), ssc::Vector3D(1,1,1), 1);

  /// convert one frame to a vtkImageData: base
  uchar* raw_source = data->getFrame(frame);
  vtkImageDataPtr base = ssc::generateVtkImageData(Eigen::Array3i(dimX, dimY, 1), ssc::Vector3D(1,1,1), 0);
  uchar* base_ptr = static_cast<uchar*>(base->GetScalarPointer());
  std::copy(raw_source, raw_source+dimX*dimY, base_ptr);

  // run the base frame through the mask. Result is source.
  vtkImageMaskPtr maskFilter = vtkImageMaskPtr::New();
  maskFilter->SetMaskInput(mFileData.mMask->getBaseVtkImageData());
  maskFilter->SetImageInput(base);
  maskFilter->SetMaskedOutputValue(0.0);
  maskFilter->Update();
  uchar* source = static_cast<uchar*>(maskFilter->GetOutput()->GetScalarPointer());

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







}//namespace cx



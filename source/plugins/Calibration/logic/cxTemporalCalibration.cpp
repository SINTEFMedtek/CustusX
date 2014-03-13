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
#include "cxToolManager.h"
#include <vtkPiecewiseFunction.h>
#include <vtkPointData.h>
#include <vtkImageData.h>
#include "cxTypeConversions.h"
#include "cxSettings.h"
#include "cxUtilHelpers.h"
#include "cxVolumeHelpers.h"
#include "vtkImageCorrelation.h"
#include "cxUSFrameData.h"
#include "cxImage.h"
#include "cxUsReconstructionFileReader.h"
#include "cxMessageManager.h"
#include "cxTime.h"
#include <vtkImageMask.h>

typedef vtkSmartPointer<vtkImageMask> vtkImageMaskPtr;
typedef vtkSmartPointer<vtkImageCorrelation> vtkImageCorrelationPtr;

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
    }
    r = sxy / denom;
    corr[delay+maxdelay] = r;//(sxy/denom+1) * 128;

    /* r is the correlation coefficient at "delay" */

  }

}

TemporalCalibration::TemporalCalibration()
{
	mAddRawToDebug = false;
	mMask = vtkImageDataPtr();
}

void TemporalCalibration::selectData(QString filename)
{
  mFilename = filename;
  mFileData = USReconstructInputData();

  if (!QFileInfo(filename).exists())
    return;

  UsReconstructionFileReader fileReader;
  mFileData = fileReader.readAllFiles(filename);

  if (!mFileData.mUsRaw)
  {
    messageManager()->sendWarning("Failed to load data from " + filename);
    return;
  }

  messageManager()->sendInfo("Temporal Calibration: Successfully loaded data from " + filename);
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
  QFile file(dbFilename);
  file.remove();

  if (!file.open(QIODevice::ReadWrite))
  {
    messageManager()->sendError("Failed to write file " + file.fileName() + ".");
    return;
  }

  file.write(qstring_cast(mDebugStream.str()).toAscii());
  messageManager()->sendInfo("Saved temporal calibration details to " + file.fileName());
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
    messageManager()->sendWarning("Temporal calib: No data loaded");
    return 0;
  }
  if (mFileData.mPositions.empty())
  {
    messageManager()->sendWarning("Temporal calib: Missing tracking data.");
    return 0;
  }

  mDebugStream << "Temporal Calibration " << QDateTime::currentDateTime().toString(timestampSecondsFormatNice()) << std::endl;
  mDebugStream << "Loaded data: " << mFilename << std::endl;
  mDebugStream << "=======================================" << std::endl;

  mProcessedFrames = mFileData.mUsRaw->initializeFrames(std::vector<bool>(1, false)).front();

  std::vector<double> frameMovement = this->computeProbeMovement();

  if (!this->checkFrameMovementQuality(frameMovement))
  {
	  messageManager()->sendError("Failed to detect movement in images. Make sure that the first image is clear and visible.");
	  *success = false;
	  return 0;
  }


  std::vector<double> trackingMovement = this->computeTrackingMovement();

  // set a resolution, resample both tracking and frames to that
  double resolution = 5; // ms

  double offset = mFileData.mFrames.front().mTime - mFileData.mPositions.front().mTime;
  std::vector<double> frameMovementRegular = this->resample(frameMovement, mFileData.mFrames, resolution);
  std::vector<double> trackingMovementRegular = this->resample(trackingMovement, mFileData.mPositions, resolution);

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
		if (similar(pos[i], 0))
			count++;

	// accept if less than 20% zeros.
	double error = double(count)/pos.size();
	if (error > 0.05)
		messageManager()->sendWarning(QString("Found %1 % zeros in frame movement").arg(error*100));
	return error < 0.2;
}

/** shift tracking data with the input shift, then compute RMS value of function
 *
 */
double TemporalCalibration::findLeastSquares(std::vector<double> frames, std::vector<double> tracking, int shift) const
{
	size_t r0 = 0;
	size_t r1 = frames.size();

	r0 = std::max<int>(r0, - shift);
	r1 = std::min<int>(r1, tracking.size() - shift);

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
	size_t N = std::min(tracking.size(), frames.size());
  N = std::min<int>(N, 2*maxShift/resolution); // constrain search to 1 second in each direction
  std::vector<double> result(N, 0);
  int W = N/2;

  for (int i=-W; i<W; ++i)
  {
  	double rms = this->findLeastSquares(frames, tracking, i);
  	result[i+W] = rms;
  }

  int top = std::distance(result.begin(), std::min_element(result.begin(), result.end()));
  double shift = (W-top) * resolution; // convert to shift in ms.

  mDebugStream << "=======================================" << std::endl;
  mDebugStream << "tracking vs frames fit using least squares:" << std::endl;
  mDebugStream << "Temporal resolution " << resolution << " ms" << std::endl;
  mDebugStream << "Max shift " << maxShift << " ms" << std::endl;
  mDebugStream << "#frames=" << frames.size() << ", #tracks=" << tracking.size() << std::endl;
  mDebugStream << std::endl;
  mDebugStream << "Frame pos" << "\t" << "Track pos" << "\t" << "RMS(center=" << W << ")"	<< std::endl;
	for (size_t x = 0; x < std::min<int>(tracking.size(), frames.size()); ++x)
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
	size_t N = std::min(tracking.size(), frames.size());
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


void TemporalCalibration::writePositions(QString title, std::vector<double> pos, std::vector<TimedPosition> time, double shift)
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
 */
std::vector<double> TemporalCalibration::resample(std::vector<double> shift, std::vector<TimedPosition> time, double resolution)
{
  if (shift.size()!=time.size())
  {
    messageManager()->sendError("Assert failure, shift and time different sizes");
  }

  vtkPiecewiseFunctionPtr frames = vtkPiecewiseFunctionPtr::New();
  for (unsigned i=0; i<shift.size(); ++i)
  {
    frames->AddPoint(time[i].mTime, shift[i]);
  }
  double r0, r1;
  frames->GetRange(r0, r1);
  double range = r1-r0;
  int N_r = range/resolution;

  std::vector<double> framesRegular;
  for (int i=0; i<N_r; ++i)
    framesRegular.push_back(frames->GetValue(r0 + i*resolution));

  return framesRegular;
}

std::vector<double> TemporalCalibration::computeTrackingMovement()
{
  std::vector<double> retval;
  Vector3D e_z(0,0,1);
  Vector3D origin(0,0,0);
  double zero = 0;
  Transform3D prM0t = mFileData.mPositions[0].mPos;
  Vector3D ez_pr = prM0t.vector(e_z);

  for (unsigned i=0; i<mFileData.mPositions.size(); ++i)
  {
    Transform3D prMt = mFileData.mPositions[i].mPos;
    Vector3D p_pr = prMt.coord(origin);

    double val = dot(ez_pr, p_pr);

    if (retval.empty())
      zero = val;

    retval.push_back(val-zero);
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

	mMask = mFileData.getMask();
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

double TemporalCalibration::findCorrelation(USFrameDataPtr data, int frame_a, int frame_b, double maxShift, double lastVal)
{
	int maxShift_pix = maxShift / mFileData.mUsRaw->getSpacing()[1];
	int lastVal_pix = lastVal / mFileData.mUsRaw->getSpacing()[1];

	int line_index_x = mFileData.mProbeData.mData.getOrigin_p()[0];

  int dimY = mFileData.mUsRaw->getDimensions()[1];

	vtkImageDataPtr line1 = this->extractLine_y(mFileData.mUsRaw, line_index_x, frame_a);
	vtkImageDataPtr line2 = this->extractLine_y(mFileData.mUsRaw, line_index_x, frame_b);

  int N = 2*dimY; //result vector allocate space on both sides of zero
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

  return hit;
}

/**extract the y-line with x-index line_index_x from frame ( data[line_index_x, y_varying, frame] )
 *
 */
vtkImageDataPtr TemporalCalibration::extractLine_y(USFrameDataPtr data, int line_index_x, int frame)
{
  int dimX = data->getDimensions()[0];
  int dimY = data->getDimensions()[1];

  vtkImageDataPtr retval = generateVtkImageDataDouble(Eigen::Array3i(dimY, 1, 1), Vector3D(1,1,1), 1);

  vtkImageDataPtr base = mProcessedFrames[frame];

  // run the base frame through the mask. Result is source.
  vtkImageMaskPtr maskFilter = vtkImageMaskPtr::New();
	maskFilter->SetMaskInput(mMask);
  maskFilter->SetImageInput(base);
  maskFilter->SetMaskedOutputValue(0.0);
  maskFilter->Update();
  uchar* source = static_cast<uchar*>(maskFilter->GetOutput()->GetScalarPointer());

  double* dest = static_cast<double*>(retval->GetScalarPointer());

  for (int y=0; y<dimY; ++y)
  {
    dest[y] = source[y*dimX + line_index_x];
  }

  return retval;
}







}//namespace cx



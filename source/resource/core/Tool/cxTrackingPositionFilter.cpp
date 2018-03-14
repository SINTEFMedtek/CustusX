/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxTrackingPositionFilter.h"
#include "iir/Butterworth.h"

namespace cx
{

TrackingPositionFilter::TrackingPositionFilter()
{
	mCutOffFrequency = 3;
	mResampleFrequency = 100;
	this->reset();
}

void TrackingPositionFilter::setCutOffFrequency(double freq)
{
	mCutOffFrequency = freq;
	this->reset();
}

void TrackingPositionFilter::addPosition(Transform3D pos, double timestamp)
{
	this->clearIfTimestampIsOlderThanHead(pos, timestamp);
	this->clearIfJumpInTimestamps(pos, timestamp);

	if (mResampled.empty())
	{
		mResampled[timestamp] = pos;
		mHistory[timestamp] = pos;
		return;
	}

	this->interpolateAndFilterPositions(pos, timestamp);
	mHistory[timestamp] = pos;
}

Transform3D TrackingPositionFilter::getFilteredPosition()
{
	if (mFiltered.size() > mResampleFrequency) //check if mFiltered contains enough positions for the filter to be stable
		return mFiltered.rbegin()->second;
	else if (!mHistory.empty())
		return mHistory.rbegin()->second;
	else
		return Transform3D::Identity();
}

void TrackingPositionFilter::clearIfTimestampIsOlderThanHead(Transform3D pos, double timestamp)
{
	if (mResampled.empty())
		return;

	if (timestamp < mResampled.rbegin()->first)
	{
		// clear history if old timestamps appear
		this->reset();
	}
}

void TrackingPositionFilter::clearIfJumpInTimestamps(Transform3D pos, double timestamp)
{
	if (mResampled.empty())
		return;

	double timeStep = timestamp - mResampled.rbegin()->first;
	if ( timeStep > 1000)
	{
		// clear history of resampled and filtered data if jump in timestamps of more than 1 second
		this->reset();
	}
}

void TrackingPositionFilter::interpolateAndFilterPositions(Transform3D pos, double timestamp)
{
	Transform3D previousPositionMatrix = mHistory.rbegin()->second;
	double deltaT = timestamp - mHistory.rbegin()->first; //time from previous measured position to this position
	int numberOfInterpolationPoints = floor( (timestamp - mResampled.rbegin()->first)/1000 * mResampleFrequency ); // interpolate from last resampled position to current measured position
	Transform3D interpolatedPosition;
	Transform3D filteredPosition;
	for (int i=0; i < numberOfInterpolationPoints; i++)
	{
		double resampledTimestamp = mResampled.rbegin()->first + 1000/mResampleFrequency;
		double deltaTpast = resampledTimestamp - mHistory.rbegin()->first;
		double deltaTfuture = timestamp - resampledTimestamp;
		interpolatedPosition = pos.matrix() * deltaTpast/deltaT + previousPositionMatrix.matrix() * deltaTfuture/deltaT; // linear interpolation between previous and current measured position
		mResampled[resampledTimestamp] = interpolatedPosition;

		filteredPosition = interpolatedPosition;
		filteredPosition(0,3) = fx.filter(interpolatedPosition(0,3));
		filteredPosition(1,3) = fy.filter(interpolatedPosition(1,3));
		filteredPosition(2,3) = fz.filter(interpolatedPosition(2,3));
		mFiltered[resampledTimestamp] = filteredPosition;
	}
}

void TrackingPositionFilter::reset()
{
	mHistory.clear();
	mResampled.clear();
	mFiltered.clear();

	fx.setup (mFilterOrder, mResampleFrequency, mCutOffFrequency);  // Lag perker isteden
	fx.reset ();
	fy.setup (mFilterOrder, mResampleFrequency, mCutOffFrequency);
	fy.reset ();
	fz.setup (mFilterOrder, mResampleFrequency, mCutOffFrequency);
	fz.reset ();
}


} // namespace cx



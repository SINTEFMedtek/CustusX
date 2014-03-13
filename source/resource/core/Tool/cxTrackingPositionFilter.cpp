// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.
#include "cxTrackingPositionFilter.h"
#include "iir/Butterworth.h"

namespace cx
{

TrackingPositionFilter::TrackingPositionFilter()
{
	mCutOffFrequency = 3;
	mResampleFrequency = 100;

	fx.setup (mFilterOrder, mResampleFrequency, mCutOffFrequency);  // Lag perker isteden
	fx.reset ();
	fy.setup (mFilterOrder, mResampleFrequency, mCutOffFrequency);
	fy.reset ();
	fz.setup (mFilterOrder, mResampleFrequency, mCutOffFrequency);
	fz.reset ();
}

void TrackingPositionFilter::addPosition(Transform3D pos, double timestamp)
{

	this->clearIfTimestampIsOlderThanHead(pos, timestamp);
	this->clearIfJumpInTimestamps(pos, timestamp);

	if (mResampled.empty()){
		mResampled[timestamp] = pos;
		mHistory[timestamp] = pos;
		return;
	}

	this->interpolateAndFilterPositions(pos, timestamp);
	mHistory[timestamp] = pos;
}

Transform3D TrackingPositionFilter::getFilteredPosition()
{
	if (mFiltered.size() > 10) //check if mFiltered contains enough positions for the filter to be stable
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

	if (timestamp < mResampled.rbegin()->first){ // clear history if old timestamps appear
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

}

void TrackingPositionFilter::clearIfJumpInTimestamps(Transform3D pos, double timestamp)
{
	if (mResampled.empty())
		return;

	double timeStep = timestamp - mResampled.rbegin()->first;
	if ( timeStep > 1000){ // clear history of resampled and filtered data if jump in timestamps of more than 1 second
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

} // namespace cx



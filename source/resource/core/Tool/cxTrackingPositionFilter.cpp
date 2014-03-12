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

namespace cx
{

TrackingPositionFilter::TrackingPositionFilter()
{
	cutOffFrequency = 3;
	filterOrder = 4;
	resampleFrequency = 40;
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
	//std::cout << mFiltered.size() << std::endl;

	if (mFiltered.size() > 50) //check if mFiltered contains enough positions for the filter to be stable
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

	if (timestamp < mResampled.rbegin()->first) // clear history if old timestamps appear
		mResampled.clear();

}

void TrackingPositionFilter::clearIfJumpInTimestamps(Transform3D pos, double timestamp)
{
	if (mResampled.empty())
		return;

	double timeStep = timestamp - mResampled.rbegin()->first;
	if ( timeStep > 1000) // clear history of resampled data if jump in timestamps of more than 1 second
		mResampled.clear();

}

void TrackingPositionFilter::interpolateAndFilterPositions(Transform3D pos, double timestamp)
{
	Transform3D previousPositionMatrix = mHistory.rbegin()->second;
	double deltaT = timestamp - mHistory.rbegin()->first; //time from previous measured position to this position
	int numberOfInterpolationPoints = floor( (timestamp - mResampled.rbegin()->first)/1000 * resampleFrequency ); // interpolate from last resampled position to current measured position
	//std::cout << timestamp << std::endl;
	//std::cout << mResampled.rbegin()->first << std::endl;
	//std::cout << numberOfInterpolationPoints << std::endl;
	//std::cout << mHistory.rbegin()->first << std::endl;
	for (int i=0; i < numberOfInterpolationPoints; i++)
	{
		double resampledTimestamp = mResampled.rbegin()->first + 1000/resampleFrequency;
		double deltaTpast = resampledTimestamp - mHistory.rbegin()->first;
		double deltaTfuture = timestamp - resampledTimestamp;
		Transform3D interpolatedPosition;
		interpolatedPosition = pos.matrix() * deltaTpast/deltaT + previousPositionMatrix.matrix() * deltaTfuture/deltaT; // linear interpolation between previous and current measured position
		mResampled[resampledTimestamp] = interpolatedPosition;
		//add position to low-pass filter here
		mFiltered[resampledTimestamp] = interpolatedPosition;
	}

}
} // namespace cx



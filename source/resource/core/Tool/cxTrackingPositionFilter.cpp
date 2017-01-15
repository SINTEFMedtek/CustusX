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



/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxPositionFilter.h"
#include "cxMathUtils.h"
#include "cxLogger.h"
#include <algorithm>

namespace cx
{

PositionFilter::PositionFilter(unsigned filterStrength, std::vector<class TimedPosition> &inputImagePositions) :
	mFilterStrength(filterStrength)
{
	mFilterLength = 1+2*filterStrength;
	mInputImagePositions = &inputImagePositions;
	mNumberInputPositions=inputImagePositions.size();
	mNumberQuaternions = mNumberInputPositions;

	mQPosArray = Eigen::ArrayXXd::Zero(7,long(mNumberQuaternions));
	mQPosFiltered = Eigen::ArrayXXd::Zero(7,long(mNumberInputPositions));
}

void PositionFilter::convertToQuaternions()
{
	for (unsigned long i = 0; i < mNumberQuaternions; i++) //For each pose (Tx)
	{
		mQPosArray.col(long(i)) = matrixToQuaternion(mInputImagePositions->at(i).mPos); // Convert each Tx to quaternions

		// q and -q describe the same rotation. Make the sign consistent with the previous pose, otherwise the
		// averaging below gives wrong rotations when the sign of the quaternion changes between two poses.
		if (i > 0)
		{
			double dot = (mQPosArray.col(long(i)).head(4) * mQPosArray.col(long(i-1)).head(4)).sum();
			if (dot < 0)
				mQPosArray.col(long(i)).head(4) *= -1;
		}
	}
}

void PositionFilter::filterQuaternionArray()
{
	// Moving average with window length 2*filterStrength+1, centered on each position.
	// Close to the ends of the sequence the window is made shorter, but kept symmetric around the position
	// (half length = min(filterStrength, distance to nearest end)). The first and the last position are therefore
	// unchanged. Padding with copies of the end positions would pull the end positions towards the interior of the
	// sequence, as the averaged window would no longer be centered on the position.
	long nPositions = long(mNumberInputPositions);
	for (long k = 0; k < nPositions; k++)
	{
		long halfLength = std::min(long(mFilterStrength), std::min(k, nPositions-1-k));
		mQPosFiltered.col(k) = mQPosArray.block(0, k-halfLength, 7, 2*halfLength+1).rowwise().sum() / double(2*halfLength+1);
	}
}

void PositionFilter::convertFromQuaternion()
{
	for (unsigned int i = 0; i < mInputImagePositions->size(); i++) //For each pose after filtering
	{
		// The average of unit quaternions is not a unit quaternion. Normalize before converting to a rotation matrix
		Eigen::ArrayXd qPose = mQPosFiltered.col(i);
		double norm = qPose.head(4).matrix().norm();
		if (norm > 0)
			qPose.head(4) /= norm;
		// Convert back to position data
		mInputImagePositions->at(i).mPos = quaternionToMatrix(qPose);
	}
}

void PositionFilter::filterPositions()
{
	if (mFilterStrength > 0) //Position filter enabled?
	{
		if (mNumberInputPositions > mFilterLength) //Position sequence sufficient long?
		{
			convertToQuaternions();
			filterQuaternionArray();
			convertFromQuaternion();
		}
	}
}

} //cx

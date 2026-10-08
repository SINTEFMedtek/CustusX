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
	mInputImagePositions = &inputImagePositions;
	mNumberInputPositions = inputImagePositions.size();

	mQPosArray = Eigen::ArrayXXd::Zero(7,long(mNumberInputPositions));
	mQPosFiltered = Eigen::ArrayXXd::Zero(7,long(mNumberInputPositions));
}

void PositionFilter::convertToQuaternions()
{
	for (unsigned long i = 0; i < mNumberInputPositions; i++)
	{
		mQPosArray.col(long(i)) = matrixToQuaternion(mInputImagePositions->at(i).mPos);
		// Same sign as the previous pose, so that the average below doesn't mix q and -q
		if (i > 0)
		{
			mQPosArray.col(long(i)) = alignQuaternionSign(mQPosArray.col(long(i)), mQPosArray.col(long(i-1)));
		}
	}
}

void PositionFilter::filterQuaternionArray()
{
	// Centered moving average, shortened symmetrically near the ends: the first and last positions are not moved
	long nPositions = long(mNumberInputPositions);
	for (long k = 0; k < nPositions; k++)
	{
		long halfLength = std::min(long(mFilterStrength), std::min(k, nPositions-1-k));
		mQPosFiltered.col(k) = mQPosArray.block(0, k-halfLength, 7, 2*halfLength+1).rowwise().sum() / double(2*halfLength+1);
	}
}

void PositionFilter::convertFromQuaternion()
{
	for (unsigned int i = 0; i < mInputImagePositions->size(); i++)
	{
		mInputImagePositions->at(i).mPos = quaternionToMatrix(mQPosFiltered.col(i)); // normalizes the averaged quaternion
	}
}

void PositionFilter::filterPositions()
{
	// A sequence not longer than the filter window is not filtered
	if (mFilterStrength > 0 && mNumberInputPositions > 2*mFilterStrength + 1)
	{
		convertToQuaternions();
		filterQuaternionArray();
		convertFromQuaternion();
	}
}

} //cx

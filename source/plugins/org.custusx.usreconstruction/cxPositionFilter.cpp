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

namespace cx
{

PositionFilter::PositionFilter(unsigned filterStrength, std::vector<class TimedPosition> &inputImagePositions) :
	mFilterStrength(filterStrength)
{
	mFilterLength = 1+2*filterStrength;
	mInputImagePositions = &inputImagePositions;
	mNumberInputPositions=inputImagePositions.size();
	mNumberQuaternions = mNumberInputPositions+mFilterLength;

	mQPosArray = Eigen::ArrayXXd::Zero(7,long(mNumberQuaternions));
	mQPosFiltered = Eigen::ArrayXXd::Zero(7,long(mNumberInputPositions));
}

void PositionFilter::convertToQuaternions()
{
	for (unsigned int i = 0; i < mNumberQuaternions; i++) //For each pose (Tx), with edge padding
	{
		unsigned long sourceIdx =  (i > mFilterStrength) ? (i-mFilterStrength) : 0; // Calculate index in Tx array, pad with edge elements //Skriv om
		sourceIdx =  (sourceIdx < mNumberInputPositions) ? sourceIdx : (mNumberInputPositions-1);
		mQPosArray.col(i) = matrixToQuaternion(mInputImagePositions->at(sourceIdx).mPos); // Convert each Tx to quaternions
	}
}

void PositionFilter::filterQuaternionArray()
{
	for (unsigned int i = 0; i < mFilterLength; i++)
	{
		mQPosFiltered = mQPosFiltered + mQPosArray.block(0,i,7,long(mNumberInputPositions));
	}
	mQPosFiltered = mQPosFiltered / mFilterLength; // Scale and write back to qPosArray
}

void PositionFilter::convertFromQuaternion()
{
	for (unsigned int i = 0; i < mInputImagePositions->size(); i++) //For each pose after filtering
	{
		// Convert back to position data
		mInputImagePositions->at(i).mPos = quaternionToMatrix(mQPosFiltered.col(i));
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

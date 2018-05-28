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

PositionFilter::PositionFilter(int filterStrength, std::vector<class TimedPosition> &inputImagePositions) :
    mFilterStrength(filterStrength)
{
    mFilterLength = 1+2*filterStrength;
    mNumberInputPositions=inputImagePositions.size();
    mNumberQuaternions = mNumberInputPositions+mFilterLength;

    qPosArray = Eigen::ArrayXXd::Zero(7,mNumberQuaternions);
    qPosFiltered = Eigen::ArrayXXd::Zero(7,mNumberInputPositions);
}

void PositionFilter::convertToQuaternions(std::vector<class TimedPosition> &inputImagePositions)
{
    for (unsigned int i = 0; i < mNumberQuaternions; i++) //For each pose (Tx), with edge padding
    {
        unsigned int sourceIdx =  (i > mFilterStrength) ? (i-mFilterStrength) : 0; // Calculate index in Tx array, pad with edge elements //Skriv om
        sourceIdx =  (sourceIdx < mNumberInputPositions) ? sourceIdx : (mNumberInputPositions-1);
        qPosArray.col(i) = matrixToQuaternion(inputImagePositions.at(sourceIdx).mPos); // Convert each Tx to quaternions
    }
}

void PositionFilter::filterQuaternionArray()
{
    for (unsigned int i = 0; i < mFilterLength; i++)
    {
        qPosFiltered = qPosFiltered + qPosArray.block(0,i,7,mNumberInputPositions);
    }
    qPosFiltered = qPosFiltered / mFilterLength; // Scale and write back to qPosArray
}

void PositionFilter::convertFromQuaternion(std::vector<class TimedPosition> &inputImagePositions)
{
    for (unsigned int i = 0; i < inputImagePositions.size(); i++) //For each pose after filtering
    {
        // Convert back to position data
        inputImagePositions.at(i).mPos = quaternionToMatrix(qPosFiltered.col(i));
    }
}

void PositionFilter::filterPositions(std::vector<class TimedPosition> &inputImagePositions)
{
    if (mFilterStrength > 0) //Position filter enabled?
    {
        if (mNumberInputPositions > mFilterLength) //Position sequence sufficient long?
        {
            convertToQuaternions(inputImagePositions);
            filterQuaternionArray();
            convertFromQuaternion(inputImagePositions);
        }
    }
}

} //cx

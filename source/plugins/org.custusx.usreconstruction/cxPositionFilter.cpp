/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxPositionFilter.h"
#include "cxMathUtils.h"

namespace cx
{

PositionFilter::PositionFilter(int filterStrength, std::vector<class TimedPosition> positions) :
    mFilterStrength(filterStrength),
    mInputImagePositions(positions)
{
    mFilterLength = 1+2*filterStrength;
    mNumberInputPositions=mInputImagePositions.size();
    mNumberQuaternions = 2*filterStrength + mNumberInputPositions;
}

void PositionFilter::convertToQuaternions(Eigen::ArrayXXd qPosArray)
{
    for (unsigned int i = 0; i < mNumberQuaternions; i++) //For each pose (Tx), with edge padding
    {
        unsigned int sourceIdx =  (i > mFilterStrength) ? (i-mFilterStrength) : 0; // Calculate index in Tx array, pad with edge elements //Skriv om
        sourceIdx =  (sourceIdx < mNumberInputPositions) ? sourceIdx : (mNumberInputPositions-1);
        qPosArray.col(i) = matrixToQuaternion(mInputImagePositions.at(sourceIdx).mPos); // Convert each Tx to quaternions
    }
}

Eigen::ArrayXXd PositionFilter::filterQuaternionArray(Eigen::ArrayXXd qPosArray)
{
    Eigen::ArrayXXd qPosFiltered = Eigen::ArrayXXd::Zero(7,mNumberInputPositions); // Fill with zeros
    for (unsigned int i = 0; i < mFilterLength; i++)
    {
        qPosFiltered = qPosFiltered + qPosArray.block(0,i,7,mNumberInputPositions);
    }
    qPosFiltered = qPosFiltered / mFilterLength;

    return qPosFiltered;
}

void PositionFilter::convertFromQuaternion(Eigen::ArrayXXd qPosFiltered)
{
    for (unsigned int i = 0; i < mInputImagePositions.size(); i++) //For each pose after filtering
    {
        // Convert back to position data
        mInputImagePositions.at(i).mPos = quaternionToMatrix(qPosFiltered.col(i));
    }
}

void PositionFilter::filterPositions()
{

    if (mFilterStrength > 0) //Position filter enabled?
    {


        if (mNumberInputPositions > mFilterLength) //Position sequence sufficient long?
        {
            // Init array to hold positions converted to quaternions:
            int nQuaternions = mNumberInputPositions+(2*mFilterStrength); // Add room for FIR-filtering
            Eigen::ArrayXXd qPosArray(7,nQuaternions);

            convertToQuaternions(qPosArray);

            Eigen::ArrayXXd qPosFiltered = filterQuaternionArray(qPosArray);

            convertFromQuaternion(qPosFiltered);
        }
    }

}


} //cx

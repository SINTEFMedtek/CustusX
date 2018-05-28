/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXPOSITIONFILTER_H
#define CXPOSITIONFILTER_H

#include <boost/shared_ptr.hpp>
#include "cxMathBase.h"
#include "cxUSReconstructInputData.h"

namespace cx
{
//typedef boost::shared_ptr<std::vector<class TimedPosition> > PositionsPtr;

class PositionFilter
{
public:
    PositionFilter(int filterStrength, std::vector<TimedPosition> &inputImagePositions);
    void filterPositions(std::vector<class TimedPosition> &inputImagePositions);
protected:
    int mFilterStrength;
    std::vector<TimedPosition> inputImagePositions;
    int mNumberInputPositions;
    int mFilterLength;
    int mNumberQuaternions;
    Eigen::ArrayXXd qPosArray;
    Eigen::ArrayXXd qPosFiltered;

    void convertToQuaternions(std::vector<TimedPosition> &inputImagePositions);
    void filterQuaternionArray();
    void convertFromQuaternion(std::vector<TimedPosition> &inputImagePositions);
};

}//cx

#endif // CXPOSITIONFILTER_H

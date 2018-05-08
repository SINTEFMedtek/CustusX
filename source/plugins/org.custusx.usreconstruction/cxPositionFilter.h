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
    PositionFilter(int filterStrength, std::vector<class TimedPosition> positions);
    void filterPositions();
private:
    int mFilterStrength;
    std::vector<TimedPosition> mInputImagePositions;
    int mNumberInputPositions;
    int mFilterLength;
    int mNumberQuaternions;

    void convertToQuaternions(Eigen::ArrayXXd &qPosArray);
    void filterQuaternionArray(Eigen::ArrayXXd &qPosArray);
    void convertFromQuaternion(Eigen::ArrayXXd &qPosFiltered);
};

}//cx

#endif // CXPOSITIONFILTER_H

/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXPOSITIONFILTER_H
#define CXPOSITIONFILTER_H

#include "org_custusx_usreconstruction_Export.h"

#include <boost/shared_ptr.hpp>
#include "cxMathBase.h"
#include "cxUSReconstructInputData.h"

namespace cx
{
//typedef boost::shared_ptr<std::vector<class TimedPosition> > PositionsPtr;

class org_custusx_usreconstruction_EXPORT PositionFilter
{
public:
	PositionFilter(unsigned filterStrength, std::vector<TimedPosition> &inputImagePositions);
	void filterPositions();
protected:
	unsigned mFilterStrength;
	std::vector<TimedPosition> *mInputImagePositions;
	unsigned long mNumberInputPositions;
	unsigned mFilterLength;
	unsigned long mNumberQuaternions;
	Eigen::ArrayXXd mQPosArray;
	Eigen::ArrayXXd mQPosFiltered;

	void convertToQuaternions();
	void filterQuaternionArray();
	void convertFromQuaternion();
};

}//cx

#endif // CXPOSITIONFILTER_H

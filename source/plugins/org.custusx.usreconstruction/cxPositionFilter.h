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

#include <memory>
#include "cxMathBase.h"
#include "cxUSReconstructInputData.h"

namespace cx
{
//typedef std::shared_ptr<std::vector<class TimedPosition> > PositionsPtr;

class org_custusx_usreconstruction_EXPORT PositionFilter
{
public:
	PositionFilter(unsigned filterStrength, std::vector<TimedPosition> &inputImagePositions);
	void filterPositions();
protected:
	unsigned mFilterStrength;
	std::vector<TimedPosition> *mInputImagePositions;
	unsigned long mNumberInputPositions;
	Eigen::ArrayXXd mQPosArray;
	Eigen::ArrayXXd mQPosFiltered;

	void convertToQuaternions();
	void filterQuaternionArray();
	void convertFromQuaternion();
};

/**
 * Holt's linear exponential smoothing ("Holt filter") of each row of the array, along the columns (time).
 *
 * For each column t (starting with level = y_0, trend = y_1 - y_0):
 *   level_t = alpha*y_t + (1 - alpha)*(level_(t-1) + trend_(t-1))
 *   trend_t = beta*(level_t - level_(t-1)) + (1 - beta)*trend_(t-1)
 *   output_t = level_t, or level_t + trend_t if addTrend (the output used in the article that describes the filter)
 * The first column is not changed.
 *
 * Alpha is the smoothing of the level and beta of the trend, both in [0,1]. A high value follows the input closely.
 * Alpha = 1 gives output = input (if addTrend is false).
 * All columns are assumed to have the same time interval.
 *
 * Note that this is a causal filter, so the output is delayed compared to the input when the signal accelerates
 * (and for noise), and it is ahead of the input if addTrend is true. Use holtFilterBothDirections() to avoid delay.
 */
org_custusx_usreconstruction_EXPORT Eigen::ArrayXXd holtFilter(const Eigen::ArrayXXd &values, double alpha, double beta, bool addTrend);

/**
 * The mean of holtFilter() done forward in time and backward in time. The delay (or lead) of the two cancel each other,
 * so there is no time shift for a signal with constant speed, and the result for a time reversed
 * input is the time reversed result.
 */
org_custusx_usreconstruction_EXPORT Eigen::ArrayXXd holtFilterBothDirections(const Eigen::ArrayXXd &values, double alpha, double beta, bool addTrend);

/**
 * Position filter based on the Holt filter (see holtFilter()), applied to the position and
 * the quaternion of the poses (the quaternion is normalized after the filtering).
 */
class org_custusx_usreconstruction_EXPORT HoltPositionFilter : public PositionFilter
{
public:
	HoltPositionFilter(std::vector<TimedPosition> &inputImagePositions, double alpha, double beta, bool bothDirections, bool addTrend);
	void filterPositions();
protected:
	double mAlpha;
	double mBeta;
	bool mBothDirections;
	bool mAddTrend;
};

}//cx

#endif // CXPOSITIONFILTER_H

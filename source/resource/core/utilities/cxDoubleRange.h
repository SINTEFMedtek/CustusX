/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


/*
 * sscDoubleRange.h
 *
 *  Created on: Jun 23, 2010
 *      Author: christiana
 */
#ifndef CXDOUBLERANGE_H_
#define CXDOUBLERANGE_H_

#include "cxResourceExport.h"

namespace cx
{

/**\brief Utility class for describing a bounded numeric range.
 *
 *
 * \ingroup cx_resource_core_utilities
 */
struct cxResource_EXPORT DoubleRange
{
	DoubleRange() :
		mMin(0), mMax(1), mStep(0.1)
	{
	}
	DoubleRange(double min, double max, double step) :
		mMin(min), mMax(max), mStep(step)
	{
	}
	double mMin;
	double mMax;
	double mStep;
	double min() const
	{
		return mMin;
	} ///< minimum value
	double max() const
	{
		return mMax;
	} ///< maximum value
	double step() const
	{
		return mStep;
	} ///< smallest reasonable increment
	double resolution() const
	{
		return (mMax - mMin) / mStep;
	} ///< number of steps in range (1 step means 1 resolution).
	double range() const
	{
		return mMax - mMin;
	} ///< max - min

	/**Constrain the input to the range |min,max|
	 */
	double constrainValue(double val)
	{
		if (val <= mMin)
			return mMin;
		if (val >= mMax)
			return mMax;
		return val;
	}
};

}
#endif /* CXDOUBLERANGE_H_ */

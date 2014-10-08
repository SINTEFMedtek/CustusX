/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

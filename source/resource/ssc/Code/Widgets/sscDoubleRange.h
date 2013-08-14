// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

/*
 * sscDoubleRange.h
 *
 *  Created on: Jun 23, 2010
 *      Author: christiana
 */
#ifndef SSCDOUBLERANGE_H_
#define SSCDOUBLERANGE_H_

namespace ssc
{

/**\brief Utility class for describing a bounded numeric range.
 *
 *
 * \ingroup sscWidget
 */
struct DoubleRange
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
#endif /* SSCDOUBLERANGE_H_ */

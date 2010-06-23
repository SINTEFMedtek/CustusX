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

/** Utility class for describing a bounded numeric range. */
struct DoubleRange
{
  DoubleRange() : mMin(0), mMax(1), mStep(0.1) {}
  DoubleRange(double min, double max, double step) : mMin(min), mMax(max), mStep(step) {}
  double mMin;
  double mMax;
  double mStep;
  double min() const { return mMin; } ///< minimum value
  double max() const { return mMax; } ///< maximum value
  double step() const { return mStep; } ///< smallest reasonable increment
  double resolution() const { return (mMax-mMin)/mStep; } ///< number of steps in range (1 step means 1 resolution).
  double range() const { return mMax-mMin; } ///< max - min

  /**Constrain the input to the range |min,max|
   */
  double constrainValue(double val)
  {
    if (val<=mMin)
      return mMin;
    if (val>=mMax)
      return mMax;
    return val;
  }
};

}
#endif /* SSCDOUBLERANGE_H_ */

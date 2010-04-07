#include "sscUtilHelpers.h"

namespace ssc
{

/**Constrain the input to the range |min,max|
 */
double constrainValue(double val, double min, double max)
{
  if (val<=min)
    return min;
  if (val>=max)
    return max;
  return val;
}

int sign(double x)
{
  if (x>=0)
    return 1;
  return -1;
}

} // namespace ssc


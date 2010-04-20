/*
 * sscTime.cpp
 *
 *  Created on: Apr 20, 2010
 *      Author: christiana
 */
#include "sscTime.h"

namespace ssc
{

QString timestampSecondsFormat()
{
  return QString("yyyyMMdd'T'hhmmss");
}
QString timestampSecondsFormatNice()
{
  return QString("yyyy-MM-dd hh:mm:ss");
}

} // namespace ssc

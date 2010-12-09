/*
 * sscTime.cpp
 *
 *  Created on: Apr 20, 2010
 *      Author: christiana
 */
#include "sscTime.h"
#include <QDateTime>

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

double getMicroSecondsSinceEpoch()
{
#if QT_VERSION >= 0x040700
  return QDateTime().toMSecsSinceEpoch()*1000; //microseconds
#else
  return QDateTime().toTime_t()*1000; //microseconds
#endif
}

double getMilliSecondsSinceEpoch()
{
#if QT_VERSION >= 0x040700
  return QDateTime().toMSecsSinceEpoch(); //milliseconds
#else
  return QDateTime().toTime_t(); //milliseconds
#endif
}

} // namespace ssc

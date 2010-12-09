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
  return QDateTime().toMSecsSinceEpoch()*1000; //microseconds
}

} // namespace ssc

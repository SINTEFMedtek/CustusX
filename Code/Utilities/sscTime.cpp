/*
 * sscTime.cpp
 *
 *  Created on: Apr 20, 2010
 *      Author: christiana
 */
#include "sscTime.h"
#include <QDateTime>
#include <boost/cstdint.hpp>

namespace ssc
{

QString timestampSecondsFormat()
{
	return QString("yyyyMMdd'T'hhmmss");
}
QString timestampMilliSecondsFormat()
{
	return QString("yyyyMMdd'T'hhmmsszzz");
}
QString timestampSecondsFormatNice()
{
	return QString("yyyy-MM-dd hh:mm:ss");
}
QString timestampMilliSecondsFormatNice()
{
	return QString("yyyy-MM-dd hh:mm:ss.zzz");
}

double getMicroSecondsSinceEpoch()
{
#if QT_VERSION >= 0x040700
	return QDateTime::currentDateTime().toMSecsSinceEpoch()*1000; //microseconds
#else
	return getMilliSecondsSinceEpoch()*1000;
#endif
}

double getMilliSecondsSinceEpoch()
{
#if QT_VERSION >= 0x040700
	return QDateTime::currentDateTime().toMSecsSinceEpoch(); //milliseconds
#else
	QDateTime now = QDateTime::currentDateTime();
	boost::uint64_t now_t64 = now.toTime_t();
	now_t64 *= 1000;
	now_t64 += now.time().msec();
	return now_t64;
#endif
}

} // namespace ssc

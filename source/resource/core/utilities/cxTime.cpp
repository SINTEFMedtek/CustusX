/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxTime.h"
#include <QDateTime>
#include <boost/cstdint.hpp>

namespace cx
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

QString toMillisecondsFormatNice(double secondsSinceEpoch)
{
    QDateTime time;
    time.setMSecsSinceEpoch(secondsSinceEpoch);
    return time.toString(timestampMilliSecondsFormatNice());
}

} // namespace cx

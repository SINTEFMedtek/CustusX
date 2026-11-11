/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxTime.h"
#include <QDateTime>
#include <cstdint>
#include <chrono>

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
	return getMilliSecondsSinceEpoch() * 1000;
}

double getMilliSecondsSinceEpoch()
{
	// sub-millisecond resolution, unlike QDateTime
	std::chrono::duration<double, std::milli> sinceEpoch = std::chrono::system_clock::now().time_since_epoch();
	return sinceEpoch.count();
}

QString toMillisecondsFormatNice(double secondsSinceEpoch)
{
    QDateTime time;
    time.setMSecsSinceEpoch(secondsSinceEpoch);
    return time.toString(timestampMilliSecondsFormatNice());
}

} // namespace cx

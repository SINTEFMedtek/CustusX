/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxTimeKeeper.h"
#include "cxLogger.h"
#include "cxReporter.h"

namespace cx
{

TimeKeeper::TimeKeeper() : mTime(QDateTime::currentDateTime()) {}

void TimeKeeper::printElapsedSeconds(QString text)
{
    reporter()->sendDebug(text + ": " + this->getElapsedSecondsAsString() + "s");
}
void TimeKeeper::printElapsedms(QString text) const
{
    reporter()->sendDebug(QString(text + ": %1").arg(getElapsedms()));
}

QString TimeKeeper::getElapsedSecondsAsString() const
{
    double secs = double(this->getElapsedms())/1000;
    return QString("%1").arg(secs, 0, 'f', 2);
}
int TimeKeeper::getElapsedms() const
{
    return mTime.msecsTo(QDateTime::currentDateTime());
}

void TimeKeeper::reset()
{
    mTime = QDateTime::currentDateTime();
}

} // namespace cx

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

#include "sscTimeKeeper.h"
#include "sscMessageManager.h"

namespace ssc
{

TimeKeeper::TimeKeeper() : mTime(QDateTime::currentDateTime()) {}

void TimeKeeper::printElapsedSeconds(QString text)
{
    messageManager()->sendDebug(text + ": " + this->getElapsedSecondsAsString() + "s");
}
void TimeKeeper::printElapsedms(QString text) const
{
    messageManager()->sendDebug(QString(text + ": %1").arg(getElapsedms()));
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

} // namespace ssc

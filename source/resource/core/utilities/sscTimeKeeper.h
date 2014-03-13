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
#ifndef SSCTIMEKEEPER_H
#define SSCTIMEKEEPER_H

#include <QDateTime>
#include <QString>

namespace cx
{

/**Helper class for measuring and printing the time spent in code.
 *
 *  \date Nov 06, 2012
 *  \author christiana
 *
 * \ingroup cx_resource_core_utilities
 */
class TimeKeeper
{
public:
    TimeKeeper();
    void printElapsedSeconds(QString text="Elapsed time");
    void printElapsedms(QString text="Elapsed") const;
    QString getElapsedSecondsAsString() const;
    int getElapsedms() const;
    void reset();

private:
    QDateTime mTime;
};

} // namespace cx

#endif // SSCTIMEKEEPER_H

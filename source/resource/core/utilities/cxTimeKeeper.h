/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTIMEKEEPER_H_
#define CXTIMEKEEPER_H_

#include "cxResourceExport.h"

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
class cxResource_EXPORT TimeKeeper
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

#endif // CXTIMEKEEPER_H_

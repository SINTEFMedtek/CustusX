/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXTIME_H_
#define CXTIME_H_

#include "cxResourceExport.h"

#include <QString>

namespace cx
{

/**
 * \addtogroup cx_resource_core_utilities
 * \{
 */

//namespace time {

/** A standard format for use when converting between QDateTime and QString
 *  Usage:
 *    QDateTime().toString(timestampSecondsFormat());
 *    QDateTime().fromString(text, timestampSecondsFormat())
 */
cxResource_EXPORT QString timestampSecondsFormat();

/** A more human-readable version of the above formatter.
 */
cxResource_EXPORT QString timestampSecondsFormatNice();

cxResource_EXPORT QString timestampMilliSecondsFormat();
cxResource_EXPORT QString timestampMilliSecondsFormatNice();


/** Microseconds since epoch (1970)
 */
cxResource_EXPORT double getMicroSecondsSinceEpoch();

/** Milliseconds since epoch (1970)
 */
cxResource_EXPORT double getMilliSecondsSinceEpoch();

cxResource_EXPORT QString toMillisecondsFormatNice(double secondsSinceEpoch);

/**
 * \}
 */

//} // time
} // ssc

#endif /* CXTIME_H_ */

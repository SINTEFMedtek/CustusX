/*
 * sscTime.h
 *
 *  Created on: Apr 20, 2010
 *      Author: christiana
 */
#ifndef CXTIME_H_
#define CXTIME_H_

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
QString timestampSecondsFormat();

/** A more human-readable version of the above formatter.
 */
QString timestampSecondsFormatNice();

QString timestampMilliSecondsFormat();
QString timestampMilliSecondsFormatNice();


/** Microseconds since epoch (1970)
 */
double getMicroSecondsSinceEpoch();

/** Milliseconds since epoch (1970)
 */
double getMilliSecondsSinceEpoch();

/**
 * \}
 */

//} // time
} // ssc

#endif /* CXTIME_H_ */

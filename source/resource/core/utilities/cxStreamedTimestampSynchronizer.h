/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef STREAMEDTIMESTAMPSYNCHRONIZER_H
#define STREAMEDTIMESTAMPSYNCHRONIZER_H

#include "cxResourceExport.h"


#include <vector>
#include "boost/shared_ptr.hpp"
#include <QDateTime>
#include "cxForwardDeclarations.h"

namespace cx
{

/**
 * Process a stream of incoming timestamps, and generate a mean shift
 * that synchronizes their time to this computer.
 *
 * Use case: Timestamp series coming from another remote computer not synced
 * to the clock of this computer should be run through this algorithm. This
 * because CustusX stores its own timestamps during acquisition.
 *
 * If the remote is in sync, either because it is localhost or otherwise synced
 * using e.g. "sudo ntpdate -u time.euro.apple.com" on both machines or similar,
 * this is not necessary.
 *
 * \sa http://openigtlink.org/protocols/v2_timestamp.html
 *
 */
class cxResource_EXPORT StreamedTimestampSynchronizer
{
public:
    StreamedTimestampSynchronizer();
    /**
     * Utility for updating the shift based on the incoming image timestamp,
     * and also synchronizing the image timestamp (i.e. shifting it).
     */
    void syncToCurrentTime(ImagePtr imgMsg);

    /**
     * Insert a new timestamp. Use it to update
     * the shift estimate.
     */
    void addTimestamp(QDateTime timestamp);
    /**
     * Overloaded method, input is QDateTime::fromMSecsSinceEpoch(timestamp)
     */
    void addTimestamp(double timestamp);

    /**
     * Get the current shift, i.e. the correction to be applied
     * to timestamps in order to synchronize them with this computer
     * clock.
     */
    double getShift() const;

private:
    /** Calibrate the time stamps of the incoming message based on the computer
     * clock. Calibration is based on an average of several of the last messages.
     * The calibration is updated every 20-30 sec.
     */
    template<class ITER>
    double average(ITER begin, ITER end) const;

    mutable double mLastComputedTimestampShift;
    QList<double> mDeltaWindow;
    int mMaxWindowSize;
};


/**
 * @}
 */
} //end namespace cx

#endif // STREAMEDTIMESTAMPSYNCHRONIZER_H

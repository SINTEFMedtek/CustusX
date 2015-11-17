/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxStreamerService.h"

#include "cxStreamerServiceNull.h"
#include "cxNullDeleter.h"

namespace cx
{

StreamerServicePtr StreamerService::getNullObject()
{
    static StreamerServicePtr mNull;
    if (!mNull)
        mNull.reset(new StreamerServiceNull, null_deleter());
    return mNull;
}

} //end namespace cx

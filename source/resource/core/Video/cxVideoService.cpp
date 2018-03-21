/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxVideoService.h"
#include "cxVideoServiceNull.h"
#include "cxNullDeleter.h"

namespace cx
{

VideoServicePtr VideoService::getNullObject()
{
	static VideoServicePtr mNull;
	if (!mNull)
		mNull.reset(new VideoServiceNull, null_deleter());
	return mNull;
}

} //cx

/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxUsReconstructionService.h"
#include "cxUsReconstructionServiceNull.h"
#include "cxNullDeleter.h"

namespace cx
{

UsReconstructionServicePtr UsReconstructionService::getNullObject()
{
	static UsReconstructionServicePtr mNull;
	if (!mNull)
		mNull.reset(new UsReconstructionServiceNull, null_deleter());
	return mNull;
}
} //cx

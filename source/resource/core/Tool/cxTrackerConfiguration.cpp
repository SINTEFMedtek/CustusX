/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxTrackerConfiguration.h"
#include "cxTrackerConfigurationNull.h"
#include "cxNullDeleter.h"

namespace cx
{

TrackerConfiguration::~TrackerConfiguration() {}

TrackerConfigurationPtr TrackerConfiguration::getNullObject()
{
	static TrackerConfigurationPtr mNull;
	if (!mNull)
		mNull.reset(new TrackerConfigurationNull, null_deleter());
	return mNull;
}

} // namespace cx


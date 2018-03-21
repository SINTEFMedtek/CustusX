/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxUSReconstructInputData.h"
#include "cxUSFrameData.h"
#include "cxLogger.h"

namespace cx
{
vtkImageDataPtr USReconstructInputData::getMask()
{
	vtkImageDataPtr retval = mProbeDefinition.getMask();
	return retval;
}

bool USReconstructInputData::isValid() const
{
	if (mFrames.empty() || !mUsRaw || mPositions.empty())
		return false;
	if(mUsRaw->is4D())
	{
		reportWarning("US reconstructer do not handle 4D US data");
		return false;
	}
	return true;
}

bool USReconstructInputData::is8bit() const
{
	return mUsRaw->is8bit();
}

} // namespace cx

// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxUSReconstructInputData.h"
#include "cxUSFrameData.h"
#include "cxReporter.h"

namespace cx
{
vtkImageDataPtr USReconstructInputData::getMask()
{
	vtkImageDataPtr retval = mProbeData.getMask();
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

} // namespace cx

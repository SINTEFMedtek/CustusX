/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxSpaceProvider.h"
#include "cxSpaceProviderNull.h"

namespace cx
{

SpaceProviderPtr SpaceProvider::getNullObject()
{
	static SpaceProviderPtr mNull;
	if (!mNull)
		mNull.reset(new SpaceProviderNull);
	return mNull;
}


} // namespace cx

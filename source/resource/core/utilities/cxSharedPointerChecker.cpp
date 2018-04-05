/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxSharedPointerChecker.h"
#include "cxLogger.h"

namespace cx
{

void requireUnique(int use_count, QString objectName)
{
	if (use_count>1)
	{
		QString msg = QString("Detected %1 users for object [%2], should be unique. ")
				.arg(use_count)
				.arg(objectName);
		cx::reportError(msg);
	}
}


} // namespace cx

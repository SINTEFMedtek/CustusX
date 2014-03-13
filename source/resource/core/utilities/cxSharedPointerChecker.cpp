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

#include "cxSharedPointerChecker.h"
#include "cxMessageManager.h"

namespace cx
{

void requireUnique(int use_count, QString objectName)
{
	if (use_count>1)
	{
		QString msg = QString("Detected %1 users for object [%2], should be unique. ")
				.arg(use_count)
				.arg(objectName);
		cx::messageManager()->sendError(msg);
	}
}


} // namespace cx

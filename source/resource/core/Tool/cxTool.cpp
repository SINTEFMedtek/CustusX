/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxTool.h"

#include <vtkConeSource.h>
#include "cxToolNull.h"
#include "cxNullDeleter.h"

namespace cx
{
vtkPolyDataPtr Tool::createDefaultPolyDataCone()
{
	vtkConeSourcePtr coneSource = vtkConeSourcePtr::New();
	coneSource->SetResolution(25);
	coneSource->SetRadius(10);
	coneSource->SetHeight(100);

	coneSource->SetDirection(0, 0, 1);
	double newCenter[3];
	coneSource->GetCenter(newCenter);
	newCenter[2] = newCenter[2] - coneSource->GetHeight() / 2;
	coneSource->SetCenter(newCenter);

	coneSource->Update();
	return coneSource->GetOutput();
}

Tool::Tool(const QString &uid, const QString &name) :
	mUid(uid), mName(name)
{
	if (name.isEmpty())
		mName = uid;
}

ToolPtr Tool::getNullObject()
{
	static ToolPtr mNull;
	if (!mNull)
		mNull.reset(new ToolNull, null_deleter());
	return mNull;
}

QString ToolPositionMetadata::toString() const
{
	return mData.trimmed();
	//	return mData.toElement().ownerDocument().toString();
}

} //cx


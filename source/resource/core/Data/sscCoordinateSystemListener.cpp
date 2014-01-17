// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#include "sscCoordinateSystemListener.h"
#include "sscDataManager.h"
#include "sscToolManager.h"
#include "sscData.h"

namespace cx {

CoordinateSystemListener::CoordinateSystemListener(CoordinateSystem space) :
	mSpace(space)
{
	this->doConnect();
}

CoordinateSystemListener::~CoordinateSystemListener()
{
}

void CoordinateSystemListener::setSpace(CoordinateSystem space)
{
	this->doDisconnect();
	mSpace = space;
	this->doConnect();
	emit changed();
}

void CoordinateSystemListener::reconnect()
{
	this->doDisconnect();
	this->doConnect();
	emit changed();
}

CoordinateSystem CoordinateSystemListener::getSpace() const
{
	return mSpace;
}

void CoordinateSystemListener::doConnect()
{
	if (mSpace.mId == csDATA)
	{
		DataPtr data = dataManager()->getData(mSpace.mRefObject);
		if (data)
		{
			connect(data.get(), SIGNAL(transformChanged()), this, SIGNAL(changed()));
			connect(dataManager(), SIGNAL(dataAddedOrRemoved(QString)), this, SIGNAL(changed()));
		}
	}

	if (mSpace.mId == csSENSOR || mSpace.mId == csTOOL || mSpace.mId == csTOOL_OFFSET)
	{
		ToolPtr tool = toolManager()->getTool(mSpace.mRefObject);
		if (tool)
		{
			connect(tool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D,double)), this, SIGNAL(changed()));
			connect(tool.get(), SIGNAL(tooltipOffset(double)), this, SIGNAL(changed()));

			if (mSpace.mRefObject == "active")
			{
				connect(toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SIGNAL(changed()));
				connect(toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(reconnect()));
			}
			connect(toolManager(), SIGNAL(rMprChanged()), this, SIGNAL(changed()));
		}
	}

	if (mSpace.mId == csPATIENTREF)
	{
		connect(toolManager(), SIGNAL(rMprChanged()), this, SIGNAL(changed()));
	}
}

void CoordinateSystemListener::doDisconnect()
{
	if (mSpace.mId == csDATA)
	{
		DataPtr data = dataManager()->getData(mSpace.mRefObject);
		if (data)
		{
			disconnect(data.get(), SIGNAL(transformChanged()), this, SIGNAL(changed()));
			disconnect(dataManager(), SIGNAL(dataRemoved(QString)), this, SIGNAL(changed()));
		}
	}

	if (mSpace.mId == csSENSOR || mSpace.mId == csTOOL || mSpace.mId == csTOOL_OFFSET)
	{
		ToolPtr tool = toolManager()->getTool(mSpace.mRefObject);
		if (tool)
		{
			disconnect(tool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D,double)), this, SIGNAL(changed()));
			disconnect(tool.get(), SIGNAL(tooltipOffset(double)), this, SIGNAL(changed()));

			if (mSpace.mRefObject == "active")
			{
				disconnect(toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SIGNAL(changed()));
				disconnect(toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(reconnect()));
			}
			disconnect(toolManager(), SIGNAL(rMprChanged()), this, SIGNAL(changed()));
		}
	}

	if (mSpace.mId == csPATIENTREF)
	{
		disconnect(toolManager(), SIGNAL(rMprChanged()), this, SIGNAL(changed()));
	}
}

} //namespace cx

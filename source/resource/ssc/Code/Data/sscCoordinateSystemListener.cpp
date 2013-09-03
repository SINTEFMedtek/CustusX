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

namespace ssc {

CoordinateSystemListener::CoordinateSystemListener(ssc::CoordinateSystem space) :
	mSpace(space)
{
	this->doConnect();
}

CoordinateSystemListener::~CoordinateSystemListener()
{
}

void CoordinateSystemListener::setSpace(ssc::CoordinateSystem space)
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

ssc::CoordinateSystem CoordinateSystemListener::getSpace() const
{
	return mSpace;
}

void CoordinateSystemListener::doConnect()
{
	if (mSpace.mId == ssc::csDATA)
	{
		ssc::DataPtr data = ssc::dataManager()->getData(mSpace.mRefObject);
		if (data)
		{
			connect(data.get(), SIGNAL(transformChanged()), this, SIGNAL(changed()));
			connect(ssc::dataManager(), SIGNAL(dataRemoved(QString)), this, SIGNAL(changed()));
		}
	}

	if (mSpace.mId == ssc::csSENSOR || mSpace.mId == ssc::csTOOL || mSpace.mId == ssc::csTOOL_OFFSET)
	{
		ssc::ToolPtr tool = ssc::toolManager()->getTool(mSpace.mRefObject);
		if (tool)
		{
			connect(tool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D,double)), this, SIGNAL(changed()));
			connect(tool.get(), SIGNAL(tooltipOffset(double)), this, SIGNAL(changed()));

			if (mSpace.mRefObject == "active")
			{
				connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SIGNAL(changed()));
				connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(reconnect()));
			}
			connect(ssc::toolManager(), SIGNAL(rMprChanged()), this, SIGNAL(changed()));
		}
	}

	if (mSpace.mId == ssc::csPATIENTREF)
	{
		connect(ssc::toolManager(), SIGNAL(rMprChanged()), this, SIGNAL(changed()));
	}
}

void CoordinateSystemListener::doDisconnect()
{
	if (mSpace.mId == ssc::csDATA)
	{
		ssc::DataPtr data = ssc::dataManager()->getData(mSpace.mRefObject);
		if (data)
		{
			disconnect(data.get(), SIGNAL(transformChanged()), this, SIGNAL(changed()));
			disconnect(ssc::dataManager(), SIGNAL(dataRemoved(QString)), this, SIGNAL(changed()));
		}
	}

	if (mSpace.mId == ssc::csSENSOR || mSpace.mId == ssc::csTOOL || mSpace.mId == ssc::csTOOL_OFFSET)
	{
		ssc::ToolPtr tool = ssc::toolManager()->getTool(mSpace.mRefObject);
		if (tool)
		{
			disconnect(tool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D,double)), this, SIGNAL(changed()));
			disconnect(tool.get(), SIGNAL(tooltipOffset(double)), this, SIGNAL(changed()));

			if (mSpace.mRefObject == "active")
			{
				disconnect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SIGNAL(changed()));
				disconnect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(reconnect()));
			}
			disconnect(ssc::toolManager(), SIGNAL(rMprChanged()), this, SIGNAL(changed()));
		}
	}

	if (mSpace.mId == ssc::csPATIENTREF)
	{
		disconnect(ssc::toolManager(), SIGNAL(rMprChanged()), this, SIGNAL(changed()));
	}
}

} //namespace ssc

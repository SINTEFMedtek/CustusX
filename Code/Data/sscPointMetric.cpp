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

#include "sscPointMetric.h"
#include "sscBoundingBox3D.h"
#include "sscTool.h"
#include "sscToolManager.h"
#include "sscTypeConversions.h"

namespace ssc
{

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

	if (mSpace.mId == ssc::csSENSOR || mSpace.mId == ssc::csTOOL || mSpace.mId == ssc::csTOOL)
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

// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------

ssc::DataPtr PointMetricReader::load(const QString& uid, const QString& filename)
{
	return ssc::DataPtr(new PointMetric(uid, filename));
}

PointMetric::PointMetric(const QString& uid, const QString& name) :
	DataMetric(uid, name),
	mCoordinate(0,0,0),
	mSpace(ssc::SpaceHelpers::getR())
{
	mSpaceListener.reset(new CoordinateSystemListener(mSpace));
	connect(mSpaceListener.get(), SIGNAL(changed()), this, SIGNAL(transformChanged()));
}

PointMetric::~PointMetric()
{
}

void PointMetric::setCoordinate(const ssc::Vector3D& p)
{
	if (p == mCoordinate)
		return;

	mCoordinate = p;
	emit transformChanged();
}

ssc::Vector3D PointMetric::getCoordinate() const
{
	return mCoordinate;
}

void PointMetric::setSpace(ssc::CoordinateSystem space)
{
	if (space == mSpace)
		return;

	// keep the absolute position (in ref) constant when changing space.
	ssc::Transform3D new_M_old = ssc::SpaceHelpers::get_toMfrom(this->getSpace(), space);
	mCoordinate = new_M_old.coord(mCoordinate);

	mSpace = space;
	mSpaceListener->setSpace(space);
}

ssc::CoordinateSystem PointMetric::getSpace() const
{
	return mSpace;
}

void PointMetric::addXml(QDomNode& dataNode)
{
	Data::addXml(dataNode);

	dataNode.toElement().setAttribute("space", mSpace.toString());
	dataNode.toElement().setAttribute("coord", qstring_cast(mCoordinate));
}

void PointMetric::parseXml(QDomNode& dataNode)
{
	Data::parseXml(dataNode);

	this->setSpace(ssc::CoordinateSystem::fromString(dataNode.toElement().attribute("space", mSpace.toString())));
	this->setCoordinate(ssc::Vector3D::fromString(dataNode.toElement().attribute("coord", qstring_cast(mCoordinate))));
}

ssc::DoubleBoundingBox3D PointMetric::boundingBox() const
{
	// convert both inputs to r space
	ssc::Transform3D rM0 = ssc::SpaceHelpers::get_toMfrom(this->getSpace(), ssc::CoordinateSystem(ssc::csREF));
	ssc::Vector3D p0_r = rM0.coord(this->getCoordinate());

	return ssc::DoubleBoundingBox3D(p0_r, p0_r);
}

/** Utility function: return the coordinate the the reference space.
 *
 */
ssc::Vector3D PointMetric::getRefCoord() const
{
	ssc::Transform3D rM1 = ssc::SpaceHelpers::get_toMfrom(this->getSpace(), ssc::CoordinateSystem(ssc::csREF));
	return rM1.coord(this->getCoordinate());
}

}

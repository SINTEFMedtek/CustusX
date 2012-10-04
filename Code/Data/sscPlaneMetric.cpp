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

#include <sscPlaneMetric.h>

#include "sscBoundingBox3D.h"
#include "sscTool.h"
#include "sscToolManager.h"
#include "sscTypeConversions.h"

namespace ssc
{

ssc::DataPtr PlaneMetricReader::load(const QString& uid, const QString& filename)
{
	return ssc::DataPtr(new PlaneMetric(uid, filename));
}

PlaneMetric::PlaneMetric(const QString& uid, const QString& name) :
	DataMetric(uid, name),
	mSpace(ssc::SpaceHelpers::getR())
{
	mSpaceListener.reset(new CoordinateSystemListener(mSpace));
	connect(mSpaceListener.get(), SIGNAL(changed()), this, SIGNAL(transformChanged()));
}

PlaneMetric::~PlaneMetric()
{
}

Plane3D PlaneMetric::getRefPlane() const
{
	ssc::Transform3D rM1 = ssc::SpaceHelpers::get_toMfrom(this->getSpace(), ssc::CoordinateSystem(ssc::csREF));
	ssc::Vector3D p = rM1.coord(this->getCoordinate());
	ssc::Vector3D n = rM1.vector(this->getNormal()).normalized();

	return Eigen::Hyperplane<double, 3>(n, p);
}

void PlaneMetric::setCoordinate(const ssc::Vector3D& p)
{
	if (p == mCoordinate)
		return;
	mCoordinate = p;
	emit transformChanged();
}

ssc::Vector3D PlaneMetric::getCoordinate() const
{
	return mCoordinate;
}

void PlaneMetric::setNormal(const ssc::Vector3D& p)
{
	if (p == mNormal)
		return;
	mNormal = p;
	emit transformChanged();
}

ssc::Vector3D PlaneMetric::getNormal() const
{
	return mNormal;
}

void PlaneMetric::setSpace(ssc::CoordinateSystem space)
{
	if (space == mSpace)
		return;

	// keep the absolute position (in ref) constant when changing space.
	ssc::Transform3D new_M_old = ssc::SpaceHelpers::get_toMfrom(this->getSpace(), space);
	mCoordinate = new_M_old.coord(mCoordinate);
	mNormal = new_M_old.vector(mNormal);

	mSpace = space;

	mSpaceListener->setSpace(space);
//	//TODO connect to the owner of space - data or tool or whatever
//	if (mSpace.mId == ssc::csTOOL)
//	{
//		connect(ssc::toolManager()->getTool(mSpace.mRefObject).get(),
//						SIGNAL(toolTransformAndTimestamp(Transform3D,double)), this,
//						SIGNAL(transformChanged()));
//	}

	emit transformChanged();
}

ssc::CoordinateSystem PlaneMetric::getSpace() const
{
	return mSpace;
}

void PlaneMetric::addXml(QDomNode& dataNode)
{
	Data::addXml(dataNode);

	dataNode.toElement().setAttribute("space", mSpace.toString());
	dataNode.toElement().setAttribute("coord", qstring_cast(mCoordinate));
	dataNode.toElement().setAttribute("normal", qstring_cast(mNormal));
}

void PlaneMetric::parseXml(QDomNode& dataNode)
{
	Data::parseXml(dataNode);

	mSpace = ssc::CoordinateSystem::fromString(dataNode.toElement().attribute("space", mSpace.toString()));
	mCoordinate = ssc::Vector3D::fromString(dataNode.toElement().attribute("coord", qstring_cast(mCoordinate)));
	mNormal = ssc::Vector3D::fromString(dataNode.toElement().attribute("normal", qstring_cast(mNormal)));
}

ssc::DoubleBoundingBox3D PlaneMetric::boundingBox() const
{
	// convert both inputs to r space
	ssc::Transform3D rM0 = ssc::SpaceHelpers::get_toMfrom(this->getSpace(), ssc::CoordinateSystem(ssc::csREF));
	ssc::Vector3D p0_r = rM0.coord(this->getCoordinate());

	return ssc::DoubleBoundingBox3D(p0_r, p0_r);
}

}

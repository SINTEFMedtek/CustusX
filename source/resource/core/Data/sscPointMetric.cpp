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

namespace cx
{

DataPtr PointMetricReader::load(const QString& uid, const QString& filename)
{
	return DataPtr(new PointMetric(uid, filename));
}

PointMetric::PointMetric(const QString& uid, const QString& name) :
	DataMetric(uid, name),
	mCoordinate(0,0,0),
	mSpace(SpaceHelpers::getR())
{
	mSpaceListener.reset(new CoordinateSystemListener(mSpace));
	connect(mSpaceListener.get(), SIGNAL(changed()), this, SIGNAL(transformChanged()));
}

PointMetricPtr PointMetric::create(QString uid, QString name)
{
    return PointMetricPtr(new PointMetric(uid, name));
}

PointMetricPtr PointMetric::create(QDomNode node)
{
    PointMetricPtr retval = PointMetric::create("");
    retval->parseXml(node);
    return retval;
}

PointMetric::~PointMetric()
{
}

void PointMetric::setCoordinate(const Vector3D& p)
{
	if (p == mCoordinate)
		return;

	mCoordinate = p;
	emit transformChanged();
}

Vector3D PointMetric::getCoordinate() const
{
	return mCoordinate;
}

void PointMetric::setSpace(CoordinateSystem space)
{
	if (space == mSpace)
		return;

	// keep the absolute position (in ref) constant when changing space.
	Transform3D new_M_old = SpaceHelpers::get_toMfrom(this->getSpace(), space);
	mCoordinate = new_M_old.coord(mCoordinate);

	mSpace = space;
	mSpaceListener->setSpace(space);
}

CoordinateSystem PointMetric::getSpace() const
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

	this->setSpace(CoordinateSystem::fromString(dataNode.toElement().attribute("space", mSpace.toString())));
	this->setCoordinate(Vector3D::fromString(dataNode.toElement().attribute("coord", qstring_cast(mCoordinate))));
}

DoubleBoundingBox3D PointMetric::boundingBox() const
{
	// convert both inputs to r space
	Transform3D rM0 = SpaceHelpers::get_toMfrom(this->getSpace(), CoordinateSystem(csREF));
	Vector3D p0_r = rM0.coord(this->getCoordinate());

	return DoubleBoundingBox3D(p0_r, p0_r);
}

/** Utility function: return the coordinate the the reference space.
 *
 */
Vector3D PointMetric::getRefCoord() const
{
	Transform3D rM1 = SpaceHelpers::get_toMfrom(this->getSpace(), CoordinateSystem(csREF));
	return rM1.coord(this->getCoordinate());
}

QString PointMetric::getAsSingleLineString() const
{
	return QString("%1 \"%2\" %3")
			.arg(this->getSingleLineHeader())
			.arg(mSpace.toString())
			.arg(qstring_cast(this->getCoordinate()));
}

}

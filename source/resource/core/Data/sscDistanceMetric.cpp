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

#include <sscDistanceMetric.h>
#include "sscBoundingBox3D.h"
#include "sscTypeConversions.h"
#include "sscPlaneMetric.h"
#include "sscPointMetric.h"

//TODO: this inclusion adds an unwanted dependency - must be solved.
#include "sscDataManager.h"

namespace cx
{

DataPtr DistanceMetricReader::load(const QString& uid, const QString& filename)
{
	return DataPtr(new DistanceMetric(uid, filename));
}

DistanceMetric::DistanceMetric(const QString& uid, const QString& name) :
				DataMetric(uid, name)
{
}

DistanceMetricPtr DistanceMetric::create(QString uid, QString name)
{
    return DistanceMetricPtr(new DistanceMetric(uid, name));
}

DistanceMetricPtr DistanceMetric::create(QDomNode node)
{
    DistanceMetricPtr retval = DistanceMetric::create("");
    retval->parseXml(node);
    return retval;
}
DistanceMetric::~DistanceMetric()
{
}

unsigned DistanceMetric::getArgumentCount() const
{
	return mArgument.size();
}

void DistanceMetric::setArgument(int index, DataPtr p)
{
	if (mArgument[index] == p)
		return;

	if (mArgument[index])
		disconnect(mArgument[index].get(), SIGNAL(transformChanged()), this, SIGNAL(transformChanged()));

	mArgument[index] = p;

	if (mArgument[index])
		connect(mArgument[index].get(), SIGNAL(transformChanged()), this, SIGNAL(transformChanged()));

	emit transformChanged();
}

DataPtr DistanceMetric::getArgument(int index)
{
	return mArgument[index];
}

bool DistanceMetric::validArgument(DataPtr p) const
{
	return p->getType() == "pointMetric" || p->getType() == "planeMetric";
}

Vector3D DistanceMetric::getRefCoord() const
{
    return this->boundingBox().center();
}

bool DistanceMetric::isValid() const
{
    return this->getEndpoints().size()==2;
}

void DistanceMetric::addXml(QDomNode& dataNode)
{
	Data::addXml(dataNode);

	for (unsigned i = 0; i < mArgument.size(); ++i)
	{
		if (mArgument[i])
			dataNode.toElement().setAttribute(QString("p%1").arg(i), mArgument[i]->getUid());
	}
}

void DistanceMetric::parseXml(QDomNode& dataNode)
{
	Data::parseXml(dataNode);

	for (unsigned i = 0; i < mArgument.size(); ++i)
	{
		QString uid = dataNode.toElement().attribute(QString("p%1").arg(i), "");
		this->setArgument(i, dataManager()->getData(uid));
	}
}

std::vector<Vector3D> DistanceMetric::getEndpoints() const
{
	if (!mArgument[0] || !mArgument[1])
		return std::vector<Vector3D>();
//	PointMetricPtr pt = boost::dynamic_pointer_cast<PointMetric>(dataManager()->getData(uid));
	std::vector<Vector3D> retval(2);
	// case   I: point-point
	// case  II: point-plane
	// case III: plane-plane (not implemented)

	if ((mArgument[0]->getType() == "pointMetric") && (mArgument[1]->getType() == "pointMetric"))
	{
		retval[0] = boost::dynamic_pointer_cast<PointMetric>(mArgument[0])->getRefCoord();
		retval[1] = boost::dynamic_pointer_cast<PointMetric>(mArgument[1])->getRefCoord();
	}
	else if ((mArgument[0]->getType() == "planeMetric") && (mArgument[1]->getType() == "pointMetric"))
	{
		Plane3D plane = boost::dynamic_pointer_cast<PlaneMetric>(mArgument[0])->getRefPlane();
		Vector3D p = boost::dynamic_pointer_cast<PointMetric>(mArgument[1])->getRefCoord();

		retval[0] = plane.projection(p);
		retval[1] = p;
	}
	else if ((mArgument[0]->getType() == "pointMetric") && (mArgument[1]->getType() == "planeMetric"))
	{
		Plane3D plane = boost::dynamic_pointer_cast<PlaneMetric>(mArgument[1])->getRefPlane();
		Vector3D p = boost::dynamic_pointer_cast<PointMetric>(mArgument[0])->getRefCoord();

		retval[1] = plane.projection(p);
		retval[0] = p;
	}
//	else if ((mArgument[0]->getType()=="planeMetric") && (mArgument[0]->getType()=="pointMetric"))
//	{
//		... only if planes are parallel... not much use.
//
//		Plane3D plane0 = boost::dynamic_pointer_cast<PlaneMetric>(mArgument[0])->getRefPlane();
//		Plane3D plane1 = boost::dynamic_pointer_cast<PlaneMetric>(mArgument[1])->getRefPlane();
//
//		Vector3D p0 = - plane0.normal() * plane0.offset();
//		Vector3D p0 = - plane0.normal() * plane0.offset();
//
//		if (cross(plane0.))
//	}
	else
	{
		return std::vector<Vector3D>();
	}

	return retval;
}

double DistanceMetric::getDistance() const
{
	std::vector<Vector3D> endpoints = this->getEndpoints();
	if (endpoints.size() != 2)
		return -1;

	return (endpoints[1] - endpoints[0]).length();
}

DoubleBoundingBox3D DistanceMetric::boundingBox() const
{
	return DoubleBoundingBox3D::fromCloud(this->getEndpoints());
}

QString DistanceMetric::getAsSingleLineString() const
{
	return QString("%1 %2")
			.arg(this->getSingleLineHeader())
			.arg(qstring_cast(this->getDistance()));
}

}

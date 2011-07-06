/*
 * cxDistanceMetric.cpp
 *
 *  Created on: Jul 4, 2011
 *      Author: christiana
 */

#include <cxDistanceMetric.h>
#include "sscBoundingBox3D.h"

namespace cx
{

DistanceMetric::DistanceMetric(const QString& uid, const QString& name) :
			ssc::Data(uid, name)
{
}

DistanceMetric::~DistanceMetric()
{
}

void DistanceMetric::setPoint(int index, PointMetricPtr p)
{
	if (mPoint[index])
		disconnect(mPoint[index].get(), SIGNAL(transformChanged()), this, SIGNAL(transformChanged()));

	mPoint[index] = p;

	if (mPoint[index])
		connect(mPoint[index].get(), SIGNAL(transformChanged()), this, SIGNAL(transformChanged()));
}

PointMetricPtr DistanceMetric::getPoint(int index)
{
	return mPoint[index];
}

void DistanceMetric::addXml(QDomNode& dataNode)
{

}

void DistanceMetric::parseXml(QDomNode& dataNode)
{

}

double DistanceMetric::getDistance() const
{
	// convert both inputs to r space
	ssc::Transform3D rM0 = ssc::SpaceHelpers::get_toMfrom(mPoint[0]->getFrame(), ssc::CoordinateSystem(ssc::csREF));
	ssc::Vector3D p0_r = rM0.coord(mPoint[0]->getCoordinate());

	ssc::Transform3D rM1 = ssc::SpaceHelpers::get_toMfrom(mPoint[1]->getFrame(), ssc::CoordinateSystem(ssc::csREF));
	ssc::Vector3D p1_r = rM1.coord(mPoint[1]->getCoordinate());


	std::cout << "p0: " << p0_r << ", p1: " << p1_r << ", d=" << (p0_r - p1_r).length() << std::endl;

	return (p0_r - p1_r).length();
}

ssc::DoubleBoundingBox3D DistanceMetric::boundingBox() const
{
	// convert both inputs to r space
	ssc::Transform3D rM0 = ssc::SpaceHelpers::get_toMfrom(mPoint[0]->getFrame(), ssc::CoordinateSystem(ssc::csREF));
	ssc::Vector3D p0_r = rM0.coord(mPoint[0]->getCoordinate());

	ssc::Transform3D rM1 = ssc::SpaceHelpers::get_toMfrom(mPoint[1]->getFrame(), ssc::CoordinateSystem(ssc::csREF));
	ssc::Vector3D p1_r = rM1.coord(mPoint[1]->getCoordinate());

	return ssc::DoubleBoundingBox3D(p0_r, p1_r);
}

}

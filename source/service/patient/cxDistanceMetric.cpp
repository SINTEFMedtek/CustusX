/*
 * cxDistanceMetric.cpp
 *
 *  Created on: Jul 4, 2011
 *      Author: christiana
 */

#include <cxDistanceMetric.h>
#include "sscBoundingBox3D.h"
#include "sscTypeConversions.h"

//TODO: this inclusion adds an unwanted dependency - must be solved.
#include "sscDataManager.h"

namespace cx
{

ssc::DataPtr DistanceMetricReader::load(const QString& uid, const QString& filename)
{
  return ssc::DataPtr(new DistanceMetric(uid,filename));
}


DistanceMetric::DistanceMetric(const QString& uid, const QString& name) :
			ssc::Data(uid, name)
{
}

DistanceMetric::~DistanceMetric()
{
}

void DistanceMetric::setPoint(int index, PointMetricPtr p)
{
  if (mPoint[index]==p)
    return;

	if (mPoint[index])
		disconnect(mPoint[index].get(), SIGNAL(transformChanged()), this, SIGNAL(transformChanged()));

	mPoint[index] = p;

	if (mPoint[index])
		connect(mPoint[index].get(), SIGNAL(transformChanged()), this, SIGNAL(transformChanged()));

  emit transformChanged();
}

PointMetricPtr DistanceMetric::getPoint(int index)
{
	return mPoint[index];
}

void DistanceMetric::addXml(QDomNode& dataNode)
{
  Data::addXml(dataNode);

  for (unsigned i=0; i<mPoint.size(); ++i)
  {
    if (mPoint[i])
      dataNode.toElement().setAttribute(QString("p%1").arg(i), mPoint[i]->getUid());
  }
}

void DistanceMetric::parseXml(QDomNode& dataNode)
{
  Data::parseXml(dataNode);

  for (unsigned i=0; i<mPoint.size(); ++i)
  {
    QString uid = dataNode.toElement().attribute(QString("p%1").arg(i), "");
    PointMetricPtr pt = boost::shared_dynamic_cast<PointMetric>(ssc::dataManager()->getData(uid));
    this->setPoint(i, pt);
  }
}

double DistanceMetric::getDistance() const
{
  if (!mPoint[0] || !mPoint[1])
    return -1;
	// convert both inputs to r space
	ssc::Transform3D rM0 = ssc::SpaceHelpers::get_toMfrom(mPoint[0]->getFrame(), ssc::CoordinateSystem(ssc::csREF));
	ssc::Vector3D p0_r = rM0.coord(mPoint[0]->getCoordinate());

	ssc::Transform3D rM1 = ssc::SpaceHelpers::get_toMfrom(mPoint[1]->getFrame(), ssc::CoordinateSystem(ssc::csREF));
	ssc::Vector3D p1_r = rM1.coord(mPoint[1]->getCoordinate());


//	std::cout << "p0: " << p0_r << ", p1: " << p1_r << ", d=" << (p0_r - p1_r).length() << std::endl;

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

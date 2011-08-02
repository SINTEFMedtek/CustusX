/*
 * cxDistanceMetric.cpp
 *
 *  Created on: Jul 4, 2011
 *      Author: christiana
 */

#include <cxDistanceMetric.h>
#include "sscBoundingBox3D.h"
#include "sscTypeConversions.h"
#include "cxPlaneMetric.h"
#include "cxPointMetric.h"

//TODO: this inclusion adds an unwanted dependency - must be solved.
#include "sscDataManager.h"


namespace cx
{

ssc::DataPtr DistanceMetricReader::load(const QString& uid, const QString& filename)
{
  return ssc::DataPtr(new DistanceMetric(uid,filename));
}


DistanceMetric::DistanceMetric(const QString& uid, const QString& name) :
		DataMetric(uid, name)
{
}


DistanceMetric::~DistanceMetric()
{
}

unsigned DistanceMetric::getArgumentCount() const
{
	return mArgument.size();
}

void DistanceMetric::setArgument(int index, ssc::DataPtr p)
{
  if (mArgument[index]==p)
    return;

	if (mArgument[index])
		disconnect(mArgument[index].get(), SIGNAL(transformChanged()), this, SIGNAL(transformChanged()));

	mArgument[index] = p;

	if (mArgument[index])
		connect(mArgument[index].get(), SIGNAL(transformChanged()), this, SIGNAL(transformChanged()));

  emit transformChanged();
}

ssc::DataPtr DistanceMetric::getArgument(int index)
{
	return mArgument[index];
}

bool DistanceMetric::validArgument(ssc::DataPtr p) const
{
	return p->getType()=="pointMetric" || p->getType()=="planeMetric";
}

//void DistanceMetric::setPoint(int index, PointMetricPtr p)
//{
//  if (mPoint[index]==p)
//    return;
//
//	if (mPoint[index])
//		disconnect(mPoint[index].get(), SIGNAL(transformChanged()), this, SIGNAL(transformChanged()));
//
//	mPoint[index] = p;
//
//	if (mPoint[index])
//		connect(mPoint[index].get(), SIGNAL(transformChanged()), this, SIGNAL(transformChanged()));
//
//  emit transformChanged();
//}
//
//PointMetricPtr DistanceMetric::getPoint(int index)
//{
//	return mArguments[index];
//}

void DistanceMetric::addXml(QDomNode& dataNode)
{
  Data::addXml(dataNode);

  for (unsigned i=0; i<mArgument.size(); ++i)
  {
    if (mArgument[i])
      dataNode.toElement().setAttribute(QString("p%1").arg(i), mArgument[i]->getUid());
  }
}

void DistanceMetric::parseXml(QDomNode& dataNode)
{
  Data::parseXml(dataNode);

  for (unsigned i=0; i<mArgument.size(); ++i)
  {
    QString uid = dataNode.toElement().attribute(QString("p%1").arg(i), "");
    this->setArgument(i, ssc::dataManager()->getData(uid));
  }
}


std::vector<ssc::Vector3D> DistanceMetric::getEndpoints() const
{
	if (!mArgument[0] || !mArgument[1])
		return std::vector<ssc::Vector3D>();
//	PointMetricPtr pt = boost::shared_dynamic_cast<PointMetric>(ssc::dataManager()->getData(uid));
	std::vector<ssc::Vector3D> retval(2);
  // case   I: point-point
  // case  II: point-plane
  // case III: plane-plane (not implemented)

	if ((mArgument[0]->getType()=="pointMetric") && (mArgument[1]->getType()=="pointMetric"))
	{
		retval[0] = boost::shared_dynamic_cast<PointMetric>(mArgument[0])->getRefCoord();
		retval[1] = boost::shared_dynamic_cast<PointMetric>(mArgument[1])->getRefCoord();
	}
	else if ((mArgument[0]->getType()=="planeMetric") && (mArgument[1]->getType()=="pointMetric"))
	{
		Plane3D plane = boost::shared_dynamic_cast<PlaneMetric>(mArgument[0])->getRefPlane();
		ssc::Vector3D p = boost::shared_dynamic_cast<PointMetric>(mArgument[1])->getRefCoord();

		retval[0] = plane.projection(p);
		retval[1] = p;
	}
	else if ((mArgument[0]->getType()=="pointMetric") && (mArgument[1]->getType()=="planeMetric"))
	{
		Plane3D plane = boost::shared_dynamic_cast<PlaneMetric>(mArgument[1])->getRefPlane();
		ssc::Vector3D p = boost::shared_dynamic_cast<PointMetric>(mArgument[0])->getRefCoord();

		retval[1] = plane.projection(p);
		retval[0] = p;
	}
//	else if ((mArgument[0]->getType()=="planeMetric") && (mArgument[0]->getType()=="pointMetric"))
//	{
//		... only if planes are parallel... not much use.
//
//		Plane3D plane0 = boost::shared_dynamic_cast<PlaneMetric>(mArgument[0])->getRefPlane();
//		Plane3D plane1 = boost::shared_dynamic_cast<PlaneMetric>(mArgument[1])->getRefPlane();
//
//		ssc::Vector3D p0 = - plane0.normal() * plane0.offset();
//		ssc::Vector3D p0 = - plane0.normal() * plane0.offset();
//
//		if (ssc::cross(plane0.))
//	}
	else
	{
		return std::vector<ssc::Vector3D>();
	}

	return retval;
}


double DistanceMetric::getDistance() const
{
	std::vector<ssc::Vector3D> endpoints = this->getEndpoints();
	if (endpoints.size()!=2)
		return -1;

	return (endpoints[1] - endpoints[0]).length();
}

ssc::DoubleBoundingBox3D DistanceMetric::boundingBox() const
{
	return ssc::DoubleBoundingBox3D::fromCloud(this->getEndpoints());
}

}

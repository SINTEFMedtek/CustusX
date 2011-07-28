/*
 * cxAngleMetric.cpp
 *
 *  Created on: Jul 27, 2011
 *      Author: christiana
 */

#include <cxAngleMetric.h>

#include "sscBoundingBox3D.h"
#include "sscTypeConversions.h"

//TODO: this inclusion adds an unwanted dependency - must be solved.
#include "sscDataManager.h"

namespace cx
{

ssc::DataPtr AngleMetricReader::load(const QString& uid, const QString& filename)
{
  return ssc::DataPtr(new AngleMetric(uid,filename));
}


AngleMetric::AngleMetric(const QString& uid, const QString& name) :
      ssc::Data(uid, name)
{
}

AngleMetric::~AngleMetric()
{
}

void AngleMetric::setArgument(int index, ssc::DataPtr p)
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

ssc::DataPtr AngleMetric::getArgument(int index)
{
	return mArgument[index];
}

bool AngleMetric::validArgument(ssc::DataPtr p) const
{
	return p->getType()=="pointMetric";// || p->getType()=="planeMetric";
}


//void AngleMetric::setPoint(int index, PointMetricPtr p)
//{
//  if (mPoint[index]==p)
//    return;
//
//  if (mPoint[index])
//  {
//    disconnect(mPoint[index].get(), SIGNAL(transformChanged()), this, SIGNAL(transformChanged()));
//  }
//
//  mPoint[index] = p;
//
//  if (mPoint[index])
//  {
//    connect(mPoint[index].get(), SIGNAL(transformChanged()), this, SIGNAL(transformChanged()));
//  }
//
//  emit transformChanged();
//}
//
//PointMetricPtr AngleMetric::getPoint(int index)
//{
//  return mPoint[index];
//}

void AngleMetric::addXml(QDomNode& dataNode)
{
  Data::addXml(dataNode);

  for (unsigned i=0; i<mArgument.size(); ++i)
  {
    if (mArgument[i])
      dataNode.toElement().setAttribute(QString("p%1").arg(i), mArgument[i]->getUid());
  }
}

void AngleMetric::parseXml(QDomNode& dataNode)
{
  Data::parseXml(dataNode);

  for (unsigned i=0; i<mArgument.size(); ++i)
  {
    QString uid = dataNode.toElement().attribute(QString("p%1").arg(i), "");
    this->setArgument(i, ssc::dataManager()->getData(uid));
  }
}

bool AngleMetric::isValid() const
{
	return !this->getEndpoints().empty();
}

unsigned AngleMetric::getArgumentCount() const
{
	return mArgument.size();
}

std::vector<ssc::Vector3D> AngleMetric::getEndpoints() const
{
  std::vector<ssc::Vector3D> p(this->getArgumentCount());
  for (unsigned i=0; i<p.size(); ++i)
  {
  	if (!mArgument[i])
  		return std::vector<ssc::Vector3D>();
    p[i] = boost::shared_dynamic_cast<PointMetric>(mArgument[i])->getRefCoord();
  }
  return p;
}

double AngleMetric::getAngle() const
{
  std::vector<ssc::Vector3D> p = this->getEndpoints();

  if (p.empty())
  	return -1;

  ssc::Vector3D a = (p[0]-p[1]).normalized();
  ssc::Vector3D b = (p[3]-p[2]).normalized();

  double angle = acos(ssc::dot(a,b)/a.length()/b.length());
  return angle;
}

ssc::DoubleBoundingBox3D AngleMetric::boundingBox() const
{
  return ssc::DoubleBoundingBox3D::fromCloud(this->getEndpoints());
}

}

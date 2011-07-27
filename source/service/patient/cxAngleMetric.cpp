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

void AngleMetric::setPoint(int index, PointMetricPtr p)
{
  if (mPoint[index]==p)
    return;

  if (mPoint[index])
  {
    disconnect(mPoint[index].get(), SIGNAL(transformChanged()), this, SIGNAL(transformChanged()));
  }

  mPoint[index] = p;

  if (mPoint[index])
  {
    connect(mPoint[index].get(), SIGNAL(transformChanged()), this, SIGNAL(transformChanged()));
  }

  emit transformChanged();
}

PointMetricPtr AngleMetric::getPoint(int index)
{
  return mPoint[index];
}

void AngleMetric::addXml(QDomNode& dataNode)
{
  Data::addXml(dataNode);

  for (unsigned i=0; i<mPoint.size(); ++i)
  {
    if (mPoint[i])
      dataNode.toElement().setAttribute(QString("p%1").arg(i), mPoint[i]->getUid());
  }
}

void AngleMetric::parseXml(QDomNode& dataNode)
{
  Data::parseXml(dataNode);

  for (unsigned i=0; i<mPoint.size(); ++i)
  {
    QString uid = dataNode.toElement().attribute(QString("p%1").arg(i), "");
    PointMetricPtr pt = boost::shared_dynamic_cast<PointMetric>(ssc::dataManager()->getData(uid));
//    mPoint[i] = boost::shared_dynamic_cast<PointMetric>(ssc::dataManager()->getData(uid));
    this->setPoint(i, pt);
  }
}

bool AngleMetric::isValid() const
{
  for (unsigned i=0; i<mPoint.size(); ++i)
  {
    if (!mPoint[i])
      return false;
  }
  return true;
}

double AngleMetric::getAngle() const
{
  if (!this->isValid())
    return -1;

  std::vector<ssc::Vector3D> p_r(4);
  for (unsigned i=0; i<4; ++i)
  {
    ssc::Transform3D rMi = ssc::SpaceHelpers::get_toMfrom(mPoint[i]->getFrame(), ssc::CoordinateSystem(ssc::csREF));
    p_r[i] = rMi.coord(mPoint[i]->getCoordinate());
  }

  ssc::Vector3D a = (p_r[0]-p_r[1]).normalized();
  ssc::Vector3D b = (p_r[3]-p_r[2]).normalized();

  double angle = acos(ssc::dot(a,b)/a.length()/b.length());
//  emit transformChanged();
  return angle;
}

ssc::DoubleBoundingBox3D AngleMetric::boundingBox() const
{
  std::vector<ssc::Vector3D> p_r(4);
  for (unsigned i=0; i<4; ++i)
  {
    ssc::Transform3D rMi = ssc::SpaceHelpers::get_toMfrom(mPoint[i]->getFrame(), ssc::CoordinateSystem(ssc::csREF));
    p_r[i] = rMi.coord(mPoint[i]->getCoordinate());
  }

  return ssc::DoubleBoundingBox3D::fromCloud(p_r);
}

void AngleMetric::transformChangedSlot()
{
//  std::cout << "AngleMetric::transformChangedSlot()" << std::endl;
}

}

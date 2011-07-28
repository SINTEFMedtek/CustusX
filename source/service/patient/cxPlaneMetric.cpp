/*
 * cxPlaneMetric.cpp
 *
 *  Created on: Jul 27, 2011
 *      Author: christiana
 */

#include <cxPlaneMetric.h>

#include "sscBoundingBox3D.h"
#include "sscTool.h"
#include "sscToolManager.h"
#include "sscTypeConversions.h"
//#include ""

namespace cx
{

ssc::DataPtr PlaneMetricReader::load(const QString& uid, const QString& filename)
{
  return ssc::DataPtr(new PlaneMetric(uid,filename));
}


PlaneMetric::PlaneMetric(const QString& uid, const QString& name) :
  Data(uid, name)
{
}

PlaneMetric::~PlaneMetric()
{
}

Plane3D PlaneMetric::getRefPlane() const
{
	ssc::Transform3D rM1 = ssc::SpaceHelpers::get_toMfrom(this->getFrame(), ssc::CoordinateSystem(ssc::csREF));
	ssc::Vector3D p = rM1.coord(this->getCoordinate());
	ssc::Vector3D n = rM1.vector(this->getNormal()).normalized();

	return Eigen::Hyperplane<double,3>(n,p);
}

void PlaneMetric::setCoordinate(const ssc::Vector3D& p)
{
  if (p==mCoordinate)
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
  if (p==mNormal)
    return;
  mNormal = p;
  emit transformChanged();
}

ssc::Vector3D PlaneMetric::getNormal() const
{
  return mNormal;
}

void PlaneMetric::setFrame(ssc::CoordinateSystem space)
{
  if (space==mFrame)
    return;

  mFrame = space;

  //TODO connect to the owner of space - data or tool or whatever
  if (mFrame.mId==ssc::csTOOL)
  {
    connect(ssc::toolManager()->getTool(mFrame.mRefObject).get(), SIGNAL(toolTransformAndTimestamp(Transform3D,double)), this, SIGNAL(transformChanged()));
  }

  emit transformChanged();
}

ssc::CoordinateSystem PlaneMetric::getFrame() const
{
  return mFrame;
}

void PlaneMetric::addXml(QDomNode& dataNode)
{
  Data::addXml(dataNode);

  dataNode.toElement().setAttribute("frame", mFrame.toString());
  dataNode.toElement().setAttribute("coord", qstring_cast(mCoordinate));
  dataNode.toElement().setAttribute("normal", qstring_cast(mNormal));
}

void PlaneMetric::parseXml(QDomNode& dataNode)
{
  Data::parseXml(dataNode);

  mFrame = ssc::CoordinateSystem::fromString(dataNode.toElement().attribute("frame", mFrame.toString()));
  mCoordinate = ssc::Vector3D::fromString(dataNode.toElement().attribute("coord", qstring_cast(mCoordinate)));
  mNormal = ssc::Vector3D::fromString(dataNode.toElement().attribute("normal", qstring_cast(mNormal)));
}

ssc::DoubleBoundingBox3D PlaneMetric::boundingBox() const
{
  // convert both inputs to r space
  ssc::Transform3D rM0 = ssc::SpaceHelpers::get_toMfrom(this->getFrame(), ssc::CoordinateSystem(ssc::csREF));
  ssc::Vector3D p0_r = rM0.coord(this->getCoordinate());

  return ssc::DoubleBoundingBox3D(p0_r, p0_r);
}


}

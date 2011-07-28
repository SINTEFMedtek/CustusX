/*
 * cxPointMetric.cpp
 *
 *  Created on: Jul 4, 2011
 *      Author: christiana
 */

#include <cxPointMetric.h>
#include "sscBoundingBox3D.h"
#include "sscTool.h"
#include "sscToolManager.h"
#include "sscTypeConversions.h"

namespace cx
{

ssc::DataPtr PointMetricReader::load(const QString& uid, const QString& filename)
{
  return ssc::DataPtr(new PointMetric(uid,filename));
}


PointMetric::PointMetric(const QString& uid, const QString& name) :
	Data(uid, name)
{
}

PointMetric::~PointMetric()
{
}

void PointMetric::setCoordinate(const ssc::Vector3D& p)
{
	if (p==mCoordinate)
		return;

	mCoordinate = p;
	emit transformChanged();
}

ssc::Vector3D PointMetric::getCoordinate() const
{
	return mCoordinate;
}


void PointMetric::setFrame(ssc::CoordinateSystem space)
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

ssc::CoordinateSystem PointMetric::getFrame() const
{
	return mFrame;
}

void PointMetric::addXml(QDomNode& dataNode)
{
  Data::addXml(dataNode);

  dataNode.toElement().setAttribute("frame", mFrame.toString());
  dataNode.toElement().setAttribute("coord", qstring_cast(mCoordinate));
}

void PointMetric::parseXml(QDomNode& dataNode)
{
  Data::parseXml(dataNode);

  mFrame = ssc::CoordinateSystem::fromString(dataNode.toElement().attribute("frame", mFrame.toString()));
  mCoordinate = ssc::Vector3D::fromString(dataNode.toElement().attribute("coord", qstring_cast(mCoordinate)));
}

ssc::DoubleBoundingBox3D PointMetric::boundingBox() const
{
  // convert both inputs to r space
  ssc::Transform3D rM0 = ssc::SpaceHelpers::get_toMfrom(this->getFrame(), ssc::CoordinateSystem(ssc::csREF));
  ssc::Vector3D p0_r = rM0.coord(this->getCoordinate());

  return ssc::DoubleBoundingBox3D(p0_r, p0_r);
}

/** Utility function: return the coordinate the the reference space.
 *
 */
ssc::Vector3D PointMetric::getRefCoord() const
{
	ssc::Transform3D rM1 = ssc::SpaceHelpers::get_toMfrom(this->getFrame(), ssc::CoordinateSystem(ssc::csREF));
	return rM1.coord(this->getCoordinate());
}

}

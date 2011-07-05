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

namespace cx
{

PointMetric::PointMetric(const QString& uid, const QString& name) :
	Data(uid, name)
{
	// TODO Auto-generated constructor stub
}

PointMetric::~PointMetric()
{
	// TODO Auto-generated destructor stub
}

void PointMetric::setCoordinate(const ssc::Vector3D& p)
{
	mCoordinate = p;
	emit transformChanged();
}

ssc::Vector3D PointMetric::getCoordinate() const
{
	return mCoordinate;
}


void PointMetric::setFrame(ssc::CoordinateSystem space)
{
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

}

void PointMetric::parseXml(QDomNode& dataNode)
{

}

ssc::DoubleBoundingBox3D PointMetric::boundingBox() const
{
	return ssc::DoubleBoundingBox3D(mCoordinate, mCoordinate);
}


}

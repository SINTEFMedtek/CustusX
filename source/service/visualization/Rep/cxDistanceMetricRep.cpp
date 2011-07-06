/*
 * cxDistanceMetricRep.cpp
 *
 *  Created on: Jul 5, 2011
 *      Author: christiana
 */

#include <cxDistanceMetricRep.h>
#include "sscView.h"

namespace cx
{

DistanceMetricRepPtr DistanceMetricRep::New(const QString& uid, const QString& name)
{
	DistanceMetricRepPtr retval(new DistanceMetricRep(uid,name));
	return retval;
}

DistanceMetricRep::DistanceMetricRep(const QString& uid, const QString& name) :
		ssc::RepImpl(uid,name),
		mView(NULL)
{
}


void DistanceMetricRep::setDistanceMetric(DistanceMetricPtr point)
{
	if (mDistanceMetric)
		disconnect(mDistanceMetric.get(), SIGNAL(transformChanged()), this, SLOT(changedSlot()));

	mDistanceMetric = point;

	if (mDistanceMetric)
		connect(mDistanceMetric.get(), SIGNAL(transformChanged()), this, SLOT(changedSlot()));

	mGraphicalLine.reset();
	this->changedSlot();
}

void DistanceMetricRep::addRepActorsToViewRenderer(ssc::View* view)
{
	mView = view;
	mGraphicalLine.reset();
	this->changedSlot();
}

void DistanceMetricRep::removeRepActorsFromViewRenderer(ssc::View* view)
{
	mView = NULL;
	mGraphicalLine.reset();
}

void DistanceMetricRep::changedSlot()
{
	if (!mGraphicalLine && mView && mDistanceMetric)
		mGraphicalLine.reset(new ssc::GraphicalLine3D(mView->getRenderer()));

	if (!mGraphicalLine)
		return;

	ssc::Transform3D rM0 = ssc::SpaceHelpers::get_toMfrom(mDistanceMetric->getPoint(0)->getFrame(), ssc::CoordinateSystem(ssc::csREF));
	ssc::Vector3D p0_r = rM0.coord(mDistanceMetric->getPoint(0)->getCoordinate());

	ssc::Transform3D rM1 = ssc::SpaceHelpers::get_toMfrom(mDistanceMetric->getPoint(1)->getFrame(), ssc::CoordinateSystem(ssc::csREF));
	ssc::Vector3D p1_r = rM1.coord(mDistanceMetric->getPoint(1)->getCoordinate());

	mGraphicalLine->setColor(ssc::Vector3D(1,0,0));
	mGraphicalLine->setValue(p0_r, p1_r);
	mGraphicalLine->setStipple(0xF0FF);
}


}

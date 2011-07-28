/*
 * cxPointMetricRep.cpp
 *
 *  Created on: Jul 5, 2011
 *      Author: christiana
 */

#include <cxPointMetricRep.h>
#include "sscView.h"
#include "boost/bind.hpp"

namespace cx
{

PointMetricRepPtr PointMetricRep::New(const QString& uid, const QString& name)
{
	PointMetricRepPtr retval(new PointMetricRep(uid,name));
	return retval;
}

PointMetricRep::PointMetricRep(const QString& uid, const QString& name) :
		RepImpl(uid,name),
		mView(NULL),
		mSphereRadius(1)
{
  mViewportListener.reset(new ssc::ViewportListener);
	mViewportListener->setCallback(boost::bind(&PointMetricRep::scaleText, this));
}


void PointMetricRep::setPointMetric(PointMetricPtr point)
{
	if (mPointMetric)
		disconnect(mPointMetric.get(), SIGNAL(transformChanged()), this, SLOT(changedSlot()));

	mPointMetric = point;

	if (mPointMetric)
		connect(mPointMetric.get(), SIGNAL(transformChanged()), this, SLOT(changedSlot()));

	mGraphicalPoint.reset();
	this->changedSlot();
}

void PointMetricRep::addRepActorsToViewRenderer(ssc::View* view)
{
	mView = view;
	mGraphicalPoint.reset();
	mViewportListener->startListen(mView->getRenderer());
	this->changedSlot();
}

void PointMetricRep::removeRepActorsFromViewRenderer(ssc::View* view)
{
	mView = NULL;
	mGraphicalPoint.reset();
	mViewportListener->stopListen();
}

void PointMetricRep::setSphereRadius(double radius)
{
  mSphereRadius = radius;
  this->changedSlot();
}

void PointMetricRep::changedSlot()
{
	if (!mGraphicalPoint && mView && mPointMetric)
		mGraphicalPoint.reset(new ssc::GraphicalPoint3D(mView->getRenderer()));

	if (!mGraphicalPoint)
		return;

	ssc::Transform3D rM0 = ssc::SpaceHelpers::get_toMfrom(mPointMetric->getFrame(), ssc::CoordinateSystem(ssc::csREF));
	ssc::Vector3D p0_r = rM0.coord(mPointMetric->getCoordinate());

	mGraphicalPoint->setValue(p0_r);
	mGraphicalPoint->setRadius(mSphereRadius);
	mGraphicalPoint->setColor(ssc::Vector3D(1,0,0));
	this->scaleText();
}

/**Note: Internal method!
 *
 * Scale the text to be a constant fraction of the viewport height
 * Called from a vtk camera observer
 *
 */
void PointMetricRep::scaleText()
{
  if (!mGraphicalPoint)
    return;

	double size = mViewportListener->getVpnZoom();
//  double sphereSize = 0.007/size;
  double sphereSize = mSphereRadius/100/size;
  mGraphicalPoint->setRadius(sphereSize);
}

}

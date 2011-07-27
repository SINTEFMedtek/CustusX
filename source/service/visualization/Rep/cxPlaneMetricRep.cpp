/*
 * cxPlaneMetricRep.cpp
 *
 *  Created on: Jul 27, 2011
 *      Author: christiana
 */

#include <cxPlaneMetricRep.h>

#include "sscView.h"


namespace cx
{

PlaneMetricRepPtr PlaneMetricRep::New(const QString& uid, const QString& name)
{
  PlaneMetricRepPtr retval(new PlaneMetricRep(uid,name));
  return retval;
}

PlaneMetricRep::PlaneMetricRep(const QString& uid, const QString& name) :
    RepImpl(uid,name),
    mView(NULL),
    mSphereRadius(1)
{
}


void PlaneMetricRep::setMetric(PlaneMetricPtr point)
{
  if (mMetric)
    disconnect(mMetric.get(), SIGNAL(transformChanged()), this, SLOT(changedSlot()));

  mMetric = point;

  if (mMetric)
    connect(mMetric.get(), SIGNAL(transformChanged()), this, SLOT(changedSlot()));

  mGraphicalPoint.reset();
  this->changedSlot();
}

void PlaneMetricRep::addRepActorsToViewRenderer(ssc::View* view)
{
  mView = view;
  mGraphicalPoint.reset();
  this->changedSlot();
}

void PlaneMetricRep::removeRepActorsFromViewRenderer(ssc::View* view)
{
  mView = NULL;
  mGraphicalPoint.reset();
}

void PlaneMetricRep::setSphereRadius(double radius)
{
  mSphereRadius = radius;
  this->changedSlot();
}

void PlaneMetricRep::changedSlot()
{
  if (!mGraphicalPoint && mView && mMetric)
    mGraphicalPoint.reset(new ssc::GraphicalPoint3D(mView->getRenderer()));

  if (!mGraphicalPoint)
    return;

  ssc::Transform3D rM0 = ssc::SpaceHelpers::get_toMfrom(mMetric->getFrame(), ssc::CoordinateSystem(ssc::csREF));
  ssc::Vector3D p0_r = rM0.coord(mMetric->getCoordinate());

  mGraphicalPoint->setValue(p0_r);
  mGraphicalPoint->setRadius(mSphereRadius);
  mGraphicalPoint->setColor(ssc::Vector3D(1,0,0));
}


}

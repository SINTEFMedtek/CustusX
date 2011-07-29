/*
 * cxPlaneMetricRep.cpp
 *
 *  Created on: Jul 27, 2011
 *      Author: christiana
 */

#include <cxPlaneMetricRep.h>

#include "sscView.h"
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include "boost/bind.hpp"

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
    mSphereRadius(1),
    mColor(1,0,0),
    mShowLabel(false)
{
  mViewportListener.reset(new ssc::ViewportListener);
	mViewportListener->setCallback(boost::bind(&PlaneMetricRep::scaleText, this));
}

void PlaneMetricRep::setShowLabel(bool on)
{
  mShowLabel = on;
  this->changedSlot();
}

void PlaneMetricRep::setMetric(PlaneMetricPtr point)
{
  if (mMetric)
    disconnect(mMetric.get(), SIGNAL(transformChanged()), this, SLOT(changedSlot()));

  mMetric = point;

  if (mMetric)
    connect(mMetric.get(), SIGNAL(transformChanged()), this, SLOT(changedSlot()));

  mGraphicalPoint.reset();
  mNormal.reset();
  this->changedSlot();
}

void PlaneMetricRep::addRepActorsToViewRenderer(ssc::View* view)
{
  mView = view;
  mGraphicalPoint.reset();
  mNormal.reset();
	mViewportListener->startListen(mView->getRenderer());
  this->changedSlot();
}

void PlaneMetricRep::removeRepActorsFromViewRenderer(ssc::View* view)
{
  mView = NULL;
  mGraphicalPoint.reset();
  mNormal.reset();
	mViewportListener->stopListen();
}

void PlaneMetricRep::setSphereRadius(double radius)
{
  mSphereRadius = radius;
  this->changedSlot();
}

void PlaneMetricRep::changedSlot()
{
  if (!mMetric)
    return;

  if (!mGraphicalPoint && mView && mMetric)
  {
    mGraphicalPoint.reset(new ssc::GraphicalPoint3D(mView->getRenderer()));
    mNormal.reset(new ssc::GraphicalArrow3D(mView->getRenderer()));
  }

  if (!mGraphicalPoint)
    return;

//  ssc::Transform3D rM0 = ssc::SpaceHelpers::get_toMfrom(mMetric->getFrame(), ssc::CoordinateSystem(ssc::csREF));
//  ssc::Vector3D p0_r = rM0.coord(mMetric->getCoordinate());
//  ssc::Vector3D n_r = rM0.vector(mMetric->getNormal());

//  mGraphicalPoint->setValue(p0_r);
//  mGraphicalPoint->setRadius(mSphereRadius);
  mGraphicalPoint->setColor(mColor);
  mNormal->setColor(mColor);
//  mNormal->setValue(p0_r, n_r, 10);

  this->scaleText();
}

/**Note: Internal method!
 *
 * Scale the text to be a constant fraction of the viewport height
 * Called from a vtk camera observer
 *
 */
void PlaneMetricRep::scaleText()
{
  if (!mGraphicalPoint)
    return;

  ssc::Transform3D rM0 = ssc::SpaceHelpers::get_toMfrom(mMetric->getFrame(), ssc::CoordinateSystem(ssc::csREF));
  ssc::Vector3D p0_r = rM0.coord(mMetric->getCoordinate());
  ssc::Vector3D n_r = rM0.vector(mMetric->getNormal());

  double size = mViewportListener->getVpnZoom();
  double sphereSize = mSphereRadius/100/size;
//  double mSize = mSphereRadius/100/size*10;
//	double mSize = 0.07; // ratio of vp height
//  double scale = mSize/size;
//  std::cout << "s= " << size << "  ,scale= " << scale << std::endl;

  mGraphicalPoint->setValue(p0_r);
  mNormal->setValue(p0_r, n_r, sphereSize*10);

//  double sphereSize = 0.007/size;
  mGraphicalPoint->setRadius(sphereSize);

  if (!mShowLabel)
    mText.reset();
  if (!mText && mShowLabel)
    mText.reset(new ssc::FollowerText3D(mView->getRenderer()));
  if (mText)
  {
    mText->setColor(mColor);
    mText->setText(mMetric->getName());
    mText->setPosition(p0_r);
    mText->setSizeInNormalizedViewport(true, 0.025);
  }
}

}

/*
 * cxAngleMetricRep.cpp
 *
 *  Created on: Jul 27, 2011
 *      Author: christiana
 */

#include <cxAngleMetricRep.h>

#include "sscView.h"

#include <vtkVectorText.h>
#include <vtkFollower.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include "sscTypeConversions.h"

namespace cx
{

AngleMetricRepPtr AngleMetricRep::New(const QString& uid, const QString& name)
{
  AngleMetricRepPtr retval(new AngleMetricRep(uid,name));
  return retval;
}

AngleMetricRep::AngleMetricRep(const QString& uid, const QString& name) :
    ssc::RepImpl(uid,name),
    mView(NULL),
    mColor(1,0,0)
{
}


void AngleMetricRep::setMetric(AngleMetricPtr point)
{
  if (mMetric)
    disconnect(mMetric.get(), SIGNAL(transformChanged()), this, SLOT(changedSlot()));

  mMetric = point;

  if (mMetric)
    connect(mMetric.get(), SIGNAL(transformChanged()), this, SLOT(changedSlot()));

  mLine0.reset();
  mLine1.reset();
  mArc.reset();
  mText.reset();
  this->changedSlot();
}

void AngleMetricRep::addRepActorsToViewRenderer(ssc::View* view)
{
  mView = view;

  mLine0.reset();
  mLine1.reset();
  mArc.reset();
  mText.reset();

  this->changedSlot();
}

void AngleMetricRep::removeRepActorsFromViewRenderer(ssc::View* view)
{
  mView = NULL;

  mLine0.reset();
  mLine1.reset();
  mArc.reset();
  mText.reset();
}

void AngleMetricRep::changedSlot()
{
  if (!mMetric->isValid())
    return;

  if (!mLine0 && !mLine1 && mView && mMetric)
  {
    mLine0.reset(new ssc::GraphicalLine3D(mView->getRenderer()));
    mLine1.reset(new ssc::GraphicalLine3D(mView->getRenderer()));
    mArc.reset(new ssc::GraphicalArc3D(mView->getRenderer()));
    mText.reset(new ssc::FollowerText3D(mView->getRenderer()));
  }

  if (!mLine0)
  	return;

  std::vector<ssc::Vector3D> p = mMetric->getEndpoints();

  mLine0->setColor(mColor);
  mLine1->setColor(mColor);
  mArc->setColor(mColor);
  mLine0->setStipple(0x0F0F);
  mLine1->setStipple(0x0F0F);
  mArc->setStipple(0xF0FF);
  mLine0->setValue(p[0], p[1]);
  mLine1->setValue(p[2], p[3]);

  ssc::Vector3D a_center = (p[1]+p[2])/2;
  ssc::Vector3D l0 = p[0]-p[1];
  ssc::Vector3D l1 = p[3]-p[2];
  double d = (l0.length() + l1.length())/2 * 0.5;
  ssc::Vector3D a_start = a_center + l0.normalized() * d;
  ssc::Vector3D a_end = a_center + l1.normalized() * d;
  mArc->setValue(a_start, a_end, a_center);

  ssc::Vector3D a_text = (a_center + a_start + a_end)/3;

  QString text = QString("%1*").arg(mMetric->getAngle()/M_PI*180, 0, 'f', 1);
  mText->setColor(mColor);
  mText->setText(text);
  mText->setPosition(a_text);
  mText->setSizeInNormalizedViewport(true, 0.025);
}



}

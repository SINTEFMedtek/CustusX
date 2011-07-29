/*
 * cxDistanceMetricRep.cpp
 *
 *  Created on: Jul 5, 2011
 *      Author: christiana
 */

#include <cxDistanceMetricRep.h>
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

DistanceMetricRepPtr DistanceMetricRep::New(const QString& uid, const QString& name)
{
	DistanceMetricRepPtr retval(new DistanceMetricRep(uid,name));
	return retval;
}

DistanceMetricRep::DistanceMetricRep(const QString& uid, const QString& name) :
		ssc::RepImpl(uid,name),
		mView(NULL),
		mColor(1,0,0),
		mShowLabel(false)
{
}

void DistanceMetricRep::setShowLabel(bool on)
{
  mShowLabel = on;
  this->changedSlot();
}

void DistanceMetricRep::setDistanceMetric(DistanceMetricPtr point)
{
	if (mMetric)
		disconnect(mMetric.get(), SIGNAL(transformChanged()), this, SLOT(changedSlot()));

	mMetric = point;

	if (mMetric)
		connect(mMetric.get(), SIGNAL(transformChanged()), this, SLOT(changedSlot()));

	mGraphicalLine.reset();
	mText.reset();
	this->changedSlot();
}

void DistanceMetricRep::addRepActorsToViewRenderer(ssc::View* view)
{
	mView = view;
	mGraphicalLine.reset();
	mText.reset();

	this->changedSlot();
}

void DistanceMetricRep::removeRepActorsFromViewRenderer(ssc::View* view)
{
	mView = NULL;
	mGraphicalLine.reset();
	mText.reset();
}

void DistanceMetricRep::changedSlot()
{
  if (!mMetric)
    return;

	std::vector<ssc::Vector3D> p = mMetric->getEndpoints();
	if (p.size()!=2)
		return;

  if (!mGraphicalLine && mView && mMetric)
  {
		mGraphicalLine.reset(new ssc::GraphicalLine3D(mView->getRenderer()));
    mText.reset(new ssc::FollowerText3D(mView->getRenderer()));
  }

	if (!mGraphicalLine)
		return;

	mGraphicalLine->setColor(mColor);
	mGraphicalLine->setValue(p[0], p[1]);
	mGraphicalLine->setStipple(0xF0FF);

	QString text = QString("%1 mm").arg(mMetric->getDistance(), 0, 'f', 1);
  if (mShowLabel)
    text = mMetric->getName() + " = " + text;
	ssc::Vector3D p_mean = (p[0]+p[1])/2;

  mText->setColor(mColor);
  mText->setText(text);
  mText->setPosition(p_mean);
  mText->setSizeInNormalizedViewport(true, 0.025);
}

}

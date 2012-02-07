// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

/*
 * cxDistanceMetricRep.cpp
 *
 *  \date Jul 5, 2011
 *      \author christiana
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
	DistanceMetricRepPtr retval(new DistanceMetricRep(uid, name));
	return retval;
}

DistanceMetricRep::DistanceMetricRep(const QString& uid, const QString& name) :
				DataMetricRep(uid, name), mView(NULL)
{
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
	if (p.size() != 2)
		return;

	if (!mGraphicalLine && mView && mMetric)
	{
		mGraphicalLine.reset(new ssc::GraphicalLine3D(mView->getRenderer()));
		mText.reset(new ssc::CaptionText3D(mView->getRenderer()));
	}

	if (!mGraphicalLine)
		return;

	mGraphicalLine->setColor(mColor);
	mGraphicalLine->setValue(p[0], p[1]);
	mGraphicalLine->setStipple(0xF0FF);

	QString text = QString("%1 mm").arg(mMetric->getDistance(), 0, 'f', 1);
	if (mShowLabel)
		text = mMetric->getName() + " = " + text;
	ssc::Vector3D p_mean = (p[0] + p[1]) / 2;

	mText->setColor(mColor);
	mText->setText(text);
	mText->setPosition(p_mean);
	mText->setSize(mLabelSize / 100);
//  mText->setSizeInNormalizedViewport(true, mLabelSize/100);
}

}

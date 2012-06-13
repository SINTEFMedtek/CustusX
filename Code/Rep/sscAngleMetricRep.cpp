// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#include <sscAngleMetricRep.h>

#include "sscView.h"

#include <vtkVectorText.h>
#include <vtkFollower.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include "sscTypeConversions.h"
#include "vtkTextActor.h"
#include "sscGraphicalPrimitives.h"
#include "sscAngleMetric.h"
#include "sscGraphicalPrimitives.h"

namespace ssc
{

AngleMetricRepPtr AngleMetricRep::New(const QString& uid, const QString& name)
{
	AngleMetricRepPtr retval(new AngleMetricRep(uid, name));
	return retval;
}

AngleMetricRep::AngleMetricRep(const QString& uid, const QString& name) :
				DataMetricRep(uid, name),
				mView(NULL)
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

void AngleMetricRep::addRepActorsToViewRenderer(ssc::View *view)
{
	mView = view;

	mLine0.reset();
	mLine1.reset();
	mArc.reset();
	mText.reset();

	this->changedSlot();
}

void AngleMetricRep::removeRepActorsFromViewRenderer(ssc::View *view)
{
	mView = NULL;

	mLine0.reset();
	mLine1.reset();
	mArc.reset();
	mText.reset();
}

void AngleMetricRep::changedSlot()
{
	if (!mMetric || !mMetric->isValid())
		return;

	if (!mLine0 && !mLine1 && mView && mMetric)
	{
		mLine0.reset(new ssc::GraphicalLine3D(mView->getRenderer()));
		mLine1.reset(new ssc::GraphicalLine3D(mView->getRenderer()));
		mArc.reset(new ssc::GraphicalArc3D(mView->getRenderer()));
		mText.reset(new ssc::CaptionText3D(mView->getRenderer()));
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

	ssc::Vector3D a_center = (p[1] + p[2]) / 2;
	ssc::Vector3D l0 = p[0] - p[1];
	ssc::Vector3D l1 = p[3] - p[2];
	double d = (l0.length() + l1.length()) / 2 * 0.5;
	ssc::Vector3D a_start = a_center + l0.normalized() * d;
	ssc::Vector3D a_end = a_center + l1.normalized() * d;
	mArc->setValue(a_start, a_end, a_center);

	ssc::Vector3D a_text = (a_center + a_start + a_end) / 3;

	QString text = QString("%1*").arg(mMetric->getAngle() / M_PI * 180, 0, 'f', 1);
	if (mShowLabel)
		text = mMetric->getName() + " = " + text;
	mText->setColor(mColor);
	mText->setText(text);
	mText->setPosition(a_text);
	mText->setSize(mLabelSize / 100);
}

}

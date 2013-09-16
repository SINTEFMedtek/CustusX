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

#include <sscDistanceMetricRep.h>
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
                DataMetricRep(uid, name)
{
}

void DistanceMetricRep::clear()
{
    DataMetricRep::clear();
    mGraphicalLine.reset();
}

DistanceMetricPtr DistanceMetricRep::getDistanceMetric()
{
    return boost::dynamic_pointer_cast<DistanceMetric>(mMetric);
}


void DistanceMetricRep::changedSlot()
{
    DistanceMetricPtr distanceMetric = this->getDistanceMetric();
    if (!distanceMetric)
		return;

    if (!mMetric->isValid())
        return;
//	if (p.size() != 2)
//		return;

    if (!mGraphicalLine && mView)
	{
		mGraphicalLine.reset(new GraphicalLine3D(mView->getRenderer()));
//		mText.reset(new CaptionText3D(mView->getRenderer()));
	}

    if (mGraphicalLine)
    {
        mGraphicalLine->setColor(mColor);
        std::vector<Vector3D> p = distanceMetric->getEndpoints();
        mGraphicalLine->setValue(p[0], p[1]);
        mGraphicalLine->setStipple(0xF0FF);
    }

//	QString text = QString("%1 mm").arg(mMetric->getDistance(), 0, 'f', 1);
//	if (mShowLabel)
//		text = mMetric->getName() + " = " + text;
//	Vector3D p_mean = (p[0] + p[1]) / 2;

//	mText->setColor(mColor);
//	mText->setText(text);
//	mText->setPosition(p_mean);
//	mText->setSize(mLabelSize / 100);
    this->drawText();
}

QString DistanceMetricRep::getText()
{
    QString text = QString("%1 mm").arg(this->getDistanceMetric()->getDistance(), 0, 'f', 1);
    if (mShowLabel)
        text = mMetric->getName() + " = " + text;
    return text;
}

}

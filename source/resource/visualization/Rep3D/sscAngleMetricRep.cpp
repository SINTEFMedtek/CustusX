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

namespace cx
{

AngleMetricRepPtr AngleMetricRep::New(const QString& uid, const QString& name)
{
	AngleMetricRepPtr retval(new AngleMetricRep(uid, name));
	return retval;
}

AngleMetricRep::AngleMetricRep(const QString& uid, const QString& name) :
                DataMetricRep(uid, name)
{
}

void AngleMetricRep::clear()
{
    DataMetricRep::clear();
    mLine0.reset();
    mLine1.reset();
    mArc.reset();
}

AngleMetricPtr AngleMetricRep::getAngleMetric()
{
    return boost::dynamic_pointer_cast<AngleMetric>(mMetric);
}

void AngleMetricRep::changedSlot()
{
    AngleMetricPtr angleMetric = this->getAngleMetric();

    if (!angleMetric || !mMetric->isValid())
		return;
	if (!mMetric)
		return;
	if (!mView)
		return;

    std::vector<Vector3D> p = angleMetric->getEndpoints();

	if (angleMetric->getUseSimpleVisualization())
	{
		mLine0.reset();
		mLine1.reset();
	}
	else
	{
		if (!mLine0)
		{
			mLine0.reset(new GraphicalLine3D(mView->getRenderer()));
		}
		mLine0->setColor(mMetric->getColor());
		mLine0->setStipple(0x0F0F);
		mLine0->setValue(p[0], p[1]);

		if (!mLine1)
		{
			mLine1.reset(new GraphicalLine3D(mView->getRenderer()));
		}
		mLine1->setColor(mMetric->getColor());
		mLine1->setStipple(0x0F0F);
		mLine1->setValue(p[2], p[3]);
	}

	if (!mArc)
	{
		mArc.reset(new GraphicalArc3D(mView->getRenderer()));
	}
	mArc->setColor(mMetric->getColor());
	mArc->setStipple(0xF0FF);
	Vector3D a_center = (p[1] + p[2]) / 2;
	Vector3D l0 = p[0] - p[1];
	Vector3D l1 = p[3] - p[2];
	double d = (l0.length() + l1.length()) / 2 * 0.5;
	Vector3D a_start = a_center + l0.normalized() * d;
	Vector3D a_end = a_center + l1.normalized() * d;
	mArc->setValue(a_start, a_end, a_center);

    this->drawText();
}


}

/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxAngleMetricRep.h"

#include "cxView.h"

#include <vtkVectorText.h>
#include <vtkFollower.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include "cxTypeConversions.h"
#include "vtkTextActor.h"
#include "cxGraphicalPrimitives.h"
#include "cxAngleMetric.h"
#include "cxGraphicalPrimitives.h"

namespace cx
{

AngleMetricRepPtr AngleMetricRep::New(const QString& uid)
{
	return wrap_new(new AngleMetricRep(), uid);
}

AngleMetricRep::AngleMetricRep()
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

void AngleMetricRep::onModifiedStartRender()
{
    AngleMetricPtr angleMetric = this->getAngleMetric();

    if (!angleMetric || !mMetric->isValid())
		return;
	if (!mMetric)
		return;
	if (!this->getView())
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
			mLine0.reset(new GraphicalLine3D(this->getRenderer()));
		}
		mLine0->setColor(mMetric->getColor());
		mLine0->setStipple(0x0F0F);
		mLine0->setValue(p[0], p[1]);

		if (!mLine1)
		{
			mLine1.reset(new GraphicalLine3D(this->getRenderer()));
		}
		mLine1->setColor(mMetric->getColor());
		mLine1->setStipple(0x0F0F);
		mLine1->setValue(p[2], p[3]);
	}

	if (!mArc)
	{
		mArc.reset(new GraphicalArc3D(this->getRenderer()));
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

/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

void AngleMetricRep::onModifiedStartRender()
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

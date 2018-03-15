/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxDistanceMetricRep.h"
#include "cxView.h"

#include <vtkVectorText.h>
#include <vtkFollower.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include "cxTypeConversions.h"

namespace cx
{

DistanceMetricRepPtr DistanceMetricRep::New(const QString& uid)
{
	return wrap_new(new DistanceMetricRep(), uid);
}

DistanceMetricRep::DistanceMetricRep()
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

void DistanceMetricRep::onModifiedStartRender()
{
	DistanceMetricPtr distanceMetric = this->getDistanceMetric();
	if (!distanceMetric)
		return;

	if (!mMetric->isValid())
		return;

	if (!mGraphicalLine && this->getView())
	{
		mGraphicalLine.reset(new GraphicalLine3D(this->getRenderer()));
	}

	if (mGraphicalLine)
	{
		mGraphicalLine->setColor(mMetric->getColor());
		std::vector<Vector3D> p = distanceMetric->getEndpoints();
		mGraphicalLine->setValue(p[0], p[1]);
		mGraphicalLine->setStipple(0xF0FF);
	}

	this->drawText();
}


}

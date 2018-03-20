/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxSphereMetricRep.h"

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
#include "cxSphereMetric.h"
#include "cxGraphicalPrimitives.h"

namespace cx
{

SphereMetricRepPtr SphereMetricRep::New(const QString& uid)
{
	return wrap_new(new SphereMetricRep(), uid);
}

SphereMetricRep::SphereMetricRep()
{
}

void SphereMetricRep::clear()
{
	DataMetricRep::clear();
	mGraphicalPoint.reset();
}

SphereMetricPtr SphereMetricRep::getSphereMetric()
{
	return boost::dynamic_pointer_cast<SphereMetric>(mMetric);
}

void SphereMetricRep::onModifiedStartRender()
{
	if (!mMetric)
		return;

	if (!mGraphicalPoint && this->getView() && mMetric)
		mGraphicalPoint.reset(new GraphicalPoint3D(this->getRenderer()));

	if (!mGraphicalPoint)
		return;

	mGraphicalPoint->setValue(mMetric->getRefCoord());
	mGraphicalPoint->setRadius(this->getSphereMetric()->getRadius());
	mGraphicalPoint->setColor(mMetric->getColor());

	this->drawText();
}

}

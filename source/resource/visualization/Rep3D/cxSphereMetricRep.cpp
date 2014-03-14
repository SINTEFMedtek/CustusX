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

SphereMetricRepPtr SphereMetricRep::New(const QString& uid, const QString& name)
{
	SphereMetricRepPtr retval(new SphereMetricRep(uid, name));
	return retval;
}

SphereMetricRep::SphereMetricRep(const QString& uid, const QString& name) :
				DataMetricRep(uid, name)
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

	if (!mGraphicalPoint && mView && mMetric)
		mGraphicalPoint.reset(new GraphicalPoint3D(mView->getRenderer()));

	if (!mGraphicalPoint)
		return;

	mGraphicalPoint->setValue(mMetric->getRefCoord());
	mGraphicalPoint->setRadius(this->getSphereMetric()->getRadius());
	mGraphicalPoint->setColor(mMetric->getColor());

	this->drawText();
}

}

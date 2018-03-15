/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxDonutMetricRep.h"

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
#include "cxShapedMetric.h"
#include "cxGraphicalPrimitives.h"
#include "vtkMatrix4x4.h"
#include "cxGraphicalTorus3D.h"
#include "cxGraphicalDisk.h"

namespace cx
{

DonutMetricRepPtr DonutMetricRep::New(const QString& uid)
{
	return wrap_new(new DonutMetricRep(), uid);
}

DonutMetricRep::DonutMetricRep()
{
}

void DonutMetricRep::clear()
{
	DataMetricRep::clear();
	mTorus.reset();
	mDisk.reset();
}

DonutMetricPtr DonutMetricRep::getDonutMetric()
{
	return boost::dynamic_pointer_cast<DonutMetric>(mMetric);
}

void DonutMetricRep::onModifiedStartRender()
{
	if (!mMetric)
		return;

	this->updateTorus();
	this->updateDisc();

	this->drawText();
}

void DonutMetricRep::updateTorus()
{
	if (!mMetric)
		return;

	DonutMetricPtr donut = this->getDonutMetric();

	if (donut->getFlat())
	{
		mTorus.reset();
		return;
	}

	if (!mTorus && this->getView() && mMetric)
		mTorus.reset(new GraphicalTorus3D(this->getRenderer()));

	if (!mTorus)
		return;

	mTorus->setPosition(donut->getPosition());
	mTorus->setDirection(donut->getDirection());
	mTorus->setRadius(donut->getRadius());
	mTorus->setThickness(donut->getThickness());
	mTorus->setColor(donut->getColor());
}

void DonutMetricRep::updateDisc()
{
	if (!mMetric)
		return;

	DonutMetricPtr donut = this->getDonutMetric();

	if (!donut->getFlat())
	{
		mDisk.reset();
		return;
	}

	if (!mDisk && this->getView() && mMetric)
	{
		mDisk.reset(new GraphicalDisk());
		mDisk->setRenderer(this->getRenderer());
	}

	if (!mDisk)
		return;

	mDisk->setPosition(donut->getPosition());
	mDisk->setDirection(donut->getDirection());
	mDisk->setRadius(donut->getRadius());
	mDisk->setHeight(donut->getHeight());
	mDisk->setColor(donut->getColor());
	mDisk->setOutlineColor(donut->getColor());
	mDisk->setOutlineWidth(donut->getThickness());
	mDisk->setFillVisible(false);
	mDisk->setLighting(true);

	mDisk->update();
}

}

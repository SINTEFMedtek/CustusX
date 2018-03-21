/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxRegionOfInterestMetricRep.h"
#include "cxView.h"

#include <vtkVectorText.h>
#include <vtkFollower.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include "cxTypeConversions.h"
#include "cxGraphicalBox.h"

namespace cx
{

RegionOfInterestMetricRepPtr RegionOfInterestMetricRep::New(const QString& uid)
{
	return wrap_new(new RegionOfInterestMetricRep(), uid);
}

RegionOfInterestMetricRep::RegionOfInterestMetricRep()
{
}

void RegionOfInterestMetricRep::clear()
{
	DataMetricRep::clear();
	mGraphicalBox.reset();
}

RegionOfInterestMetricPtr RegionOfInterestMetricRep::getRegionOfInterestMetric()
{
	return boost::dynamic_pointer_cast<RegionOfInterestMetric>(mMetric);
}

void RegionOfInterestMetricRep::onModifiedStartRender()
{
	RegionOfInterestMetricPtr metric = this->getRegionOfInterestMetric();
	if (!metric)
		return;

	if (!mMetric->isValid())
		return;

	if (!mGraphicalBox && this->getView())
	{
		mGraphicalBox.reset(new GraphicalBox());
		mGraphicalBox->setRenderWindow(this->getView()->getRenderWindow());
	}

	if (mGraphicalBox)
	{
		mGraphicalBox->setBoundingBox(metric->getROI().getBox());
		mGraphicalBox->setPosition(Transform3D::Identity());
//		mGraphicalBox->setPosition(metric->get_rMd()); // ROI is already in r
//		mGraphicalBox->setColor(mMetric->getColor());
	}


		mGraphicalBox->setInteractive(false);
//	mGraphicalBox->setVisible(false);

	this->drawText();
}


}

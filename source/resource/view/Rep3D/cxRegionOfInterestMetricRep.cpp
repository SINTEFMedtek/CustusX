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
